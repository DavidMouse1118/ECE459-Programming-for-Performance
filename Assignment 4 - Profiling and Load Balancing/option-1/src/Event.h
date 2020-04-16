#pragma once

#include <string>

//-----------------------------------------------------------------------------
// Event
//-----------------------------------------------------------------------------

class Event
{
public:
    enum EventType {
        UNDEFINED,
        OUT_OF_IDEAS,
        NEW_IDEA,
        DOWNLOAD_COMPLETE,
    };

    class EventData
    {
    public:
        EventData() { }
        virtual ~EventData() { }
        virtual void dump() = 0;
        virtual EventData* clone() = 0;
    };

private:
    EventType type = UNDEFINED;
    EventData* data = nullptr;

    void deepCopy(const Event &other);

public:
    Event(EventType type = UNDEFINED, EventData* data = nullptr);
    Event(const Event &other);
    ~Event();

    Event& operator=(const Event &other);
    EventType getType();
    EventData* getData();
    void dump();
};

//-----------------------------------------------------------------------------
// Idea
//-----------------------------------------------------------------------------

class Idea : public Event::EventData
{
    std::string name;
    int numPackagesReq;

public:
    Idea(std::string name, int numPackagesReq);
    virtual ~Idea();
    virtual void dump();
    virtual EventData* clone();

    std::string getName() const;
    int getNumPackagesReq() const;
};

//-----------------------------------------------------------------------------
// Package
//-----------------------------------------------------------------------------

class Package : public Event::EventData
{
    std::string name;

public:
    Package(std::string name);
    virtual ~Package();
    virtual void dump();
    virtual EventData* clone();

    std::string getName() const;
};
