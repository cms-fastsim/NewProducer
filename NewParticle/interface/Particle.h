#ifndef FASTSIM_PARTCILE_H
#define FASTSIM_PARTICLE_H

#include "DataFormats/Math/interface/LorentzVector.h"

namespace fastsim
{
    class Particle
    {
    public:
	Particle(int pdgId,
		 double charge,
		 const math::XYZTLorentzVector & position,
		 const math::XYZTLorentzVector & momentum,
		 double properLifeTime = -1.)
	    : pdgId_(pdgId)
	    , charge_(charge)
	    , position_(position)
	    , momentum_(momentum)
	    , properLifeTime_(properLifeTime)
	{;}
	
	// ordinary getters
	int pdgId() const {return pdgId_;}
        double charge() const {return charge_;}
	const math::XYZTLorentzVector & position() const {return position_;}
	const math::XYZTLorentzVector & momentum() const {return momentum_;}
	double properLifeTime() const {return properLifeTime_;}
	double remainingProperLifeTime() const {return remainingProperLifeTime_;}
	
	// non-const getters
	math::XYZTLorentzVector & position() {return position_;}
	math::XYZTLorentzVector & momentum() {return momentum_;}

	// modifiers
	void reduceRemainingProperLifeTime(double reduction)
	{
	    remainingProperLifeTime_ = std::max(0., remainingProperLifeTime_- reduction);
	}

	friend std::ostream& operator << (std::ostream& os , const Particle & particle);

    private:
	const int pdgId_;
	const double charge_;
	math::XYZTLorentzVector position_;
	math::XYZTLorentzVector momentum_;
	const double properLifeTime_;
	double remainingProperLifeTime_;
    };

    std::ostream& operator << (std::ostream& os , const Particle & particle);

}

#endif
