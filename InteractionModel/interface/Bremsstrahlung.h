#ifndef FASTSIM_BREMSSTRAHLUNG_H
#define FASTSIM_BREHMSSTRAHLUNG_H


#include "FastSimulation/InteractionModel/interface/InteractionModel.h"
#include "DataFormats/Math/interface/LorentzVector.h"

namespace edm
{
    class ParameterSet;
}

namespace fastsim
{
    class Bremsstrahlung : public InteractionModel
    {
    public:
	Bremsstrahlung(const edm::ParameterSet & cfg);
	void interact(Particle & particle,const Layer & layer,std::vector<Particle> & secondaries,const RandomEngineAndDistribution & random);
    private:
	math::XYZTLorentzVector brem(Particle & particle , double xmin,const RandomEngineAndDistribution & random) const;
	double gbteth(const double ener,
		      const double partm,
		      const double efrac,
		      const RandomEngineAndDistribution & random) const ;
	// why do we have a dedicated implementation here? check it, probably it can go...
	unsigned int poisson(double ymu, const RandomEngineAndDistribution & random);
	double minPhotonEnergy_;
	double minPhotonEnergyFraction_;
    };
}

#endif
