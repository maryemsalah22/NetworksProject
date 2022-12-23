package networkproject;

@license(LGPL);

network Network
{
    parameters:
        //int N = default(6);
        //@display("bgb=429,264");
        @display("bgb=552,281");
    submodules:
        Sender_Node: Sender {

            @display("p=98,210");
        }
        Receiver_Node: Receiver {
            @display("p=391,210");
        }
        Coordinator_Node: Coordinator {

            @display("p=238,70");
        }
    connections:
        Sender_Node.inPort_rcv <-- Receiver_Node.outPort;
        Sender_Node.outPort_rcv --> Receiver_Node.inPort;
        Sender_Node.inPort_crd <-- Coordinator_Node.outPort_snd;
        Receiver_Node.inPort_for_coordinator <-- Coordinator_Node.outPort_snd_rcv;
}
