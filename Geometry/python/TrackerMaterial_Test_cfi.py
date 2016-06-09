import FWCore.ParameterSet.Config as cms

# Material effects to be simulated in the tracker material and associated cuts 
TrackerMaterialBlock = cms.PSet(
    TrackerMaterial = cms.PSet(
        BarrelLayers = cms.VPSet(
            cms.PSet(
                radius = cms.untracked.double(10.),
                limits = cms.untracked.vdouble(0.0,1.),
                thickness = cms.untracked.vdouble(0.01)
                ),
            cms.PSet(
                radius = cms.untracked.double(50.),
                limits = cms.untracked.vdouble(0.0,5.),
                thickness = cms.untracked.vdouble(0.05)
                ),
            cms.PSet(
                radius = cms.untracked.double(100.),
                limits = cms.untracked.vdouble(0.0,100.),
                thickness = cms.untracked.vdouble(0.1),
                ),
            ),
        ForwardLayers = cms.VPSet(
            cms.PSet(
                z = cms.untracked.double(10.),
                limits = cms.untracked.vdouble(0.0,1.),
                thickness = cms.untracked.vdouble(0.01)
                ),
            cms.PSet(
                z = cms.untracked.double(50.),
                limits = cms.untracked.vdouble(0.0,5.),
                thickness = cms.untracked.vdouble(0.05)
                ),
            cms.PSet(
                z = cms.untracked.double(100.),
                limits = cms.untracked.vdouble(0.0,100.),
                thickness = cms.untracked.vdouble(0.1),
                ),
            ),
        )
    )
