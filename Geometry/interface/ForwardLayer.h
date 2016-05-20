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

			ForwardLayer(double z, std::vector<double> limits, std::vector<double> thickness, const DetLayer *detLayer = 0) :
				Layer(z, limits, thickness, detLayer) {}

			void setMagneticField(const MagneticField &pMF, double limit){
				field = TH1F("h", "h", 100, 0., limit);
				field.SetDirectory(0);

				for(int i = 0; i <= 101; i++){
      				field.SetBinContent(i, pMF.inTesla(GlobalPoint(field.GetXaxis()->GetBinCenter(i), 0., position)).z());
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
			bool isForward() const override {return true;}

    };

}

#endif
