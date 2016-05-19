#ifndef FASTSIM_BARRELLAYER_H
#define FASTSIM_BARRELLAYER_H

#include "FastSimulation/Geometry/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "MagneticField/Engine/interface/MagneticField.h"

class DetLayer;

namespace fastsim{

    class BarrelLayer : public Layer
    {
		public:
			~BarrelLayer(){};

			BarrelLayer(double radius, DetLayer *barrelDetLayer = 0) :
				Layer(),
				radius(radius),
				barrelDetLayer(barrelDetLayer) {}


			BarrelLayer(double radius, std::vector<double> limits, std::vector<double> thickness, const DetLayer *barrelDetLayer = 0) :
				Layer(limits, thickness),
				radius(radius),
				barrelDetLayer(barrelDetLayer) {}

			void setMagneticField(const MagneticField &pMF, double limit){
				field = TH1F("h", "h", 100, 0., limit);
				field.SetDirectory(0);

				for(int i = 0; i <= 101; i++){
      				field.SetBinContent(i, pMF.inTesla(GlobalPoint(radius, 0., field.GetXaxis()->GetBinCenter(i))).z());
      			}
			}

			const double getRadius() const { return radius; }
			const DetLayer* getDetLayer(double z = 0) const { return barrelDetLayer; }

			const double materialMaxAbsZ() const { return maxMaterial; }

			const double getThickness(const math::XYZTLorentzVector& coord, const math::XYZTLorentzVector& mom) const {
				GlobalVector P(mom.Px(),mom.Py(),mom.Pz());
				GlobalVector N(coord.X()/coord.R(),coord.Y()/coord.R(),0.);
				return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(coord.Z()))) / fabs(P.dot(N)) * P.mag();
			}


    	private:
    		double radius;
			const DetLayer *barrelDetLayer = 0;
	
    };

}

#endif
