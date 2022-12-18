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
#include <fstream>
#include "string.h"
#include <bits/stdc++.h>
#include "utils.h"
Define_Module(Sender);

void Sender::addParity(MyMessage_Base* msg){
    std::string payload = msg->getPayload();
    int parity = Utils::calculate_parity(payload);
    msg->setTrailer(parity);
}


/* Send a self message after TP ro simulate processing time. */
void Sender::processTime(){
    double interval = par("PT").doubleValue();
    cMessage *msg=new cMessage("end processing");
    scheduleAt(simTime()+interval,msg);
}

message_info* Sender::readLine(){
    MyMessage_Base* msg = new MyMessage_Base("new msg");
    std::string line;
    message_info *msg_container = new message_info();
    if(this->file.is_open() && getline(file, line)){//checking whether the file is open and there are lines to be read.
        msg_container->errors = line.substr(0, 4);
        std::string framed_msg=framing(line.substr(5));       //do the framing step for the message that has been read.
        char content[framed_msg.length()+1];        //
        strcpy(content,framed_msg.c_str());         //string processing
        msg->setPayload(content);                   //make Payload holds the message after framing
        msg->setHeader(seq_num);                    //put the message number in the header.
        seq_num=increment(seq_num);
        addParity(msg);
        msg_container->msg=msg;
        EV <<"At Time "<<simTime()<<" Node["<<this->node_number<<"] Introducing channel error"
            "with code =["<< msg_container->errors <<"]"<<'\n';

        return msg_container;
     }
     return nullptr; //if can't open the file or we reached the end, return nullptr.
}


std::string Sender::framing(std::string plain_msg){
    std::string framed_msg=plain_msg;
    int frame_ptr=0;

    for (int i=0;i<plain_msg.size();i++){
        //add the escape char before any escape char or frame char in original message
        if(plain_msg[i]=='$' || plain_msg[i]=='/'){
            framed_msg=framed_msg.substr(0, frame_ptr)+'/'+framed_msg.substr(frame_ptr,framed_msg.size());
            frame_ptr+=1;
        }
        frame_ptr+=1;
    }
    //add the frame char at start and end of the message
    return  '$'+framed_msg+'$';
}


void Sender::sendMessage(MyMessage_Base* m, std:: string errors){
//    TODO:: WHY this line?
    if(m == nullptr) return;

    // Handling the loss of the message
    // if no loss

    int modified= errors[0]=='1'?1:-1;
    std::string Lost= errors[1]=='1'?"Yes":"No";
    int duplicate=errors[2]=='0'?0:2;
    double delay=errors[3]=='0'?0.0:par("ED").doubleValue();
    double channel_time = par("TD").doubleValue();
    MyMessage_Base* copiedMsg = m->dup();

    if(errors[1]=='0'){
            if (errors[0]=='1'){
                // adding modification to the payload
                std::string s(copiedMsg->getPayload());
                s[0] +=1 ;
                char const *pchar = s.c_str();
                copiedMsg->setPayload(pchar);
                EV<<"modified message  = "<<copiedMsg->getPayload() <<endl ;
            }
            if(errors[3]=='1'){
                // Handling the delayed case (send after TD+ED)
                channel_time+=par("ED").doubleValue();

            }
            if(errors[2]=='1'){
                // Handling the duplicate and delayed case (send after TD+ED+DD)
                //sending the 1st message before duplication
                MyMessage_Base* copiedMsg2 = copiedMsg->dup();

//                copiedMsg->setSchedulingPriority(1);
                cSimpleModule::sendDelayed(copiedMsg2, channel_time,"outPort_rcv");

                // time to send duplicate message
                channel_time+= par("DD").doubleValue();
                EV<<"At time ["<<simTime()<<"] Node["<<this->node_number<<"] [send] frame with seq_num=["<<copiedMsg2->getHeader()<<"] "
                     "and payload =["<<copiedMsg2->getPayload()<<"] and trailer=["<<std::bitset<8>(std::to_string(copiedMsg2->getTrailer()))<<"]"
                     " , Modified ["<<modified<<"],"
                     "Lost ["<<Lost<<"]  Duplicate [1], Delay ["<<delay<<"]"<<"\n";

          }



    copiedMsg->setSchedulingPriority(1);
    cSimpleModule::sendDelayed(copiedMsg, channel_time,"outPort_rcv");
    }

    EV<<"At time ["<<simTime()<<"] Node["<<this->node_number<<"] [send] frame with seq_num=["<<copiedMsg->getHeader()<<"] "
        "and payload =["<<copiedMsg->getPayload()<<"] and trailer=["<<std::bitset<8>(std::to_string(copiedMsg->getTrailer()))<<"]"
        " , Modified ["<<modified<<"],"
        "Lost ["<<Lost<<"]  Duplicate ["<<duplicate<<"], Delay ["<<delay<<"]"<<"\n";

//    TODO: Setting timer on 1 even in case of duplication , until confirmation recieved from TA.
    double interval=par("TO").doubleValue();
    EV<<"Waiting for timeout on msg ="<<this->next_to_send<<"at time ="<<simTime()+interval<<"\n";
    scheduleAt(simTime()+interval,timers[next_to_send]);    //start acknowledgement for the sent message.

    next_to_send=increment(next_to_send);
}


void Sender::initialize()
{
    //start processing the initial window after the start time you recieved from the coordinator
   scheduleAt(simTime()+this->start_time,new cMessage("start time"));
}

void Sender::fill_initial_window(){
    file.open("data.txt",std::ios::in);
    seq_num=0;
    next_to_send=0;

    start=0;
    end=par("WS").intValue()-1;
    hold_send=0;

   for (int j=0;j<=end;j++){

       //create N instances of timers.
       std::string s = std::to_string(j);
       char const *pchar = s.c_str();
       EV<<"this is us = "<<pchar<<"\n";
       timers.push_back(new cMessage(pchar));
       //fill the window with initial N messages garbage.
       window.push_back(nullptr);
       errors.push_back("");
   }

   message_info* read_message = readLine();
   window[0]=read_message->msg;
   errors[0]=read_message->errors;
   end=start;
}

/*    This function stops upcoming timers and re-send the timed out message and apply the go back N concept     */
void Sender::handleTimeout(){
    EV<<"Time out event at time ["<<simTime()<<"], at Node["<<this->node_number<<"] for frame with seq_num=["<<start<<"]"<<"\n";
    int i=increment(start);
    while(i != next_to_send){
            EV<<"timer "<< i << " is cancelled";
            cancelEvent(timers[i]);
            i=increment(i);
    }
    next_to_send=start;             //go back N to re-send the timed out message and the next ones.
    // The timeout message should be sent error free
    errors[next_to_send]="0000";
    // and should take the processing time as usual
    processTime();
    hold_send=0;
}



/* circular increment to be within the window size. */
int Sender::increment(int num){
    return (num+1)%par("WS").intValue();
}

void Sender::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()){                      //Sender can receives 3 messages from itself :

        if(!strcmp(msg->getName(),"end processing") ){        //1) it is a self-timer to send a new message after PT(processing time)

            if(!hold_send && window[next_to_send]!= nullptr){                                 //So, it sends a message if there's a not-sent yet message in the window.

               sendMessage(window[next_to_send],errors[next_to_send]);
               int window_size=par("WS").intValue();
               // if there is space in the buffer to read more , the do
               // (frame holds elements less than the window size)
               if ((start+window_size-1)%window_size!=end){
                   message_info* read_message = readLine();
                   end= increment(end);
                   window[end] = read_message->msg;
                   errors[end] = read_message->errors;
               }

               if(increment(end) == next_to_send )          //stop sending temporarily if all messages in the window are sent.
                   hold_send=1,EV<<"Hold sending... "<< '\n';
               else{
                   processTime();
               }
            }
        }
        else if (!strcmp(msg->getName(),"start time")){  //2) it is the start of sending after the time start from coordinator
            fill_initial_window();
            processTime();
        }
        else{                                       //3) it is a time out timer.
            EV<<"Timeout! on message "<<  msg->getName()<< endl;
            handleTimeout();
        }
    }
    else{
        //Receives an acknowledgment from receiver :
        MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
        EV<<"Sender received an ACK "<<mmsg->getAck_number() <<'\n';
        if(mmsg->getFrame_type() == 1){                  //in case of +ve acknowledgement
            int received_ack = mmsg->getAck_number()%par("WS").intValue();
            if(received_ack == increment(start)){                   //accept the expected acknowledge only
                EV<<"Sender deletes ack's timer "<< '\n';
                cancelEvent(timers[start]);              //cancel the timer of the received acknowledge

                hold_send=0;                             //reset the fold flag as there is a new message will be read from network layer.
                message_info* read_message = readLine(); //read the new message.
                // fill the new line in the start since we have just
                // recieved +ve ack on it
                window[start] = read_message != nullptr ? read_message->msg: nullptr ;
                errors[start] = read_message != nullptr ? read_message->errors: "" ;;
                start = increment(start);                //shift the window
                end= increment(end);

                processTime();
            }
        }
    }
    EV<<"Hello from sender handle ,start = "<< start << ",end = "<< end << " next_to_send = " << next_to_send;
}

