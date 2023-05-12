#ifndef GENERATOR
#define GENERATOR

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Generator : public cSimpleModule
{
private:
    cMessage *sendMsgEvent;
    cStdDev transmissionStats;
    int genPacketsCount;
    cOutVector generatedPackets;

public:
    Generator();
    virtual ~Generator();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};
Define_Module(Generator);

Generator::Generator()
{
    sendMsgEvent = NULL;
}

Generator::~Generator()
{
    cancelAndDelete(sendMsgEvent);
}

void Generator::initialize()
{
    // keeps track of the number of generated packets
    genPacketsCount = 0;
    generatedPackets.setName("Generated packets");

    transmissionStats.setName("TotalTransmissions");
    // create the send packet
    sendMsgEvent = new cMessage("sendEvent");
    // schedule the first event at random time
    scheduleAt(par("generationInterval"), sendMsgEvent);
}

void Generator::finish()
{
    recordScalar("Generated packets", genPacketsCount);
}

void Generator::handleMessage(cMessage *msg)
{
    // create new packet
    cPacket *pkt;
    pkt = new cPacket("packet");
    pkt->setByteLength(par("packetByteSize"));
    // send to the output
    send(pkt, "out");

    genPacketsCount++;
    generatedPackets.record(genPacketsCount);

    // compute the new departure time
    simtime_t departureTime = simTime() + par("generationInterval");
    // schedule the new packet generation
    scheduleAt(departureTime, sendMsgEvent);
}

#endif /* GENERATOR */
