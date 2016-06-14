#ifndef FASTSIM_BARRELLAYER_H
#define FASTSIM_BARRELLAYER_H

#include "FastSimulation/Layer/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "MagneticField/Engine/interface/MagneticField.h"

namespace fastsim{

    class BarrelLayer : public Layer
    {
		public:
			~BarrelLayer(){};

			BarrelLayer(double radius, const std::vector<double> & limits, const std::vector<double> & thickness, const DetLayer *detLayer) :
				Layer(radius, limits, thickness, detLayer) {}
                         
	BarrelLayer(BarrelLayer &&) = default;

	void setMagneticField(const MagneticField & magneticField, double limit)
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

			const double getThickness(const math::XYZTLorentzVector& pos, const math::XYZTLorentzVector& mom) const {
				GlobalVector P(mom.Px(),mom.Py(),mom.Pz());
				GlobalVector N(pos.X()/pos.R(),pos.Y()/pos.R(),0.);
				return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(pos.Z()))) / fabs(P.dot(N)) * P.mag();
			}

	const double getMagneticFieldZ(math::XYZTLorentzVector pos) const {
	    return Layer::getMagneticFieldZ(fabs(pos.z()));
	}

	bool isForward() const override 
	{ 
	    return false;
	}

	bool isOnSurface(const math::XYZTLorentzVector & position) const override
	{
	    return fabs(position_ - position.rho()) < epsilonDistance_;
	}
    };

}

#endif
