//
//  MuRecoder.cpp
//  MuMRT
//
//  Created by Carlos Eduardo Mello on 3/4/19.
//  Copyright © 2019 Carlos Eduardo Mello. All rights reserved.
//

#include "MuRecorder.h"

MuRecorder::MuRecorder(void)
{
    midiClient = 0;
    midiInPort = 0;
    midiSource = 0;
    
    buff1.data = NULL;
    buff1.max = 0;
    buff1.count = 0;
    
    buff2.data = NULL;
    buff2.max = 0;
    buff2.count = 0;
    
    currentBuffer = NULL;
    
    initialStamp = 0;
}

MuRecorder::~MuRecorder(void)
{
    if(midiInPort != 0 && midiSource != 0)
        MIDIPortDisconnectSource(midiInPort,midiSource);
    
    if(buff1.data != NULL)
        delete [] buff1.data;
    
    if(buff2.data != NULL)
        delete [] buff2.data;
}

bool MuRecorder::Init(long buffSize = DEFAULT_BUFFER_SIZE)
{
    // remember when the Recorder started to run...
    initialStamp = ClockStamp();
    
    // ALLOCATE MIDI BUFFERS...
    if(buff1.data == NULL)
    {
        buff1.data = new MuMIDIMessage[buffSize];
        if(buff1.data != NULL)
        {
            buff1.max = buffSize;
            buff1.count = 0;
        }
    }
    
    if(buff2.data == NULL)
    {
        buff2.data = new MuMIDIMessage[buffSize];
        if(buff2.data != NULL)
        {
            buff2.max = buffSize;
            buff2.count = 0;
        }
    }
    
    currentBuffer = &buff1;
    
    // INITIALIZE MIDI PORTS...
    long n,i;
    OSStatus err = noErr;
    
    // create Client...
    if(midiClient == 0)
    {
        err = MIDIClientCreate(CFSTR("MuM Recorder"), NULL, NULL, &midiClient);
        if(err == noErr)
        {
            // Create Input Port...
            err = MIDIInputPortCreate(midiClient, CFSTR("MuM Input"), MIDIInputCallback, this, &midiInPort);           if(err == noErr)
            {
                // Count Available MIDI Sources...
                n = MIDIGetNumberOfSources();
                if(n != 0)
                {
                    CFStringRef name;
                    char cname[64];
                    MIDIEndpointRef source;
                    
                    // List Possible Sources...
                    for(i = 0; i < n; i++)
                    {
                        source = MIDIGetSource(i);
                        if (source != 0)
                        {
                            MIDIObjectGetStringProperty(source, kMIDIPropertyName, &name);
                            CFStringGetCString(name, cname, sizeof(cname), 0);
                            CFRelease(name);
                            cout << "[Source " << i << "]: " << cname << endl << endl;
                        }
                    }
                    
                    // Choose the first MIDI source to get input from...
                    midiSource = MIDIGetSource(0);
                    OSStatus result;
                    result = MIDIPortConnectSource(midiInPort, midiSource, NULL);
                    if(result == noErr)
                        return true;
                }
                else
                {
                    cout << "No MIDI destinations present!\n" << endl;
                }
            }
            else
            {
                cout << "Failed to open output port!\n" << endl;
            }
        }
        else
        {
            cout << "Failed to create MIDI client!\n" << endl;
        }
    }
    else
    {
        cout << "Client already initialized! (call reset MIDI)\n" << endl;
    }
    return false;
}

bool MuRecorder::SelectMIDISource(int sourceNumber)
{
    OSStatus result;
    
    if(midiSource != 0)
    {
        result = MIDIPortDisconnectSource(midiInPort,midiSource);
        if(result != noErr)
            cout << "Couldn't disconnect from previously selected source..." << endl;
    }
    
    if(sourceNumber > 0)
    {
        midiSource = MIDIGetSource(sourceNumber);
        if (midiSource != 0)
        {
            result = MIDIPortConnectSource(midiInPort, midiSource, NULL);
            if(result == noErr)
                return true;
            else
                cout << endl << "MIDI Source Connection Failed!" << endl;
        }
    }
    
    return false;
}

void MuRecorder::ToggleCurrentBuffer(void)
{
    if (currentBuffer == &buff1)
        currentBuffer = &buff2;
    else
        currentBuffer = &buff1;
}

MuMIDIBuffer MuRecorder::GetData(void)
{
    MuMIDIBuffer outBuffer;
    outBuffer.data = NULL;
    outBuffer.max = 0;
    outBuffer.count = 0;
    MuMIDIBuffer * previous;
    
    // Keep the address of where the data is...
    previous = currentBuffer;
    
    // redirect input to the other buffer...
    ToggleCurrentBuffer();
    
    // allocate memory to copy data...
    long i;
    long n = previous->count;
    if(n > 0)
    {
        outBuffer.data = new MuMIDIMessage[n];
        if(outBuffer.data != NULL)
        {
            for(i = 0; i < n; i++)
                outBuffer.data[i] = previous->data[i];
            outBuffer.max = n;
            outBuffer.count = n;
            
            // flush previous buffer, so we don't run out of space
            previous->count = 0;
        }
    }
    return outBuffer;
}

void MuRecorder::MIDIInputCallback (const MIDIPacketList *list, void *procRef,void *srcRef)
{
    //cout << "MIDIInputCallback was called" << endl;
    
    unsigned int i;
    MuRecorder * recorder = (MuRecorder *)procRef;
    const MIDIPacket *packet = &(list->packet[0]);
    UInt16 nBytes,j;
    MuMIDIMessage msg;
    msg.time = ((ClockStamp() - recorder->initialStamp)/ (float)ONE_SECOND);
    
    for ( i = 0; i < list->numPackets; i++)
    {
        nBytes = packet->length;
        
        j = 0;
        while(j < nBytes)
        {
            Byte next = packet->data[j];
            // if this is a note event (on or off)...
            if( ((next & 0xF0) == 0x90) || ((next & 0xF0) == 0x80))
            {
                // extract and store it...
                msg.status = next;
                msg.data1 = packet->data[j+1];
                msg.data2 = packet->data[j+2];
                recorder->AddMessageToBuffer(msg);
                j += 3;
            }
            else // otherwise just ignore it and move to the next byte...
            {
                j++;
            }
        }
        
        // when done with this packet, move to the next...
        packet = MIDIPacketNext(packet);
    }
}

void MuRecorder::AddMessageToBuffer(MuMIDIMessage msg)
{
    if(currentBuffer->count < (currentBuffer->max - 1))
    {
        currentBuffer->data[currentBuffer->count] = msg;
        currentBuffer->count++;
    }
}

// Obs.:
// 1) upon return, if .count == 0 or .data == NULL, join operation failed
// 2) calling code is responsible for releasing buffer memory.
MuMIDIBuffer MuRecorder::JoinMIDIBuffers(MuMIDIBuffer buff1, MuMIDIBuffer buff2)
{
    MuMIDIBuffer res;
    res.data = NULL;
    res.max = 0;
    res.count = 0;
    long i;
    long n = buff1.count + buff2.count;
    
    if(n > 0)
    {
        // allocate memory enough to put data from both buffers...
        MuMIDIMessage * temp = new MuMIDIMessage[n];
        if(temp != NULL)
        {
            // copy data from buff1...
            if((buff1.data != NULL) && (buff1.count > 0))
            {
                for (i = 0; i < buff1.count; i++)
                    temp[i] = buff1.data[i];
            }
            
            // copy data from buff2...
            if((buff2.data != NULL) && (buff2.count > 0))
            {
                for(i = 0; i < buff2.count; i++)
                    temp[i+buff1.count] = buff2.data[i];
            }
            
            res.data = temp;
            res.max = n;
            res.count = n;
        }
    }
    
    return res;
}