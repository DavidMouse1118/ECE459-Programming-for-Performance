#pragma once

#include "Vec3.h"
#include <iostream>

class Particle
{
public:
    enum ParticleType {
        UNDEFINED,
        PROTON,
        ELECTRON,
    };

    const Vec3 position;
    const ParticleType type;

    Particle(ParticleType type, Vec3 position);
    Particle(const Particle &other);
    ~Particle();

    float getCharge() const;
    float getMass() const;
    Vec3 computeForceOnMe(const Particle* other) const;
};

std::ostream &operator<<(std::ostream &os, Particle const &p);
