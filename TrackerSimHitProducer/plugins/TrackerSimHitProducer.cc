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

// geometry
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

// fastsim
#include "FastSimulation/Utilities/interface/RandomEngineAndDistribution.h"
#include "FastSimulation/Event/interface/FSimEvent.h"
#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/Geometry/interface/Layer.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"
#include "FastSimulation/TrackerSimHitProducer/interface/TrackerSimHitFactory.h"
#include "FastSimulation/Particle/interface/RawParticle.h"
#include "FastSimulation/Particle/interface/ParticleTable.h"

class TrackerSimHitProducer : public edm::stream::EDProducer<> {
public:

    explicit TrackerSimHitProducer(const edm::ParameterSet&);
    ~TrackerSimHitProducer(){;}

private:

    virtual void produce(edm::Event&, const edm::EventSetup&) override;
    void beginLuminosityBlock(edm::LuminosityBlock const & lumiBlock, edm::EventSetup const & iSetup);

    std::string alignmentLabel_;
    edm::EDGetTokenT<edm::HepMCProduct> genParticlesToken_;
    FSimEvent simEvent_;
    edm::ParameterSet detectorLayersCfg_;
    bool makeSimHits_;
    bool magneticFieldIsFixed_;
    double magneticFieldZ_;
    std::unique_ptr<fastsim::Geometry> geometry_;
    static const std::string MESSAGECATEGORY;
};

const std::string TrackerSimHitProducer::MESSAGECATEGORY = "FastSimulation";

//
// constructors and destructor
//
TrackerSimHitProducer::TrackerSimHitProducer(const edm::ParameterSet& iConfig)
    : alignmentLabel_(iConfig.getParameter<std::string>("alignmentLabel"))
    , genParticlesToken_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("src"))) 
    , simEvent_(iConfig.getParameter<edm::ParameterSet>("particleFilter"))
    , detectorLayersCfg_(iConfig.getParameter<edm::ParameterSet>("detectorLayers"))
    , makeSimHits_(iConfig.getUntrackedParameter<bool>("makeSimHits",true))
    , magneticFieldIsFixed_(iConfig.exists("magneticFieldZ"))
    , magneticFieldZ_(magneticFieldIsFixed_ ? iConfig.getParameter<double>("magneticFieldZ") : 0)
{
    produces<edm::SimTrackContainer>();
    produces<edm::SimVertexContainer>();
    produces<edm::PSimHitContainer>("TrackerHits");
    
    /*
    if(iConfig.exists("materialEffects"))
    {
	materialEffects_.reset(new MaterialEffects(iConfig.getParameter<edm::ParameterSet>("MaterialEffects")));
    }
    */
}


void TrackerSimHitProducer::beginLuminosityBlock(edm::LuminosityBlock const & lumiBlock, edm::EventSetup const & iSetup)
{
    const GeometricSearchTracker * geometricSearchTracker = 0;
    if(makeSimHits_)
    {
	edm::ESHandle<GeometricSearchTracker> geometricSearchTrackerHandle;
	iSetup.get<TrackerRecoGeometryRecord>().get(alignmentLabel_,geometricSearchTrackerHandle);
	geometricSearchTracker = &(*geometricSearchTrackerHandle);
    }
    geometry_.reset(new fastsim::Geometry(detectorLayersCfg_,geometricSearchTracker));
    

    edm::ESHandle<MagneticField> magneticField;
    if(!magneticFieldIsFixed_)
    {
	iSetup.get<IdealMagneticFieldRecord>().get(magneticField);
	geometry_->setMagneticField(*magneticField);
    }
    else
    {
	geometry_->setMagneticFieldZ(magneticFieldZ_);
    }
    
    LogDebug(MESSAGECATEGORY) << "Constructed geometry::\n" << *geometry_;
    
}   

// ------------ method called to produce the data  ------------
void
TrackerSimHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    LogDebug(MESSAGECATEGORY) << "   produce";

    std::unique_ptr<edm::PSimHitContainer> output_simHits(new edm::PSimHitContainer);
    std::unique_ptr<edm::SimTrackContainer> output_simTracks(new edm::SimTrackContainer);
    std::unique_ptr<edm::SimTrackContainer> TEMP(new edm::SimTrackContainer);
    std::unique_ptr<edm::SimVertexContainer> output_simVertices(new edm::SimVertexContainer);

    edm::ESHandle < HepPDT::ParticleDataTable > pdt;
    iSetup.getData(pdt);
    simEvent_.initializePdt(&(*pdt));
    ParticleTable::Sentry ptable(&(*pdt));

    edm::Handle<edm::HepMCProduct> genParticles;
    iEvent.getByToken(genParticlesToken_,genParticles);
    
    RandomEngineAndDistribution random(iEvent.streamID());

    simEvent_.clear();
    edm::EventID eventId = iEvent.id();
    simEvent_.fill(*genParticles->GetEvent(),eventId);
    
    fastsim::TrackerSimHitFactory simHitFactory;

    LogDebug(MESSAGECATEGORY) << "################################"
			      << "\n###############################";

    for( unsigned simTrackIndex=0; simTrackIndex < simEvent_.nTracks(); ++simTrackIndex) 
    {
	    
	const FSimTrack & simTrack = simEvent_.track(simTrackIndex);

	// initialise navigator, need to 
	// TODO provide decay time as well
	fastsim::LayerNavigator layerNavigator(simTrack.type(),simTrack.vertex().position(),simTrack.momentum(),*geometry_);
	LogDebug(MESSAGECATEGORY) << "process particle:" << layerNavigator.particle() << std::endl;

	LogDebug(MESSAGECATEGORY) << "--------------------------------"
				  << "\n-------------------------------";

	while(layerNavigator.moveToNextLayer())
	{
	    
	    LogDebug(MESSAGECATEGORY) << "   moved to next layer:" << layerNavigator.currentLayer()
				      << "\n   new state:" << layerNavigator.particle();
	    
	    // do the material effects
	    // commented out for now, cause it depends on the layer interface, which changes
	    /*
	    if(materialEffects_)
	    {
		materialEffects_->interact(simEvent_,*layer,particle,simTrackIndex,&random);
	    }
	    */

	    /*
	    // create simhits
	    if(layer->getDetLayer())
	    {
		// associate hits to simTrack
		int simTrackId = simTrackIndex;
		// If a hadron suffered a nuclear interaction, associate hits of the closest daughter to the mother's track.
		// See MaterialEffects for more details
		// The same applies to a charged particle decay into another charged particle.
		// See ... for more details
		if (makeSimHits_)
		{
		    if ( !simTrack.noMother() && simTrack.mother().closestDaughterId() == simTrackId )
		    {
			simTrackId = simTrack.mother().id();
		    }
		    simHitFactory.createHitsOnDetLayer(particle,simTrackId,*(layer->getDetLayer()),layer->getMagneticFieldInTeslaZ(particle.vertex()),*output_simHits);
		}
	    }
	    */
	    
	    // kinematic cuts
	    //if()
	    //{
	    //break;
	    //}

	    // temporary: break after 25 ns
	    if(layerNavigator.particle().t() > 100)
	    {
		break;
	    }

	    // move to next layer
	    //layer = layerNavigator.moveToNextLayer(particle,*geometry_);
	    

	    LogDebug(MESSAGECATEGORY) << "--------------------------------"
				      << "\n-------------------------------";

	}

	/*
	// do decays, if needed
	if()
	{
	    //...;
	    // add decay vertex, add daughters, and break
	}
	*/

	LogDebug(MESSAGECATEGORY) << "################################"
				  << "\n###############################";
    }

    simEvent_.load(*output_simTracks,*TEMP);
    simEvent_.load(*output_simVertices);
    iEvent.put(std::move(output_simTracks));
    iEvent.put(std::move(output_simVertices));
    std::cout << output_simHits->size() << std::endl;
    iEvent.put(std::move(output_simHits),"TrackerHits");
}

DEFINE_FWK_MODULE(TrackerSimHitProducer);
