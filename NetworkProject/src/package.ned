package networkproject;

@license(LGPL);

network Network
{
    parameters:
        //int N = default(6);
        //@display("bgb=429,264");
    submodules:
        Sender_Node: Sender {
        }
        
        Receiver_Node: Receiver {
        }
        Coordinator_Node: Coordinator {
        }
        
    connections:
            Sender_Node.inPort_rcv <-- Receiver_Node.outPort;
            Sender_Node.outPort_rcv --> Receiver_Node.inPort;
			Sender_Node.inPort_crd <-- Coordinator_Node.outPort_snd;
}