#ifndef FASTSIM_FORWARDLAYER_H
#define FASTSIM_FORWARDLAYER_H

#include "FastSimulation/Geometry/interface/Layer.h"
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
	
	void setMagneticField(const MagneticField & magneticField, double limit)
	{
	    field.reset(new TH1F("h", "h", 100, 0., limit));
	    field->SetDirectory(0);

	    for(int i = 0; i <= 101; i++)
	    {
		field->SetBinContent(i, magneticField.inTesla(GlobalPoint(field->GetXaxis()->GetBinCenter(i), 0., position)).z());
	    }
	}

			const double getZ() const { return position; }

			const double getMinMaterialR() const { return minMaterial; }
			const double getMaxMaterialR() const { return maxMaterial; }

			const double getThickness(const math::XYZTLorentzVector& pos, const math::XYZTLorentzVector& mom) const {
				GlobalVector P(mom.Px(),mom.Py(),mom.Pz());
				GlobalVector N(0.,0.,1.);
				return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(pos.R()))) / fabs(P.dot(N)) * P.mag();

			}

	const double getMagneticFieldZ(math::XYZTLorentzVector pos) const 
	{
	    return Layer::getMagneticFieldZ(pos.rho());
	}
	
			bool isForward() const override {return true;}

    };

}

#endif
