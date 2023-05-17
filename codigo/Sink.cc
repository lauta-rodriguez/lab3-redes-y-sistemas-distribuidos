#ifndef SINK
#define SINK

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule
{
private:
    cStdDev delayStats;
    cOutVector delayVector;

    unsigned int deliveredPackets;
    cOutVector deliveredPacketsVector;

public:
    Sink();
    virtual ~Sink();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Sink);

Sink::Sink()
{
}

Sink::~Sink()
{
}

void Sink::initialize()
{
    // stats and vector names
    delayStats.setName("total delay");
    delayVector.setName("delay");

    deliveredPackets = 0;
    deliveredPacketsVector.setName("delivered packets");
}

void Sink::finish()
{
    // stats record at the end of simulation
    recordScalar("average delay", delayStats.getMean());
    recordScalar("delay count", delayStats.getCount()); //???
    recordScalar("delivered packets", deliveredPackets);
}

void Sink::handleMessage(cMessage *msg)
{
    // compute queuing delay
    simtime_t delay = simTime() - msg->getCreationTime();
    // update stats
    delayStats.collect(delay);
    delayVector.record(delay);

    deliveredPackets++;
    deliveredPacketsVector.record(deliveredPackets);

    // delete msg
    delete (msg);
}

#endif /* SINK */
