#include "FastSimulation/ParticlePropagator/interface/ParticlePropagator.h"
#include "FastSimulation/ParticleDecay/interface/PythiaDecays.h"
#include "FWCore/ServiceRegistry/interface/RandomEngineSentry.h"

#include <Pythia8/Pythia.h>
#include "Pythia8Plugins/HepMC2.h"

PythiaDecays::PythiaDecays()
{
    // inspired by method Pythia8Hadronizer::residualDecay() in GeneratorInterface/Pythia8Interface/src/Py8GunBase.cc
    p8_.reset(new Pythia8::Pythia);
    p8RndmEngine.reset(new gen::P8RndmEngine);
    p8_->setRndmEnginePtr(p8RndmEngine.get());
    p8_->settings.flag("ProcessLevel:all",false);
    p8_->settings.flag("PartonLevel:FSRinResonances",false);
    p8_->settings.flag("ProcessLevel:resonanceDecays",false);
    p8_->init();

    // forbid all decays
    // (decays are allowed selectively in the particleDaughters function)
    Pythia8::ParticleData & pdt = p8_->particleData;
    int pid = 1;
    while(pdt.nextId(pid) > pid){
      pid = pdt.nextId(pid);
      pdt.mayDecay(pid,false);
    }
}

const DaughterParticleList&
PythiaDecays::particleDaughters(ParticlePropagator& particle, CLHEP::HepRandomEngine* engine)
{
  edm::RandomEngineSentry<gen::P8RndmEngine> sentry(p8RndmEngine.get(), engine);

  secondaries.clear();

  // inspired by method Pythia8Hadronizer::residualDecay() in GeneratorInterface/Pythia8Interface/src/Py8GunBase.cc
  int pid = particle.pid();
  p8_->event.reset();
  Pythia8::Particle py8part( pid , 93, 0, 0, 0, 0, 0, 0,
		     particle.momentum().x(), // note: momentum().x() and Px() are the same
		     particle.momentum().y(),
		     particle.momentum().z(),
		     particle.momentum().t(),
		     particle.mass() );
  py8part.vProd( particle.X(), particle.Y(), 
		 particle.Z(), particle.T() );
  p8_->event.append( py8part );

  int nentries_before = p8_->event.size();
  p8_->particleData.mayDecay(pid,true);   // switch on the decay of this and only this particle (avoid double decays)
  p8_->next();                           // do the decay
  p8_->particleData.mayDecay(pid,false);  // switch it off again
  int nentries_after = p8_->event.size();
  if ( nentries_after <= nentries_before ) return secondaries;

  secondaries.resize(nentries_after - nentries_before,RawParticle());


  for ( int ipart=nentries_before; ipart<nentries_after; ipart++ )
    {
      Pythia8::Particle& py8daughter = p8_->event[ipart];
      secondaries[ipart-nentries_before].SetXYZT( py8daughter.px(), py8daughter.py(), py8daughter.pz(), py8daughter.e() );
      secondaries[ipart-nentries_before].setVertex( py8daughter.xProd(),
					   py8daughter.yProd(),
					   py8daughter.zProd(),
					   py8daughter.tProd() );
      secondaries[ipart-nentries_before].setID( py8daughter.id() );
      secondaries[ipart-nentries_before].setMass( py8daughter.m() );
    }

  return;
}
