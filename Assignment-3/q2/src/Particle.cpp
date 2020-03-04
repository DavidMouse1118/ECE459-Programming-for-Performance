#include <cmath>
#include "Particle.h"
#include "Constants.h"

Particle::Particle(ParticleType type, Vec3 position)
    : position(position)
    , type(type) {
    // nop
}

Particle::Particle(const Particle &other)
    : position(other.position)
    , type(other.type) {
    // nop
}

Particle::~Particle() {
    // nop
}

float Particle::getCharge() const {
    switch (type) {
        case PROTON: return chargeProton;
        case ELECTRON: return chargeElectron;
        default: return 0;
    }
}

float Particle::getMass() const {
    switch (type) {
        case PROTON: return massProton;
        case ELECTRON: return massElectron;
        default: return 0;
    }
}

Vec3 Particle::computeForceOnMe(const Particle* other) const {
    if (type == PROTON) {
        // Special rule for our simulation
        // Protons have 0 force acting on them
        return Vec3();
    }

    if (this == other) {
        // Assert 0 force on self
        return Vec3();
    }

    // If same charge (q1 and q2 same sign), then they repel; thus
    // the direction vector must be from other particle to me
    Vec3 direction = position - other->position;
    float q1 = getCharge();
    float q2 = other->getCharge();
    float r = direction.magnitude();

    return direction.normal() * (coulombConstant * q1 * q2 / std::pow(r, 2));
}

std::ostream &operator<<(std::ostream &os, Particle const &p) {
    switch (p.type) {
        case Particle::PROTON:
            os << "Proton   ";
            break;
        case Particle::ELECTRON:
            os << "Electron ";
            break;
        default:
            os << "Undefined";
    }

    os << " "
       << p.position;

    return os;
}
