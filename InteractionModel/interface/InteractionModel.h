#ifndef FASTSIM_INTERACTIONMODEL
#define FASTSIM_INTERACTIONMODEL

#include "string"

namespace edm
{
    class Event;
    class ProducerBase;
}

class FSimEvent;

namespace fastsim
{
    class Layer;
    class Particle;
    class InteractionModel
    {
    public:
	InteractionModel(std::string className)
	    : className_(className){;}
	virtual ~InteractionModel(){;}
	virtual void interact(Particle & particle,const Layer & layer,FSimEvent& simEvent) = 0;
	virtual void registerProducts(edm::ProducerBase & producer) const{;}
	virtual void storeProducts(edm::Event & iEvent) {;}
 	friend std::ostream& operator << (std::ostream& o , const InteractionModel & model); 
   private:
	const std::string className_;
    };
    std::ostream& operator << (std::ostream& os , const InteractionModel & interactionModel);

}

#endif
