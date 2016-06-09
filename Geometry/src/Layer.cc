#include "FastSimulation/Geometry/interface/Layer.h"

std::ostream& fastsim::operator << (std::ostream& os , const Layer & layer)
{
    os << (layer.isForward() ? "ForwardLayer" : "BarrelLayer")
       << " index=" << layer.index_
       << (layer.isForward() ? " z=" : " radius=") << layer.position;
    return os;
}
