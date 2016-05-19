#ifndef FASTSIM_LAYER_H
#define FASTSIM_LAYER_H

#include "FWCore/Utilities/interface/Exception.h"

#include <vector>
#include <TH1.h>

class MagneticField;
class DetLayer;

namespace fastsim{

    class Layer
    {
		public:
			~Layer(){};

			/// constructor from private members
			Layer(double coord = 0){
				// Prepare histograms
				thickness = TH1F("h", "h", 100, 0., coord);
				thickness.SetDirectory(0);	
				field = TH1F("h", "h", 100, 0., coord);
				field.SetDirectory(0);	
			}

			Layer(std::vector<double> li, std::vector<double> th) : Layer(li.back()){				
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
			}

			// Setters
			void setNuclearInteractionThicknessFactor(double fac){ nuclearInteractionThicknessFactor = fac; }
			virtual void setMagneticField(const MagneticField &pMF, double limit) = 0;

			// Getters
			double getThickness(double coord) const { return thickness.GetBinContent(thickness.GetXaxis()->FindBin(fabs(coord))); }
			double getNuclearInteractionThicknessFactor() const {return nuclearInteractionThicknessFactor; }
			double getMagneticFieldInTeslaZ(double coord) const{ return field.GetBinContent(field.GetXaxis()->FindBin(fabs(coord))); }

	                DetLayer * getDetLayer(){return 0;}

    	protected:
			double nuclearInteractionThicknessFactor = 1.;

			TH1F thickness;
			TH1F field;

			double minMaterial=0, maxMaterial=0;
	

    };

}

#endif
