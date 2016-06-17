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
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/LorentzVector.h"

// fastsim
#include "FastSimulation/Utilities/interface/RandomEngineAndDistribution.h"
#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/Layer/interface/Layer.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"
#include "FastSimulation/NewParticle/interface/Particle.h"
#include "FastSimulation/Particle/interface/ParticleTable.h"
#include "FastSimulation/InteractionModel/interface/InteractionModel.h"
#include "FastSimulation/TrackerSimHitProducer/interface/ParticleLooper.h"

// other

class TrackerSimHitProducer : public edm::stream::EDProducer<> {
public:

    explicit TrackerSimHitProducer(const edm::ParameterSet&);
    ~TrackerSimHitProducer(){;}

private:

    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    void beginLuminosityBlock(edm::LuminosityBlock const & lumiBlock, edm::EventSetup const & iSetup) override;

    edm::EDGetTokenT<edm::HepMCProduct> genParticlesToken_;
    edm::ParameterSet detectorDefinitionCfg_;
    double beamPipeRadius_;
    std::unique_ptr<fastsim::Geometry> detector_;
    static const std::string MESSAGECATEGORY;
};

const std::string TrackerSimHitProducer::MESSAGECATEGORY = "FastSimulation";

//
// constructors and destructor
//
TrackerSimHitProducer::TrackerSimHitProducer(const edm::ParameterSet& iConfig)
    : genParticlesToken_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("src"))) 
    , detectorDefinitionCfg_(iConfig.getParameter<edm::ParameterSet>("detectorDefinition"))
    , beamPipeRadius_(iConfig.getParameter<double>("beamPipeRadius"))
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
    // NEED SENTRY?
    ParticleTable::Sentry ptable(&(*pdt));

    edm::Handle<edm::HepMCProduct> genParticles;
    iEvent.getByToken(genParticlesToken_,genParticles);

    // ?? is this the right place ??
    RandomEngineAndDistribution random(iEvent.streamID());

    fastsim::ParticleLooper particleLooper(
	*genParticles->GetEvent()
	,*pdt
	,beamPipeRadius_
	,output_simTracks
	,output_simVertices);
	
    // loop over particles
    LogDebug(MESSAGECATEGORY) << "################################"
			      << "\n###############################";    

    for(std::unique_ptr<fastsim::Particle> particle = particleLooper.nextParticle(); particle != 0;particle=particleLooper.nextParticle()) 
    {
	// move the particle through the layers
	fastsim::LayerNavigator layerNavigator(*detector_);
	const fastsim::Layer * layer = 0;
	while(layerNavigator.moveParticleToNextLayer(*particle,layer))
	{

	    if(layer)
	    {
		LogDebug(MESSAGECATEGORY) << "   moved to next layer:" << *layer;
	    }
	    LogDebug(MESSAGECATEGORY) << "\n   new state:" << particle;
	    
	    // perform interaction between layer and particle
	    LogDebug(MESSAGECATEGORY) << "   performing interactions. " << layer->getInteractionModels().size() << " interactionModels for this layer";
	    std::vector<fastsim::Particle> secondaries;
	    for(fastsim::InteractionModel * interactionModel : layer->getInteractionModels())
	    {
		secondaries.clear();
		interactionModel->interact(*particle,*layer,secondaries,random);
		//addSecondaries(particle,particleIndex,secondaries);
	    }

	    // kinematic cuts
	    // temporary: break after 100 ns
	    if(particle->position().T() > 100)
	    {
		break;
	    }
	    
	    LogDebug(MESSAGECATEGORY) << "--------------------------------"
				      << "\n-------------------------------";

	}

	// do decays
	if(particle->remainingProperLifeTime() == 0.)
	{
	    //std::vector<fastsim::Particle> secondaries;
	    //decayer.decay(*particle,secondaries);
	    //addSecondaries(*particle,particleIndex,secondaries);
	}
	
	LogDebug(MESSAGECATEGORY) << "################################"
				  << "\n###############################";
    }

    // store simHits and simTracks
    iEvent.put(particleLooper.harvestSimTracks());
    iEvent.put(particleLooper.harvestSimVertices());
    // store products of interaction models, i.e. simHits
    for(fastsim::InteractionModel * interactionModel : detector_->getInteractionModels())
    {
	interactionModel->storeProducts(iEvent);
    }
}

// TODO: this should actually become a member function of FSimEvent
//       to be used by decays as well
/*
void TrackerSimHitProducer::addSecondaries(const fastsim::Particle parent,int parentIndex,std::vector<fastsim::Particle> secondaries)
{
    // add secondaries to the event
    if(secondaries.size() > 0)
    {
	// TODO: probably want to get rid of the TLorentzVector in Particle etc.
	//    see https://root.cern.ch/root/html/MATH_GENVECTOR_Index.html
	//    it got all kinds of methods to do transformations
	int vertexIndex = simEvent_.addSimVertex(math::XYZTLorentzVector(parent.position().X(),
									 parent.position().Y(),
									 parent.position().Z(),
									 parent.position().T())
						 simTrackIndex);
	// TODO: deal with closest charged daughter
	for(const auto & secondary : secondaries)
	{
	    RawParticle _secondary(secondary.pdgId(),
				   math::XYZTLorentzVector(secondary.momentum.X(),
							   secondary.momentum.Y()
							   secondary.momentum.Z()
							   secondary.momentum.E()));
	    simEvent_.addSimTrack(_secondary,vertexIndex);
	}
    }
}
*/
DEFINE_FWK_MODULE(TrackerSimHitProducer);
