#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>

#include "FeedbackPkt_m.h"

using namespace omnetpp;

class Queue : public cSimpleModule
{
private:
    cQueue buffer;
    cMessage *endServiceEvent;
    simtime_t serviceTime;

    // variables for statistics logging
    unsigned int droppedPackets; // acc for the number of dropped packets
    cOutVector packetDropVector;
    cOutVector bufferSizeVector;

    // helper function for handling the queuing process in the buffer
    void enqueueInBuffer(cMessage *msg);

public:
    Queue();
    virtual ~Queue();

protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Queue);

Queue::Queue()
{
    endServiceEvent = NULL;
}

Queue::~Queue()
{
    cancelAndDelete(endServiceEvent);
}

void Queue::initialize()
{
    buffer.setName("buffer");
    endServiceEvent = new cMessage("endService");

    droppedPackets = 0u;
    packetDropVector.setName("Dropped packets");

    bufferSizeVector.setName("Buffer size");
}

void Queue::finish()
{
    // stats record
    recordScalar("Number of dropped packets", droppedPackets);
    recordScalar("Final buffer size", buffer.getLength());
}

void Queue::handleMessage(cMessage *msg)
{
    // if msg is signaling an endServiceEvent
    if (msg == endServiceEvent)
    {
        // if packet in buffer, send next one
        if (!buffer.isEmpty())
        {
            // dequeue packet
            cPacket *pkt = (cPacket *)buffer.pop();
            // send packet
            send(pkt, "out");
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
void Queue::enqueueInBuffer(cMessage *msg)
{

    // threshold calculation
    int threshold = 0.85 * par("bufferSize").intValue();

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
        // if threshold is exceeded, a Feedback message is generated
        if (buffer.getLength() >= threshold)
        {
            // Feedback message initialization
            FeedbackPkt *feedbackPkt = new FeedbackPkt();

            // set packet type to Feedback (2)
            feedbackPkt->setKind(2);

            feedbackPkt->setByteLength(20);

            feedbackPkt->setBufferSNFull(true);

            // the next message to be sent will be Feedback
            buffer.insertBefore(buffer.front(), feedbackPkt);
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

#endif /* QUEUE */
