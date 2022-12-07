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

Define_Module(Sender);


std::string Sender::read_line(){
    if (this->file.is_open()){ //checking whether the file is open
        std::string line;
        if(getline(file, line))
             return line;
        }
        return "ended";
}
void Sender::initialize()
{
    int i=0;
    file.open("data.txt",std::ios::in);
    while(i < 5){ //checking whether the file is open
         window.push_back(read_line());
         i++;
    }
    start=0;
    end=4;

    char content[window[0].length() + 1];
    strcpy(content,window[0].c_str());
    cMessage* msg = new cMessage(content);
    send(msg,"outPort_rcv");
    next_to_send=1;
    EV<<"Hello from sender initialize ,start = "<< start
            << ",end = "<< end << "next to send = " << next_to_send;
}


int Sender::increment(int num){
    return (num+1)%5;
}

void Sender::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
    //after some random time, sender will send a new message to receiver
    if (msg->isSelfMessage()){
            EV<<"Sender is sending a new message "<< '\n';
            if(increment(end) != next_to_send){
                char content[window[next_to_send].length() + 1];
                strcpy(content,window[next_to_send].c_str());
                cMessage* msg = new cMessage(content);
                send(msg,"outPort_rcv");
                next_to_send=increment(next_to_send);
            }
    }
    else{
        EV<<"Sender received an ack "<< '\n';
        //An ack is received, so :
        //read a new message from text file.
        // 1- shift the window to make space for a new message
        start= increment(start);
        end= increment(end);
        // 2- read the new message
        window[end]= read_line();
        // 3- Send a self dummy message to sender.
        double interval=  exponential(2.0);
        scheduleAt(simTime()+interval,new cMessage("send"));
    }
    EV<<"Hello from sender handle ,start = "<< start
            << ",end = "<< end << "next to send = " << next_to_send;
}
