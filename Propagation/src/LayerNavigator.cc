#include "FastSimulation/Propagation/interface/LayerNavigator.h"

#include "vector"

#include "FastSimulation/Particle/interface/RawParticle.h"
#include "FastSimulation/Geometry/interface/Geometry.h"
#include "FastSimulation/Geometry/interface/BarrelLayer.h"
#include "FastSimulation/Geometry/interface/ForwardLayer.h"
#include "FastSimulation/Propagation/interface/LayerNavigator.h"
#include "FastSimulation/Propagation/interface/Trajectory.h"      // new class, to be defined, based on ParticlePropagator

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

const fastsim::Layer * fastsim::LayerNavigator::moveToNextLayer(RawParticle & particle,const Geometry & geometry,double magneticFieldZ) const
{

    // caclulate and store some variables related to the particle's trajectory
    std::unique_ptr<fastsim::Trajectory> trajectory = Trajectory::createTrajectory(particle,magneticFieldZ);
    
    /*
      find the barrel layers that enclose the particle
      i.e. closest barrel layers with 
      - layer.r < particle.r  
      - layer.r > particle.r
      assume barrel layers are ordered with increasing r
      only consider layers that are crossed by the trajectory
      only consider layers that have material in the z range of the trajectory
    */

    // 
    bool negativeRSpeed = particle.px()*particle.x() + particle.py()*particle.y() < 0;
    auto innerBarrelLayer = geometry.barrelLayers().end();
    auto outerBarrelLayer = geometry.barrelLayers().end();
    for(auto layer = geometry.barrelLayers().begin(); layer != geometry.barrelLayers().end(); ++layer)
    {
	// only consider layers that have material beyond particle.z(), 
	// (taking into account the direction of the particle)
	if( (particle.pz() > 0 && particle.z() > layer->materialMaxAbsZ()) ||
	    (particle.pz() < 0 && particle.z() < -layer->materialMaxAbsZ()))
	{
	    continue;
	}
	// only consier layers that are crossed by the particle
	if(trajectory->crosses(*layer))
	{
	    continue;
	}
	if(particle.r() < layer->getRadius() || (negativeRSpeed && particle.r() <= layer->getRadius()))
	{
	    outerBarrelLayer = layer;
	    break;
	}
	else
	{
	    innerBarrelLayer = layer;
	}
    }

    /*
      find the closest forward layer towards which the particle moves
      only consider layers with material in the r-region of the trajectory
      only consider layers that have material betweeen innerBarrel and outerBarrel
    */
    
    auto forwardLayer = geometry.forwardLayers().end(); // iterator to next forward layer
    if(particle.pz()!=0)
    {
	for(auto layer = geometry.forwardLayers().begin(); layer != geometry.forwardLayers().end() ; ++layer)
	{
	    // only consider layers that have material betweeen innerBarrel and outerBarrel
	    if( (innerBarrelLayer != geometry.barrelLayers().end() && layer->getMaxMaterialR() < innerBarrelLayer->getRadius()) ||
		(outerBarrelLayer != geometry.barrelLayers().end() && layer->getMinMaterialR() > outerBarrelLayer->getRadius()))
	    {
		continue;
	    }
	    //only consider layers with material in the r-region of the trajectory
	    if(trajectory->crossesMaterial(*layer))
	    {
		continue;
	    }
	    if(particle.pz() < 0)  // case particle moves negative direction
	    {
		if(particle.z() <= layer->getZ())
		{
		    break;
		}
		else
		{
		    forwardLayer = layer;
		}
	    }
	    else               // case particle moves positive direction
	    {
		if(particle.z() < layer->getZ())
		{
		    forwardLayer = layer;
		    break;
		}
	    }
	}
    }
    
    /* 
       find earliest intersection with time > particle.t
    */
    
    std::vector<const fastsim::Layer*> layers;
    layers.push_back(forwardLayer == geometry.forwardLayers().end() ? 0 : &(*forwardLayer));
    const fastsim::Layer * crossedLayer = 0;
    double deltaTime = -1;
    for(auto layer : layers)
    {
	if(layer)
	{
	    double tempDeltaTime = trajectory->nextCrossingTimeC(*layer);
	    if(tempDeltaTime > 0 && (crossedLayer == 0 || tempDeltaTime< deltaTime))
	    {
		crossedLayer = layer;
		deltaTime = tempDeltaTime;
	    }
	}
    }

    /* 
       move particle in space, time and momentum
    */
    if(crossedLayer)
    {
	trajectory->move(deltaTime);
	particle.setVertex(trajectory->getPosition());
	particle.SetXYZT(trajectory->getMomentum().Px(),trajectory->getMomentum().Py(),trajectory->getMomentum().Pz(),trajectory->getMomentum().E());
    }

    /*
      return the crossed layer
      returning null pointer means it makes no sense to move the particle any further
     */
    return crossedLayer;
}

