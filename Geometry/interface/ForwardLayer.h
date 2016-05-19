#ifndef FASTSIM_FORWARDLAYER_H
#define FASTSIM_FORWARDLAYER_H

#include "FastSimulation/Geometry/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "MagneticField/Engine/interface/MagneticField.h"

class DetLayer;

namespace fastsim{

    class ForwardLayer  : public Layer
    {
		public:
			~ForwardLayer(){};

			ForwardLayer(double z, DetLayer *posDiskDetLayer = 0, DetLayer *negDiskDetLayer = 0) :
				Layer(),
				z(z),
				posDiskDetLayer(posDiskDetLayer),
				negDiskDetLayer(negDiskDetLayer) {}

			ForwardLayer(double z, std::vector<double> limits, std::vector<double> thickness, const DetLayer *posDiskDetLayer = 0, const DetLayer *negDiskDetLayer = 0) :
				Layer(limits, thickness),
				z(z),
				posDiskDetLayer(posDiskDetLayer),
				negDiskDetLayer(negDiskDetLayer) {}

			void setMagneticField(const MagneticField &pMF, double limit){
				field = TH1F("h", "h", 100, 0., limit);
				field.SetDirectory(0);

				for(int i = 0; i <= 101; i++){
      				field.SetBinContent(i, pMF.inTesla(GlobalPoint(field.GetXaxis()->GetBinCenter(i), 0., z)).z());
      			}
			}

			const double getZ() const { return z; }
			const DetLayer* getDetLayer(double z = 0) const { return (z < 0 ? negDiskDetLayer : posDiskDetLayer); }

			const double minMaterialR() const { return minMaterial; }
			const double maxMaterialR() const { return maxMaterial; }

			const double getThickness(const math::XYZTLorentzVector& coord, const math::XYZTLorentzVector& mom) const {
				GlobalVector P(mom.Px(),mom.Py(),mom.Pz());
				GlobalVector N(0.,0.,1.);
				return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(coord.R()))) / fabs(P.dot(N)) * P.mag();
			}

    	private:
    		double z;
			const DetLayer *posDiskDetLayer = 0;
			const DetLayer *negDiskDetLayer = 0;
	
    };

}

#endif
