#ifndef QUEUE
#define QUEUE

#include <string.h>
#include <omnetpp.h>

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
}

void Queue::finish()
{
    // stats record
    recordScalar("Number of dropped packets", droppedPackets);
}

void Queue::handleMessage(cMessage *msg)
{
    bufferSizeVector.record(buffer.getLength());

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
        enqueueMessage(msg);
    }
}
}

/* If the buffer is not full, enqueue message
 * Otherwise, drop it
 */
void Queue::enqueueMessage(cMessage *msg)
{

    // check buffer limit
    if (buffer.getLength() >= par("bufferSize").longValue())
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
        // if the server is idle
        if (!endServiceEvent->isScheduled())
        {
            // start the service
            scheduleAt(simTime() + 0, endServiceEvent);
        }
    }

#endif /* QUEUE */
