#ifndef FASTSIM_FORWARDLAYER_H
#define FASTSIM_FORWARDLAYER_H

#include "FastSimulation/Layer/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "DataFormats/Math/interface/LorentzVector.h"

namespace fastsim{

    class ForwardLayer  : public Layer
    {
    public:
	~ForwardLayer(){};

	ForwardLayer(double z,const std::vector<double> & limits,const std::vector<double> & thickness, const DetLayer *detLayer) :
	    Layer(z, limits, thickness, detLayer) {}

	//ForwardLayer(ForwardLayer &&) = default;
	
	void setMagneticField(const MagneticField & magneticField, double limit) override
	{
	    field.reset(new TH1F("h", "h", 100, 0., limit));
	    field->SetDirectory(0);

	    for(int i = 0; i <= 101; i++)
	    {
		field->SetBinContent(i, magneticField.inTesla(GlobalPoint(field->GetXaxis()->GetBinCenter(i), 0., position_)).z());
	    }
	}

	const double getZ() const { return position_; }

	const double getMinMaterialR() const { return minMaterial; }
	const double getMaxMaterialR() const { return maxMaterial; }

	const double getThickness(const math::XYZTLorentzVector & position, const math::XYZTLorentzVector & momentum) const override
	{
	    if(!this->isOnSurface(position))
	    {
		return 0;
	    }
	    return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(position.Pt()))) / fabs(momentum.Pz()) * momentum.P();
	}

	const double getMagneticFieldZ(const math::XYZTLorentzVector & pos) const override
	{
	    return Layer::getMagneticFieldZ(pos.Pt());
	}
	
	bool isForward() const override {return true;}
	
	bool isOnSurface(const math::XYZTLorentzVector & position) const override
	{
	    return fabs(position_ - position.Z()) < epsilonDistance_;
	}

    };

}

#endif
