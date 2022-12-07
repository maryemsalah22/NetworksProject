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

void Sender::add_parity(MyMessage_Base* msg){
    std::string payload = msg->getPayload();
    int parity = Utils::calculate_parity(payload);
    msg->setTrailer(parity);
}

std::string Sender::read_line(){

    if (this->file.is_open()){ //checking whether the file is open
        std::string line;
        if(getline(file, line))
             return line;
        }
    return "ended";
}

std::string Sender::framing(std::string plain_msg){
    std::string framed_msg=plain_msg;
    int frame_ptr=0;

    for (int i=0;i<plain_msg.size();i++){
//    add the escape char before any escape char or frame chr in original message
        if(plain_msg[i]=='$' || plain_msg[i]=='/'){
            framed_msg=framed_msg.substr(0, frame_ptr)+'/'+framed_msg.substr(frame_ptr,framed_msg.size());
            frame_ptr+=1;
        }
        frame_ptr+=1;
    }
//    add the frame char at start and end of the message
    return  '$'+framed_msg+'$';

}

void Sender::send_msg(std::string m){
    char content[m.length() + 1];
    strcpy(content,m.c_str());
    cMessage* msg = new cMessage(content);
    send(msg,"outPort_rcv");
    double interval= 4;
    scheduleAt(simTime()+interval,timers[next_to_send]);
    next_to_send=increment(next_to_send);
}


void Sender::initialize()
{
    int i=0;
        file.open("data.txt",std::ios::in);
        while(i < 5){ //checking whether the file is open
             window.push_back(read_line());
             i++;
        }
         timers.push_back(new cMessage("0"));
         timers.push_back(new cMessage("1"));
         timers.push_back(new cMessage("2"));
         timers.push_back(new cMessage("3"));
         timers.push_back(new cMessage("4"));
        start=0;
        end=4;
        hold_send=0;
        next_to_send=0;
        send_msg(window[0]);
        next_to_send=1;
        EV<<"Hello from sender initialize ,start = "<< start
                << ",end = "<< end << " next to send = " << next_to_send;
}
void Sender::handle_timeout(){
    for(int i=0 ; i< 5; i++)
        if(i != start)
            cancelEvent(timers[i]);
    next_to_send=start;
    send_msg(window[next_to_send]);
    hold_send=0;
}

int Sender::increment(int num){
    return (num+1)%5;
}

void Sender::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    //after some random time, sender will send a new message to receiver
    if (msg->isSelfMessage()){
        if(!strcmp(msg->getName(),"send") ){
            EV<<"Sender is sending a new message "<< '\n';
            if(!hold_send){
               send_msg(window[next_to_send]);
               if(increment(end) == next_to_send )
                   hold_send=1;
            }
        }
        else{
            EV<<"first if is invoked";
            handle_timeout();
        }
        double interval=  exponential(2.0);
        scheduleAt(simTime()+interval,new cMessage("send"));
    }
    else{
        EV<<"Sender received an ack "<< '\n';
        //An ack is received, so :
        // 1- shift the window to make space for a new message
        cancelEvent(timers[start]);
        start= increment(start);
        end= increment(end);
        hold_send=0;
        // 2- read the new message
        window[end]= read_line();
        // 3- Send a self dummy message to sender.
        double interval=  exponential(2.0);
        scheduleAt(simTime()+interval,new cMessage("send"));
    }
    EV<<"Hello from sender handle ,start = "<< start
            << ",end = "<< end << " next to send = " << next_to_send;
}
