#include "FastSimulation/InteractionModel/interface/SimpleLayerHitProducer.h"
#include "FastSimulation/NewParticle/interface/Particle.h"
#include "FastSimulation/Layer/interface/Layer.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "FWCore/Framework/interface/ProducerBase.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

fastsim::SimpleLayerHitProducer::SimpleLayerHitProducer()
    : fastsim::InteractionModel("SimpleLayerHitProducer")
    , layerHits_(new std::vector<math::XYZTLorentzVector>())
{
}

void fastsim::SimpleLayerHitProducer::registerProducts(edm::ProducerBase & producer) const
{
    LogDebug("FastSimulation") << "      registering products" << std::endl;
    producer.produces<std::vector<math::XYZTLorentzVector> >();
}

void fastsim::SimpleLayerHitProducer::interact(Particle & particle,
					       const fastsim::Layer & layer,
					       std::vector<Particle> & secondaries,
					       const RandomEngineAndDistribution & random)
{
    if(layer.isOnSurface(particle.position()))
    {
	layerHits_->push_back(math::XYZTLorentzVector(particle.position().X(),particle.position().Y(),particle.position().Z(),particle.position().T()));
    }
}

void fastsim::SimpleLayerHitProducer::storeProducts(edm::Event & iEvent)
{
    LogDebug("FastSimulation") << "      storing products" << std::endl;
    iEvent.put(std::move(layerHits_));
}
