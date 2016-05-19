// -*- C++ -*-
//
// Package:    FastSimulation/TrackerSimHitProducer
// Class:      TrackerSimHitProducer
// 
/**\class TrackerSimHitProducer TrackerSimHitProducer.cc FastSimulation/TrackerSimHitProducer/plugins/TrackerSimHitProducer.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Lukas Vanelderen
//         Created:  Wed, 18 May 2016 12:53:13 GMT
//
//


// system include files
#include <memory>
#include <string>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Framework/interface/ESHandle.h"

// data formats
#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"

// fastsim includes
#include "FastSimulation/Event/interface/FSimEvent.h"
#include "FastSimulation/Geometry/interface/GeometryRecord.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"

//
// class declaration
//

class TrackerSimHitProducer : public edm::stream::EDProducer<> {
public:

    explicit TrackerSimHitProducer(const edm::ParameterSet&);
    ~TrackerSimHitProducer(){;}

private:

    virtual void produce(edm::Event&, const edm::EventSetup&) override;

    std::string geometryLabel_;
    edm::EDGetTokenT<edm::HepMCProduct> genParticlesToken_;
    FSimEvent simEvent_
};

//
// constructors and destructor
//
TrackerSimHitProducer::TrackerSimHitProducer(const edm::ParameterSet& iConfig)
    : geometryLabel_(iConfig.get("geometry"))
    , genParticlesToken_(consumes<edm::HepMCProduct>(iConfig.getParameter<edm::InputTag>("src"))) 
    , simEvent_(iConfig.getParameter<edm::ParameterSet>("ParticleFilter"));
{
    produces<edm::SimTrackContainer>();
    produces<edm::SimVertexContainer>();
    produces<edm::PSimHitContainer>("TrackerHits");
}


// ------------ method called to produce the data  ------------
void
TrackerSimHitProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    std::unique_ptr<edm::PSimHitContainer> output_simHits(new edm::PSimHitContainer);
    std::unique_ptr<edm::SimTrackContainer> output_simTracks(new edm::PSimHitContainer);
    std::unique_ptr<edm::SimVertexContainer> output_simVertices(new edm::PSimHitContainer);

    simEvent_.clear();

    edm::ESHandle < HepPDT::ParticleDataTable > pdt;
    es.getData(pdt);
    simEvent_.initializePdt(&(*pdt));

    edm::ESHandle<fastsim::Geometry>  geometry;
    es.get<fastsim::GeometryRecord>().get(geometryLabel_,geometry);

    Handle<HepMCProduct> genParticles;
    iEvent.getByToken(genParticlesToken,genParticles);
    
    RandomEngineAndDistribution random(iEvent.streamID());

    simEvent.fill(*genParticles->GetEvent());
    
    fastsim::LayerNavigator layerNavigator;
    
    for( unsigned simTrackIndex=0; simTrackIndex < simEvent_.nTracks(); ++simTrackIndex) 
    {
	RawParticle particle(simEvent_.track(simTrackIndex));
	// TODO: what is returned in case the particle decays before it hits a layer
	fastsim::Layer * layer = layerNavigator.moveToNextLayer(particle,geometry,geometry.magneticField(position).z()); // TODO: take magneticfield from 
	while(layer != 0)
	{
	    // does it really make sense to retrieve the magnetic field from the layer?
	    // is it much faster than getting it straight from the magnetic field?
	    double magneticFieldZ = layer.getMagneticField(particle.position());

	    // do decays, if needed
	    if()
	    {
		//...;
		// add decay vertex, add daughters, and break
		break;
	    }

	    // do the material effects
	    if(materialEffects)
	    {
		double thickness = layer.getThickNess(particle.position(),particle.momentum());
		//...
		// add secondaries, add endvertex
		// if energy got too low, stop
		if()
		{
		    break;
		}
	    }

	    // create simhits
	    if()
	    {
		TrackerSimhitFactor::create(/*....*/,output_simHits);
	    }
	    

	    // move to next layer
	    layer = layerNavigator.moveToNextLayer(particle,geometry,magneticFieldZ);
	    
	}
    }

    simEvent_->load(*output_simTracks);
    simEvent_->load(*output_simVertices);
    iEvent.put(output_simTracks.move());
    iEvent.put(output_simVertices.move());
    iEvent.put(output_simHits.move());
}

DEFINE_FWK_MODULE(TrackerSimHitProducer);
