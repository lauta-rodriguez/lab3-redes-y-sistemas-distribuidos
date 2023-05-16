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
    unsigned int droppedPackets; // acc for the number of dropped packets
    cOutVector packetDropVector;
    cOutVector bufferSizeVector;

    // keeps track of how long should the  handling algorithm run
    const unsigned int TIMER = 5;
    const unsigned int PENALTY_WINDOW = 10;
    const unsigned int k = 20;
    unsigned int mod;
    bool bottleneck;

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

    BOTTLENECK_WINDOW = 0u;
    mod = k;

    bottleneck = false;
}

void TransportTx::finish()
{
    // stats record
    recordScalar("Transmitter Number of dropped packets", droppedPackets);
    recordScalar("Transmitter Final buffer size", buffer.getLength());
}

void TransportTx::handleMessage(cMessage *msg)
{
    // encontramos congestion, seteamos estado de congestion y determinamos
    // en que momento de la simulacion se considera terminado
    if (msg->getKind() == 2 && !bottleneck)
    {
        bottleneck = true;
        // si no pasaron 4 segundos desde la ultima congestion
        if (simTime() - BOTTLENECK_WINDOW < PENALTY_WINDOW)
        {
            mod *= 2;
        }
        BOTTLENECK_WINDOW = simTime() + TIMER;
    }
    // uso un else/if porque no quiero que el cambio anterior provoque una 
    // congestión ficticia
    else if (simTime() >= BOTTLENECK_WINDOW && bottleneck) // pasó la ventana de congestión
    {
        bottleneck = false;
        // reset mode a default value
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
