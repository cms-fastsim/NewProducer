import FWCore.ParameterSet.Config as cms

from FastSimulation.Event.ParticleFilter_cfi import  ParticleFilterBlock
from FastSimulation.Geometry.TrackerMaterial_cfi import TrackerMaterialBlock

trackerSimHits = cms.EDProducer(
    "TrackerSimHitProducer",
    alignmentLabel = cms.string("MisAligned"),
    src = cms.InputTag("generatorSmeared"),
    particleFilter =  ParticleFilterBlock.ParticleFilter,
    detectorLayers = TrackerMaterialBlock.TrackerMaterial
    )
