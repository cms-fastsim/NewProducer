#ifndef FASTSIM_LAYERNAVIGATOR_H
#define FASTSIM_LAYERNAVIGATOR_H

#include "FastSimulation/Particle/interface/RawParticle.h"

namespace fastsim
{
    class Layer;
    class ForwardLayer;
    class BarrelLayer;
    class Geometry;
    class LayerNavigator
    {
    public:
	LayerNavigator(const int type,const XYZTLorentzVector & vertex,const XYZTLorentzVector& momentum,const Geometry & geometry);
	bool moveToNextLayer();
	const RawParticle & particle() const {return particle_;}
	const Layer & currentLayer() const {return *currentLayer_;}
    private:
	bool initialized_;
	const Geometry * const geometry_;
	RawParticle particle_;
	const BarrelLayer * nextBarrelLayer_;
	const ForwardLayer * nextForwardLayer_;
	const Layer * currentLayer_;
	static const std::string MESSAGECATEGORY;
    };
}

#endif
