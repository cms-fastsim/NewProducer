#include "FastSimulation/Geometry/plugins/GeometryESProducer.h"
#include "RecoTracker/Record/interface/TrackerRecoGeometryRecord.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"

#include <memory>


using namespace fastsim;

GeometryESProducer::GeometryESProducer(const edm::ParameterSet & p) 
{
    setWhatProduced(this);
    _label = p.getUntrackedParameter<std::string>("trackerGeometryLabel","");

    theTrackerMaterial = p.getParameter<edm::ParameterSet>("TrackerMaterial");

}

GeometryESProducer::~GeometryESProducer() {}

std::shared_ptr<Geometry>
GeometryESProducer::produce(const GeometryRecord & iRecord){ 

  edm::ESHandle<GeometricSearchTracker> theGeomSearchTracker;
  edm::ESHandle<MagneticField> theMagneticField;
  
  iRecord.getRecord<TrackerRecoGeometryRecord>().get(_label, theGeomSearchTracker );
  iRecord.getRecord<IdealMagneticFieldRecord>().get(theMagneticField );
  _tracker = std::make_shared<Geometry>(theTrackerMaterial,*theGeomSearchTracker,&(*theMagneticField));
  return _tracker;

}


DEFINE_FWK_EVENTSETUP_MODULE(GeometryESProducer);
