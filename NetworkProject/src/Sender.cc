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

void Sender::getErrorCodes(std::string  errors){
    if(errors[0] == '1')
        EV<<"Modification error "<< endl ;
    if(errors[1] == '1')
            EV<<"loss error"<< endl ;
    if(errors[2] == '1')
            EV<<"Duplication error"<< endl ;
    if(errors[3] == '1')
            EV<<"Delay error"<< endl ;
}

/* Send a self message after TP ro simulate processing time. */
void Sender::processTime(){
    double interval = 0.5;//par("PT");
    scheduleAt(simTime()+interval,new cMessage("end processing"));
}

message_info* Sender::readLine(){
    MyMessage_Base* msg = new MyMessage_Base("Noran");
    std::string line;
    message_info *msg_container = new message_info();
    if(this->file.is_open() && getline(file, line)){//checking whether the file is open and there are lines to be read.
        msg_container->errors = line.substr(0, 4);
        std::string framed_msg=framing(line.substr(4));       //do the framing step for the message that has been read.
        char content[framed_msg.length()+1];        //
        strcpy(content,framed_msg.c_str());         //string processing
        msg->setPayload(content);                   //make Payload holds the message after framing
        msg->setHeader(seq_num);                    //put the message number in the header.
        seq_num++;
        addParity(msg);
        msg_container->msg=msg;
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

/* Send a self message to continue transmission. */
void Sender::resumeTransmission(){
    double interval = 0.1; //par("TD");
    scheduleAt(simTime()+interval,new cMessage("send"));
}


void Sender::sendDuplicated(MyMessage_Base* m){
    /*send(m->dup(),"outPort_rcv");   //send the message(a copy of it to prevent sender & receiver from accessing the same memory location)
    double interval= 10;
    scheduleAt(simTime()+interval,timers[next_to_send]);    //start acknowledgement for the sent message.
    duplicatedMsg = m ;
    interval = 0.1; par("DD");
    scheduleAt(simTime()+interval,new cMessage("Duplicate"));*/

}

void Sender::sendModified(MyMessage_Base* m){
    /*MyMessage_Base* modified = m->dup();
    char* new_payload = modified->getPayload();
    new_payload[0]+=1;
    modified->setPayload(new_payload);
    send(modified, "outPort_rcv");*/   //send the message(a copy of it to prevent sender & receiver from accessing the same memory location)
}


void Sender::sendLost(MyMessage_Base* m){
    //send(m->dup(),"outPort_rcv");   //send the message(a copy of it to prevent sender & receiver from accessing the same memory location)
    /*double interval= 10;
    scheduleAt(simTime()+interval,timers[next_to_send]);*/    //start acknowledgement for the sent message.
}


void Sender::sendDelayed(MyMessage_Base* m){
    /*send(m->dup(),"outPort_rcv");   //send the message(a copy of it to prevent sender & receiver from accessing the same memory location)
    double interval= 10;
    scheduleAt(simTime()+interval,timers[next_to_send]);    //start acknowledgement for the sent message.

    delayedMsg = m ;
    interval = par("ED"); //to be changed to delay time
    scheduleAt(simTime()+interval,new cMessage("Delayed"));*/
}


void Sender::sendMessage(MyMessage_Base* m, std:: string errors){
    if(m == nullptr) return;
    //if(errors[2] == '1')
        //sendDuplicated(m);
    MyMessage_Base* copiedMsg = m->dup();
    send(copiedMsg,"outPort_rcv");   //send the message(a copy of it to prevent sender & receiver from accessing the same memory location)

    double interval= 10; //par("TO");
    scheduleAt(simTime()+interval,timers[next_to_send]);    //start acknowledgement for the sent message.
    next_to_send=increment(next_to_send);
}


void Sender::initialize()
{
    EV << "1" << endl;
    int i=0;
    file.open("data.txt",std::ios::in);
    seq_num=0;
    next_to_send=0;
    timers.push_back(new cMessage("0")); //
    timers.push_back(new cMessage("1")); //
    timers.push_back(new cMessage("2")); //create N instances of timers.
    timers.push_back(new cMessage("3")); //
    timers.push_back(new cMessage("4")); //
    while(i < 5){                       //fill the window with initial N messages.
        message_info* read_message = readLine();
        window.push_back(read_message->msg);
        errors.push_back(read_message->errors);
        if(i == 0)
            sendMessage(window[0], errors[0]); //send the first message to start communication.
        i++;
    }
    start=0;
    end=4; ;//par("WS")-1;
    hold_send=0;

}


/*    This function stops upcoming timers and re-send the timed out message and apply the go back N concept     */
void Sender::handleTimeout(){
    int i=increment(start);
    while(i != next_to_send){
            EV<<"timer "<< i << " is cancelled";
            cancelEvent(timers[i]);
            i=increment(i);
    }
    next_to_send=start;             //go back N to re-send the timed out message and the next ones.
    sendMessage(window[next_to_send],errors[next_to_send] );
    hold_send=0;
}



/* circular increment to be within the window size. */
int Sender::increment(int num){
    return (num+1)%5;
}

void Sender::handleMessage(cMessage *msg)
{

    if (msg->isSelfMessage()){                      //Sender can receives 2 messages from itself :

        if(!strcmp(msg->getName(),"send") ){        //1) it is a self-timer to send a new message after PT(processing time)

            if(!hold_send){                                 //So, it sends a message if there's a not-sent yet message in the window.
               EV<<"Sender is sending a new message "<< window[next_to_send]->getPayload()<<'\n';
               sendMessage(window[next_to_send],errors[next_to_send]);
               if(increment(end) == next_to_send )          //stop sending temporarily if all messages in the window are sent.
                   hold_send=1,EV<<"Hold sending... "<< '\n';
               resumeTransmission();
            }
        }
        else{                                       //2) it is a time out timer.
            EV<<"Timeout! on message "<<  msg->getName()<< endl;
            handleTimeout();
        }
    }
    else{
        //Receives an acknowledgement from receiver :
        MyMessage_Base *mmsg = check_and_cast<MyMessage_Base *>(msg);
        EV<<"Sender received an ACK "<<mmsg->getAck_number() << '\n';
        if(mmsg->getFrame_type() == 1){                  //in case of acknowledgement
            int received_ack = mmsg->getAck_number()%5;
            if(received_ack == start){                   //accept the expected acknowledge only
                EV<<"Sender deletes ack's timer "<< '\n';
                cancelEvent(timers[start]);              //cancel the timer of the received acknowledge
                start = increment(start);                //shift the window
                end= increment(end);
                if(window[start] == nullptr)
                    this->endSimulation();

                hold_send=0;                             //reset the fold flag as there is a new message will be read from network layer.
                message_info* read_message = readLine(); //read the new message.
                window[end] = read_message->msg;
                errors[end] = read_message->errors;
                resumeTransmission();
            }
        }
    }
    EV<<"Hello from sender handle ,start = "<< start << ",end = "<< end << " next_to_send = " << next_to_send;
}
