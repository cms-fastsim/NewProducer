#ifndef FASTSIM_LAYER_H
#define FASTSIM_LAYER_H

#include "FWCore/Utilities/interface/Exception.h"
#include "DataFormats/Math/interface/LorentzVector.h"

#include <memory>
#include <vector>
#include <TH1.h>

class DetLayer;
class MagneticField;
class DetLayer;

namespace fastsim{

    class InteractionModel;
    class Layer
    {
    public:
	virtual ~Layer(){};

	Layer(double pos, const std::vector<double> & li, const std::vector<double> & th, const DetLayer* dL) 
	    : position_(pos)
	    , magneticFieldZ_(0)
	    , nuclearInteractionThicknessFactor(0)
	    , detLayer(dL)
	    , minMaterial(0)
	    , maxMaterial(0)
	    , index_(0)
	    {
		// Check compatibility of thickness and limits
		if(li.size() < 1 || th.size() != li.size() - 1){
		    throw cms::Exception("FastSimulation/TrackerInteractionGeometry ") 
			<< "Length of layer thickness not defined properly! There are " 
			<< li.size() << " limits (=N [>=2]) and "
			<< th.size() << " thicknesses (=N-1 [>=1]) "
			<< li.back()
			<< std::endl;
		}
		// Check if limits are in increasing order
		for(std::vector<double>::const_iterator it = li.begin()+1; it != li.end(); it++){
		    if(*(it-1) > *it)
			throw cms::Exception("FastSimulation/TrackerInteractionGeometry ")
			    << "with limits of thickness! Ranges must be in increasing order: "
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
	void setMagneticFieldZ(double magneticFieldZ)
	{
	    field.reset(0);
	    magneticFieldZ_ = magneticFieldZ;
	}
	void setIndex(int index)
	{
	    index_ = index;
	}

	// Getters
	int index() const {return index_;}
	virtual const double getThickness(const math::XYZTLorentzVector& pos, const math::XYZTLorentzVector& mom) const = 0;
	const double getNuclearInteractionThicknessFactor() const {return nuclearInteractionThicknessFactor; }
	const DetLayer* getDetLayer(double z = 0) const { return detLayer; }
	const double getMagneticFieldZ(double pos) const
	{
	    if(field)
	    {
		return field->GetBinContent(field->GetXaxis()->FindBin(fabs(pos))); 
	    }
	    else
	    {
		return magneticFieldZ_;
	    }
	}
	virtual const double getMagneticFieldZ(math::XYZTLorentzVector pos) const = 0;
	virtual bool isForward() const = 0;

	friend std::ostream& operator << (std::ostream& os , const Layer & layer);

	virtual bool isOnSurface(const math::XYZTLorentzVector & position) const = 0;

	const std::vector<InteractionModel *> & getInteractionModels() const
	{
	    return interactionModels_;
	}

	void addInteractionModel(InteractionModel & interactionModel)
	{
	    interactionModels_.push_back(&interactionModel);
	}

    protected:
	// position is either r (barrelLayer) or z (forwardLayer)
	double position_;
	double magneticFieldZ_;
	double nuclearInteractionThicknessFactor;
	const DetLayer * detLayer;
	double minMaterial;
	double maxMaterial;
	int index_;
	
	TH1F thickness;
	std::unique_ptr<TH1F> field;
	
	static const double epsilonDistance_;

	std::vector<InteractionModel *> interactionModels_;

    };

    std::ostream& operator << (std::ostream& os , const Layer & layer);
}

#endif
