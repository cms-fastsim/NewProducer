#ifndef FastSimulation_Geometry_GeometryESProducer_H
#define FastSimulation_Geometry_GeometryESProducer_H

#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FastSimulation/Geometry/interface/GeometryRecord.h"
#include "FastSimulation/Geometry/interface/Geometry.h"
#include <memory>
#include <string>


namespace fastsim{

	class  GeometryESProducer: public edm::ESProducer{
	 public:
	  GeometryESProducer(const edm::ParameterSet & p);
	  virtual ~GeometryESProducer(); 
	  std::shared_ptr<Geometry> produce(const GeometryRecord &);
	 private:
	  std::shared_ptr<Geometry> _tracker;
	  std::string _label;
	  edm::ParameterSet theTrackerMaterial;
	};

}


#endif




