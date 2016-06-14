//Framework Headers
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/ESHandle.h"

// Tracker/Tracking Headers
#include "RecoTracker/TkDetLayers/interface/GeometricSearchTracker.h"

#include "FastSimulation/Layer/interface/BarrelLayer.h"
#include "FastSimulation/Layer/interface/ForwardLayer.h"

#include "TrackingTools/DetLayers/interface/DetLayer.h"
#include "TrackingTools/DetLayers/interface/BarrelDetLayer.h"
#include "TrackingTools/DetLayers/interface/ForwardDetLayer.h"

#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/InteractionModel/interface/InteractionModelFactory.h"
#include "FastSimulation/InteractionModel/interface/InteractionModel.h"

#include <iostream>
#include <map>

using namespace fastsim;

Geometry::~Geometry(){;}

Geometry::Geometry(const edm::ParameterSet& cfg,edm::EventSetup const & iSetup)
    : magneticField_(0)
    , fixedMagneticFieldZ_(0.)
{
    
    /*
      initialize the interaction models
    */
    const edm::ParameterSet & interactionModelsCfg = cfg.getParameter<edm::ParameterSet>("interactionModels");
    LogDebug("FastSimulation") << "   found " << interactionModelsCfg.getParameterNames().size() << " interactionModels" << std::endl;
    for( const std::string & name : interactionModelsCfg.getParameterNames())
    {
	const edm::ParameterSet & _cfg = interactionModelsCfg.getParameter<edm::ParameterSet>(name);
	auto entry = interactionModels_.insert(std::make_pair(name,std::move(fastsim::InteractionModelFactory::create(_cfg))));
	interactionModelVector_.push_back(entry.first->second.get());
    }

    /*
      setup magnetic field
    */
    if(cfg.exists("magneticFieldZ"))
    {
	this->setMagneticFieldZ(cfg.getParameter<double>("magneticFieldZ"));
    }
    else
    {
	edm::ESHandle<MagneticField> magneticField;
	iSetup.get<IdealMagneticFieldRecord>().get(magneticField);
	this->setMagneticField(*magneticField);
    }

    /*
      setup tracker reconstruction geometry
    */
    const GeometricSearchTracker * geometricSearchTracker = 0;
    bool useTrackerRecoGeometryRecord = cfg.getUntrackedParameter<bool>("useTrackerRecoGeometryRecord",true);
    if(useTrackerRecoGeometryRecord)
    {
	edm::ESHandle<GeometricSearchTracker> geometricSearchTrackerHandle;
	std::string alignmentLabel = cfg.getParameter<std::string>("trackerAlignmentLabel");
	iSetup.get<TrackerRecoGeometryRecord>().get(alignmentLabel,geometricSearchTrackerHandle);
	geometricSearchTracker = &(*geometricSearchTrackerHandle);
    }

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

	// try extracting the nuclear interaction thickness factor
	double nuclearInteractionThicknessFactor = layerCfg.getUntrackedParameter<double>("nuclearInteractionThicknessFactor",-1);
	if(nuclearInteractionThicknessFactor >= 0)
	{
	    barrelLayers_.front()->setNuclearInteractionThicknessFactor(nuclearInteractionThicknessFactor);
	}

	// add interaction models
	std::vector<std::string> interactionModelLabels = layerCfg.getUntrackedParameter<std::vector<std::string> >("interactionModels");
	for(const auto & label : interactionModelLabels)
	{
	    auto interactionModel = interactionModels_.find(label);
	    if(interactionModel == interactionModels_.end())
	    {
		throw cms::Exception("FastSimulation") << "unknown interaction model '" << label << "'";
	    }
	    barrelLayers_.back()->addInteractionModel(*(*interactionModel).second);
	}
    }

    /*
      create the forward layers
    */
    for(const edm::ParameterSet & layerCfg : cfg.getParameter<std::vector<edm::ParameterSet>>("ForwardLayers"))
    {

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

	// add interaction models
	std::vector<std::string> interactionModelLabels = layerCfg.getUntrackedParameter<std::vector<std::string> >("interactionModels");
	for(const auto & label : interactionModelLabels)
	{
	    auto interactionModel = interactionModels_.find(label);
	    if(interactionModel == interactionModels_.end())
	    {
		throw cms::Exception("FastSimulation") << "unknown interaction model '" << label << "'";
	    }
	    forwardLayers_.front()->addInteractionModel(*(*interactionModel).second);
	    forwardLayers_.back()->addInteractionModel(*(*interactionModel).second);
	}
    }

    // set forward layer indices
    for(unsigned index = 0;index < forwardLayers_.size();index++)
    {
	forwardLayers_[index]->setIndex(index);
    }

}

void fastsim::Geometry::setMagneticFieldZ(double magneticFieldZ)
{
    magneticField_ = 0;
    fixedMagneticFieldZ_ = magneticFieldZ;
    for(auto & layer : barrelLayers_)
    {
        layer->setMagneticFieldZ(fixedMagneticFieldZ_);
    }
    for(auto & layer : forwardLayers_)
    {
        layer->setMagneticFieldZ(fixedMagneticFieldZ_);
    }
}

void fastsim::Geometry::setMagneticField(const MagneticField & magneticField)
{
    magneticField_ = &magneticField;
    fixedMagneticFieldZ_ = 0;
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
	for(auto interactionModel : layer->getInteractionModels())
	{
	    os << "\n      " << *interactionModel;
	}
    }
    os << "\n## ForwardLayers:";
    for(const auto & layer : geometry.forwardLayers_)
    {
	os << "\n   " << *layer;
	for(auto interactionModel : layer->getInteractionModels())
	{
	    os << "\n      " << *interactionModel;
	}
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
	return fixedMagneticFieldZ_;
    }
}
