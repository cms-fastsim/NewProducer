#ifndef FASTSIM_LAYERNAVIGATOR_H
#define FASTSIM_LAYERNAVIGATOR_H

class RawParticle;

namespace fastsim
{
    class Layer;
    class LayerNavigator
    {
	Layer * moveToNextLayer(RawParticle * particle,const Geometry & geometry,double magneticFieldZ) const;
    };
}

#endif
