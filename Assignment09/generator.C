#include "generator.h"
#include <iostream>
#include <cmath>

#include <GL/gl.h>
#include <GL/glut.h>

void Generator::SetColors(Vec3f color, Vec3f dead_color, float color_randomness)
{
    Color = color;
    ColorDead = dead_color;
    ColorRandomness = color_randomness;
}

void Generator::SetLifespan(float lifespan, float lifespan_randomness, 
    int desired_num_particles)
{
    Life = lifespan;
    LifeRandomness = lifespan_randomness;
    DesiredNumParticles = desired_num_particles;
}

void Generator::SetMass(float mass, float mass_randomness)
{
    Mass = mass;
    MassRandomness = mass_randomness;
}

float Generator::GetRandomF(float randomness)
{
    //return rand() / (RAND_MAX + 1.0f) * 2.0f * randomness - randomness;
    return RNG.next() * 2.0f * randomness - randomness;
}

Vec3f Generator::GetRandomVec3f(float randomness)
{
    //return Vec3f(GetRandomF(randomness), GetRandomF(randomness), GetRandomF(randomness));
    return RNG.randomVector() * randomness;
}

int Generator::numNewParticles(float current_time, float dt) const
{
    int num = max(1, int(dt * DesiredNumParticles / Life + .00001f));
    return num;
}

Particle* HoseGenerator::Generate(float current_time, int i)
{
    Vec3f pos = Position + GetRandomVec3f(PositionRandomness);
    Vec3f v = Velocity + GetRandomVec3f(VelocityRandomness);
    Vec3f col = Color + GetRandomVec3f(ColorRandomness);
    Vec3f col_dead = ColorDead + GetRandomVec3f(ColorRandomness);
    float mass = Mass + GetRandomF(MassRandomness);
    float life = Life + GetRandomF(LifeRandomness);
    
    Particle *particle = new Particle(pos, v, col, col_dead, mass, life);
    return particle;
}

int RingGenerator::numNewParticles(float current_time, float dt) const
{
    float k = current_time < MaxTime ? current_time / MaxTime : 1.0f;
    return max(1, int(k * dt * DesiredNumParticles * MaxTime / Life + .00001f));
}

Particle* RingGenerator::Generate(float current_time, int i)
{
    float t = min(current_time, MaxTime);
    float radius = t * SpanSpeed;
    float theta = GetRandomF(1.0f) * M_PI + M_PI;
    Vec3f pos = Vec3f(radius * cos(theta), Height, radius * sin(theta)) + 
        GetRandomVec3f(PositionRandomness);
    Vec3f v = Velocity + GetRandomVec3f(VelocityRandomness);
    Vec3f col = Color + GetRandomVec3f(ColorRandomness);
    Vec3f col_dead = ColorDead + GetRandomVec3f(ColorRandomness);
    float mass = Mass + GetRandomF(MassRandomness);
    float life = Life + GetRandomF(LifeRandomness);

    Particle *particle = new Particle(pos, v, col, col_dead, mass, life);
    return particle;

}

void RingGenerator::Paint() const {
    float l = 16.0f;
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glVertex3f(l, Height, l);
    glVertex3f(l, Height, -l);
    glVertex3f(-l, Height, -l);
    glVertex3f(-l, Height, l);
    glEnd();
}