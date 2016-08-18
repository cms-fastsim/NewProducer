import FWCore.ParameterSet.Config as cms

from FastSimulation.Event.ParticleFilter_cfi import  ParticleFilterBlock
from FastSimulation.Geometry.TrackerMaterial_Test_cfi import TrackerMaterialBlock

fastSimProducer = cms.EDProducer(
    "FastSimProducer",
    src = cms.InputTag("generatorSmeared"),
    particleFilter =  ParticleFilterBlock.ParticleFilter,
    detectorDefinition = TrackerMaterialBlock.TrackerMaterial,
    beamPipeRadius = cms.double(0.),
    interactionModels = cms.PSet(
            simpleLayerHits = cms.PSet(
                className = cms.string("fastsim::SimpleLayerHitProducer")
                ),
            bremsstrahlung = cms.PSet(
                className = cms.string("fastsim::Bremsstrahlung"),
                minPhotonEnergy = cms.double(0.1),
                minPhotonEnergyFraction = cms.double(0.005)
                )
            ),
    )
