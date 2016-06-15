#ifndef FASTSIM_TRAJECTORY_H
#define FASTSIM_TRAJECTORY_H

#include "memory"

#include "TLorentzVector.h"

namespace fastsim
{
    class Layer;
    class BarrelLayer;
    class ForwardLayer;
    class Particle;
    class Trajectory
    {
    public:
	static std::unique_ptr<Trajectory> createTrajectory(const fastsim::Particle & particle,const double magneticFieldZ);
	virtual bool crosses(const BarrelLayer & layer) const = 0;
	virtual bool crossesMaterial(const ForwardLayer & layer) const = 0;
	const TLorentzVector & getPosition(){return position_;}
	const TLorentzVector & getMomentum(){return momentum_;}
	double nextCrossingTimeC(const Layer & layer) const;
	double nextCrossingTimeC(const ForwardLayer & layer) const;
	virtual double nextCrossingTimeC(const BarrelLayer & layer) const = 0;
	virtual void move(double deltaTC) = 0;
    protected:
	Trajectory(const fastsim::Particle & particle);
	// probably just storing const pointer to position and momentum is fine
	TLorentzVector position_;
	TLorentzVector momentum_;
	static const double speedOfLight_; // in cm / ns
	static const double epsiloneTimeC_;
	// consider storing particle mass, as it is needed frequently
    };
}

#endif
