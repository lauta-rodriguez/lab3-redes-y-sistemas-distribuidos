#ifndef TRANSPORTX
#define TRANSPORTX

#include <string.h>
#include <omnetpp.h>

#include "FeedbackPkt_m.h"

using namespace omnetpp;

// this class is the same as Queue, but with an extra funcionality
// it expects either a data packet or a Feedback message
// if it is a data packet, it will be enqueued in the buffer
// if it is a Feedback message, it will be sent out through the output gate
// that corresponds to communicates the TransportTx module with the FeedbackChannel module
// that gate is "connSubnet$o"

class TransportTx : public cSimpleModule
{
private:
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;
    simtime_t BOTTLENECK_WINDOW;

    // variables for statistics logging
    unsigned int sentPackets;
    cOutVector bufferSizeVector;

    // defines how many seconds the network is considered to be bottlenecked
    // after a feedback message is recieved
    const unsigned int TIMER = 5;
    // if a bottleneck occurs in less than PENALTY_WINDOW seconds since the last
    // one, a penalty is applied to the transmitter
    const unsigned int PENALTY_WINDOW = 10;
    // default modifier for serviceTime
    const unsigned int k = 20;
    // variable modifier for serviceTime
    unsigned int mod;
    // flag for bottleneck
    bool bottleneck;

    // helper function for handling the queuing process in the buffer
    void enqueueInBuffer(cMessage *msg);

public:
    TransportTx();
    virtual ~TransportTx();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(TransportTx);

TransportTx::TransportTx()
{
    endServiceEvent = NULL;
}

TransportTx::~TransportTx()
{
    cancelAndDelete(endServiceEvent);
}

void TransportTx::initialize()
{
    buffer.setName("Transmitter Data buffer");
    endServiceEvent = new cMessage("endService");

    sentPackets = 0u;

    bufferSizeVector.setName("buffer size");

    BOTTLENECK_WINDOW = 0u;
    mod = k;

    bottleneck = false;
}

void TransportTx::finish()
{
    // save scalar results
    recordScalar("sent packets", sentPackets);
    recordScalar("final buffer size", buffer.getLength());
}

void TransportTx::handleMessage(cMessage *msg)
{
    // determine if a bottleneck has occurred
    if (msg->getKind() == 2 && !bottleneck)
    {
        bottleneck = true;
        // apply penalty to the transmitter if appropriate
        if (simTime() - BOTTLENECK_WINDOW < PENALTY_WINDOW)
        {
            mod *= 2;
        }
        BOTTLENECK_WINDOW = simTime() + TIMER;
    }

    // determine if bottleneck is over
    if (simTime() >= BOTTLENECK_WINDOW && bottleneck)
    {
        bottleneck = false;
        // reset modifier to default value
        mod = k;
    }

    if (msg->getKind() != 2)
    {
        if (msg == endServiceEvent)
        {
            if (!buffer.isEmpty())
            {
                // dequeue packet
                cPacket *pkt = (cPacket *)buffer.pop();
                // send packet
                send(pkt, "connSubnet$o");
                // update stats
                sentPackets++;

                // start new service
                // serviceTime now depends on pkt->getDuration()
                serviceTime = pkt->getDuration();

                if (bottleneck)
                {
                    serviceTime *= mod;
                }

                scheduleAt(simTime() + serviceTime, endServiceEvent);
            }
        }
        else
        {
            enqueueInBuffer(msg);
        }
    }
}

/* If the buffer is not full, enqueue message
 * Otherwise, drop it
 */
void TransportTx::enqueueInBuffer(cMessage *msg)
{

    // check buffer limit
    if (buffer.getLength() >= par("bufferSize").intValue())
    {
        // drop the packet
        delete msg;
        this->bubble("packet dropped");
    }
    else
    {
        // enqueue the packet
        buffer.insert(msg);

        // record stats
        bufferSizeVector.record(buffer.getLength());

        // if the server is idle
        if (!endServiceEvent->isScheduled())
        {
            // start the service
            scheduleAt(simTime() + 0, endServiceEvent);
        }
    }
}

#endif
