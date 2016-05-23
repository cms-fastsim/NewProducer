#ifndef FASTSIM_BARRELLAYER_H
#define FASTSIM_BARRELLAYER_H

#include "FastSimulation/Geometry/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "MagneticField/Engine/interface/MagneticField.h"

namespace fastsim{

    class BarrelLayer : public Layer
    {
		public:
			~BarrelLayer(){};

			BarrelLayer(double radius, std::vector<double> limits, std::vector<double> thickness, const DetLayer *detLayer = 0) :
				Layer(radius, limits, thickness, detLayer) {}

			void setMagneticField(const MagneticField &pMF, double limit){
				field = TH1F("h", "h", 100, 0., limit);
				field.SetDirectory(0);

				for(int i = 0; i <= 101; i++){
      				field.SetBinContent(i, pMF.inTesla(GlobalPoint(position, 0., field.GetXaxis()->GetBinCenter(i))).z());
      			}
			}

			const double getRadius() const { return position; }

			const double materialMaxAbsZ() const { return maxMaterial; }

			const double getThickness(const math::XYZTLorentzVector& pos, const math::XYZTLorentzVector& mom) const {
				GlobalVector P(mom.Px(),mom.Py(),mom.Pz());
				GlobalVector N(pos.X()/pos.R(),pos.Y()/pos.R(),0.);
				return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(pos.Z()))) / fabs(P.dot(N)) * P.mag();
			}

			const double getMagneticFieldInTeslaZ(math::XYZTLorentzVector pos) const {
				return field.GetBinContent(field.GetXaxis()->FindBin(fabs(pos.z())));
			}

	        bool isForward() const override { return false;}
	};

}

#endif
