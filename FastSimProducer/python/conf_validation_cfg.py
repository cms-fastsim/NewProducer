import FWCore.ParameterSet.Config as cms

from Configuration.StandardSequences.Eras import eras

process = cms.Process("DEMO",eras.Run2_2016,eras.fastSim)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(10)
)

# load particle data table
process.load("SimGeneral.HepPDTESSource.pythiapdt_cfi")
# load geometry
process.load('FastSimulation.Configuration.Geometries_MC_cff')
# load magnetic field
process.load('Configuration.StandardSequences.MagneticField_cff')
#load and set conditions (required by geometry and magnetic field)
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')          

# read generator event from file
process.source = cms.Source("PoolSource",
    dropDescendantsOfDroppedBranches = cms.untracked.bool(False),
    fileNames = cms.untracked.vstring('file:gen_muGun.root'),
    inputCommands = cms.untracked.vstring('keep *', 
        'drop *_genParticles_*_*', 
        'drop *_genParticlesForJets_*_*', 
        'drop *_kt4GenJets_*_*', 
        'drop *_kt6GenJets_*_*', 
        'drop *_iterativeCone5GenJets_*_*', 
        'drop *_ak4GenJets_*_*', 
        'drop *_ak7GenJets_*_*', 
        'drop *_ak8GenJets_*_*', 
        'drop *_ak4GenJetsNoNu_*_*', 
        'drop *_ak8GenJetsNoNu_*_*', 
        'drop *_genCandidatesForMET_*_*', 
        'drop *_genParticlesForMETAllVisible_*_*', 
        'drop *_genMetCalo_*_*', 
        'drop *_genMetCaloAndNonPrompt_*_*', 
        'drop *_genMetTrue_*_*', 
        'drop *_genMetIC5GenJs_*_*'),
    secondaryFileNames = cms.untracked.vstring()
)

# configure random number generator for simhit production
process.load('Configuration.StandardSequences.Services_cff')
process.RandomNumberGeneratorService = cms.Service(
    "RandomNumberGeneratorService",
    fastSimProducer = cms.PSet(
        initialSeed = cms.untracked.uint32(234567),
        engineName = cms.untracked.string('TRandom3')
        ),
    siTrackerGaussianSmearingRecHits = cms.PSet(
         initialSeed = cms.untracked.uint32(24680),
         engineName = cms.untracked.string('TRandom3')
     ),
    mix = cms.PSet(
         initialSeed = cms.untracked.uint32(12345),
         engineName = cms.untracked.string('HepJamesRandom')
     ),
    simMuonCSCDigis = cms.PSet(
         initialSeed = cms.untracked.uint32(11223344),
         engineName = cms.untracked.string('HepJamesRandom')
     ),
    )

# output
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('FastSimulation.Configuration.SimIdeal_cff')
process.load('FastSimulation.Configuration.Reconstruction_BefMix_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('FastSimulation.Configuration.Reconstruction_AftMix_cff')
process.load('Configuration.StandardSequences.Validation_cff')

# load simhit producer
process.load("FastSimulation.FastSimProducer.fastSimProducer_cff")

# Output definition
process.FEVTDEBUGHLTEventContent.outputCommands.append(
        'keep *_fastSimProducer_*_*',
    )

# Output definition
process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('GEN-SIM-DIGI-RECO'),
        filterName = cms.untracked.string('')
    ),
    eventAutoFlushCompressedSize = cms.untracked.int32(10485760),
    fileName = cms.untracked.string('dqm_fastsim.root'),
    outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

process.DQMoutput = cms.OutputModule("DQMRootOutputModule",
    dataset = cms.untracked.PSet(
        dataTier = cms.untracked.string('DQMIO'),
        filterName = cms.untracked.string('')
    ),
    fileName = cms.untracked.string('dqm_fastsim_inDQM.root'),
    outputCommands = process.DQMEventContent.outputCommands,
    splitLevel = cms.untracked.int32(0)
)

# use new TrackerSimHitProducer
process.siTrackerGaussianSmearingRecHits.InputSimHits = cms.InputTag("fastSimProducer","TrackerHits")
process.fastMatchedTrackerRecHits.simHits = cms.InputTag("fastSimProducer","TrackerHits")
process.fastMatchedTrackerRecHitCombinations.simHits = cms.InputTag("fastSimProducer","TrackerHits")
process.simMuonCSCDigis.InputCollection = cms.string("MuonCSCHits")
process.simMuonCSCDigis.mixLabel = cms.string("fastSimProducer")

# Path and EndPath definitions
process.simulation_step = cms.Path(process.fastSimProducer)
process.reconstruction_befmix_step = cms.Path(process.reconstruction_befmix)
process.digitisation_step = cms.Path(process.pdigi_valid)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.validation_step = cms.EndPath(process.tracksValidationTrackingOnly)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)
process.DQMoutput_step = cms.EndPath(process.DQMoutput)

# Schedule definition
#process.schedule = cms.Schedule(process.simulation_step,process.FEVTDEBUGHLToutput_step,process.DQMoutput_step)
process.schedule = cms.Schedule(process.simulation_step,process.reconstruction_befmix_step,process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.L1Reco_step,process.reconstruction_step,process.validation_step,process.FEVTDEBUGHLToutput_step,process.DQMoutput_step)

# debugging options
# debug messages will only be printed for packages compiled with following command
# USER_CXXFLAGS="-g -D=EDM_ML_DEBUG" scram b -v # for bash
#process.load("FWCore.MessageLogger.MessageLogger_cfi")
#process.MessageLogger = cms.Service(
#    "MessageLogger",
#    destinations = cms.untracked.vstring('cout'),
#    cout = cms.untracked.PSet(
#        threshold = cms.untracked.string('DEBUG'),
#        ),
#    debugModules = cms.untracked.vstring('fastSimProducer')
#    )

