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

/**
 * TODO - Generated class
 */
class Sender : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void add_parity(MyMessage_Base* msg);
    void send_msg(std::string m);
    void handle_timeout();
    std::string read_line();
    int increment(int num);
    std::vector<std::string> window;
    std::fstream file;
    int start;
    int end;
    int next_to_send;
    bool hold_send;
    bool resend_window;
    // array of timers for each message sent
    std::vector<cMessage*> timers;

};

#endif
