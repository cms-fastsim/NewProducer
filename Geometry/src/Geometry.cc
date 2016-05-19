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

using namespace fastsim;

Geometry::Geometry(const edm::ParameterSet& trackerMaterial, const GeometricSearchTracker& theGeomSearchTracker, const MagneticField* pMF) : theField(pMF)
{
    
    std::vector<edm::ParameterSet> thePSetBarrelLayers = trackerMaterial.getParameter<std::vector<edm::ParameterSet>>("BarrelLayers");
    std::vector<edm::ParameterSet> thePSetForwardLayers = trackerMaterial.getParameter<std::vector<edm::ParameterSet>>("ForwardLayers");

    // Get max dimensions of barrel/forward layers
    double maxZ = 0., maxR = 0.;
    edm::ParameterSet outerBarrel = thePSetBarrelLayers.back();
    edm::ParameterSet outerForward = thePSetForwardLayers.back();

    if(outerBarrel.existsAs<double>("radius")){
        maxR = outerBarrel.getParameter<double>("radius");
    }else if(outerBarrel.existsAs<std::string>("activeLayer")){
        if(getBarrelDetLayer(outerBarrel.getParameter<std::string>("activeLayer"), theGeomSearchTracker))    
            maxR = static_cast<BarrelDetLayer const*>(getBarrelDetLayer(outerBarrel.getParameter<std::string>("activeLayer"), theGeomSearchTracker))->specificSurface().radius();
    }
    if(outerForward.existsAs<double>("z")){
        maxZ = outerForward.getParameter<double>("z");
    }else if(outerForward.existsAs<std::string>("activeLayer")){
        if(getForwardDetLayers(outerForward.getParameter<std::string>("activeLayer"), theGeomSearchTracker)[0])    
            maxZ = static_cast<BarrelDetLayer const*>(getForwardDetLayers(outerForward.getParameter<std::string>("activeLayer"), theGeomSearchTracker)[0])->specificSurface().radius();
    }

    for(std::vector<edm::ParameterSet>::const_iterator it_barrel = thePSetBarrelLayers.begin(); it_barrel != thePSetBarrelLayers.end(); ++it_barrel){
        double radius = 0.;
        std::vector<double> limits, thickness;
        const DetLayer* detLayer = 0;

        if(it_barrel->existsAs<std::vector<double>>("limits")) limits = it_barrel->getParameter<std::vector<double>>("limits");
        if(it_barrel->existsAs<std::vector<double>>("thickness")) thickness = it_barrel->getParameter<std::vector<double>>("thickness");
        if(it_barrel->existsAs<std::string>("activeLayer")) detLayer = getBarrelDetLayer(it_barrel->getParameter<std::string>("activeLayer"), theGeomSearchTracker);

        // If hardcoded radius is defined: overwrite active layer properties
        if(it_barrel->existsAs<double>("radius")){
            radius = it_barrel->getParameter<double>("radius");
        }else if(detLayer){
            radius = static_cast<BarrelDetLayer const*>(detLayer)->specificSurface().radius();
        }

        BarrelLayer blayer(radius, limits, thickness, detLayer);
        if(pMF) blayer.setMagneticField(*pMF, maxZ);

        theBarrelLayers.push_back(blayer);        


        // Check overall compatibility of barrel dimensions
        if(theBarrelLayers.size() <= 1) continue;
        //std::cout.precision(4);
        const BarrelLayer Lout = theBarrelLayers[theBarrelLayers.size()-1];
        const BarrelLayer Lin = theBarrelLayers[theBarrelLayers.size()-2];
        if(Lout.getRadius() < Lin.getRadius())
            throw cms::Exception("FastSimulation/Geometry ") 
                << " WARNING with BarrelLayer number " << theBarrelLayers.size() 
                << " has radius smaller than previous layer"<< std::endl
                << " rOuter/rInner=" << Lout.getRadius() << "/" << Lin.getRadius() << std::endl;
    }


    for(std::vector<edm::ParameterSet>::const_iterator it_forward = thePSetForwardLayers.begin(); it_forward != thePSetForwardLayers.end(); ++it_forward){
        double z = 0.;
        std::vector<double> limits, thickness;
        std::vector<const DetLayer*> detLayers(2, 0);

        if(it_forward->existsAs<std::vector<double>>("limits")) limits = it_forward->getParameter<std::vector<double>>("limits");
        if(it_forward->existsAs<std::vector<double>>("thickness")) thickness = it_forward->getParameter<std::vector<double>>("thickness");
        if(it_forward->existsAs<std::string>("activeLayer")) detLayers = getForwardDetLayers(it_forward->getParameter<std::string>("activeLayer"), theGeomSearchTracker);

        // If hardcoded z is defined: overwrite active layer properties
        if(it_forward->existsAs<double>("z")){
            z = it_forward->getParameter<double>("z");
        }else if(detLayers[0]){
            z = static_cast<ForwardDetLayer const*>(detLayers.at(0))->surface().position().z();
        }

        ForwardLayer flayer(z, limits, thickness, detLayers[0], detLayers[1]);
        if(pMF) flayer.setMagneticField(*pMF, maxR);
        if(flayer.getDetLayer()) if(flayer.getDetLayer()->subDetector() == GeomDetEnumerators::subDetGeom[GeomDetEnumerators::TEC])
            flayer.setNuclearInteractionThicknessFactor(1.2);

        theForwardLayers.push_back(flayer);        


        // Check overall compatibility of forward dimensions
        if(theForwardLayers.size() <= 1) continue;
        //std::cout.precision(4);
        const ForwardLayer Lout = theForwardLayers[theForwardLayers.size()-1];
        const ForwardLayer Lin = theForwardLayers[theForwardLayers.size()-2];
        if(Lout.getZ() < Lin.getZ())
            throw cms::Exception("FastSimulation/Geometry ") 
                << " WARNING with ForwardLayer number " << theForwardLayers.size() 
                << " has z smaller than previous layer"<< std::endl
                << " zOuter/zInner=" << Lout.getZ() << "/" << Lin.getZ() << std::endl;
    }

}

const DetLayer*
Geometry::getBarrelDetLayer(std::string layerName, const GeometricSearchTracker& theGeomSearchTracker){

    if(layerName.empty()) return 0;

    const DetLayer *detLayer = 0;

    if (layerName.substr(0,4)=="BPix"){
        int index = std::atoi(layerName.substr(4,1).c_str()) -1;
        if(index < 0) throw cms::Exception("FastSimulation/Geometry") << "No valid index in '" << layerName.c_str() << "'";
        if(theGeomSearchTracker.pixelBarrelLayers().size() <= std::abs(index))
            throw cms::Exception("FastSimulation/Geometry") << "Layer index out of range: '" << layerName.c_str() << "'";
        detLayer = *(theGeomSearchTracker.pixelBarrelLayers().begin() + index);
    }else if (layerName.substr(0,3)=="TIB"){
        int index = std::atoi(layerName.substr(3,1).c_str()) -1;
        if(index < 0) throw cms::Exception("FastSimulation/Geometry") << "No valid index in '" << layerName.c_str() << "'";
        if(theGeomSearchTracker.tibLayers().size() <= std::abs(index))
            throw cms::Exception("FastSimulation/Geometry") << "Layer index out of range: '" << layerName.c_str() << "'";
        detLayer = *(theGeomSearchTracker.tibLayers().begin() + index);
    }else if (layerName.substr(0,3)=="TOB"){
        int index = std::atoi(layerName.substr(3,1).c_str()) -1;
        if(index < 0) throw cms::Exception("FastSimulation/Geometry") << "No valid index in '" << layerName.c_str() << "'";
        if(theGeomSearchTracker.tobLayers().size() <= std::abs(index))
            throw cms::Exception("FastSimulation/Geometry") << "Layer index out of range: '" << layerName.c_str() << "'";
        detLayer = *(theGeomSearchTracker.tobLayers().begin() + index);
    }
    else{
        throw cms::Exception("FastSimulation/Geometry")
            << "Bad data naming in tracker layer configuration."
            << "no case sensitive name of ['BPix','TIB',TOB'] matches '"<<layerName.c_str()<<"'";
    }
    
    return detLayer;
}

std::vector<const DetLayer*>
Geometry::getForwardDetLayers(std::string layerName, const GeometricSearchTracker& theGeomSearchTracker){

    std::vector<const DetLayer*> detLayers(2, 0);

    if(layerName.empty()) return detLayers;

    if (layerName.substr(0,4)=="FPix"){
        int index = std::atoi(layerName.substr(4,1).c_str()) -1;    
        if(index < 0) throw cms::Exception("FastSimulation/Geometry") << "No valid index in '" << layerName.c_str() << "'";
        if(theGeomSearchTracker.posPixelForwardLayers().size() <= std::abs(index))
            throw cms::Exception("FastSimulation/Geometry") << "Layer index out of range: '" << layerName.c_str() << "'";
        detLayers.at(0) = *(theGeomSearchTracker.posPixelForwardLayers().begin() + index);
        detLayers.at(1) = *(theGeomSearchTracker.negPixelForwardLayers().begin() + index);
    }else if (layerName.substr(0,3)=="TID"){
        int index = std::atoi(layerName.substr(3,1).c_str()) -1;
        if(index < 0) throw cms::Exception("FastSimulation/Geometry") << "No valid index in '" << layerName.c_str() << "'";
        if(theGeomSearchTracker.posTidLayers().size() <= std::abs(index))
            throw cms::Exception("FastSimulation/Geometry") << "Layer index out of range: '" << layerName.c_str() << "'";
        detLayers.at(0) = *(theGeomSearchTracker.posTidLayers().begin() + index);
        detLayers.at(1) = *(theGeomSearchTracker.negTidLayers().begin() + index);
    }else if (layerName.substr(0,3)=="TEC"){
        int index = std::atoi(layerName.substr(3,1).c_str()) -1;
        if(index < 0) throw cms::Exception("FastSimulation/Geometry") << "No valid index in '" << layerName.c_str() << "'";
        if(theGeomSearchTracker.posTecLayers().size() <= std::abs(index))
            throw cms::Exception("FastSimulation/Geometry") << "Layer index out of range: '" << layerName.c_str() << "'";
        detLayers.at(0) = *(theGeomSearchTracker.posTecLayers().begin() + index);
        detLayers.at(1) = *(theGeomSearchTracker.negTecLayers().begin() + index);
    }
    else{
        throw cms::Exception("FastSimulation/Geometry")
            << "Bad data naming in tracker layer configuration."
            << "no case sensitive name of ['FPix',TID','TEC'] matches '"<<layerName.c_str()<<"'";
    }
    
    return detLayers;
}
