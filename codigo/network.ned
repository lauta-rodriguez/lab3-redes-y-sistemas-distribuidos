simple Generator
{
    parameters:
        volatile double generationInterval; // sec
        int packetByteSize;
        @display("i=block/source");
    gates:
        output out;
}

simple Queue
{
    parameters:
        int bufferSize;
        @display("i=block/queue;q=buffer");
    gates:
        input in;
        output out;
}

simple Sink
{
    parameters:
        @display("i=block/sink");
    gates:
        input in;
}

simple TransportTx
{
    parameters:
        int bufferSize;
        @display("i=block/queue;q=buffer");
    gates:
        input fromGenToTx;	// inyectamos paquetes en la subred
        inout connSubnet;	// toOut en el pdf, comunica a NodeRx con queue[0] y queue[1]
}

simple TransportRx
{
    parameters:
        int bufferSize;
        @display("i=block/queue;q=buffer");
    gates:
        output fromRxToSink;	// toApp en el pdf
        inout connSubnet;		// toOut en el pdf, comunica a NodeRx con queue[0] y queue[1]
}

module NodeTx
{
    parameters:
        @display("i=block/routing");
    gates:
        input in; // por acá entran los ACK
        output out;
    submodules:
        gen: Generator {
            parameters:
                @display("p=75,50");
        }
        traTx: TransportTx {
            parameters:
                @display("p=75,190");
        }
    connections:
        in --> traTx.connSubnet$i;		// por acá entran los ACK
        gen.out --> traTx.fromGenToTx; 	// salen paquetes del Generator y entran en TransportTx
        traTx.connSubnet$o --> out;		// salen de NodeTx a la subred [0]
}

module NodeRx
{
    parameters:
        @display("i=block/routing");

    gates:
        input in;
        output out;
    submodules:
        sink: Sink {
            parameters:
                @display("p=75,50");
        }
        traRx: TransportRx {
            parameters:
                @display("p=75,190");
        }


    connections:
        in --> traRx.connSubnet$i;	// primer stage: de la subred a el modulo NodeRx (en particular a traRx)
        traRx.connSubnet$o --> out; // la connSubnet$o manda Feedback messages a la subred [1]
        // case study 1
        traRx.fromRxToSink --> {  datarate = 0.5Mbps; } --> sink.in; // la toApp va a la sink

}

network Network
{
    @display("bgl=2");
    submodules:
        Transmitter: NodeTx {
            @display("p=30,30");
        }
        Subnet: Queue {
            @display("p=130,30");
        }
        FeedbackChannel: Queue {
            @display("p=130,90");
        }
        Receiver: NodeRx {
            @display("p=230,30");
        }

    connections:
        Transmitter.out --> {  datarate = 1Mbps; delay = 100us; } --> Subnet.in;
        Subnet.out --> {  datarate = 1Mbps; delay = 100us; } --> Receiver.in;	// case study 1

        Receiver.out --> {  datarate = 1Mbps; delay = 100us; } --> FeedbackChannel.in;		// case study 1
        FeedbackChannel.out --> {  datarate = 1Mbps; delay = 100us; } --> Transmitter.in;		// case study 1

}









