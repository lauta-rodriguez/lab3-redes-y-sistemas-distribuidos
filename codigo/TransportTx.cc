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

    // variables for statistics logging
    unsigned int sentPackets;
    cOutVector bufferSizeVector;

    // defines how many seconds the network is considered to be bottlenecked
    // after a feedback message is recieved
    const unsigned int TIMER = 5;

    // determines a point in time until which the network is considered to be
    // bottlenecked, calculated as simTime() + TIMER
    simtime_t BOTTLENECK_WINDOW;

    // if a bottleneck occurs in less than PENALTY_WINDOW seconds since the last
    // bottleneck, a penalty is applied to the transmitter
    const unsigned int PENALTY_WINDOW = 10;

    // default modifier for serviceTime
    const unsigned int k = 20;

    // variable modifier for serviceTime
    unsigned int mod;

    // flag used for indicating a bottleneck is occurring
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
    // if a feedback message is received and the bottleneck flag is off, then
    // a new bottleneck has occurred
    if (msg->getKind() == 2 && !bottleneck)
    {
        // set bottleneck flag to indicate that a bottleneck is occurring
        bottleneck = true;

        // calculate penalty to be applied to the transmitter if less than
        // PENALTY_WINDOW seconds have passed since the last bottleneck
        if (simTime() - BOTTLENECK_WINDOW < PENALTY_WINDOW)
        {
            // increment modifier used to increase serviceTime of data packets
            mod *= 2;
        }

        // set end time for bottleneck as TIMER seconds from now
        BOTTLENECK_WINDOW = simTime() + TIMER;
    }

    // if the bottleneck window has passed, then the bottleneck is over
    if (simTime() >= BOTTLENECK_WINDOW && bottleneck)
    {
        // reset bottleneck flag to indicate that the bottleneck is over
        bottleneck = false;

        // reset modifier to default value to remove penalty for next bottleneck
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

                // if a bottleneck is occurring, apply modifier to serviceTime
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
