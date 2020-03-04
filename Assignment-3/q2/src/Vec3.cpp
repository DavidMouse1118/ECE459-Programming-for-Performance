#include <cmath>
#include "Vec3.h"

Vec3::Vec3() : x(0.0), y(0.0), z(0.0){
    // nop
}

Vec3::Vec3(const Vec3 &other) : x(other.x), y(other.y), z(other.z) {
    // nop
}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {
    // nop
}

Vec3& Vec3::operator=(const Vec3 &rhs) {
    if(&rhs == this) {
        return *this;
    }

    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
}

Vec3& Vec3::operator+=(const Vec3 &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

Vec3 Vec3::operator+(const Vec3 &rhs) const {
    return {
        x + rhs.x,
        y + rhs.y,
        z + rhs.z
    };
}

Vec3 Vec3::operator-(const Vec3 &rhs) const {
    return {
        x - rhs.x,
        y - rhs.y,
        z - rhs.z
    };
}

Vec3 Vec3::operator*(const float scalar) const {
    return {
        x * scalar,
        y * scalar,
        z * scalar
    };
}

Vec3 Vec3::operator/(const float scalar) const {
    return {
        x / scalar,
        y / scalar,
        z / scalar
    };
}

float Vec3::magnitude() const {
    return std::sqrt(
        std::pow(x, 2) +
        std::pow(y, 2) +
        std::pow(z, 2)
    );
}

Vec3 Vec3::normal() const {
    float factor = 1.0 / magnitude();
    return (*this) * factor;
}

std::ostream &operator<<(std::ostream &os, Vec3 const &v) {
    os << "{"
       << v.x << ", "
       << v.y << ", "
       << v.z
       << "}";

    return os;
}
