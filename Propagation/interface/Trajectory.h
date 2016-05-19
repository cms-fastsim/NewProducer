#ifndef FASTSIM_TRAJECTORY_H
#define FASTSIM_TRAJECTORY_H

#include "memory"
#include "DataFormats/Math/interface/LorentzVector.h"
class RawParticle;

namespace fastsim
{
    class Layer;
    class BarrelLayer;
    class ForwardLayer;
    class Trajectory
    {
    public:
	static std::unique_ptr<Trajectory> createTrajectory(const RawParticle & particle,double magneticFieldZ);
	virtual bool crosses(const BarrelLayer & layer) const = 0;
	virtual bool crossesMaterial(const ForwardLayer & layer) const = 0;
	math::XYZTLorentzVectorD & getPosition(){return position_;}
	math::XYZTLorentzVectorD & getMomentum(){return momentum_;}
	virtual double nextCrossingTime(const Layer & layer) const = 0;
	virtual void move(double deltaT) = 0;
    private:
	math::XYZTLorentzVectorD position_;
	math::XYZTLorentzVectorD momentum_;
    };
}

#endif
