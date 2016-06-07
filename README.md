# How to install

```
cmsrel CMSSW_8_1_0_pre5
cd CMSSW_8_1_0_pre5/src
git clone git@github.com:cms-fastsim/NewProducer.git FastSimulation
scram b -j 8 # compilation crashes at this point
```

# How to compile the Geometry package

```
cd FastSimulation/Geometry
scram b # works right now
```

# Hot to run

```
# create a file with generated events
source FastSimulation/TrackerSimHitProducer/test/gen.sh
# pass the generated events to simulation
source FastSimulation/TrackerSimHitProducer/python/conf_cfg.py
```

# More info on the project

See comments in FastSimulation/Propagation/src/LayerNavigator.cc

# How to push and pull changes

```
git pull 
git push
```
