#include "FastSimulation/Propagation/interface/StraightTrajectory.h"
#include "FastSimulation/Propagation/interface/HelixTrajectory.h"
#include "FastSimulation/Geometry/interface/ForwardLayer.h"
#include "FastSimulation/Geometry/interface/BarrelLayer.h"
#include "FastSimulation/Particle/interface/RawParticle.h"

const double fastsim::Trajectory::speedOfLight_ = 29.98; // cm per ns

fastsim::Trajectory::Trajectory(const RawParticle & particle)
{
    position_ = particle.vertex();
    momentum_ = particle;
}


std::unique_ptr<fastsim::Trajectory> fastsim::Trajectory::createTrajectory(const RawParticle & particle,double magneticFieldZ)
{
    if(particle.charge() == 0. || magneticFieldZ == 0.)
    {
	return std::unique_ptr<fastsim::Trajectory>(new fastsim::StraightTrajectory(particle));
    }
    else
    {
	return std::unique_ptr<fastsim::Trajectory>(new fastsim::HelixTrajectory(particle,magneticFieldZ));
    }
}


double fastsim::Trajectory::nextCrossingTimeC(const fastsim::Layer & layer) const
{
    if(layer.isForward())
    {
	return this->nextCrossingTimeC(static_cast<const fastsim::ForwardLayer &>(layer));
    }
    else
    {
	return this->nextCrossingTimeC(static_cast<const fastsim::BarrelLayer &>(layer));
    }
}


double fastsim::Trajectory::nextCrossingTimeC(const fastsim::ForwardLayer & layer) const
{
    // t = (z - z_0) / v_z
    // substitute: v_z = p_z / E * c  ( note: extra * c absorbed in p_z units)
    // => t*c = (z - z_0) / p_z * E
    double deltaTimeC = (layer.getZ() - position_.z()) / momentum_.pz() * momentum_.E();
    return deltaTimeC > 0. ? deltaTimeC : -1.;
}


