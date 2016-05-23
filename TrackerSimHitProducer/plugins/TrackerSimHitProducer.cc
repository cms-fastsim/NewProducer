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

// data formats
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "DataFormats/Common/interface/Handle.h"

// fastsim
#include "FastSimulation/Utilities/interface/RandomEngineAndDistribution.h"
#include "FastSimulation/Event/interface/FSimEvent.h"
#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/Geometry/interface/Layer.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"
#include "FastSimulation/TrackerSimHitProducer/interface/TrackerSimHitFactory.h"
#include "FastSimulation/Particle/interface/RawParticle.h"

class TrackerSimHitProducer : public edm::stream::EDProducer<> {
public:

    explicit TrackerSimHitProducer(const edm::ParameterSet&);
    ~TrackerSimHitProducer(){;}

private:

    virtual void produce(edm::Event&, const edm::EventSetup&) override;

    std::string geometryLabel_;
    edm::EDGetTokenT<edm::HepMCProduct> genParticlesToken_;
    FSimEvent simEvent_;
    //std::unique_ptr<MaterialEffects> materialEffects_;
};

//
// constructors and destructor
//
TrackerSimHitProducer::TrackerSimHitProducer(const edm::ParameterSet& iConfig)
    : geometryLabel_(iConfig.getParameter<std::string>("geometry"))
    , genParticlesToken_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("src"))) 
    , simEvent_(iConfig.getParameter<edm::ParameterSet>("ParticleFilter"))
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


// ------------ method called to produce the data  ------------
void
TrackerSimHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    std::unique_ptr<edm::PSimHitContainer> output_simHits(new edm::PSimHitContainer);
    std::unique_ptr<edm::SimTrackContainer> output_simTracks(new edm::SimTrackContainer);
    std::unique_ptr<edm::SimTrackContainer> TEMP(new edm::SimTrackContainer);
    std::unique_ptr<edm::SimVertexContainer> output_simVertices(new edm::SimVertexContainer);

    edm::ESHandle < HepPDT::ParticleDataTable > pdt;
    iSetup.getData(pdt);
    simEvent_.initializePdt(&(*pdt));

    edm::ESHandle<fastsim::Geometry>  geometry;
    //iSetup.get<fastsim::GeometryRecord>().get(geometryLabel_,geometry);

    edm::Handle<edm::HepMCProduct> genParticles;
    iEvent.getByToken(genParticlesToken_,genParticles);
    
    RandomEngineAndDistribution random(iEvent.streamID());

    simEvent_.clear();
    edm::EventID eventId = iEvent.id();
    simEvent_.fill(*genParticles->GetEvent(),eventId);
    
    fastsim::LayerNavigator layerNavigator;

    fastsim::TrackerSimHitFactory simHitFactory;
    
    for( unsigned simTrackIndex=0; simTrackIndex < simEvent_.nTracks(); ++simTrackIndex) 
    {
	const FSimTrack & simTrack = simEvent_.track(simTrackIndex);

	// define a particle
	RawParticle particle(simTrack.type(),simTrack.momentum());
	particle.setVertex(simTrack.vertex().position());
	// TODO: set the decay time

	// TODO: what is returned in case the particle decays before it hits a layer
	//fastsim::Layer * layer = layerNavigator.moveToNextLayer(particle,geometry,geometry.getMagField().inTesla(
	//		GlobalPoint(particle.vertex().position().x(), particle.vertex().position().y(), particle.vertex().position().z())).z()); // TODO: take magneticfield from 
	const fastsim::Layer * layer = layerNavigator.moveToNextLayer(particle,*geometry,3.8);// TODO: take magneticfield
	while(layer != 0)
	{
	    // does it really make sense to retrieve the magnetic field from the layer?
	    // is it much faster than getting it straight from the magnetic field?
	    //double magneticFieldZ = layer.getMagneticFieldInTeslaZ(particle.vertex().position());
	    double magneticFieldZ = 3.8;
	    
	    // do the material effects
	    // commented out for now, cause it depends on the layer interface, which changes
	    /*
	    if(materialEffects_)
	    {
		materialEffects_->interact(simEvent_,*layer,particle,simTrackIndex,&random);
	    }
	    */

	    // create simhits
	    if(layer->getDetLayer())
	    {
		// associate hits to simTrack
		int simTrackId = simTrackIndex;
		// If a hadron suffered a nuclear interaction, associate hits of the closest daughter to the mother's track.
		// See MaterialEffects for more details
		// The same applies to a charged particle decay into another charged particle.
		// See ... for more details
		if ( !simTrack.noMother() && simTrack.mother().closestDaughterId() == simTrackId )
		    simTrackId = simTrack.mother().id();
		simHitFactory.createHitsOnDetLayer(particle,simTrackId,*(layer->getDetLayer()),magneticFieldZ,*output_simHits);
	    }
	    
	    // kinematic cuts
	    //if()
	    //{
	    //break;
	    //}

	    // temporary: break after 25 ns
	    if(particle.t() > 25)
	    {
		break;
	    }

	    // move to next layer
	    layer = layerNavigator.moveToNextLayer(particle,*geometry,magneticFieldZ);
	    
	}

	/*
	// do decays, if needed
	if()
	{
	    //...;
	    // add decay vertex, add daughters, and break
	}
	*/

    }

    simEvent_.load(*output_simTracks,*TEMP);
    simEvent_.load(*output_simVertices);
    iEvent.put(std::move(output_simTracks));
    iEvent.put(std::move(output_simVertices));
    iEvent.put(std::move(output_simHits));
}

DEFINE_FWK_MODULE(TrackerSimHitProducer);
