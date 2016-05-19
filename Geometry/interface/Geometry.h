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
				    	const GeometricSearchTracker& geomSearchTracker,
			        	const MagneticField* pMF);

			/// Destructor
			~Geometry(){};

			// Returns the magnetic field
			const MagneticField* getMagField() const { return theField; }

			const std::vector<BarrelLayer>& barrelLayers() const { return theBarrelLayers; }
			const std::vector<ForwardLayer>& forwardLayers() const { return theForwardLayers; }

		private:
			const DetLayer* getBarrelDetLayer(std::string layerName, const GeometricSearchTracker& theGeomSearchTracker);
			std::vector<const DetLayer*> getForwardDetLayers(std::string layerName, const GeometricSearchTracker& theGeomSearchTracker);

			/// The list of tracker layers
			std::vector<BarrelLayer> theBarrelLayers;
			std::vector<ForwardLayer> theForwardLayers;

			// Pointer to the magnetic field
			const MagneticField* theField;	
    };

}

#endif
