#ifndef FASTSIM_DECAYER_H
#define FASTSIM_DECAYER_H

#include <memory>
#include <vector>

namespace gen {
  class P8RndmEngine;
}

namespace CLHEP {
  class HepRandomEngine;
}

namespace Pythia8 {
  class Pythia;
}

typedef std::vector<Particle> DaughterParticleList;
typedef DaughterParticleList::const_iterator DaughterParticleIterator; 

namespace fastsim
{
    class Particle;
    class Decayer 
    {
    public:
	
	Decayer();
	~Decayer();
	void decay(const Particle & particle,std::vector<Particle> & secondaries,CLHEP::HepRandomEngine*);
	
    private:
	
	std::auto_ptr<Pythia8::Pythia>   p8_; 
	std::unique_ptr<gen::P8RndmEngine> p8RndmEngine_;
    };
}
#endif
