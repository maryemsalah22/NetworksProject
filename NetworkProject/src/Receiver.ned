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
package networkproject;
simple Receiver
{
     parameters:
        double LP = default(0.0);
        double PT = default(0.5);
        double TD = default(1.0); 
        int WS = default(4);
    gates:
        input inPort;
        input inPort_for_coordinator;
        output outPort;
}
