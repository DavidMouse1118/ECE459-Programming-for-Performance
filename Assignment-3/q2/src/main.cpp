#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include "Simulation.h"

using std::cerr;
using std::endl;

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        cerr << "Expected 3 arguments but got " << (argc - 1) << endl;
        cerr << "Usage: ./" << argv[0] << " h e inputFile" << endl;
        exit(1);
    }

    float h = std::stod(argv[1]);
    float e = std::stod(argv[2]);
    std::string inputFile = argv[3];

    Simulation sim(h, e);
    sim.readInputFile(inputFile);
    sim.run();
    sim.print();

    return 0;
}
