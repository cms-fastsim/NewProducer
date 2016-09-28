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
#process.RandomNumberGeneratorService = cms.Service(
#    "RandomNumberGeneratorService",
#    fastSimProducer = cms.PSet(
#        initialSeed = cms.untracked.uint32(234567),
#        engineName = cms.untracked.string('TRandom3')
#        ),
#    siTrackerGaussianSmearingRecHits = cms.PSet(
#         initialSeed = cms.untracked.uint32(24680),
#         engineName = cms.untracked.string('TRandom3')
#     ),
#    mix = cms.PSet(
#         initialSeed = cms.untracked.uint32(12345),
#         engineName = cms.untracked.string('HepJamesRandom')
#     ),
#    simMuonCSCDigis = cms.PSet(
#         initialSeed = cms.untracked.uint32(11223344),
#         engineName = cms.untracked.string('HepJamesRandom')
#     ),
#    )

# How can I append the fastSimProducer = cms.PSet(...) to process.RandomNumberGeneratorService instead of copying everything?
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
    fastSimProducer = cms.PSet(
        initialSeed = cms.untracked.uint32(234567),
        engineName = cms.untracked.string('TRandom3')
    ),
    externalLHEProducer = cms.PSet(
        initialSeed = cms.untracked.uint32(234567),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    generator = cms.PSet(
        initialSeed = cms.untracked.uint32(123456789),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    VtxSmeared = cms.PSet(
        initialSeed = cms.untracked.uint32(98765432),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    LHCTransport = cms.PSet(
        initialSeed = cms.untracked.uint32(87654321),
        engineName = cms.untracked.string('TRandom3')
    ),
    hiSignalLHCTransport = cms.PSet(
        initialSeed = cms.untracked.uint32(88776655),
        engineName = cms.untracked.string('TRandom3')
    ),
    g4SimHits = cms.PSet(
        initialSeed = cms.untracked.uint32(11),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    mix = cms.PSet(
        initialSeed = cms.untracked.uint32(12345),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    mixData = cms.PSet(
        initialSeed = cms.untracked.uint32(12345),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    simSiStripDigiSimLink = cms.PSet(
        initialSeed = cms.untracked.uint32(1234567),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    simMuonDTDigis = cms.PSet(
        initialSeed = cms.untracked.uint32(1234567),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    simMuonCSCDigis = cms.PSet(
        initialSeed = cms.untracked.uint32(11223344),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    simMuonRPCDigis = cms.PSet(
        initialSeed = cms.untracked.uint32(1234567),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    hiSignal = cms.PSet(
        initialSeed = cms.untracked.uint32(123456789),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    hiSignalG4SimHits = cms.PSet(
        initialSeed = cms.untracked.uint32(11),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    famosPileUp = cms.PSet(
        initialSeed = cms.untracked.uint32(918273),
        engineName = cms.untracked.string('TRandom3')
    ),
    mixGenPU = cms.PSet(
        initialSeed = cms.untracked.uint32(918273), # intentionally the same as famosPileUp
        engineName = cms.untracked.string('TRandom3')
    ),    
    mixSimCaloHits = cms.PSet(
         initialSeed = cms.untracked.uint32(918273), 
         engineName = cms.untracked.string('TRandom3')
    ),
    mixRecoTracks = cms.PSet(
         initialSeed = cms.untracked.uint32(918273), 
         engineName = cms.untracked.string('TRandom3')
    ),                                           
    famosSimHits = cms.PSet(
        initialSeed = cms.untracked.uint32(13579),
        engineName = cms.untracked.string('TRandom3')
    ),
    siTrackerGaussianSmearingRecHits = cms.PSet(
        initialSeed = cms.untracked.uint32(24680),
        engineName = cms.untracked.string('TRandom3')
    ),
    ecalRecHit = cms.PSet(
        initialSeed = cms.untracked.uint32(654321),
        engineName = cms.untracked.string('TRandom3')
    ),
    ecalPreshowerRecHit = cms.PSet(
        initialSeed = cms.untracked.uint32(6541321),
        engineName = cms.untracked.string('TRandom3')
    ),
    hbhereco = cms.PSet(
        initialSeed = cms.untracked.uint32(541321),
        engineName = cms.untracked.string('TRandom3')
    ),
    horeco = cms.PSet(
        initialSeed = cms.untracked.uint32(541321),
        engineName = cms.untracked.string('TRandom3')
    ),
    hfreco = cms.PSet(
        initialSeed = cms.untracked.uint32(541321),
        engineName = cms.untracked.string('TRandom3')
    ),    
    paramMuons = cms.PSet(
        initialSeed = cms.untracked.uint32(54525),
        engineName = cms.untracked.string('TRandom3')
    ),
    l1ParamMuons = cms.PSet(
        initialSeed = cms.untracked.uint32(6453209),
        engineName = cms.untracked.string('TRandom3')
    ),
    MuonSimHits = cms.PSet(
        initialSeed = cms.untracked.uint32(987346),
        engineName = cms.untracked.string('TRandom3')
    ),
    simBeamSpotFilter = cms.PSet(
        initialSeed = cms.untracked.uint32(87654321),
        engineName = cms.untracked.string('HepJamesRandom')
    ),
    saveFileName = cms.untracked.string('')
)

# Remaining Steps
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


# use new TrackerSimHitProducer
process.siTrackerGaussianSmearingRecHits.InputSimHits = cms.InputTag("fastSimProducer","TrackerHits")
process.fastMatchedTrackerRecHits.simHits = cms.InputTag("fastSimProducer","TrackerHits")
process.fastMatchedTrackerRecHitCombinations.simHits = cms.InputTag("fastSimProducer","TrackerHits")
process.simMuonCSCDigis.InputCollection = cms.string("fastSimProducerMuonCSCHits")

process.theMixObjects.mixCH.input = cms.VInputTag(cms.InputTag("fastSimProducer","EcalHitsEB"), cms.InputTag("fastSimProducer","EcalHitsEE"), cms.InputTag("fastSimProducer","EcalHitsES"), cms.InputTag("fastSimProducer","HcalHits"))
process.theMixObjects.mixSH.input = cms.VInputTag(cms.InputTag("fastSimProducer","MuonCSCHits"), cms.InputTag("fastSimProducer","MuonDTHits"), cms.InputTag("fastSimProducer","MuonRPCHits"), cms.InputTag("fastSimProducer","TrackerHits"))
process.theMixObjects.mixTracks.input = cms.VInputTag(cms.InputTag("fastSimProducer"))
process.theMixObjects.mixVertices.input = cms.VInputTag(cms.InputTag("fastSimProducer"))

process.mixSimHits.input = cms.VInputTag(cms.InputTag("fastSimProducer","MuonCSCHits"), cms.InputTag("fastSimProducer","MuonDTHits"), cms.InputTag("fastSimProducer","MuonRPCHits"), cms.InputTag("fastSimProducer","TrackerHits"))

process.theDigitizersValid.mergedtruth.simHitCollections = cms.PSet(
        muon = cms.VInputTag(cms.InputTag("fastSimProducer","MuonDTHits"), cms.InputTag("fastSimProducer","MuonCSCHits"), cms.InputTag("fastSimProducer","MuonRPCHits")),
        trackerAndPixel = cms.VInputTag(cms.InputTag("fastSimProducer","TrackerHits"))
    )
process.theDigitizersValid.mergedtruth.simTrackCollection = cms.InputTag("fastSimProducer")
process.theDigitizersValid.mergedtruth.simVertexCollection = cms.InputTag("fastSimProducer")
process.theDigitizersValid.ecal.hitsProducer = cms.string('fastSimProducer')
process.theDigitizersValid.hcal.hitsProducer = cms.string('fastSimProducer')

process.mix.hitsProducer = cms.string('fastSimProducer')
process.mix.mixObjects  = cms.PSet(process.theMixObjects)
process.mix.digitizers = cms.PSet(process.theDigitizersValid)

process.trackingParticles.simHitCollections = cms.PSet(
        muon = cms.VInputTag(cms.InputTag("fastSimProducer","MuonDTHits"), cms.InputTag("fastSimProducer","MuonCSCHits"), cms.InputTag("fastSimProducer","MuonRPCHits")),
        trackerAndPixel = cms.VInputTag(cms.InputTag("fastSimProducer","TrackerHits"))
    )
process.simHitTPAssocProducer.simHitSrc = cms.VInputTag(cms.InputTag("fastSimProducer","TrackerHits"), cms.InputTag("fastSimProducer","MuonCSCHits"), cms.InputTag("fastSimProducer","MuonDTHits"), cms.InputTag("fastSimProducer","MuonRPCHits"))
process.simMuonDTDigis.InputCollection = cms.string('fastSimProducerMuonDTHits')
process.simMuonRPCDigis.InputCollection = cms.string('fastSimProducerMuonRPCHits')



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

#process.content=cms.EDAnalyzer('EventContentAnalyzer')
#process.content_step = cms.Path(process.content)

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