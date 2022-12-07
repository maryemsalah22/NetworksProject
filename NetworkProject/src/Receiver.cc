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

#include "Receiver.h"
#include "math.h"
#include "utils.h"
#include <bits/stdc++.h>

Define_Module(Receiver);

void Receiver::initialize()
{
    send_ack=1;
}

// Returns true if no error, False if error
bool Receiver::check_parity(MyMessage_Base *mmsg){
    int sender_parity = mmsg->getTrailer();
    int reciever_parity = Utils::calculate_parity(mmsg->getPayload());
    return sender_parity == reciever_parity;
}

void Receiver::handleMessage(cMessage *msg)
{
//    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
//    bool no_error = check_parity(mmsg);
    EV<<"Received message from Sender : ";
    EV<<"Received message is:"<< msg->getName();
    msg->setName("ack");
    if (send_ack){
        send(msg,"outPort");
    }


}
