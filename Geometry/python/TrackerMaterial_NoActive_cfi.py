import FWCore.ParameterSet.Config as cms

# Material effects to be simulated in the tracker material and associated cuts 
TrackerMaterialBlock = cms.PSet(
    TrackerMaterial = cms.PSet(
        BarrelLayers = cms.VPSet(
            ########### Beam Pipe ###########
            #PIPE
            cms.PSet(
                radius = cms.untracked.double(3.003),
                limits = cms.untracked.vdouble(0.0, 28.3),
                thickness = cms.untracked.vdouble(0.0024)
            ),
            ########### The Pixel Barrel layers 1-3 ###########
            #PIXB1
            cms.PSet(
                radius = cms.untracked.double(4.425),
                limits = cms.untracked.vdouble(0.0, 28.391),
                thickness = cms.untracked.vdouble(0.0217),
            ),
            #PIXB2
            cms.PSet(
                radius = cms.untracked.double(7.312),
                limits = cms.untracked.vdouble(0.0, 28.391),
                thickness = cms.untracked.vdouble(0.0217),
            ),
            #PIXB3
            cms.PSet(
                radius = cms.untracked.double(10.177),
                limits = cms.untracked.vdouble(0.0, 28.391),
                thickness = cms.untracked.vdouble(0.0217),
            ),
            ########### Pixel Outside walls and cables (barrel) ###########
            #PIXBOut5
            cms.PSet(
                radius = cms.untracked.double(17.6),
                limits = cms.untracked.vdouble(0.0, 27.5, 32.0, 65.0),
                thickness = cms.untracked.vdouble(0.0135, 0.095, 0.050)
            ),
            ########### Tracker Inner barrel layers 1-4 ###########
            #TIB1
            cms.PSet(
                radius = cms.untracked.double(25.767),
                limits = cms.untracked.vdouble(0.0, 35.0, 65.254),
                thickness = cms.untracked.vdouble(0.053, 0.0769),
            ),
            #TIB2
            cms.PSet(
                radius = cms.untracked.double(34.104),
                limits = cms.untracked.vdouble(0.0, 35.0, 65.231),
                thickness = cms.untracked.vdouble(0.053, 0.0769),
            ),
            #TIB3
            cms.PSet(
                radius = cms.untracked.double(41.974),
                limits = cms.untracked.vdouble(0.0, 35.0, 66.232),
                thickness = cms.untracked.vdouble(0.035, 0.0508),
            ),
            #TIB4
            cms.PSet(
                radius = cms.untracked.double(49.907),
                limits = cms.untracked.vdouble(0.0, 35.0, 66.355),
                thickness = cms.untracked.vdouble(0.04, 0.058),
            ),
            ########### TOB inside wall (barrel) ###########
            #TOBCIn
            cms.PSet(
                radius = cms.untracked.double(55.1),
                limits = cms.untracked.vdouble(0.0, 27.5, 30.5, 72.0, 108.2),
                thickness = cms.untracked.vdouble(0.009, 0.036, 0.009, 0.0495)
            ),
            ########### # Tracker Outer barrel layers 1-6 ###########
            #TOB1
            cms.PSet(
                radius = cms.untracked.double(60.937),
                limits = cms.untracked.vdouble(0.0, 18.0, 30.0, 36.0, 46.0, 55.0, 108.737),
                thickness = cms.untracked.vdouble(0.021, 0.06, 0.03, 0.06, 0.03, 0.06),
            ),
            #TOB2
            cms.PSet(
                radius = cms.untracked.double(69.322),
                limits = cms.untracked.vdouble(0.0, 18.0, 30.0, 36.0, 46.0, 55.0, 108.737),
                thickness = cms.untracked.vdouble(0.021, 0.06, 0.03, 0.06, 0.03, 0.06),
            ),
            #TOB3
            cms.PSet(
                radius = cms.untracked.double(+78.081),
                limits = cms.untracked.vdouble(0.0, 18.0, 30.0, 36.0, 46.0, 55.0, 108.737),
                thickness = cms.untracked.vdouble(0.0154, 0.044, 0.022, 0.044, 0.022, 0.044),
            ),
            #TOB4
            cms.PSet(
                radius = cms.untracked.double(86.786),
                limits = cms.untracked.vdouble(0.0, 18.0, 30.0, 36.0, 46.0, 55.0, 108.737),
                thickness = cms.untracked.vdouble(0.0154, 0.044, 0.022, 0.044, 0.022, 0.044),
            ),
            #TOB5
            cms.PSet(
                radius = cms.untracked.double(96.569),
                limits = cms.untracked.vdouble(0.0, 18.0, 30.0, 36.0, 46.0, 55.0, 108.737),
                thickness = cms.untracked.vdouble(0.0154, 0.044, 0.022, 0.044, 0.022, 0.044),
            ),
            #TOB6
            cms.PSet(
                radius = cms.untracked.double(108.063),
                limits = cms.untracked.vdouble(0.0, 18.0, 30.0, 36.0, 46.0, 55.0, 108.737),
                thickness = cms.untracked.vdouble(0.0154, 0.044, 0.022, 0.044, 0.022, 0.044),
            ),
           
            ########### Tracker Outer Barrel Outside Cables and walls (barrel) ###########
            #TBOut
            cms.PSet(
                radius = cms.untracked.double(120.0),
                limits = cms.untracked.vdouble(0.0, 120.0, 299.9),
                thickness = cms.untracked.vdouble(0.042, 0.1596)
            ),
        ),

		ForwardLayers = cms.VPSet(
			########### Pixel Barrel Outside walls and cables (endcap) ###########
            #PIXBOut4
            cms.PSet(
                z = cms.untracked.double(28.799),
                limits = cms.untracked.vdouble(4.2, 5.1, 7.1, 8.2, 10.0, 11.0, 11.9, 16.5),
                thickness = cms.untracked.vdouble(0.100, 0.00, 0.108, 0.00, 0.112, 0.02, 0.04)
            ),
            #PIXBOut
            cms.PSet(
                z = cms.untracked.double(28.8),
                limits = cms.untracked.vdouble(3.8, 16.5),
                thickness = cms.untracked.vdouble(0.012)
            ),
            ########### Pixel Disks 1-2 ###########
            #PIXD1
            cms.PSet(
                limits = cms.untracked.vdouble(4.825, 16.598),
                thickness = cms.untracked.vdouble(0.058),
                z = cms.untracked.double(50)
            ),
            #PIXD2
            cms.PSet(
                limits = cms.untracked.vdouble(4.823, 16.598),
                thickness = cms.untracked.vdouble(0.058),
                z = cms.untracked.double(53)
            ),
            ########### Pixel Endcap outside cables ###########
            #PIXBOut6
            cms.PSet(
                z = cms.untracked.double(65.1),
                limits = cms.untracked.vdouble(6.5, 10.0, 11.0, 16.0, 17.61),
                thickness = cms.untracked.vdouble(0.150, 0.325, 0.250, 0.175)
            ),
            ########### Tracker Inner Barrel Outside Cables and walls (endcap) ###########
            #TIBEOut
            cms.PSet(
                z = cms.untracked.double(74.0),
                limits = cms.untracked.vdouble(22.5, 53.9),
                thickness = cms.untracked.vdouble(0.130)
            ),
            ########### Tracker Inner disks layers 1-3 ###########
            #TID1
            cms.PSet(
                limits = cms.untracked.vdouble(0.0, 22.2, 34.0, 42.0, 53.940),
                thickness = cms.untracked.vdouble(0.04, 0.08, 0.04, 0.04), #LV: just adding one entry to make it run
                z = cms.untracked.double(80)
            ),
            #TID2
            cms.PSet(
                limits = cms.untracked.vdouble(0.0, 22.2, 34.0, 42.0, 53.942), 
                thickness = cms.untracked.vdouble(0.04, 0.08, 0.04, 0.04), #LV: just adding one entry to make it run
                z = cms.untracked.double(85)
            ),
            #TID3
            cms.PSet(
                limits = cms.untracked.vdouble(0.0, 22.2, 34.0, 42.0, 53.942), 
                thickness = cms.untracked.vdouble(0.055, 0.110, 0.055, 0.04), #LV: just adding one entry to make it run
                z = cms.untracked.double(90)
            ),
            ########### Tracker Inner Disks Outside Cables and walls (endcap) ###########
            #TIDEOut
            cms.PSet(
                z = cms.untracked.double(108.0),
                limits = cms.untracked.vdouble(22.0, 24.0, 47.5, 54.943),
                thickness = cms.untracked.vdouble(0.111, 0.074, 0.185)
            ),
            ########### Tracker Outer Barrel Outside Cables and walls (barrel and endcap) ###########
            #TOBEOut
            cms.PSet(
                z = cms.untracked.double(115.0),
                limits = cms.untracked.vdouble(55.0, 60.0, 62.0, 78.0, 92.0, 111.0),
                thickness = cms.untracked.vdouble(0.005, 0.009, 0.014, 0.016, 0.009)
            ),
            ########### Tracker EndCap disks layers 1-9 ###########
            #TEC1
            cms.PSet(
                limits = cms.untracked.vdouble(21.87, 24.0, 34.0, 39.0, 111.395),
                thickness = cms.untracked.vdouble(0.100, 0.040, 0.080, 0.050),
                z = cms.untracked.double(120.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
            ),
            #TEC2
            cms.PSet(
                limits = cms.untracked.vdouble(21.87, 24.0, 34.0, 39.0, 111.395),
                thickness = cms.untracked.vdouble(0.100, 0.040, 0.080, 0.050),
                z = cms.untracked.double(125.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
            ),
            #TEC3
            cms.PSet(
                limits = cms.untracked.vdouble(21.87, 24.0, 34.0, 39.0, 111.395),
                thickness = cms.untracked.vdouble(0.100, 0.040, 0.080, 0.050),
                z = cms.untracked.double(130.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
            ),
            #TEC4
            cms.PSet(
                limits = cms.untracked.vdouble(29.62, 32.0, 40.0, 41.0, 46.0, 111.395),
                thickness = cms.untracked.vdouble(0.115, 0.030, 0.050, 0.070, 0.050),
                z = cms.untracked.double(135.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
            ),
            #TEC5
            cms.PSet(
                limits = cms.untracked.vdouble(29.62, 32.0, 40.0, 41.0, 46.0, 111.395),
                thickness = cms.untracked.vdouble(0.115, 0.030, 0.050, 0.070, 0.050),
                z = cms.untracked.double(140.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
            ),
            #TEC6
            cms.PSet(
                limits = cms.untracked.vdouble(29.62, 32.0, 40.0, 41.0, 46.0, 111.395),
                thickness = cms.untracked.vdouble(0.125, 0.030, 0.050, 0.070, 0.050),
                z = cms.untracked.double(145.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
            ),
            #TEC7
            cms.PSet(
                limits = cms.untracked.vdouble(29.71, 32.0, 60.0, 111.395),
                thickness = cms.untracked.vdouble(0.135, 0.030, 0.050),
                z = cms.untracked.double(150.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
                ),
            #TEC8
            cms.PSet(
                limits = cms.untracked.vdouble(29.71, 32.0, 60.0, 111.395),
                thickness = cms.untracked.vdouble(0.150, 0.030, 0.050),
                z = cms.untracked.double(155.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
                ),
            #TEC9
            cms.PSet(
                limits = cms.untracked.vdouble(29.91, 32.0, 60.0, 111.395),
                thickness = cms.untracked.vdouble(0.150, 0.030, 0.050),
                z = cms.untracked.double(160.0),
                nuclearInteractionThicknessFactor = cms.untracked.double(1.2)
                ),
            ########### Tracker Endcaps Outside Cables and walls (endcaps) ###########
            #TEOut
            cms.PSet(
                z = cms.untracked.double(300.0),
                limits = cms.untracked.vdouble(4.42, 4.65, 4.84, 7.37, 10.99, 14.70, 16.24, 22.00, 28.50, 31.50, 36.0, 120.0),
                thickness = cms.untracked.vdouble(3.935, 0.483, 0.127, 0.089, 0.069, 0.124, 1.47, 0.924, 0.693, 0.294, 0.336)
                ),
            ),
        )
    )
    
    
