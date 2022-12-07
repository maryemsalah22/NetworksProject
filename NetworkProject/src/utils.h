/*
 * utils.h
 *
 *  Created on: Dec 7, 2022
 *      Author: User
 */

#ifndef UTILS_H_
#define UTILS_H_


#include <omnetpp.h>
#include <iostream>
#include <bitset>

class Utils
{
  public:
    static int calculate_parity(std::string payload){
        int n = payload.size();
        int parity = 0;

        // loop over message characters
        for (int i=0;i<n;i++) {
            std::bitset<8> chbits(payload[i]);
            // loop over character bits
            for (int j = 0; j < chbits.size(); j++) {
                // invert the parity whenever a '1' bit.
                if(chbits[j] & 1){
                    parity = 1 - parity;
                }
            }
        }
        return parity;
    }
};


#endif /* UTILS_H_ */
