#ifndef FASTSIM_PARTICLEFILTER
#define FASTSIM_PARTICLEFILTER

namespace fastsim
{
    class ParticleFilter
    {
    public:
	ParticleFilter(){;}
	bool accepts(const Particle & particle) {return true;}
    };
}

#endif
