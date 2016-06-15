#ifndef FASTSIM_PARTCILE_H
#define FASTSIM_PARTICLE_H

#include "TLorentzVector.h"

namespace fastsim
{
    class Particle
    {
    public:
	Particle(int pdgId,
		 double charge,
		 const TLorentzVector & position,
		 const TLorentzVector & momentum,
		 double properLifeTime = -1.)
	    : pdgId_(pdgId)
	    , charge_(charge)
	    , position_(std::move(position))
	    , momentum_(std::move(momentum))
	    , properLifeTime_(properLifeTime)
	    {;}

	int pdgId() const {return pdgId_;}
        double charge() const {return charge_;}
	const TLorentzVector & position() const {return position_;}
	const TLorentzVector & momentum() const {return momentum_;}
	double properLifeTime() const {return properLifeTime_;}
	double remainingProperLifeTime() const {return remainingProperLifeTime_;}
	
	TLorentzVector & position() {return position_;}
	TLorentzVector & momentum() {return momentum_;}

	/*
	void advanceProperTime(double time)
	{
	    properLifeTimeLeft_ -= time;
	}
	*/

	friend std::ostream& operator << (std::ostream& os , const Particle & particle);

    private:
	const int pdgId_;
	const double charge_;
	TLorentzVector position_;
	TLorentzVector momentum_;
	const double properLifeTime_;
	double remainingProperLifeTime_;
    };

    std::ostream& operator << (std::ostream& os , const Particle & particle);

}

#endif
