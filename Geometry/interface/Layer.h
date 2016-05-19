#ifndef FASTSIM_LAYER_H
#define FASTSIM_LAYER_H

#include "FWCore/Utilities/interface/Exception.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include <vector>
#include <TH1.h>

class DetLayer;
class MagneticField;
class DetLayer;

namespace fastsim{

    class Layer
    {
		public:
			~Layer(){};

			Layer(double pos, std::vector<double> li, std::vector<double> th, const DetLayer* dL) :
				position(pos),
				detLayer(dL){				
				// Check compatibility of thickness and limits
				if(li.size() < 1 || th.size() != li.size() - 1){
				    throw cms::Exception("FastSimulation/TrackerInteractionGeometry ") 
				        << " WARNING: Dimensions of layer th not defined properly! There are " 
				        << li.size() << " limits (=N [>=2]) and "
				        << th.size() << " thicknesses (=N-1 [>=1])"
				        << std::endl;
				}
				// Check if limits are in increasing order
				for(std::vector<double>::iterator it = li.begin()+1; it != li.end(); it++){
				    if(*(it-1) > *it)
				          throw cms::Exception("FastSimulation/TrackerInteractionGeometry ")
				              << "WARNING with limits of thickness! Ranges must be in increasing order: "
				              << *(it-1) << ">" << *it
				              << std::endl;
				}

				thickness = TH1F("h", "h", li.size()-1, &li[0]);
				thickness.SetDirectory(0);

				for(unsigned i = 1; i < li.size(); ++i){
					thickness.SetBinContent(i, thickness[i-1]);
				}

				for(unsigned i = 1; i < li.size(); ++i){
					if(thickness.Integral(0, i) < 1E-10) minMaterial = thickness.GetBinLowEdge(i);
					if(thickness.Integral(i, li.size()) < 1E-10) maxMaterial = thickness.GetBinLowEdge(i+1);
				}

				field = TH1F("h", "h", 100, 0., li.back());
				field.SetDirectory(0);
			}

			// Setters
			void setNuclearInteractionThicknessFactor(double fac){ nuclearInteractionThicknessFactor = fac; }
			virtual void setMagneticField(const MagneticField &pMF, double limit) = 0;

			// Getters
			virtual const double getThickness(const math::XYZTLorentzVector& pos, const math::XYZTLorentzVector& mom) const = 0;
			const double getNuclearInteractionThicknessFactor() const {return nuclearInteractionThicknessFactor; }
			const DetLayer* getDetLayer(double z = 0) const { return detLayer; }
			const double getMagneticFieldInTeslaZ(double pos) const{ return field.GetBinContent(field.GetXaxis()->FindBin(fabs(pos))); }

    	protected:
    		// position is either r (barrelLayer) or z (forwardLayer)
    		double position;
			double nuclearInteractionThicknessFactor = 1.;

			TH1F thickness;
			TH1F field;

			const DetLayer *detLayer = 0;

			double minMaterial = 0, maxMaterial = 0;
	

    };

}

#endif
