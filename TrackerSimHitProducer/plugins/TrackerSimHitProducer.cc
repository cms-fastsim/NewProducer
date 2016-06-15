// system include files
#include <memory>
#include <string>

// framework
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// data formats
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "DataFormats/Common/interface/Handle.h"

// fastsim
#include "FastSimulation/Utilities/interface/RandomEngineAndDistribution.h"
#include "FastSimulation/Event/interface/FSimEvent.h"
#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/Layer/interface/Layer.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"
#include "FastSimulation/NewParticle/interface/Particle.h"
#include "FastSimulation/Particle/interface/ParticleTable.h"
#include "FastSimulation/InteractionModel/interface/InteractionModel.h"

// other
#include "TLorentzVector.h"

class TrackerSimHitProducer : public edm::stream::EDProducer<> {
public:

    explicit TrackerSimHitProducer(const edm::ParameterSet&);
    ~TrackerSimHitProducer(){;}

private:

    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    void beginLuminosityBlock(edm::LuminosityBlock const & lumiBlock, edm::EventSetup const & iSetup) override;

    edm::EDGetTokenT<edm::HepMCProduct> genParticlesToken_;
    FSimEvent simEvent_;
    edm::ParameterSet detectorDefinitionCfg_;
    std::unique_ptr<fastsim::Geometry> detector_;
    static const std::string MESSAGECATEGORY;
};

const std::string TrackerSimHitProducer::MESSAGECATEGORY = "FastSimulation";

//
// constructors and destructor
//
TrackerSimHitProducer::TrackerSimHitProducer(const edm::ParameterSet& iConfig)
    : genParticlesToken_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("src"))) 
    , simEvent_(iConfig.getParameter<edm::ParameterSet>("particleFilter"))
    , detectorDefinitionCfg_(iConfig.getParameter<edm::ParameterSet>("detectorDefinition"))
{
    produces<edm::SimTrackContainer>();
    produces<edm::SimVertexContainer>();
    produces<std::vector<math::XYZTLorentzVector> >();
}


void TrackerSimHitProducer::beginLuminosityBlock(edm::LuminosityBlock const & lumiBlock, edm::EventSetup const & iSetup)
{
    detector_.reset(new fastsim::Geometry(detectorDefinitionCfg_,iSetup));
    for(const fastsim::InteractionModel * interactionModel : detector_->getInteractionModels())
    {
	interactionModel->registerProducts(*this);
    }
    LogDebug(MESSAGECATEGORY) << "Constructed geometry::\n" << *detector_;
}
   

// ------------ method called to produce the data  ------------
void
TrackerSimHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    LogDebug(MESSAGECATEGORY) << "   produce";

    std::unique_ptr<edm::SimTrackContainer> output_simTracks(new edm::SimTrackContainer);
    std::unique_ptr<edm::SimVertexContainer> output_simVertices(new edm::SimVertexContainer);

    edm::ESHandle < HepPDT::ParticleDataTable > pdt;
    iSetup.getData(pdt);
    simEvent_.initializePdt(&(*pdt));
    ParticleTable::Sentry ptable(&(*pdt));

    edm::Handle<edm::HepMCProduct> genParticles;
    iEvent.getByToken(genParticlesToken_,genParticles);

    // ?? is this the right place ??
    RandomEngineAndDistribution random(iEvent.streamID());

    simEvent_.clear();
    edm::EventID eventId = iEvent.id();
    simEvent_.fill(*genParticles->GetEvent(),eventId);
    
    // loop over particles
    LogDebug(MESSAGECATEGORY) << "################################"
			      << "\n###############################";    
    for( unsigned simTrackIndex=0; simTrackIndex < simEvent_.nTracks(); ++simTrackIndex) 
    {
	// reference to simtrack
	const FSimTrack & simTrack = simEvent_.track(simTrackIndex);

	// create particle for propagation, material interactions, decays...
	const math::XYZTLorentzVector & position = simTrack.vertex().position();
	const math::XYZTLorentzVector & momentum = simTrack.momentum();
	fastsim::Particle particle(simTrack.type(),simTrack.charge(),
				   TLorentzVector(position.X(),position.Y(),position.Z(),position.T()),
				   TLorentzVector(momentum.X(),momentum.Y(),momentum.Z(),position.E())
				   ,simTrack.decayTime());

	// move the particle through the layers
	fastsim::LayerNavigator layerNavigator(*detector_);
	const fastsim::Layer * layer = 0;
	while(layerNavigator.moveParticleToNextLayer(particle,layer))
	{

	    if(layer)
	    {
		LogDebug(MESSAGECATEGORY) << "   moved to next layer:" << *layer;
	    }
	    LogDebug(MESSAGECATEGORY) << "\n   new state:" << particle;
	    
	    // perform interaction between layer and particle
	    LogDebug(MESSAGECATEGORY) << "   performing interactions. " << layer->getInteractionModels().size() << " interactionModels for this layer";
	    for(fastsim::InteractionModel * interactionModel : layer->getInteractionModels())
	    {
		interactionModel->interact(particle,*layer,simEvent_,random);
	    }

	    // kinematic cuts
	    // temporary: break after 100 ns
	    if(particle.position().T() > 100)
	    {
		break;
	    }
	    
	    LogDebug(MESSAGECATEGORY) << "--------------------------------"
				      << "\n-------------------------------";

	}

	// do decays
	/*
	  if(particle.decayed())
	  {
	  // perform decay
	  }
	*/

	LogDebug(MESSAGECATEGORY) << "################################"
				  << "\n###############################";
    }

    // store simHits and simTracks
    // TODO: get rid of this TEMP
    std::unique_ptr<edm::SimTrackContainer> TEMP(new edm::SimTrackContainer);
    simEvent_.load(*output_simTracks,*TEMP);
    simEvent_.load(*output_simVertices);
    iEvent.put(std::move(output_simTracks));
    iEvent.put(std::move(output_simVertices));
    // store products of interaction models, i.e. simHits
    for(fastsim::InteractionModel * interactionModel : detector_->getInteractionModels())
    {
	interactionModel->storeProducts(iEvent);
    }
}

DEFINE_FWK_MODULE(TrackerSimHitProducer);
