#include "FastSimulation/Propagation/interface/Trajectory.h"
#include "FastSimulation/Propagation/interface/StraightTrajectory.h"

std::unique_ptr<fastsim::Trajectory> fastsim::Trajectory::createTrajectory(const RawParticle & particle,double magneticFieldZ)
{
    return std::unique_ptr<fastsim::Trajectory>(new fastsim::StraightTrajectory(particle));
}

