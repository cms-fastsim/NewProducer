#ifndef FASTSIM_FORWARDLAYER_H
#define FASTSIM_FORWARDLAYER_H

#include "FastSimulation/Geometry/interface/Layer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Engine/interface/MagneticField.h"

class DetLayer;

namespace fastsim{

    class ForwardLayer  : public Layer
    {
		public:
			~ForwardLayer(){};

			ForwardLayer(double z, DetLayer *posDiskDetLayer = 0, DetLayer *negDiskDetLayer = 0) :
				TrackerLayer(),
				z(z),
				posDiskDetLayer(posDiskDetLayer),
				negDiskDetLayer(negDiskDetLayer) {}

			ForwardLayer(double z, std::vector<double> limits, std::vector<double> thickness, const DetLayer *posDiskDetLayer = 0, const DetLayer *negDiskDetLayer = 0) :
				TrackerLayer(limits, thickness),
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

    	private:
    		double z;
			const DetLayer *posDiskDetLayer = 0;
			const DetLayer *negDiskDetLayer = 0;
	
    };

}

#endif
