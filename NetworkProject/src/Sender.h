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

#ifndef __NETWORKPROJECT_SENDER_H_
#define __NETWORKPROJECT_SENDER_H_

#include <omnetpp.h>
#include <fstream>
#include "string.h"
#include "MyMessage_m.h"
using namespace omnetpp;

struct message_info{
    MyMessage_Base* msg;
    std::string errors;
};

/**
 * TODO - Generated class
 */
class Sender : public cSimpleModule
{
  protected:
    virtual void initialize() override;

    //Handlers :
    virtual void handleMessage(cMessage *msg) override;
    void handleTimeout();
    void getErrorCodes(std::string);
    void fill_initial_window();

    //send messages besed on error type
    void sendMessage(MyMessage_Base* m, std::string errors);
    void sendDuplicated(MyMessage_Base* m);
    void sendModified(MyMessage_Base* m);
    void sendLost(MyMessage_Base* m);
    void sendDelayed(MyMessage_Base* m);

    //Timing methods
    void resumeTransmission();
    void processTime();
    void finish() override;

    message_info* readLine();
    //
    void addParity(MyMessage_Base* msg);
    std::string framing(std::string plain_msg);
    int node_number=1;
    double start_time=0;
    int increment(int num);
    std::vector<MyMessage_Base*> window;
    std::vector<std::string>errors;
    std::fstream file;
    std::ofstream outfile;
    int start;
    int end;
    int next_to_send;
    int seq_num;
    MyMessage_Base* duplicatedMsg;
    MyMessage_Base* delayedMsg;
    bool hold_send;
    bool resend_window;
    // array of timers for each message sent
    std::vector<cMessage*> timers;

};

#endif
