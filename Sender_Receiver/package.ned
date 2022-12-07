//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
package Sender_Receiver;

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