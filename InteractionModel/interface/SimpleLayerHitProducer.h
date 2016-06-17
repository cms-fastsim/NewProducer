#ifndef FASTSIM_SIMPLELAYERHITPRODUCER_H
#define FASTSIM_SIMPLELAYERHITPRODUCER_H

#include "memory"
#include "vector"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "FastSimulation/InteractionModel/interface/InteractionModel.h"

namespace edm 
{
    class ProducerBase;
    class Event;
}

namespace fastsim
{
    class Particle;
    class Layer;
    class SimpleLayerHitProducer : public InteractionModel
    {
    public:
	SimpleLayerHitProducer();
	void interact(Particle & particle,const Layer & layer,std::vector<Particle> & secondaries,const RandomEngineAndDistribution & random) override;
	void registerProducts(edm::ProducerBase & producer) const override;
	void storeProducts(edm::Event & iEvent) override;
    private:
	std::unique_ptr<std::vector<math::XYZTLorentzVector> > layerHits_;
    };
}

#endif
