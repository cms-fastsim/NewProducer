#include <vector>
#include <memory>
#include <algorithm>

// framework
#include "FWCore/Framework/interface/Event.h"
#include "MagneticField/UniformEngine/src/UniformMagneticField.h"

// tracking
#include "TrackingTools/DetLayers/interface/DetLayer.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateOnSurface.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackingTools/GeomPropagators/interface/HelixArbitraryPlaneCrossing.h"

// fastsim
#include "FastSimulation/TrajectoryManager/interface/InsideBoundsMeasurementEstimator.h" //TODO move this
#include "FastSimulation/NewParticle/interface/Particle.h"
#include "FastSimulation/Layer/interface/Layer.h"
#include "FastSimulation/InteractionModel/interface/InteractionModel.h"
#include "FastSimulation/InteractionModel/interface/InteractionModelFactory.h"

// data formats
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometryVector/interface/GlobalVector.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/GeometryVector/interface/LocalVector.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"
#include "SimDataFormats/TrackingHit/interface/PSimHit.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"

// other
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "CondFormats/External/interface/DetID.h"
#include "FWCore/Framework/interface/ProducerBase.h"

namespace edm
{
    class ParameterSet;
}

typedef std::pair<const GeomDet*,TrajectoryStateOnSurface> DetWithState;

namespace fastsim
{
    class TrackerSimHitProducer : public InteractionModel
    {
    public:
	TrackerSimHitProducer(const std::string & name,const edm::ParameterSet & cfg);
	~TrackerSimHitProducer(){;}
	void interact(Particle & particle,const Layer & layer,std::vector<std::unique_ptr<Particle> > & secondaries,const RandomEngineAndDistribution & random) override;
	virtual void registerProducts(edm::ProducerBase & producer) const override;
	virtual void storeProducts(edm::Event & iEvent) override;
	PSimHit* createHitOnDetector(const TrajectoryStateOnSurface & particle,int pdgId,int simTrackId,const GeomDet & detector);
    private:
	const float onSurfaceTolerance_;
	std::unique_ptr<edm::PSimHitContainer> simHitContainer_;
    };
}



fastsim::TrackerSimHitProducer::TrackerSimHitProducer(const std::string & name,const edm::ParameterSet & cfg)
    : fastsim::InteractionModel(name)
    , onSurfaceTolerance_(0.01) // 10 microns // hm, sure this is not 100 microns?
    , simHitContainer_(new edm::PSimHitContainer)
{}

void fastsim::TrackerSimHitProducer::registerProducts(edm::ProducerBase & producer) const
{
    producer.produces<edm::PSimHitContainer>("TrackerHits");
}

void fastsim::TrackerSimHitProducer::storeProducts(edm::Event & iEvent)
{
    std::cout << "Number of Hits: " << simHitContainer_->size() << std::endl;
    //for(auto shit : *(simHitContainer_.get())){
    //  std::cout<<shit.detUnitId()<<": "<<shit.localPosition().x()<<","<<shit.localPosition().y()<<std::endl;
    //}
    iEvent.put(std::move(simHitContainer_), "TrackerHits");
    simHitContainer_.reset(new edm::PSimHitContainer);
}

void fastsim::TrackerSimHitProducer::interact(Particle & particle,const Layer & layer,std::vector<std::unique_ptr<Particle> > & secondaries,const RandomEngineAndDistribution & random)
{
    //
    // check that layer has tracker modules
    //
    if(!layer.getDetLayer())
    {
	return;
    }

    //
    // create the trajectory of the particle
    //
    UniformMagneticField magneticField(layer.getMagneticFieldZ(particle.position())); 
    GlobalPoint  position( particle.position().X(), particle.position().Y(), particle.position().Z());
    GlobalVector momentum( particle.momentum().Px(), particle.momentum().Py(), particle.momentum().Pz());
    auto plane = layer.getDetLayer()->surface().tangentPlane(position);
    TrajectoryStateOnSurface trajectory(GlobalTrajectoryParameters( position, momentum, TrackCharge( particle.charge()), &magneticField), *plane);
    
    //
    // find detectors compatible with the particle's trajectory
    //
    AnalyticalPropagator propagator(&magneticField, anyDirection);
    InsideBoundsMeasurementEstimator est;
    std::vector<DetWithState> compatibleDetectors = layer.getDetLayer()->compatibleDets(trajectory, propagator, est);

    // Stores simHits so they can be sorted afterwards
    std::map<double, PSimHit*> distAndHit;
    //
    // loop over the compatible detectors
    //
    for (const auto & detectorWithState : compatibleDetectors)
    {
    	const GeomDet & detector = *detectorWithState.first;
    	const TrajectoryStateOnSurface & particleState = detectorWithState.second;
    	// if the detector has no components
    	if(detector.isLeaf())
    	{
    	    PSimHit* simHit = createHitOnDetector(particleState,particle.pdgId(),particle.simTrackIndex(),detector);
    	    if(simHit){
    		  	GlobalPoint hitPos(detector.surface().toGlobal(simHit->localPosition()));
    		  	distAndHit.insert(distAndHit.end(), std::pair<double,PSimHit*>(hitPos.mag2(), simHit));
    		  }
    	}
    	else
    	{
    	    // if the detector has components
    	    
    	    for(const auto component : detector.components())
    	    {
    		  PSimHit* simHit = createHitOnDetector(particleState,particle.pdgId(),particle.simTrackIndex(),*component);    		  
    		  if(simHit){
    		  	GlobalPoint hitPos(component->surface().toGlobal(simHit->localPosition()));
    		  	distAndHit.insert(distAndHit.end(), std::pair<double,PSimHit*>(hitPos.mag2(), simHit));
    		  }
    	    }
    	}
    }

    // You have to sort the simHits in the order they occur!

	// The old algorithm (sorting by distance to IP) doesn't seem to make sense to me (what if particle moves inwards??)

	// Detector layers have to be sorted by proximity to particle.position
	// Doesn't always work! Particle could be already have been propagated in between the layers!
	// Proximity to previous hit also doesn't work since simHits only store the localPosition
	// FIX (at least for now): propagate particle backwards a bit to make sure it's outside any components (straight line should work well enough)
	// I'd like to try old algorithm for the sake of it... However, I'd need a reference to the IP (simTrack vertex) for that :/
	/*GlobalPoint positionOutside(particle.position().X()-particle.momentum().Px()/sqrt(particle.momentum().Vect().Mag2()),
		particle.position().Y()-particle.momentum().Py()/sqrt(particle.momentum().Vect().Mag2()),
		particle.position().Z()-particle.momentum().Pz()/sqrt(particle.momentum().Vect().Mag2()));
	std::map<double, PSimHit*> distAndHit;
    for(const auto component : detector.components())
    {
	  PSimHit* simHit = createHitOnDetector(particleState,particle.pdgId(),particle.simTrackIndex(),*component);    		  
	  if(simHit){
	  	GlobalPoint hitPos(component->surface().toGlobal(simHit->localPosition()));
	  	distAndHit.insert(distAndHit.end(), std::pair<double,PSimHit*>((positionOutside-hitPos).mag2(), simHit));
	  }
    }

    for(std::map<double, PSimHit*>::const_iterator it = distAndHit.begin(); it != distAndHit.end(); it++){
    	simHitContainer_->push_back(*(it->second));
    }
    */

    // Try simplest approch. Use (0,0,0) as IP, and check if particle moves inwards
    // particle moves outwards
	if(particle.momentum().X()*particle.position().X() + particle.momentum().Y()*particle.position().Y() > 0)
	    for(std::map<double, PSimHit*>::const_iterator it = distAndHit.begin(); it != distAndHit.end(); it++){
	    	simHitContainer_->push_back(*(it->second));
	    }
	//particle moves inwards
	else
	    for(std::map<double, PSimHit*>::reverse_iterator rit = distAndHit.rbegin(); rit != distAndHit.rend(); rit++){
	    	simHitContainer_->push_back(*(rit->second));
	    }
}

PSimHit* fastsim::TrackerSimHitProducer::createHitOnDetector(const TrajectoryStateOnSurface & particle,int pdgId,int simTrackId,const GeomDet & detector)
{
    //
    // determine position and momentum of particle in the coordinate system of the detector
    //
    LocalPoint localPosition;
    LocalVector localMomentum;
    // if the particle is close enough, no further propagation is needed
    if ( fabs( detector.toLocal(particle.globalPosition()).z()) < onSurfaceTolerance_) 
    {
	localPosition = particle.localPosition();
	localMomentum = particle.localMomentum();
    }
    // else, propagate 
    else 
    {
	// find crossing of particle with 
	HelixArbitraryPlaneCrossing crossing( particle.globalPosition().basicVector(),
					      particle.globalMomentum().basicVector(),
					      particle.transverseCurvature(),
					      anyDirection);
	std::pair<bool,double> path = crossing.pathLength(detector.surface());
	// case propagation succeeds
	if (path.first) 	
	{
	    localPosition = detector.toLocal( GlobalPoint( crossing.position(path.second)));
	    localMomentum = detector.toLocal( GlobalVector( crossing.direction(path.second)));
	    localMomentum = localMomentum.unit() * particle.localMomentum().mag();
	}
	// case propagation fails
	else
	{
	    return 0;
	}
    }

    // 
    // find entry and exit point of particle in detector
    //
    const Plane& detectorPlane = detector.surface();
    float halfThick = 0.5*detectorPlane.bounds().thickness();
    float pZ = localMomentum.z();
    LocalPoint entry = localPosition + (-halfThick/pZ) * localMomentum;
    LocalPoint exit = localPosition + halfThick/pZ * localMomentum;
    float tof = particle.globalPosition().mag() / 29.9792458 ; // in nanoseconds
    
    //
    // make sure the simhit is physically on the module
    //
    double boundX = detectorPlane.bounds().width()/2.;
    double boundY = detectorPlane.bounds().length()/2.;
    // Special treatment for TID and TEC trapeziodal modules
    unsigned subdet = DetId(detector.geographicalId()).subdetId(); 
    if ( subdet == 4 || subdet == 6 ) 
	boundX *=  1. - localPosition.y()/detectorPlane.position().perp();
    if(fabs(localPosition.x()) > boundX  || fabs(localPosition.y()) > boundY )
    {
        std::cout<<"Hit position (id: " << detector.geographicalId().rawId() << "; " << simTrackId << ")= out of boundary"<<std::endl;
	   return 0;
    }

    /*    std::cout << "Hit position (id: " << detector.geographicalId().rawId() << "; " << simTrackId << ")= " 
        << localPosition.x() << " " 
        << localPosition.y() << " " 
//        << sqrt(localPosition.x()*localPosition.x() + localPosition.y()*localPosition.y()) << " " 
        << localPosition.z() << std::endl;
*/
    //
    // create the hit
    //
    double energyDeposit = 0.; // do something about the energy deposit
        return new PSimHit(entry, exit, localMomentum.mag(), tof, energyDeposit, pdgId,
				   detector.geographicalId().rawId(),simTrackId,
				   localMomentum.theta(),
				   localMomentum.phi());
}

DEFINE_EDM_PLUGIN(
    fastsim::InteractionModelFactory,
    fastsim::TrackerSimHitProducer,
    "fastsim::TrackerSimHitProducer"
    );