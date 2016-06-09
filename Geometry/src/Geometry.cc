//Framework Headers
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// Tracker/Tracking Headers
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTracker.h"

#include "FastSimulation/Geometry/interface/BarrelLayer.h"
#include "FastSimulation/Geometry/interface/ForwardLayer.h"

#include "TrackingTools/DetLayers/interface/DetLayer.h"
#include "TrackingTools/DetLayers/interface/BarrelDetLayer.h"
#include "TrackingTools/DetLayers/interface/ForwardDetLayer.h"

#include "MagneticField/Engine/interface/MagneticField.h"

#include "FastSimulation/Geometry/interface/Geometry.h"

#include <iostream>
#include <map>

using namespace fastsim;

Geometry::Geometry(const edm::ParameterSet& cfg,const GeometricSearchTracker * geometricSearchTracker)
    : magneticField_(0)
    , magneticFieldZ_(0.)
{
    
    /*
      create the barrel layers
    */
    for(const edm::ParameterSet & layerCfg : cfg.getParameter<std::vector<edm::ParameterSet>>("BarrelLayers"))
    {
	// extract the thicness of the layer
	std::vector<double> limits = layerCfg.getUntrackedParameter<std::vector<double> >("limits",std::vector<double>());
	std::vector<double> thickness = layerCfg.getUntrackedParameter<std::vector<double> >("thickness",std::vector<double>());

	// extract the associated active layer
	const DetLayer * detLayer = getBarrelDetLayer( layerCfg.getUntrackedParameter<std::string>("activeLayer",""), geometricSearchTracker );
	
        // first try extracting radius from configuration
	
        double radius = layerCfg.getUntrackedParameter<double>("radius",-1.);
	// then try extracting from detLayer
	if(radius <= 0 && detLayer)
	{
            radius = static_cast<BarrelDetLayer const*>(detLayer)->specificSurface().radius();
        }
	// then throw error
	if(radius <= 0)
	{
	    throw cms::Exception("FastSimulation/Geometry") << " no way to extract a radius for barrel layer " << barrelLayers_.size() + 1 << std::endl;
	}
	
	// check that the radius is larger than the radius of the previous layer
	if(barrelLayers_.size() > 0)
	{
	    if(radius < barrelLayers_.back()->getRadius())
	    {
		throw cms::Exception("FastSimulation/Geometry") << "barrel layers must be ordered to increasing radius" << std::endl;
	    }
	}

	// and construct the layer inside the list of barrel layers
        barrelLayers_.emplace_back(new BarrelLayer(radius, limits, thickness, detLayer));
	barrelLayers_.back()->setIndex(barrelLayers_.size()-1);
    }

    /*
      create the forward layers
    */
    for(const edm::ParameterSet & layerCfg : cfg.getParameter<std::vector<edm::ParameterSet>>("ForwardLayers"))
    {

	std::string pset;
	layerCfg.allToString(pset);

	// extract the thicness of the layer
	std::vector<double> limits = layerCfg.getUntrackedParameter<std::vector<double> >("limits",std::vector<double>());
	std::vector<double> thickness = layerCfg.getUntrackedParameter<std::vector<double> >("thickness",std::vector<double>());
	
	// extract the associated active layer
	std::string activeLayerName = layerCfg.getUntrackedParameter<std::string>("activeLayer","");
	const DetLayer * posDetLayer = 0;
	const DetLayer * negDetLayer = 0;
	if(!activeLayerName.empty())
	{
	    negDetLayer = getForwardDetLayer("neg" + activeLayerName  , geometricSearchTracker );
	    posDetLayer = getForwardDetLayer("pos" + activeLayerName  , geometricSearchTracker );
	}
	
        // first try extracting radius from configuration
        double z = layerCfg.getUntrackedParameter<double>("z",-1);
	// then try extracting from detLayer
	if(z <= 0 && posDetLayer)
	{
            z = static_cast<ForwardDetLayer const*>(posDetLayer)->position().z();
        }
	// then throw error
	if(z <= 0)
	{
	    throw cms::Exception("FastSimulation/Geometry") << " no way to extract a position for this forward layer..." << std::endl;
	}

	// check that |z| is larger than for the previous layer
	if(forwardLayers_.size() > 0)
	{
	    if(z < -forwardLayers_.back()->getZ())
	    {
		throw cms::Exception("FastSimulation/Geometry") << "forward layers must be ordered to increasing z" << std::endl;
	    }
	}

	// create and insert the layer on the negative side
	forwardLayers_.emplace(forwardLayers_.begin(),new ForwardLayer(-z,limits, thickness, negDetLayer));
	// create and insert the layer on the positive side
	forwardLayers_.emplace_back(new ForwardLayer(z,limits, thickness, posDetLayer));
    
	// try extracting the nuclear interaction thickness factor
	double nuclearInteractionThicknessFactor = layerCfg.getUntrackedParameter<double>("nuclearInteractionThicknessFactor",-1);
	if(nuclearInteractionThicknessFactor >= 0)
	{
	    forwardLayers_.front()->setNuclearInteractionThicknessFactor(nuclearInteractionThicknessFactor);
	    forwardLayers_.back()->setNuclearInteractionThicknessFactor(nuclearInteractionThicknessFactor);
	}

    }
    
    for(unsigned index = 0;index < forwardLayers_.size();index++)
    {
	forwardLayers_[index]->setIndex(index);
    }

}

void fastsim::Geometry::setMagneticFieldZ(double magneticFieldZ)
{
    magneticField_ = 0;
    magneticFieldZ_ = magneticFieldZ;
    for(auto & layer : barrelLayers_)
    {
        layer->setMagneticFieldZ(magneticFieldZ);
    }
    for(auto & layer : forwardLayers_)
    {
        layer->setMagneticFieldZ(magneticFieldZ);
    }
}

void fastsim::Geometry::setMagneticField(const MagneticField & magneticField)
{
    magneticField_ = &magneticField;
    magneticFieldZ_ = 0;
    /*
      barrel layers
    */
    double maxZ = 0;
    if(forwardLayers_.size() > 0)
    {
	forwardLayers_.back()->getZ();
    }
    for(auto & layer : barrelLayers_)
    {
        layer->setMagneticField(magneticField, maxZ);
    }
    
    /*
      forward layers
    */
    double maxR = 0;
    if(barrelLayers_.size() > 0)
    {
	barrelLayers_.back()->getRadius();
    }
    for(auto & layer : forwardLayers_)
    {
        layer->setMagneticField(magneticField, maxR);
    }
}

const DetLayer*
Geometry::getBarrelDetLayer(std::string layerName, const GeometricSearchTracker * geometricSearchTracker)
{

    if(!geometricSearchTracker || layerName.empty())
    {
	return 0;
    }
    
    std::map<std::string,const std::vector<BarrelDetLayer const *> *> detLayersMap;
    detLayersMap["BPix"] = &geometricSearchTracker->pixelBarrelLayers();
    detLayersMap["TIB"] = &geometricSearchTracker->tibLayers();
    detLayersMap["TOB"] = &geometricSearchTracker->tobLayers();
    for( auto & entry : detLayersMap)
    {
	std::string name_ = entry.first;
	auto detLayers = entry.second;
	if(layerName.find(name_)==0)
	{
	    int index = atoi(layerName.substr(name_.size()).c_str()) -1;
	    if(index < 0. || index > int(detLayers->size()))
	    {
		throw cms::Exception("FastSimulation/Geometry") << " bad index in activeLayer name " <<  layerName;
	    }
	    return (*detLayers)[index];
	}
    }
    throw cms::Exception("FastSimulation/Geometry") << " bad naming of active layer " << layerName << std::endl;
}
    

const DetLayer*
Geometry::getForwardDetLayer(std::string layerName, const GeometricSearchTracker * geometricSearchTracker)
{
    if(!geometricSearchTracker || layerName.empty())
    {
	return 0;
    }

    std::map<std::string,const std::vector<ForwardDetLayer const *> *> detLayersMap;
    detLayersMap["negFPix"] = &geometricSearchTracker->negPixelForwardLayers();
    detLayersMap["posFPix"] = &geometricSearchTracker->posPixelForwardLayers();
    detLayersMap["negTID"] = &geometricSearchTracker->negTidLayers();
    detLayersMap["posTID"] = &geometricSearchTracker->posTidLayers();
    detLayersMap["negTEC"] = &geometricSearchTracker->negTecLayers();
    detLayersMap["posTEC"] = &geometricSearchTracker->posTecLayers();

    for( auto & entry : detLayersMap)
    {
	std::string name_ = entry.first;
	auto detLayers = entry.second;
	if(layerName.find(name_)==0)
	{
	    int index = atoi(layerName.substr(name_.size()).c_str()) -1;
	    if(index < 0 || index > int(detLayers->size()))
	    {
		throw cms::Exception("FastSimulation/Geometry") << " bad index in activeLayer name " <<  layerName;
	    }
	    return (*detLayers)[index];
	}
    }
    throw cms::Exception("FastSimulation/Geometry") << " bad naming of active layer " << layerName << std::endl;
}

std::ostream& fastsim::operator << (std::ostream& os , const fastsim::Geometry & geometry)
{
    os << "-----------"
       << "\n# fastsim::Geometry"
       << "\n## BarrelLayers:";
    for(const auto & layer : geometry.barrelLayers_)
    {
	os << "\n   " << *layer;
    }
    os << "\n## ForwardLayers:";
    for(const auto & layer : geometry.forwardLayers_)
    {
	os << "\n   " << *layer;
    }
    os << "\n-----------";
    return os;
}

double fastsim::Geometry::getMagneticFieldZ (const math::XYZTLorentzVector & position) const
{
    if(magneticField_)
    {
	return magneticField_->inTesla(GlobalPoint(position.x(),position.y(),position.z())).z();
    }
    else
    {
	return magneticFieldZ_;
    }
}
