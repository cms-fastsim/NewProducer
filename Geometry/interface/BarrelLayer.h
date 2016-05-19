#ifndef FASTSIM_BARRELLAYER_H
#define FASTSIM_BARRELLAYER_H

#include "FastSimulation/Geometry/interface/TrackerLayer.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Engine/interface/MagneticField.h"

class DetLayer;

namespace fastsim{

    class BarrelLayer : public TrackerLayer
    {
		public:
			~BarrelLayer(){};

			BarrelLayer(double radius, DetLayer *barrelDetLayer = 0) :
				TrackerLayer(),
				radius(radius),
				barrelDetLayer(barrelDetLayer) {}


			BarrelLayer(double radius, std::vector<double> limits, std::vector<double> thickness, const DetLayer *barrelDetLayer = 0) :
				TrackerLayer(limits, thickness),
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

    	private:
    		double radius;
			const DetLayer *barrelDetLayer = 0;
	
    };

}

#endif
