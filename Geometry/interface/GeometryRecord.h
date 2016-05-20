#ifndef FastSimulation_Geometry_GeometryRecord_h
#define FastSimulation_Geometry_GeometryRecord_h

#include "FWCore/Framework/interface/DependentRecordImplementation.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "boost/mpl/vector.hpp"


namespace fastsim{

	class GeometryRecord : public edm::eventsetup::DependentRecordImplementation<GeometryRecord, boost::mpl::vector<TrackerRecoGeometryRecord, IdealMagneticFieldRecord> > {};

}

#endif
