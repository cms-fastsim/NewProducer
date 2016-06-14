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
		 double properLifeTime)
	    : pdgId_(pdgId)
	    , charge_(charge)
	    , position_(position)
	    , momentum_(momentum)
	    , properLifeTime_(properLifeTime)
	    {;}
	
	int pdgId() const {return pdgId_;}
        double charge() const {return charge_;}
	const math::XYZTLorentzVector & position() const {return position_;}
	const math::XYZTLorentzVector & momentum() const {return momentum_;}
	double properLifeTime() const {return properLifeTime_;}
	double properTimeLeftBeforeDecay() const {return properTimeLeftBeforeDecay_;}
	
	void setPosition(const math::XYZTLorentzVector & position)
	{
	    position_ = position;
	}

	void setMomentum(const math::XYZTLorentzVector & momentum)
	{
	    momentum_ = momentum;
	}

	void advanceProperTime(double time)
	{
	    properTimeLeftBeforeDecay_ -= time;
	}

	friend std::ostream& operator << (std::ostream& os , const Particle & particle);

    private:
	const int pdgId_;
	const double charge_;
	math::XYZTLorentzVector position_;
	math::XYZTLorentzVector momentum_;
	const double properLifeTime_;
	double properTimeLeftBeforeDecay_;
    };

    std::ostream& operator << (std::ostream& os , const Particle & particle);

}

#endif
