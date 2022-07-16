#ifndef _FORCE_FIELD_H_
#define _FORCE_FIELD_H_

#include "vectors.h"

class ForceField
{
public:
    ForceField() {}

    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const = 0;

};

class GravityForceField: public ForceField
{
public:
    GravityForceField(Vec3f gravity): Gravity(gravity) {}

    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const
        { return Gravity; }

    Vec3f Gravity;

};

class ConstantForceField: public ForceField
{
public:
    ConstantForceField(Vec3f force): Force(force) {}

    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const
        { return Force * (1.0f / mass); }

    Vec3f Force;

};

class RadialForceField: public ForceField
{
public:
    RadialForceField(float magnitude): Magnitude(magnitude) {}

    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const
    {
        Vec3f dir = position;
        dir.Negate();
        dir.Normalize();
        Vec3f force = dir * position.Length() * Magnitude;
        return force * (1.0f / mass);
    }

    float Magnitude;

};

class VerticalForceField: public ForceField
{
public:
    VerticalForceField(float magnitude): Magnitude(magnitude) {}

    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const
    {
        Vec3f force = Vec3f(0, -1.0f, 0) * position.y() * Magnitude;
        return force * (1.0f / mass);
    }

    float Magnitude;

};

// EXTRA CREDIT NOT IMPLEMENTED
class WindForceField: public ForceField
{
public:
    WindForceField(float magnitude): Magnitude(magnitude) {}

    virtual Vec3f getAcceleration(const Vec3f &position, float mass, float t) const
        { return Vec3f(0,0,0); }

    float Magnitude;

};

#endif