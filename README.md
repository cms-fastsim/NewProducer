# How to install

```
cmsrel CMSSW_8_1_0_pre5
cd CMSSW_8_1_0_pre5/src
cmsenv
git clone git@github.com:cms-fastsim/NewProducer.git FastSimulation
USER_CXXFLAGS="-g -D=EDM_ML_DEBUG" scram b -j 8 # special flags to switch on debugging code
```

# How to run

```
# create a file with generated events
source FastSimulation/TrackerSimHitProducer/test/gen.sh
# pass the generated events to simulation
cmsRun FastSimulation/TrackerSimHitProducer/python/conf_cfg.py
```

# More info on the project

See comments in FastSimulation/Propagation/src/LayerNavigator.cc

# How to push and pull changes

```
git pull 
git push
```
