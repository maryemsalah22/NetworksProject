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
simple Sender
{
    parameters:
        int  WS ;
        double PT = default(0.5);
        double TD = default(1.0); 
        double TO = default(10);
        double ED = default(4.0);
        double DD = default(0.1);
    gates:
        input inPort_crd;
        input inPort_rcv;
        output outPort_rcv;
}
