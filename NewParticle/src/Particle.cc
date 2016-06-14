#include "FastSimulation/NewParticle/interface/Particle.h"

std::ostream& fastsim::operator << (std::ostream& os , const fastsim::Particle & particle)
{
    os << "fastsim::Particle "
       << " pdgId=" << particle.pdgId_
       << " position=" << particle.position_
       << " momentum=" << particle.momentum_
       << " properLifeTime=" << particle.properLifeTime_
       << " properTimeLeftBeforeDecay" << particle.properTimeLeftBeforeDecay_;
    return os;
}
