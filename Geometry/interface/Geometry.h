#ifndef FASTSIM_GEOMETRY_H
#define FASTSIM_GEOMETRY_H

#include "FastSimulation/Geometry/interface/BarrelLayer.h"
#include "FastSimulation/Geometry/interface/ForwardLayer.h"

class GeometricSearchTracker;
class MagneticField;

#include <vector>

namespace edm { 
    class ParameterSet;
}

namespace fastsim{

    class Geometry
    {
    public:
	/// Constructor : get the configurable parameters
	Geometry(const edm::ParameterSet& trackerMaterial,
		 const GeometricSearchTracker * geomSearchTracker);


	/// set magnetic field
	void setMagneticField(const MagneticField & magneticField);
	void setMagneticFieldZ(double magneticFieldZ);

	/// Destructor
	~Geometry(){};

	// Returns the magnetic field
	double getMagneticFieldZ(const math::XYZTLorentzVector & position) const;

	const std::vector<std::unique_ptr<BarrelLayer> >& barrelLayers() const { return barrelLayers_; }
	const std::vector<std::unique_ptr<ForwardLayer> >& forwardLayers() const { return forwardLayers_; }

	friend std::ostream& operator << (std::ostream& o , const fastsim::Geometry & geometry); 
	
	// help to nagigate through layers
	const BarrelLayer * nextBarrelLayer(const BarrelLayer * layer) const
	{
	    if(layer == 0)
	    {
		return 0;
	    }
	    unsigned nextLayerIndex = layer->index() + 1;
	    return nextLayerIndex < barrelLayers_.size() ? barrelLayers_[nextLayerIndex].get() : 0;
	}

	const ForwardLayer * nextForwardLayer(const ForwardLayer * layer) const
	{
	    if(layer == 0)
	    {
		return 0;
	    }
	    unsigned nextLayerIndex = layer->index() + 1;
	    return nextLayerIndex < forwardLayers_.size() ? forwardLayers_[nextLayerIndex].get() : 0;
	}

	const BarrelLayer * previousBarrelLayer(const BarrelLayer * layer) const
	{
	    if(layer == 0)
	    {
		return barrelLayers_.back().get();
	    }
	    return layer->index() > 0 ? barrelLayers_[layer->index() -1].get() : 0;
	}

	const ForwardLayer * previousForwardLayer(const ForwardLayer * layer) const
	{
	    if(layer == 0)
	    {
		return forwardLayers_.back().get();
	    }
	    return layer->index() > 0 ? forwardLayers_[layer->index() -1].get() : 0;
	}


    private:

	const DetLayer * getBarrelDetLayer(std::string layerName, const GeometricSearchTracker * theGeomSearchTracker);
	const DetLayer * getForwardDetLayer(std::string layerName, const GeometricSearchTracker * theGeomSearchTracker);

	/// The list of tracker layers (unique ptrs to avoid what is probably a bug in the compiler used by CMSSW
	std::vector<std::unique_ptr<BarrelLayer> >barrelLayers_;
	std::vector<std::unique_ptr<ForwardLayer> > forwardLayers_;

	// Pointer to the magnetic field
	const MagneticField * magneticField_;	
	double magneticFieldZ_;
    };
    std::ostream& operator << (std::ostream& os , const fastsim::Geometry & geometry);
}


#endif
