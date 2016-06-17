#ifndef FASTSIM_BARRELLAYER_H
#define FASTSIM_BARRELLAYER_H

#include "FastSimulation/Layer/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Engine/interface/MagneticField.h"

namespace fastsim{

    class BarrelLayer : public Layer
    {
    public:
	~BarrelLayer(){};
	
	BarrelLayer(double radius, const std::vector<double> & limits, const std::vector<double> & thickness, const DetLayer *detLayer) :
	    Layer(radius, limits, thickness, detLayer) {}
	
	BarrelLayer(BarrelLayer &&) = default;
	
	void setMagneticField(const MagneticField & magneticField, double limit) override
	{
	    field.reset(new TH1F("h", "h", 100, 0., limit));
	    field->SetDirectory(0);
	    for(int i = 0; i <= 101; i++)
	    {
		field->SetBinContent(i, magneticField.inTesla(GlobalPoint(position_, 0., field->GetXaxis()->GetBinCenter(i))).z());
	    }
	}
	
	const double getRadius() const { return position_; }
	
	const double materialMaxAbsZ() const { return maxMaterial; }
	
	const double getThickness(const math::XYZTLorentzVector & position, const math::XYZTLorentzVector & momentum) const override
	{
	    if(!this->isOnSurface(position))
	    {
		return 0;
	    }
	    double fabsCosTheta = fabs(momentum.Vect().Dot(position.Vect())) / momentum.Rho() / position.Rho();
	    return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(position.Z()))) / fabsCosTheta;
	}

	const double getMagneticFieldZ(const math::XYZTLorentzVector & pos) const override
	{
	    return Layer::getMagneticFieldZ(fabs(pos.Z()));
	}

	bool isForward() const override 
	{ 
	    return false;
	}

	bool isOnSurface(const math::XYZTLorentzVector & position) const override
	{
	    return fabs(position_ - position.Pt()) < epsilonDistance_;
	}
    };

}

#endif
