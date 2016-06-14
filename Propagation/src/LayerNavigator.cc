#include "FastSimulation/Propagation/interface/LayerNavigator.h"

#include "vector"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/Layer/interface/BarrelLayer.h"
#include "FastSimulation/Layer/interface/ForwardLayer.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"
#include "FastSimulation/Propagation/interface/Trajectory.h"      // new class, to be defined, based on ParticlePropagator
#include "FastSimulation/NewParticle/interface/Particle.h"

/**
// find the next layer that the particle will cross
//
// motivation for a new algorithm
//
//    - old algorithm is flawed
//    - the new algorithm allows to put material and instruments on any plane perpendicular to z, or on any cylinder with the z-axis as axis 
//    - while the old algorith, with the requirement of nested layers, forbids the introduction of long narrow cylinders, required for a decent simulation of material in front of HF
// 
// definitions:
//
//    the geometry is described by 2 sets of layers:
//    - forward layers: 
//          flat layers, perpendicular to the z-axis, positioned at a given z
//          these layers have material / instruments between a given materialMinR and materialMaxR
//          no 2 forward layers should have the same z-position
//    - barrel layers: 
//          cylindrically shaped layers, with the z-axis as axis, infinitely long
//          these layers have material / instruments for |z| < materialMaxAbsZ
//          no 2 barrel layers should have the same radius
//    - forward(barrel) layers are ordered according to increasing z (r)

// principle
//    - neutral particles follow a straight trajectory
//    - charged particles follow a helix-shaped trajectory:
//          constant speed along the z-axis
//          circular path in the x-y plane
//    => the next layer that the particle will cross is among the following 3 layers
//    - closest forward layer with
//         - z >(<) particle.z() for particles moving in the positive(negative) direction
//    - closest barrel layer with r < particle.r
//    - closest barrel layer with r > particle.r  

// algorithm
//    - find the 3 candidate layers 
//    - find the earliest positive intersection time for each of the 3 candidate layers
//    - move the particle to the earliest intersection time
//    - select and return the layer with the earliest positive intersection time
//
// notes
//    - the implementation of the algorithm can probably be optimised, e.g.
//       - one can probably gain time in moveToNextLayer if LayerNavigator is aware of the candidate layers of the previous call to moveToNextLayer
//       - for straight tracks, the optimal strategy to find the next layer might be very different
**/

const std::string fastsim::LayerNavigator::MESSAGECATEGORY = "FastSimulation";

fastsim::LayerNavigator::LayerNavigator(const fastsim::Geometry & geometry)
    : geometry_(&geometry)
    , nextBarrelLayer_(0)
    , nextForwardLayer_(0)
{;}

bool fastsim::LayerNavigator::moveParticleToNextLayer(fastsim::Particle & particle,const fastsim::Layer * & layer)
{
    LogDebug(MESSAGECATEGORY) << "   moveToNextLayer called";

    // if the layer is provided, the particle must be on it
    if(layer)
    {
	if(!layer->isOnSurface(particle.position()))
	{
	    throw cms::Exception("FastSimulation") << "If layer is prodvided, particle must be on layer";
	}
    }

    // magnetic field at the current position of the particle
    double magneticFieldZ = layer ? layer->getMagneticFieldZ(particle.position()) : geometry_->getMagneticFieldZ(particle.position());
    LogDebug(MESSAGECATEGORY) << "   magnetic field z component:" << magneticFieldZ;

    // particle moves inwards?
    bool particleMovesInwards = particle.momentum().px()*particle.position().x() + particle.momentum().py()*particle.position().y() < 0;
    
    /*
      update nextBarrelLayer and nextForwardLayer
    */

    // first time
    if(!layer)
    {
	
	LogDebug(MESSAGECATEGORY) << "      called for first time";

	/*
	  find the narrowest barrel layers with
	  layer.r > particle.r
	  assume barrel layers are ordered with increasing r
	*/
	for(const auto & layer : geometry_->barrelLayers())
	{
	    if(particle.position().rho() < layer->getRadius() || (particleMovesInwards && particle.position().rho() == layer->getRadius()))
	    {
		nextBarrelLayer_ = layer.get();
		break;
	    }
	}

	/*
	  find the forward layer with smallest z with
	  layer.z > particle z
	*/
	for(const auto & layer : geometry_->forwardLayers())
	{
	    if(particle.position().z() < layer->getZ() || (particle.momentum().pz() < 0 && particle.position().z() == layer->getZ()))
	    {
		nextForwardLayer_ = layer.get();
		break;
	    }
	}
    }
    // last move worked, let's update
    else
    {
	LogDebug(MESSAGECATEGORY) << "      ordinary call";
	if(layer == nextBarrelLayer_)
	{
	    if(!particleMovesInwards)
	    {
		nextBarrelLayer_ = geometry_->nextBarrelLayer(nextBarrelLayer_);
	    }
	}
	else if(layer == geometry_->previousBarrelLayer(nextBarrelLayer_))
	{
	    if(particleMovesInwards)
	    {
		nextBarrelLayer_ = geometry_->previousBarrelLayer(nextBarrelLayer_);
	    }
	}
	else if(layer == nextForwardLayer_)
	{
	    if(particle.momentum().pz() > 0)
	    {
		nextForwardLayer_ = geometry_->nextForwardLayer(nextForwardLayer_);
	    }
	}
	else if(layer == geometry_->previousForwardLayer(nextForwardLayer_))
	{
	    if(particle.momentum().pz() < 0)
	    {
		nextForwardLayer_ = geometry_->previousForwardLayer(nextForwardLayer_);
	    }
	}
	layer = 0;
    }

    /*
      move particle to first hit with one of the enclosing layers
    */
    
    
    LogDebug(MESSAGECATEGORY) << "   nextBarrelLayer index: " << (nextBarrelLayer_ ? nextBarrelLayer_->index() : -1)
			      << "\n   nextForwardLayer index: " << (nextForwardLayer_ ? nextForwardLayer_->index() : -1);
    
    // caclulate and store some variables related to the particle's trajectory
    std::unique_ptr<fastsim::Trajectory> trajectory = Trajectory::createTrajectory(particle,magneticFieldZ);
    
    // now let's try to move the particle to one of the enclosing layers
    std::vector<const fastsim::Layer*> layers;
    if(nextBarrelLayer_) 
    {
	layers.push_back(nextBarrelLayer_);
    }
    if(geometry_->previousBarrelLayer(nextBarrelLayer_))
    {
	layers.push_back(geometry_->previousBarrelLayer(nextBarrelLayer_));
    }
    if(particle.momentum().pz() > 0)
    {
	if(nextForwardLayer_)
	{
	    layers.push_back(nextForwardLayer_);
	}
    }
    else
    {
	if(geometry_->previousForwardLayer(nextForwardLayer_))
	{
	    layers.push_back(geometry_->previousForwardLayer(nextForwardLayer_));
	}
    }
    
    double deltaTime = -1;
    for(auto _layer : layers)
    {
	double tempDeltaTime = trajectory->nextCrossingTimeC(*_layer);
	LogDebug(MESSAGECATEGORY) << "   particle crosses layer " << *_layer << " at time " << tempDeltaTime;
	if(tempDeltaTime > 0 && (layer == 0 || tempDeltaTime< deltaTime))
	{
	    layer = _layer;
	    deltaTime = tempDeltaTime;
	}
    }
    
    // move particle in space, time and momentum
    if(layer)
    {
	trajectory->move(deltaTime);
	particle.setPosition(trajectory->getPosition());
	particle.setMomentum(trajectory->getMomentum());
	LogDebug(MESSAGECATEGORY) << "    moved particle to layer: " << *layer;
    }

    // return true / false if propagations succeeded /failed
    LogDebug(MESSAGECATEGORY) << "    success: " << bool(layer);
    return layer;
}

	
