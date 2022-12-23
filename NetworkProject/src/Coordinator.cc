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

#include "Coordinator.h"
#include <fstream>
#include "math.h"

Define_Module(Coordinator);

void Coordinator::initialize()
{
    int sender_number=0;
    double start_time=0.0;

    // Read from coordinator file
    std::ifstream coordinator_file("coordinator.txt");
    coordinator_file>>sender_number;
    coordinator_file>>start_time;

    // Coordinator start signal should start with "coordinator_"
    // We created this convention between the coordinator and sender, and between the coordinator and receiver
    // so that the sender knows that it's the start signal.
    // and the receiver knows it's number so it can use it in logging and printing.

    cMessage* sender_number_msg = new cMessage(("coordinator_n"+std::to_string(sender_number)).c_str());
    cMessage* start_time_msg = new cMessage(("coordinator_s"+std::to_string(start_time)).c_str());

    // send the sender node its number and start time.
    send(sender_number_msg, "outPort_snd");
    send(start_time_msg, "outPort_snd");

    cMessage* receiver_number_msg = new cMessage(("coordinator_n"+std::to_string(1 - sender_number)).c_str());
    // send the receiver node its number and start time.
    send(receiver_number_msg, "outPort_snd_rcv");

}

void Coordinator::handleMessage(cMessage *msg)
{

}
