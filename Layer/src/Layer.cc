#include "FastSimulation/Layer/interface/Layer.h"

const double fastsim::Layer::epsilonDistance_ = 1.0e-7; // [cm] -> 1 nanometer = ridiculously small

std::ostream& fastsim::operator << (std::ostream& os , const Layer & layer)
{
    os << (layer.isForward() ? "ForwardLayer" : "BarrelLayer")
       << " index=" << layer.index_
       << (layer.isForward() ? " z=" : " radius=") << layer.position_;
    return os;
}

