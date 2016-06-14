#include "FastSimulation/InteractionModel/interface/SimpleLayerHitProducer.h"
#include "FastSimulation/NewParticle/interface/Particle.h"
#include "FastSimulation/Layer/interface/Layer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "FWCore/Framework/interface/ProducerBase.h"

fastsim::SimpleLayerHitProducer::SimpleLayerHitProducer()
    : fastsim::InteractionModel("SimpleLayerHitProducer")
    , layerHits_(new std::vector<math::XYZTLorentzVector>())
{
}

void fastsim::SimpleLayerHitProducer::registerProducts(edm::ProducerBase & producer) const
{
    producer.produces<std::vector<math::XYZTLorentzVector> >();
}

void fastsim::SimpleLayerHitProducer::interact(Particle & particle,const fastsim::Layer & layer,FSimEvent & simEvent)
{
    if(layer.isOnSurface(particle.position()))
    {
	layerHits_->push_back(particle.position());
    }
}

void fastsim::SimpleLayerHitProducer::storeProducts(edm::Event & iEvent)
{
    iEvent.put(std::move(layerHits_));
}
