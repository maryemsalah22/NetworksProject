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

#include "Sender.h"
#include "math.h"
#include "string.h"

Define_Module(Sender);



void Sender::initialize()
{
    //int N = getParentModule()->par("N");
    //int nodeNum = uniform(0, N);
    //EV<<"Send message to node number "<< nodeNum << '\n';
    //cMessage* msg = new cMessage("Hello From Hub");
    //send(msg,"outPort",nodeNum);
    //std::string msgs[5] = {"M1", "M2","M3", "M4","M5"};
    cMessage* msg = new cMessage("M1");
    send(msg,"outPort_rcv");
}

void Sender::handleMessage(cMessage *msg)
{
    // TODO - Generated method body

    if (msg->isSelfMessage()){
            EV<<"Send message number to receiver "<< '\n';
            cMessage* msg = new cMessage("M1");
            send(msg,"outPort_rcv");
    }
    else{
        double interval=  exponential(2.0);
        scheduleAt(simTime()+interval,new cMessage(""));
    }

}
