#pragma once

#include "Constants.h"
#include <iostream>

struct Vec3
{
    float x;
    float y;
    float z;

    Vec3();
    Vec3(const Vec3 &other);
    Vec3(float x, float y, float z);

    Vec3& operator=(const Vec3 &rhs);
    Vec3& operator+=(const Vec3 &rhs);
    Vec3 operator+(const Vec3 &rhs) const;
    Vec3 operator-(const Vec3 &rhs) const;
    Vec3 operator*(const float scalar) const;
    Vec3 operator/(const float scalar) const;

    float magnitude() const;
    Vec3 normal() const;
};

std::ostream &operator<<(std::ostream &os, Vec3 const &v);
