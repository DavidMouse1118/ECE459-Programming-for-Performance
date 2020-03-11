#include "Event.h"
#include <iostream>

using std::cout;
using std::endl;

//-----------------------------------------------------------------------------
// Event
//-----------------------------------------------------------------------------

Event::Event(EventType type, EventData* data)
    : type(type)
    , data(data)
{
    // nop
}

Event::Event(const Event &other) {
    deepCopy(other);
}

Event::~Event() {
    delete data;
}

void Event::deepCopy(const Event &other) {
    type = other.type;

    delete data;
    data = nullptr;

    if (other.data) {
        data = other.data->clone();
    }
}

Event& Event::operator=(const Event &other) {
    if (this != &other) {
        deepCopy(other);
    }

    return *this;
}

Event::EventType Event::getType() {
    return type;
}

Event::EventData* Event::getData() {
    return data;
}

void Event::dump() {
    if (data) {
        data->dump();
    }
}

//-----------------------------------------------------------------------------
// Idea
//-----------------------------------------------------------------------------

Idea::Idea(std::string name, int numPackagesReq)
    : EventData()
    , name(name)
    , numPackagesReq(numPackagesReq) {
    // nop
}

Idea::~Idea() {
    // nop
}

void Idea::dump() {
    cout << "Idea <" << name << "> (" << numPackagesReq << ")" << endl;
}

Event::EventData* Idea::clone() {
    return new Idea(name, numPackagesReq);
}

std::string Idea::getName() const {
    return name;
}

int Idea::getNumPackagesReq() const {
    return numPackagesReq;
}

//-----------------------------------------------------------------------------
// Package
//-----------------------------------------------------------------------------

Package::Package(std::string name)
    : EventData()
    , name(name) {
    // nop
}

Package::~Package() {
    // nop
}

void Package::dump() {
    cout << "Package <" << name << ">" << endl;
}

Event::EventData* Package::clone() {
    return new Package(name);
}

std::string Package::getName() const {
    return name;
}
