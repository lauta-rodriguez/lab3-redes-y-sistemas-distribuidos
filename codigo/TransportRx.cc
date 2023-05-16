#ifndef TRANSPORTRX
#define TRANSPORTRX

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

#include "FeedbackPkt_m.h"

// this class is the same as Queue, but with an extra funcionality
// it expects either a data packet or a Feedback message
// if it is a data packet, it will be enqueued in the buffer
// if it is a Feedback message, it will be sent out through the output gate
// that corresponds to communicates the TransportRx module with the FeedbackChannel module
// that gate is "connSubnet$o"

class TransportRx : public cSimpleModule
{
private:
    cQueue buffer;
    cQueue feedbackBuffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;

    // feedback msg counter
    unsigned int fdbcount;

    // variables for statistics logging
    unsigned int droppedPackets; // acc for the number of dropped packets
    cOutVector packetDropVector;
    cOutVector bufferSizeVector;

    // helper function for handling the queuing process in the buffer
    void enqueueInBuffer(cMessage *msg);

public:
    TransportRx();
    virtual ~TransportRx();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);

    virtual void generateFeedback();

    virtual void handleBufferService(cMessage *msg);
};

Define_Module(TransportRx);

TransportRx::TransportRx()
{
    endServiceEvent = NULL;
}

TransportRx::~TransportRx()
{
    cancelAndDelete(endServiceEvent);
}

void TransportRx::initialize()
{
    buffer.setName("Receiver: Data buffer");
    endServiceEvent = new cMessage("endService");

    droppedPackets = 0u;
    packetDropVector.setName("Receiver: Dropped packets");

    bufferSizeVector.setName("Receiver: Buffer size");

    fdbcount = 0u;
}

void TransportRx::finish()
{
    // stats record
    recordScalar("Receiver: Number of dropped packets", droppedPackets);
    recordScalar("Receiver: Final buffer size", buffer.getLength());
    recordScalar("Receiver: Feedback messages count", fdbcount);
}

void TransportRx::handleBufferService(cMessage *msg)
{
    // if packet in buffer, send next one
    if (!buffer.isEmpty())
    {
        // dequeue packet
        cPacket *pkt = (cPacket *)buffer.pop();

        // record stats
        bufferSizeVector.record(buffer.getLength());
        // send packet
        send(pkt, "fromRxToSink");

        // start new service
        // serviceTime now depends on pkt->getDuration()
        serviceTime = pkt->getDuration();
        scheduleAt(simTime() + serviceTime, endServiceEvent);
    }
}

void TransportRx::handleMessage(cMessage *msg)
{
    if (msg == endServiceEvent)
    {
        handleBufferService(msg);
    }
    else if (msg->getKind() == 2) // feedback packet
    {
        generateFeedback();
    }
    else // normal packet
    {
        enqueueInBuffer(msg);
    }
}

void TransportRx::generateFeedback()
{
    // Feedback message initialization
    FeedbackPkt *feedbackPkt = new FeedbackPkt();
    // set packet type to Feedback (2)
    feedbackPkt->setKind(2);
    feedbackPkt->setByteLength(20);
    feedbackPkt->setBufferRXFull(true);
    send(feedbackPkt, "connSubnet$o");
}

/* If the buffer is not full, enqueue message
 * Otherwise, drop it
 */
void TransportRx::enqueueInBuffer(cMessage *msg)
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
        // threshold calculation
        int threshold = 0.8 * par("bufferSize").intValue();

        // if threshold is exceeded, generate a feedback message
        if (buffer.getLength() >= threshold)
        {
            generateFeedback();
        }

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
