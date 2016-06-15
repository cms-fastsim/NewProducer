#ifndef FASTSIM_FORWARDLAYER_H
#define FASTSIM_FORWARDLAYER_H

#include "TLorentzVector.h"
#include "FastSimulation/Layer/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "MagneticField/Engine/interface/MagneticField.h"

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

	const double getThickness(const TLorentzVector & position, const TLorentzVector & momentum) const override
	{
	    if(!this->isOnSurface(position))
	    {
		return 0;
	    }
	    return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(position.Perp()))) / fabs(momentum.CosTheta());
	}

	const double getMagneticFieldZ(const TLorentzVector & pos) const override
	{
	    return Layer::getMagneticFieldZ(pos.Perp());
	}
	
	bool isForward() const override {return true;}
	
	bool isOnSurface(const TLorentzVector & position) const override
	{
	    return fabs(position_ - position.Z()) < epsilonDistance_;
	}

    };

}

#endif
