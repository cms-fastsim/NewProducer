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
	double nextCrossingTimeC(const Layer & layer) const;
	double nextCrossingTimeC(const ForwardLayer & layer) const;
	virtual double nextCrossingTimeC(const BarrelLayer & layer) const = 0;
	virtual void move(double deltaTC) = 0;
    protected:
	Trajectory(const RawParticle & particle);
	math::XYZTLorentzVectorD position_;
	math::XYZTLorentzVectorD momentum_;
	static const double speedOfLight_; // in cm / ns
	// consider storing particle mass, as it is needed frequently
    };
}

#endif
