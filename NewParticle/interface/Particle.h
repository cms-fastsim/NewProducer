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
		 double remainingProperLifeTime = -1.)
	    : pdgId_(pdgId)
	    , charge_(charge)
	    , position_(position)
	    , momentum_(momentum)
	    , isStable_(false)
	    , remainingProperLifeTime_(remainingProperLifeTime)
	    , simTrackIndex_(-1)
	    , simVertexIndex_(-1)
	    , genParticleIndex_(-1)
	{;}
	
	// setters
	void setSimTrackIndex(int index) {simTrackIndex_ = index;}
	void setSimVertexIndex(int index) {simVertexIndex_ = index;}
	void setGenParticleIndex(int index){genParticleIndex_ = index;}
	void setStable(bool isStable=true){isStable_ = isStable;}
	void setRemainingProperLifeTime(double remainingProperLifeTime){remainingProperLifeTime_ = remainingProperLifeTime;}

	// ordinary getters
	int pdgId() const {return pdgId_;}
        double charge() const {return charge_;}
	const math::XYZTLorentzVector & position() const {return position_;}
	const math::XYZTLorentzVector & momentum() const {return momentum_;}
	double remainingProperLifeTime() const {return remainingProperLifeTime_;}
	int simTrackIndex() const {return simTrackIndex_;}
	int simVertexIndex() const {return simTrackIndex_;}
	int genParticleIndex() const {return simTrackIndex_;}
	bool isStable() const {return isStable_;}

	// non-const getters
	math::XYZTLorentzVector & position() {return position_;}
	math::XYZTLorentzVector & momentum() {return momentum_;}

	friend std::ostream& operator << (std::ostream& os , const Particle & particle);

    private:
	const int pdgId_;
	const double charge_;
	math::XYZTLorentzVector position_;
	math::XYZTLorentzVector momentum_;
	bool isStable_;
	double remainingProperLifeTime_;
	int simTrackIndex_;
	int simVertexIndex_;
	int genParticleIndex_;
    };

    std::ostream& operator << (std::ostream& os , const Particle & particle);

}

#endif
