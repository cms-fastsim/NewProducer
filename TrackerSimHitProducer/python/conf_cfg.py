import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process("DEMO",eras.Run2_2016,eras.fastSim)

# load particle data table
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
# load geometry
process.load('FastSimulation.Configuration.Geometries_MC_cff')
# load magnetic field
process.load('Configuration.StandardSequences.MagneticField_cff')
# load and set conditions (required by geometry and magnetic field)
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')          

# read generator event from file
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:gen.root'),
)

# configure random number generator for simhit production
process.RandomNumberGeneratorService = cms.Service(
    "RandomNumberGeneratorService",
    trackerSimHits = cms.PSet(
        initialSeed = cms.untracked.uint32(234567),
        engineName = cms.untracked.string('TRandom3')
        )
    )

# load simhit producer
process.load("FastSimulation.TrackerSimHitProducer.trackerSimHits_cff")

# define a path to run
process.demo = cms.Path(process.trackerSimHits)

