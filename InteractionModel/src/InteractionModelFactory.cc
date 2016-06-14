#include "FastSimulation/InteractionModel/interface/InteractionModelFactory.h"
#include "FastSimulation/InteractionModel/interface/SimpleLayerHitProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

std::unique_ptr<fastsim::InteractionModel> fastsim::InteractionModelFactory::create(const edm::ParameterSet & cfg)
{
    std::string className = cfg.getParameter<std::string>("className");
    if(className == "SimpleLayerHitProducer")
    {
	LogDebug("FastSimulation") << "   creating interaction model of type 'SimpleLayerHitProducer'";
	return std::unique_ptr<fastsim::InteractionModel>(new fastsim::SimpleLayerHitProducer());
    }
    else
    {
	throw cms::Exception("FastSimulation") << "fastsim::InteractionModelFactory::create called with unknown className '" << className << "'" << std::endl;
    }
}

