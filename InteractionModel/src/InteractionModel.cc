#include "FastSimulation/InteractionModel/interface/InteractionModel.h"

std::ostream & fastsim::operator << (std::ostream& os , const fastsim::InteractionModel & interactionModel)
{
    os << std::string("interaction model with className '") << (interactionModel.className_) << std::string("'");
    return os;
}
    
