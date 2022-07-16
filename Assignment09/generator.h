#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include "vectors.h"
#include "particle.h"
//#include <stdlib.h>
//#include "time.h"
#include "random.h"

class Generator
{
public:
    Generator() {}

    // initialization
    void SetColors(Vec3f color, Vec3f dead_color, float color_randomness);
    void SetLifespan(float lifespan, float lifespan_randomness, int desired_num_particles);
    void SetMass(float mass, float mass_randomness);

    // on each timestep, create some particles
    float GetRandomF(float randomness);
    Vec3f GetRandomVec3f(float randomness);
    virtual int numNewParticles(float current_time, float dt) const;
    virtual Particle* Generate(float current_time, int i) = 0;

    // for the gui
    virtual void Paint() const {}
    void Restart() { RNG = Random(); }

    Vec3f Color, ColorDead;
    float ColorRandomness;
    float Life, LifeRandomness;
    int DesiredNumParticles;
    float Mass, MassRandomness;

    Random RNG;

};

class HoseGenerator: public Generator
{
public:
    HoseGenerator(Vec3f position, float position_randomness, 
        Vec3f velocity, float velocity_randomness):
        Position(position), PositionRandomness(position_randomness),
        Velocity(velocity), VelocityRandomness(velocity_randomness) {}

    virtual Particle* Generate(float current_time, int i);
    
    Vec3f Position;
    float PositionRandomness;
    Vec3f Velocity;
    float VelocityRandomness;

};

class RingGenerator: public Generator
{
public:
    RingGenerator(float position_randomness, Vec3f velocity, float velocity_randomness):
        PositionRandomness(position_randomness), SpanSpeed(1.0f), MaxTime(12.0f), 
        Velocity(velocity), VelocityRandomness(velocity_randomness), Height(-4.0f) {}

    virtual int numNewParticles(float current_time, float dt) const;
    virtual Particle* Generate(float current_time, int i);

    virtual void Paint() const;
    
    float PositionRandomness;
    Vec3f Velocity;
    float VelocityRandomness;
    float SpanSpeed, MaxTime;
    float Height;

};



#endif