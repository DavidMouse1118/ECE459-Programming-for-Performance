#pragma once

#include <string>
#include <vector>
#include "Particle.h"

class Simulation
{
    const float initTimeStep;
    const float errorTolerance;

    std::vector<Vec3> k0; // Net force at y0
    std::vector<Vec3> k1; // Net force at y1
    std::vector<Particle*> y0; // Initial positions
    std::vector<Particle*> y1; // Positions with k0
    std::vector<Particle*> z1; // Positions with (k1 + k0) / 2

#ifndef USE_OPENCL
    void computeForces(std::vector<Vec3> &ret, const std::vector<Particle*> &particles);
    void computeApproxPositions(const float h);
    void computeBetterPositions(const float h);
    bool isErrorAcceptable(const std::vector<Particle*> &p0, const std::vector<Particle*> &p1);
#endif

public:
    Simulation(float timeStep, float errorTolerance);
    ~Simulation();

    void readInputFile(std::string inputFile);
    void run();
    void print();
};
