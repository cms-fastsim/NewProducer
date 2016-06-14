

namespace fastsim
{
    class ParticleKiller : public InteractionModel
    {
	void interact(const Layer & layer,Particle & particle,std::vector<Particle> & particleList) const override{
	    particle.SetMomentum(0,0,0,0);
	}
    }
}
