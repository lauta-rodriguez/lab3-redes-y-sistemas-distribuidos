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
    unsigned int droppedPackets; // acc for the number of dropped packets
    cOutVector packetDropVector;
    cOutVector bufferSizeVector;

    // keeps track of how long should the congestion handling algorithm run
    unsigned int timer;

    bool congestion;

    // helper function for handling the queuing process in the buffer
    void enqueueInBuffer(cMessage *msg);

    // calculate serviceTime dinamycally
    simtime_t getServiceTime(cMessage *msg);

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

    droppedPackets = 0u;
    packetDropVector.setName("Transmitter Dropped packets");

    bufferSizeVector.setName("Transmitter Buffer size");

    timer = 0u;

    congestion = true;
}

void TransportTx::finish()
{
    // stats record
    recordScalar("Transmitter Number of dropped packets", droppedPackets);
    recordScalar("Transmitter Final buffer size", buffer.getLength());
}

void TransportTx::handleMessage(cMessage *msg)
{
    if(msg->getKind()==2)
    {
        ;
    }

    // if msg is signaling an endServiceEvent
    else if (msg == endServiceEvent)
    {
        // if packet in buffer, send next one
        if (!buffer.isEmpty())
        {
            // dequeue packet
            cPacket *pkt = (cPacket *)buffer.pop();
            // send packet
            send(pkt, "connSubnet$o");
            // start new service
            // serviceTime now depends on pkt->getDuration()
            serviceTime = pkt->getDuration();

            scheduleAt(simTime() + serviceTime, endServiceEvent);
        }
    }
    else
    {
        enqueueInBuffer(msg);
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
        // update the dropped packets counter
        droppedPackets++;
        packetDropVector.record(droppedPackets);
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
