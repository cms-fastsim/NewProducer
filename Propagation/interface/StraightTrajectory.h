#ifndef FASTSIM_STRAIGHTTRAJECTORY_H
#define FASTSIM_STRAIGHTTRAJECTORY_H

#include "FastSimulation/Propagation/interface/Trajectory.h"

class RawParticle;

namespace fastsim
{
    class Layer;
    class BarrelLayer;
    class ForwardLayer;
    class StraightTrajectory : public Trajectory
    {
    public:
	StraightTrajectory(const RawParticle & particle){;}
	bool crosses(const BarrelLayer & layer) const override {return true;}
	bool crossesMaterial(const ForwardLayer & layer) const override {return true;}
	double nextCrossingTime(const Layer & layer) const override {return 0;}
	void move(double deltaT) override {;}
    };
}

#endif
