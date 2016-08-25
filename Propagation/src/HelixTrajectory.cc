#include "FastSimulation/Propagation/interface/HelixTrajectory.h"
#include "FastSimulation/Layer/interface/BarrelLayer.h"
#include "FastSimulation/Layer/interface/ForwardLayer.h"
#include "FastSimulation/NewParticle/interface/Particle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include <cmath>

// helix phi definition
// ranges from 0 to 2PI
// 0 corresponds to the positive x direction
// PI increases counterclockwise

fastsim::HelixTrajectory::HelixTrajectory(const fastsim::Particle & particle,double magneticFieldZ)
    : Trajectory(particle)
    // exact: r = gamma*beta*m_0*c / (q*e*B) = p_T / (q * e * B)
    // momentum in units of GeV/c: r = p_T * 10^9 / (c * q * B)
    // in cmssw units: r = p_T / (c * 10^-4 * q * B)
    , radius_(std::abs(momentum_.Pt() / (speedOfLight_ * 1e-4 * particle.charge() * magneticFieldZ)))
    , phi_(std::atan(momentum_.Py()/momentum_.Px()) + (momentum_.Px()*particle.charge() < 0 ? M_PI/2. : 3.*M_PI/2. ))
    , centerX_(position_.X() - radius_*std::cos(phi_))
    , centerY_(position_.Y() - radius_*std::sin(phi_))
    , centerR_(std::sqrt(centerX_*centerX_ + centerY_*centerY_))
    , minR_(centerR_ - radius_)
    , maxR_(centerR_ + radius_)
    // omega = q * e * B / (gamma * m) = q * e *B / (E / c^2) = q * e * B * c^2 / E
    // omega: negative for negative q -> seems to be what we want.
    // energy in units of GeV: omega = q * B * c^2 / (E * 10^9)
    // in cmssw units: omega[1/ns] = q * B * c^2 * 10^-4 / E
    , phiSpeed_(particle.charge() * magneticFieldZ * speedOfLight_ * speedOfLight_ * 1e-4 / momentum_.E())
{std::cout<<centerX_<<std::endl;
std::cout<<centerY_<<std::endl;
std::cout<<phi_/M_PI<<std::endl;
std::cout<<phiSpeed_*1e3<<std::endl;}

bool fastsim::HelixTrajectory::crosses(const BarrelLayer & layer) const
{
    return (minR_ < layer.getRadius() && maxR_ > layer.getRadius());
}

double fastsim::HelixTrajectory::nextCrossingTimeC(const BarrelLayer & layer) const
{                        
    //
    // solve the following equation for sin(phi)
    // (x^2 + y^2 = R_L^2)     (1)      the layer 
    // x = x_c + R_H*cos(phi)  (2)      the helix in the xy plane
    // y = y_c + R_H*sin(phi)  (3)      the helix in the xy plane
    // with
    // R_L: the radius of the layer
    // x_c,y_c the center of the helix in xy plane
    // R_H, the radius of the helix
    // phi, the phase of the helix
    //
    // substitute (2) and (3) in (1)
    // =>
    //   x_c^2 + 2*x_c*R_H*cos(phi) + R_H^2*cos^2(phi)
    // + y_c^2 + 2*y_c*R_H*sin(phi) + R_H^2*sin^2(phi)
    // = R_L^2
    // =>
    // (x_c^2 + y_c^2 + R_H^2 - R_L^2) + (2*y_c*R_H)*sin(phi) = -(2*x_c*R_H)*cos(phi)
    //
    // rewrite
    //               E                 +       F    *sin(phi) =      G     *cos(phi)
    // =>
    // E^2 + 2*E*F*sin(phi) + F^2*sin^2(phi) = G^2*(1-sin^2(phi))
    // rearrange
    // sin^2(phi)*(F^2 + G^2) + sin(phi)*(2*E*F) + (E^2 - G^2) = 0
    //
    // rewrite
    // sin^2(phi)*     a      + sin(phi)*   b    +      c      = 0
    // => sin(phi) = (-b +/- sqrt(b^2 - 4*ac)) / (2*a)
    // with
    // a = F^2 + G^2
    // b = 2*E*F
    // c = E^2 - G^2
    //

    double E = centerX_*centerX_ + centerY_*centerY_ + radius_*radius_ - layer.getRadius()*layer.getRadius();
    double F = 2*centerY_*radius_;
    double G = 2*centerX_*radius_;

    double a = F*F + G*G;
    double b = 2*E*F;
    double c = E*E - G*G;
    
    double delta = b*b - 4*a*c;
    // case of no solution
    if(delta < 0)
    {   
        return -1.;
    }

    double sqrtDelta = sqrt(delta);
    double phi1 = std::asin((-b - sqrtDelta) / (2.*a));
    double phi2 = std::asin((-b + sqrtDelta) / (2.*a));
    // asin is ambiguous, make sure to have the right solution
    if(std::abs(layer.getRadius() - sqrt((centerX_ + radius_*std::cos(phi1))*(centerX_ + radius_*std::cos(phi1)) + (centerY_ + radius_*std::sin(phi1))*(centerY_ + radius_*std::sin(phi1)))) > 1e-3){
        phi1 = - phi1 + M_PI;
        std::cout<<"-> fixing phi1"<<std::endl;
    }
    if(std::abs(layer.getRadius() - sqrt((centerX_ + radius_*std::cos(phi2))*(centerX_ + radius_*std::cos(phi2)) + (centerY_ + radius_*std::sin(phi2))*(centerY_ + radius_*std::sin(phi2)))) > 1e-3){
        phi2 = - phi2 + M_PI;
        std::cout<<"-> fixing phi2"<<std::endl;
    }

    if(phi1 < 0){
        phi1 += 2. * M_PI;
    }
    if(phi2 < 0){
        phi2 += 2. * M_PI;
    }

    if(std::abs(layer.getRadius() - sqrt((centerX_ + radius_*std::cos(phi1))*(centerX_ + radius_*std::cos(phi1)) + (centerY_ + radius_*std::sin(phi1))*(centerY_ + radius_*std::sin(phi1)))) > 1e-3){
        throw cms::Exception("fastsim::HelixTrajectory::nextCrossingTimeC") << "not able to calculate phi1 of intersection";
    }
    if(std::abs(layer.getRadius() - sqrt((centerX_ + radius_*std::cos(phi2))*(centerX_ + radius_*std::cos(phi2)) + (centerY_ + radius_*std::sin(phi2))*(centerY_ + radius_*std::sin(phi2)))) > 1e-3){
        throw cms::Exception("fastsim::HelixTrajectory::nextCrossingTimeC") << "not able to calculate phi2 of intersection";
    }    

    // find the corresponding times
    // make sure they are positive
    double t1 = (phi1 - phi_)/phiSpeed_;
    while(t1 < 0)
    {
	   t1 += 2*M_PI/std::abs(phiSpeed_);
    }
    double t2 = (phi2 - phi_)/phiSpeed_;
    while(t2 < 0)
    {
	   t2 += 2*M_PI/std::abs(phiSpeed_);
    }

    // if the particle is already on the layer,
    // we need to make sure the 2nd solution is picked.
    if(std::abs(phi1 - phi_) < 1e-7){
        return t2*speedOfLight_;
    }
    if(std::abs(phi2 - phi_) < 1e-7){
        return t1*speedOfLight_;
    }

    // not sure if we should stick to this t*c strategy...
    return std::min(t1,t2)*speedOfLight_;
    
}

void fastsim::HelixTrajectory::move(double deltaTimeC)
{
    double deltaT = deltaTimeC/speedOfLight_;
    double deltaPhi = phiSpeed_*deltaT;
    position_.SetXYZT(
	   centerX_ + radius_*std::cos(phi_ + deltaPhi),
	   centerY_ + radius_*std::sin(phi_ + deltaPhi),
	   position_.Z() + momentum_.Z()/momentum_.E()*deltaTimeC,
	   position_.T() + deltaT);
    // Rotation defined by
    // x' = x cos θ - y sin θ
    // y' = x sin θ + y cos θ
    momentum_.SetXYZT(
	   momentum_.X()*std::cos(deltaPhi) - momentum_.Y()*std::sin(deltaPhi),
	   momentum_.X()*std::sin(deltaPhi) + momentum_.Y()*std::cos(deltaPhi),
	   momentum_.Z(),
	   momentum_.E());

    /*if(deltaTimeC < 1000 && (position_.X() > 1 || position_.Y() > 1)){
        if((position_.X() > 0 && momentum_.X() < 0) || (position_.X() < 0 && momentum_.X() > 0)){
            std::cout<<"WARNING"<<std::endl;
            std::cout<<"x=("<<position_.X()<<"; "<<position_.Y()<<") p=("<<momentum_.X()<<"; "<<momentum_.Y()<<")"<<std::endl;
            throw cms::Exception("fastsim::HelixTrajectory::nextCrossingTimeC") << "strange";
        }
        if((position_.Y() > 0 && momentum_.Y() < 0) || (position_.Y() < 0 && momentum_.Y() > 0)){
            std::cout<<"WARNING"<<std::endl;
            std::cout<<"x=("<<position_.X()<<"; "<<position_.Y()<<") p=("<<momentum_.X()<<"; "<<momentum_.Y()<<")"<<std::endl;
            throw cms::Exception("fastsim::HelixTrajectory::nextCrossingTimeC") << "strange";

        }
    }
    */
}
