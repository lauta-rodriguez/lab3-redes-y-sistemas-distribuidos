packet FeedbackPkt {
    // acá va lo que vamos a necesitar que el Receiver le diga al Transmitter
    bool bufferSNFull = false;	// se pone en True cuando se necesita control de Congestion
    bool bufferRXFull = false;	// se pone en True cuando se necesita control de Flujo
}
