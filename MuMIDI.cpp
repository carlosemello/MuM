//
//  MuMIDI.cpp
//  bassTest
//
//  Created by CEM on 28/08/21.
//
#include "MuMIDI.h"
#include <iostream>
using namespace std;

void ShowMIDIMessage(MuMIDIMessage msg)
{
    cout << hex;
    cout <<"[STATUS]: " << (int)msg.status << " ";
    cout <<"[DATA1]: " << (int)msg.data1 << " ";
    cout <<"[DATA2]: " << (int)msg.data2 << endl;
    cout << dec;
}

void RemoveChannel(MuMIDIMessage & msg)
{
    msg.status &= 0xF0;
}
