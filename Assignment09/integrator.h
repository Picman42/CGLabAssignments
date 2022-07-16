#ifndef _INTEGRATOR_H_
#define _INTEGRATOR_H_

#include "particle.h"
#include "forcefield.h"

class Integrator
{
public:
    Integrator() {}

    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) = 0;
    virtual Vec3f getColor() const { return Vec3f(1.0f, 0, 0); }
};

class EulerIntegrator: public Integrator
{
public:
    EulerIntegrator() {}

    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);

};

class MidpointIntegrator: public Integrator
{
public:
    MidpointIntegrator() {}

    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt);
    virtual Vec3f getColor() const { return Vec3f(0, 1.0f, 0); }
};

// EXTRA CREDIT NOT IMPLEMENTED
class RungeKuttaIntegrator: public Integrator
{
public:
    RungeKuttaIntegrator() {}

    virtual void Update(Particle *particle, ForceField *forcefield, float t, float dt) {}
    virtual Vec3f getColor() const { return Vec3f(0, 0, 1.0f); }
};

#endif