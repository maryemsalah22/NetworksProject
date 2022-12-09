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

std::string Receiver::deframing(std::string framed_msg){

    std::string plain_msg="";
    int msg_ptr=1;
    while(msg_ptr<framed_msg.size()-1){
        if(framed_msg[msg_ptr]=='/'){
            msg_ptr++;
        }
        plain_msg=plain_msg+framed_msg[msg_ptr];
        msg_ptr++;
    }

    std::cout<<framed_msg<<std::endl<<plain_msg<<std::endl;
    return plain_msg;

}

void Receiver::initialize()
{
    expected_frame_num=0;
}

// Returns true if no error, False if error
bool Receiver::check_parity(MyMessage_Base *mmsg){
    int sender_parity = mmsg->getTrailer();
    int reciever_parity = Utils::calculate_parity(mmsg->getPayload());
    return sender_parity == reciever_parity;
}

void Receiver::handleMessage(cMessage *msg)
{
    MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
    int seq_num = mmsg->getHeader();
    if(expected_frame_num != seq_num)
        return ;

    bool no_error = check_parity(mmsg);
    deframing(mmsg->getPayload());
    if(no_error)
        mmsg->setFrame_type(1);
    else mmsg->setFrame_type(2);

    bool tosend=(rand()%100)<(1-par("LP").doubleValue())*100;
    if(tosend==true){
         mmsg->setAck_number(seq_num);
         send(mmsg, "outPort");
         EV<<"Receiver sends ack "<<  seq_num << '\n';
         expected_frame_num++;
    }
    else EV<<"Received loses an ack "<< seq_num << '\n';
}
