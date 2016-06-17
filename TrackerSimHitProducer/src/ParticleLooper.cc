#include "FastSimulation/TrackerSimHitProducer/interface/ParticleLooper.h"

#include "HepMC/GenEvent.h"
#include "HepMC/Units.h"

#include "FastSimulation/NewParticle/interface/Particle.h"
#include "HepPDT/ParticleDataTable.hh"
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

fastsim::ParticleLooper::ParticleLooper(
    const HepMC::GenEvent & genEvent,
    const HepPDT::ParticleDataTable & particleDataTable,
    double beamPipeRadius,
    std::unique_ptr<std::vector<SimTrack> > & simTracks,
    std::unique_ptr<std::vector<SimVertex> > & simVertices)
    : genEvent_(&genEvent)
    , genParticleIterator_(genEvent_->particles_begin())
    , genParticleEnd_(genEvent_->particles_end())
    , genParticleIndex_(0)
    , particleDataTable_(&particleDataTable)
    , beamPipeRadius2_(beamPipeRadius*beamPipeRadius)
    , simTracks_(std::move(simTracks))
    , simVertices_(std::move(simVertices))
    // prepare unit convsersions
    //  --------------------------------------------
    // |          |      hepmc               |  cms |
    //  --------------------------------------------
    // | length   | genEvent_->length_unit   |  cm  |
    // | momentum | genEvent_->momentum_unit |  GeV |
    // | time     | length unit (t*c)        |  ns  |
    //  --------------------------------------------
    , momentumUnitConversionFactor_(conversion_factor( genEvent_->momentum_unit(), HepMC::Units::GEV ))
    , lengthUnitConversionFactor_(conversion_factor(genEvent_->length_unit(),HepMC::Units::LengthUnit::CM))
    , lengthUnitConversionFactor2_(lengthUnitConversionFactor_*lengthUnitConversionFactor_)
    , timeUnitConversionFactor_(lengthUnitConversionFactor_/29.9792458) // speed of light [cm / ns]
{
    // add the main vertex from the signal event to the simvertex collection
    if(genEvent.vertices_begin() != genEvent_->vertices_end())
    {
	const HepMC::FourVector & position = (*genEvent.vertices_begin())->position();
	addSimVertex(math::XYZTLorentzVector(position.x()*lengthUnitConversionFactor_,
					     position.y()*lengthUnitConversionFactor_,
					     position.z()*lengthUnitConversionFactor_,
					     position.t()*timeUnitConversionFactor_)
		     ,-1);
    }
}

fastsim::ParticleLooper::~ParticleLooper(){}

std::unique_ptr<fastsim::Particle> fastsim::ParticleLooper::nextParticle()
{
    if(particleBuffer_.size() > 0)
    {
	auto particle = std::move(particleBuffer_.back());
	particleBuffer_.pop_back();
	return move(particle);
    }
    else
    {
	return nextGenParticle();
    }
}

// what about the lifetime of the secondaries? => draw numbers here, use particle data
// what about decayer -> should provide particles with right units
// what about nuclear interactions: do we get the full 4-vector from the interaction model
void fastsim::ParticleLooper::addSecondaries(
    int motherSimTrackId,
    const math::XYZTLorentzVector vertexPosition,
    std::vector<std::unique_ptr<Particle> > secondaries)
{
    unsigned vertexIndex = addSimVertex(vertexPosition,motherSimTrackId);
    for(auto & secondary : secondaries)
    {
	// FIX ME !!! ALSO FIX VERTEX SELECTION
	//if(particleFilter_.selects(secondary))
	//{
	addSimTrack(secondary->pdgId(),secondary->position(),vertexIndex);
	particleBuffer_.push_back(std::move(secondary));
	//}
    }
}

unsigned fastsim::ParticleLooper::addSimVertex(
    const math::XYZTLorentzVector & position,
    int motherIndex)
{
    int vertexIndex = simVertices_->size();
    simVertices_->emplace_back(position.Vect(),
			      position.T(),
			      motherIndex,
			      vertexIndex);
    return vertexIndex;
}

void fastsim::ParticleLooper::addSimTrack(
    int pdgId,
    const math::XYZTLorentzVector & position,
    int vertexIndex,
    int genParticleIndex)
{
    simTracks_->emplace_back(pdgId,position,vertexIndex,genParticleIndex);
}

std::unique_ptr<fastsim::Particle> fastsim::ParticleLooper::nextGenParticle()
{
    // only consider particles that start in the beam pipe and end outside the beam pipe
    // try to get the decay time from pythia
    // use hepmc units
    // make the link simtrack to simvertex
    // try not to change the simvertex structure
    // print a couple of ttbar events to undertand the simtrack structure? nah...
    
    for ( ; genParticleIterator_ != genParticleEnd_ ; ++genParticleIterator_,++genParticleIndex_ ) // loop over gen particles
    {
	// some handy pointers and references
	const HepMC::GenParticle & particle = **genParticleIterator_;
	const HepMC::GenVertex & productionVertex = *particle.production_vertex();
	const HepMC::GenVertex * endVertex = particle.end_vertex();
	
	// particle must be produced within the beampipe
	// UNITS!!!
	if(productionVertex.position().perp2()*lengthUnitConversionFactor2_ > beamPipeRadius2_)
	{
	    continue;
	}
	
	// particle must not decay before it reaches the beam pipe
	// UNITS!!!
	if(endVertex && endVertex->position().perp2()*lengthUnitConversionFactor2_ < beamPipeRadius2_)
	{
	    continue;
	}
	
	// retrieve the particle data
	const HepPDT::ParticleData * particleData = particleDataTable_->particle( particle.pdg_id() );
	if(!particleData)
	{
	    throw cms::Exception("fastsim::ParticleLooper") << "unknown pdg id" << std::endl;
	}
	
	// !!! TAKE PROPER CARE OF UNITS
	
	// try to get the life time of the particle from the genEvent
	double properLifeTime = -1.;
	if(endVertex)
	{
	    // UNITS !!!
	    double labFrameLifeTime = (endVertex->position().t() - productionVertex.position().t())*timeUnitConversionFactor_;
	    properLifeTime = labFrameLifeTime * particle.momentum().m() / particle.momentum().e();
	}
	else
	{
	    /// UNITS !!!
	    // draw a number from ...?
	}
	
	// make the particle
	std::unique_ptr<Particle> newParticle(
	    new Particle(particle.pdg_id(),particleData->charge(),
			 math::XYZTLorentzVector(), // fill in 
			 math::XYZTLorentzVector(),  // fill in
			 properLifeTime));
	// make the simtrack
	addSimTrack(newParticle->pdgId(),newParticle->momentum(),0,genParticleIndex_);
	
	// and return
	return std::move(newParticle);
    }
    
    return std::unique_ptr<Particle>();
}
