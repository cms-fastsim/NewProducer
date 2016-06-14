#ifndef FASTSIM_INTERACTIONMODELFACTORY
#define FASTSIM_INTERACTIONMODELFACTORY

#include <memory>

namespace edm
{
    class ParameterSet;
    class ProducerBase;
}

namespace fastsim
{
    class InteractionModel;
    class InteractionModelFactory
    {
    public:
	static std::unique_ptr<InteractionModel> create(const edm::ParameterSet & cfg);
    };
}

#endif
