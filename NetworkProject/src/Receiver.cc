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


/* circular increment to be within the window size. */
int Receiver::increment(int num){
    return (num+1)%par("WS").intValue();
}

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
    // open output file
    this->outfile = std::ofstream("receiverlogfile.txt");
}

void Receiver::finish()
{
    // close output file
    this->outfile.close();
}

// Returns true if no error, False if error
bool Receiver::check_parity(MyMessage_Base *mmsg){
    int sender_parity = mmsg->getTrailer();
    int reciever_parity = Utils::calculate_parity(mmsg->getPayload());
    return sender_parity == reciever_parity;
}

void Receiver::handleMessage(cMessage *msg)
{

    // When the coordinator sends the receiver number.
    if(std::string(msg->getName()).substr(0, 12) == "coordinator_"){

        // save the node number
        this->node_number = std::stoi(std::string(msg->getName()).substr(13));
    }
    else{
        MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);

        if(msg->isSelfMessage()){

            bool tosend=(rand()%100)<((1-par("LP").doubleValue())*100);
//            bool tosend=true;

            std::string contol_type= mmsg->getFrame_type()==1?"ACK":"NACK";
            std::string loss= tosend==true?"No":"Yes";

            if(tosend==true) {
                cSimpleModule::sendDelayed(mmsg,par("TD").doubleValue(),"outPort");
            }

            this->outfile<<"At time["<<simTime()<<"], Node["<<this->node_number<<"] Sending ["<<contol_type<<"] with number "
                    "["<<mmsg->getAck_number()<<"] , loss ["<<loss<<"]"<<"\n";
            EV<<"At time["<<simTime()<<"], Node["<<this->node_number<<"] Sending ["<<contol_type<<"] with number "
                "["<<mmsg->getAck_number()<<"] , loss ["<<loss<<"]"<<"\n";

        }
        else{

            int seq_num = mmsg->getHeader();

            if(expected_frame_num != seq_num)
               return ;

            bool no_error = check_parity(mmsg);
            if(no_error){
                mmsg->setFrame_type(1);
                expected_frame_num = increment(expected_frame_num);
            }
            else {mmsg->setFrame_type(2);}

            mmsg->setAck_number(expected_frame_num);

            this->window=mmsg;
            MyMessage_Base *self_msg=  this->window->dup();
            msg->setSchedulingPriority(0);

            std::string received_msg= deframing(mmsg->getPayload());

            double interval = par("PT").doubleValue();
            scheduleAt(simTime()+interval,self_msg);

        }
    }
}
