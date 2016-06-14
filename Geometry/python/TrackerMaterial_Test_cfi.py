import FWCore.ParameterSet.Config as cms

# Material effects to be simulated in the tracker material and associated cuts 
TrackerMaterialBlock = cms.PSet(
    TrackerMaterial = cms.PSet(
        magneticFieldZ = cms.double(0.),
        useTrackerRecoGeometryRecord = cms.untracked.bool(False),
        trackerAlignmentLabel = cms.string("MisAligned"),
        interactionModels = cms.PSet(
            simpleLayerHits = cms.PSet(
                className = cms.string("SimpleLayerHitProducer")
                )
            ),
        BarrelLayers = cms.VPSet(
            cms.PSet(
                radius = cms.untracked.double(10.),
                limits = cms.untracked.vdouble(0.0,1.),
                thickness = cms.untracked.vdouble(0.01),
                interactionModels = cms.untracked.vstring("simpleLayerHits")
                ),
            cms.PSet(
                radius = cms.untracked.double(50.),
                limits = cms.untracked.vdouble(0.0,5.),
                thickness = cms.untracked.vdouble(0.05),
                interactionModels = cms.untracked.vstring("simpleLayerHits")
                ),
            cms.PSet(
                radius = cms.untracked.double(100.),
                limits = cms.untracked.vdouble(0.0,100.),
                thickness = cms.untracked.vdouble(0.1),
                interactionModels = cms.untracked.vstring("simpleLayerHits")
                ),
            ),
        ForwardLayers = cms.VPSet(
            cms.PSet(
                z = cms.untracked.double(10.),
                limits = cms.untracked.vdouble(0.0,1.),
                thickness = cms.untracked.vdouble(0.01),
                interactionModels = cms.untracked.vstring("simpleLayerHits")
                ),
            cms.PSet(
                z = cms.untracked.double(50.),
                limits = cms.untracked.vdouble(0.0,5.),
                thickness = cms.untracked.vdouble(0.05),
                interactionModels = cms.untracked.vstring("simpleLayerHits")
                ),
            cms.PSet(
                z = cms.untracked.double(100.),
                limits = cms.untracked.vdouble(0.0,100.),
                thickness = cms.untracked.vdouble(0.1),
                interactionModels = cms.untracked.vstring("simpleLayerHits")
                ),
            ),
        )
    )
