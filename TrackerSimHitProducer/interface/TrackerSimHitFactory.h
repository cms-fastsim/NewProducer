#ifndef FASTSIM_TRACKERSIMHITFACTORY_H
#define FASTSIM_TRACKERSIMHITFACTORY_H

#include <vector>

class RawParticle;
class DetLayer;
class PSimHit;
class GeomDet;
class TrajectoryStateOnSurface;
namespace edm {
    typedef std::vector<PSimHit> PSimHitContainer;
}

namespace fastsim
{
    class TrackerSimHitFactory
    {
    public:
	TrackerSimHitFactory();
	void createHitsOnDetLayer(const RawParticle & particle,int simTrackId,const DetLayer & detLayer,double magneticFieldZ,edm::PSimHitContainer & simHitContainer);
	void createHitOnDetector(const TrajectoryStateOnSurface & particle,int pdgId,int simTrackId,const GeomDet & detector,edm::PSimHitContainer & simHitContainer);
    private:
	const float onSurfaceTolerance_;
    };
}

#endif
