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
    cMessage *endServiceEvent;
    simtime_t serviceTime;

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
}

void TransportRx::finish()
{
    // stats record
    recordScalar("Receiver: Number of dropped packets", droppedPackets);
    recordScalar("Receiver: Final buffer size", buffer.getLength());
}

void TransportRx::handleMessage(cMessage *msg)
{
    // detect if msg is Feedback, coming from the SubNet
    if (msg->getKind() == 2)
    {
        // send it to the Transmitter right away
        send(msg, "connSubnet$o");
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
            send(pkt, "fromRxToSink");
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
void TransportRx::enqueueInBuffer(cMessage *msg)
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
        /*if (buffer.getLength() >= threshold)
        {
            // Feedback message initialization
            FeedbackPkt *feedbackPkt = new FeedbackPkt();
            feedbackPkt->setBufferRXFull(true);

            // set packet type to Feedback (2)
            feedbackPkt->setKind(2);

            // the next message to be sent will be Feedback
            buffer.insertBefore(buffer.front(), feedbackPkt);
        }

        else
        {*/
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
