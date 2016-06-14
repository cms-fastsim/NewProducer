import FWCore.ParameterSet.Config as cms

from FastSimulation.Event.ParticleFilter_cfi import  ParticleFilterBlock
from FastSimulation.Geometry.TrackerMaterial_Test_cfi import TrackerMaterialBlock

trackerSimHits = cms.EDProducer(
    "TrackerSimHitProducer",
    src = cms.InputTag("generatorSmeared"),
    particleFilter =  ParticleFilterBlock.ParticleFilter,
    detectorDefinition = TrackerMaterialBlock.TrackerMaterial,
    )
