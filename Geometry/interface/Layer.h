#ifndef FASTSIM_LAYER_H
#define FASTSIM_LAYER_H

class DetLayer;

namespace fastsim
{
    class Layer
    {
    public:
	Layer(){};
	DetLayer * detLayer(){return 0;}
    };
}
#endif
