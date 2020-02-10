//
//  MuPlayer.cpp
//  MuMRT
//
//  Created by Carlos Eduardo Mello on 2/17/19.
//  Copyright © 2019 Carlos Eduardo Mello. All rights reserved.
//

#include "MuPlayer.h"

bool MuPlayer::pause = false;
bool MuPlayer::stop = false;
pthread_mutex_t MuPlayer::sendMIDIlock;

MuPlayer::MuPlayer(void)
{
    // initialize all fields of playback pool
    // to default values...
    for(int i = 0; i < MAX_QUEUES; i++)
    {
        eqPool[i].buffer.data = NULL;
        eqPool[i].buffer.max = 0;
        eqPool[i].buffer.max = 0;
        eqPool[i].active = false;
        eqPool[i].loading = false;
        eqPool[i].paused = false;
        eqPool[i].next = 0;
        eqPool[i].material.Clear();
        eqPool[i].queueThread = 0;
        eqPool[i].loadingTime = 0;
    }
    // initialize MIDI objects...
    
#ifdef MUM_MACOSX
    midiClient = 0;
    midiOutPort = 0;
    midiDest = 0;
#endif
    
#ifdef MUM_LINUX
    // RTMidi
    midiout = NULL;
#endif
    
    // clear scheduler thread variable...
    schedulerThread = 0;
}

MuPlayer::~MuPlayer(void)
{
    Reset();
}

void MuPlayer::CleanPlaybackPool(void)
{
    // Clean Playback Pool
    for(int i = 0; i < MAX_QUEUES; i++)
    {
        if(eqPool[i].buffer.data)
            delete [] eqPool[i].buffer.data;
        eqPool[i].buffer.data = NULL;
        eqPool[i].buffer.max = 0;
        eqPool[i].buffer.count = 0;
        eqPool[i].active = false;
        eqPool[i].loading = false;
        eqPool[i].paused = false;
        eqPool[i].next = 0;
        eqPool[i].material.Clear();
        if(eqPool[i].queueThread != 0)
        {
            pthread_cancel(eqPool[i].queueThread);
            eqPool[i].queueThread = 0;
        }
        eqPool[i].loadingTime = 0;
    }
}

bool MuPlayer::Init(void)
{
#ifdef MUM_MACOSX
     long n,i;
    
    OSStatus err = noErr;
    
    // create Client...
    if(midiClient == 0)
    {
        err = MIDIClientCreate(CFSTR("MuM Playback"), NULL, NULL, &midiClient);
        if(err == noErr)
        {
            // Create Output Port...
            err = MIDIOutputPortCreate(midiClient, CFSTR("MuM Output"), &midiOutPort);
            if(err == noErr)
            {
                // List Possible Destinations...
                n = MIDIGetNumberOfDestinations();
                if(n != 0)
                {
                    CFStringRef name;
                    char cname[64];
                    MIDIEndpointRef dest;
                    
                    // List Possible MIDI Destinations...
                    for(i = 0; i < n; i++)
                    {
                        dest = MIDIGetDestination(i);
                        if (dest != 0)
                        {
                            MIDIObjectGetStringProperty(dest, kMIDIPropertyName, &name);
                            CFStringGetCString(name, cname, sizeof(cname), 0);
                            CFRelease(name);
                            cout << "[Destination " << i << "]: " << cname << endl << endl;
                        }
                    }
                    
                    // Choose a MIDI destination for playback...
                    midiDest = MIDIGetDestination(0);
                    
                    if(StartScheduler())
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
#endif
    
#ifdef MUM_LINUX
    unsigned int nPorts;
    string portName;
    
    // RtMidiOut constructor
    try
    {
        midiout = new RtMidiOut(RtMidiOut::LINUX_ALSA, MUM_CLIENT_NAME);
    }
    catch( RtMidiError &error )
    {
        error.printMessage();
        exit( EXIT_FAILURE );
    }
    
    // Check outputs.
    nPorts = midiout->getPortCount();
    
    cout << endl << "Available MIDI ports:" << endl;
    
    for( unsigned int i = 0; i < nPorts; i++ )
    {
        try
        {
            portName = midiout->getPortName(i);
        }
        catch (RtMidiError &error)
        {
            error.printMessage();
        }
        cout << "\tport " << i << ": " << portName << endl;
    }
    
    selectedPort = 0;
    
    if(selectedPort >= 0)
    {
        // Connect to available port.
        midiout->openPort( selectedPort, MUM_PORT_NAME );
    }
    else
    {
        // create a virtual port.
        midiout->openVirtualPort(MUM_PORT_NAME);
    }
    
     if(StartScheduler())
                        return true;

    return false;
    
#endif
}

bool MuPlayer::SelectMIDIDestination(int destNumber)
{
#ifdef MUM_MACOSX
    if(destNumber >= 0) // FIXED: was 'destNumber > 0' - check if it works...
    {
        midiDest = MIDIGetDestination(destNumber);
        if (midiDest != 0)
        {
            return true;
        }
    }
#endif
    
#ifdef MUM_LINUX
    if(destNumber >= 0)
    {
        midiout->closePort();
        midiout->openPort( destNumber, MUM_PORT_NAME );
        selectedPort = destNumber;
        return true;
    }
#endif
    
    return false;
}

string MuPlayer::ListDestinations(void)
{
    string list;
    

#ifdef MUM_MACOSX
    long n,i;
    // Get number of destinations...
    if(midiClient != 0)
    {
        n = MIDIGetNumberOfDestinations();
        if(n != 0)
        {
            CFStringRef name;
            char cname[64];
            MIDIEndpointRef dest;
            
            // List Possible MIDI Destinations...
            for(i = 0; i < n; i++)
            {
                dest = MIDIGetDestination(i);
                if (dest != 0)
                {
                    MIDIObjectGetStringProperty(dest, kMIDIPropertyName, &name);
                    CFStringGetCString(name, cname, sizeof(cname), 0);
                    CFRelease(name);
                    list += cname;
                    list += "\n";
                }
            }
        }
    }
#endif
    
#ifdef MUM_LINUX
    
    unsigned int nPorts;
    string portName;
    
    // Check outputs.
    nPorts = midiout->getPortCount();
    
    for ( unsigned int i = 0; i < nPorts; i++ )
    {
        try
        {
            portName = midiout->getPortName(i);
        }
        catch (RtMidiError &error)
        {
            error.printMessage();
        }
        
        list += portName;
    }

#endif
    
    return list;
}

void MuPlayer::Reset(void)
{
    // Stop scheduler thread
    if(schedulerThread != 0)
    {
        pthread_cancel(schedulerThread);
        schedulerThread = 0;
    }

    // Release all queue buffers and threads
    CleanPlaybackPool();
    
    // Release MIDI components...
#ifdef MUM_MACOSX
    if(midiOutPort != 0)
    {
        CFRelease(&midiOutPort);
        midiOutPort = 0;
    }
    
    if(midiClient != 0)
    {
        CFRelease(&midiClient);
        midiClient = 0;
    }
    
    midiDest = 0;
#endif
    
#ifdef MUM_LINUX
    midiout->closePort();
    delete midiout;
#endif
}

bool MuPlayer::Play(MuMaterial & inMat, int mode)
{
    int i;
    int selectedQueue = -1;
    MuNote note;
    
    // First find a usable event queue...
    if(mode == PLAYBACK_MODE_NORMAL)
    {
        // at the end of this loop, if at
        // least one queue is available,
        // selectedQueue contains its index..
        for (i = 0; i < MAX_QUEUES; i++)
        {
            // if the current queue is not being played or filled,...
            if(eqPool[i].active == false && eqPool[i].loading == false)
            {
                // it can be selected for a new material...
                selectedQueue = i;
                // mark queue as under construction...
                eqPool[i].loading = true;
                break;
            }
        }
        // if unused queue is found...
        if(selectedQueue >= 0)
        {
            // start the queue's working thread...
            StartQueueThread(inMat,selectedQueue);
        }
        else
        {
            // otherwise report failure...
            return false;
        }
    }

    return true;
}

bool MuPlayer::SendEvents(MuMIDIBuffer events)
{
    int selectedQueue = -1;
    int i;
    
    // at the end of this loop, if at
    // least one queue is available,
    // selectedQueue contains its index..
    for (i = 0; i < MAX_QUEUES; i++)
    {
        // if the current queue is not being played or filled,...
        if(eqPool[i].active == false && eqPool[i].loading == false)
        {
            // it can be selected for a new material...
            selectedQueue = i;
            // mark queue as under construction...
            eqPool[i].loading = true;
            break;
        }
    }
    // if unused queue is found...
    if(selectedQueue >= 0)
    {
        // start the queue thread...
        StartQueueThread(events,selectedQueue);
    }
    else
    {
        // otherwise report failure...
        return false;
    }
    return true;
}

bool MuPlayer::StartQueueThread(MuMaterial & inMat, int queueIdx)
{
    int res;
    
    // make a copy of the input material so the thread can
    // work on it safely, as it will be working assynchronously
    eqPool[queueIdx].material = inMat;
    
    // Start the thread...
    res = pthread_create(&(eqPool[queueIdx].queueThread), NULL, MuPlayer::EnqueueMaterial, (void*)(&eqPool[queueIdx]));
    if(res)
    {
        // if we fail, terminate process...
        cout << "THREAD ERROR! - Terminating..." << endl;
        exit(EXIT_FAILURE);
    }
    
    // if successful...
    return true;
}

bool MuPlayer::StartQueueThread(MuMIDIBuffer events, int queueIdx)
{
    int res;
    
    // make a copy of the input event buffer so the thread can
    // work on it safely, as it will be working assynchronously
    eqPool[queueIdx].buffer = events;
    
    // Start the thread...
    res = pthread_create(&(eqPool[queueIdx].queueThread), NULL, MuPlayer::EnqueueEvents, (void*)(&eqPool[queueIdx]));
    if(res)
    {
        // if we fail, terminate process...
        cout << "THREAD ERROR! - Terminating..." << endl;
        exit(EXIT_FAILURE);
    }
    
    // if successful...
    return true;
    
    
    return true;
}

void * MuPlayer::EnqueueMaterial(void* arg)
{
    int numVoices, i;
    MuNote note;
    long numNotes,numEvents,nextEvent, j, k;
    EventQueue * queue = (EventQueue *)arg;
    
    // get the total number of notes in input material...
    numNotes = queue->material.NumberOfNotes();
    
    // each note needs two MIDI events (on/off)
    numEvents = numNotes * 2;
    
    // allocate memory for the note events...
    if(numEvents > 0)
    {
        // Attention! this buffer needs to be released when
        // the scheduler is done sending its events...
        queue->buffer.data = new MuMIDIMessage[numEvents];
        if(queue->buffer.data)
        {
            // If Allocation worked, extract MIDI events from notes...
            queue->buffer.max = numEvents;
            nextEvent = 0;
            numVoices = queue->material.NumberOfVoices();
            
            for(i = 0; i < numVoices; i++)
            {
                numNotes = queue->material.NumberOfNotes(i);
                for (j = 0; j < numNotes; j++)
                {
                    note = queue->material.GetNote(i, j);
                    queue->buffer.data[nextEvent] = note.MIDIOn();
                    nextEvent++;
                    queue->buffer.data[nextEvent] = note.MIDIOff();
                    nextEvent++;
                }
            }
            // After extracting notes from all voices, nextEvent contains
            // the number of events used in the buffer (which should be
            // the same as buffer.max, but just in case...)
            queue->buffer.count = nextEvent;
            
            // sort events by timestamp...
            long n = queue->buffer.count;
            for(j = n; j >= 1; j-- )
            {
                for( k = 0; k < j-1; k++ )
                {
                    if( queue->buffer.data[k].time > queue->buffer.data[k+1].time )
                    {
                        // swap messages...
                        MuMIDIMessage temp;
                        temp = queue->buffer.data[k];
                        queue->buffer.data[k] = queue->buffer.data[k+1];
                        queue->buffer.data[k+1] = temp;
                    }
                }
            }
            queue->material.Clear();
            queue->next = 0;
            queue->paused = false;
            
            // IMPORTANT: LOADING TIME
            // The following timestamp is registering this moment, after
            // the event buffer has been successfully allocated and filled,
            // to be the initial time for playback of this queue. All events
            // in the queue will be referenced  from this point. The amount
            // of microseconds retrieved hear will be added to the timestamp
            // of every event so the scheduler can compare stamps and decide
            // when to send the messages.
            queue->loadingTime = ClockStamp();
            //cout << "[Loading Time]: " << queue->loadingTime << endl;
            
            // after the queue is set to 'active' the scheduler may
            // use it at any moment (even at interrupt time). That's
            // why this MUST BE THE LAST ACTION!
            queue->active = true;
            
            // after the queue is active we turn off the loading flag...
            queue->loading = false;
        }
    }
    
    // after the work is done we terminate this thread...
    pthread_exit(NULL);
}

void * MuPlayer::EnqueueEvents(void* arg)
{
    long i, n;
    EventQueue * queue = (EventQueue *)arg;
    MuMIDIBuffer tempBuff = queue->buffer;
    n = queue->buffer.count;
    
    // allocate memory for the note events...
    if(n > 0)
    {
        // Attention! this buffer needs to be released when
        // the scheduler is done sending its events...
        queue->buffer.data = new MuMIDIMessage[n];
    
        // extract MIDI events from notes...
        if(queue->buffer.data)
        {
            for(i = 0; i < n; i++)
            {
                queue->buffer.data[i] = tempBuff.data[i];
            }
            
            queue->paused = false;
            
            // IMPORTANT: LOADING TIME
            // The following timestamp is registering this moment, after
            // the event buffer has been successfully allocated and filled,
            // to be the initial time for playback of this queue. All events
            // in the queue will be referenced  from this point. The amount
            // of microseconds retrieved hear will be added to the timestamp
            // of every event so the scheduler can compare stamps and decide
            // when to send the messages.
            queue->loadingTime = ClockStamp();
            //cout << "[Loading Time]: " << queue->loadingTime << endl;
            
            // after the queue is set to 'active' the scheduler may
            // use it at any moment (even at interrupt time). That's
            // why this MUST BE THE LAST ACTION!
            queue->active = true;
            
            // after the queue is active we turn off the loading flag...
            queue->loading = false;
        }
    }
    // after the work is done we terminate this thread...
    pthread_exit(NULL);
}


bool MuPlayer::StartScheduler(void)
{
    int res;
    res = pthread_create(&schedulerThread, NULL, MuPlayer::ScheduleEvents, (void*)eqPool);
    if(res)
    {
        cout << "THREAD ERROR! - Terminating..." << endl;
        exit(EXIT_FAILURE);
    }

    return true;
}

// FIX FIX FIX: FINISH IMPLEMENTING THIS CAREFULLY!!
// 1) REMEMBER TO RESET EMPTY QUEUES SO THEY CAN BE REUSED
// 2) REMEMBER TO IMPLEMENT GLOBAL PAUSE AND STOP CORRECTLY
// 3) Individual queue pause and stop must be planned better
//    for later
void * MuPlayer::ScheduleEvents(void * pl)
{
    int i;
    
    MuPlayer * player = (MuPlayer *)pl;
    EventQueue * pool = player->eqPool;
    
    // this thread will terminate
    // when the Player's stop flag is set...
    while (!MuPlayer::stop)
    {
        // only do work if the player is not paused...
        if(!MuPlayer::pause)
        {
            for(i = 0; i < MAX_QUEUES; i++)
            {
                // if current queue is active,...
                if (pool[i].active == true)
                {
                    // look for its next event...
                    MuMIDIMessage msg = pool[i].buffer.data[pool[i].next];
                    long msgTime = (long)(msg.time * ONE_SECOND) + (pool[i].loadingTime);
                    // get current time from the system
                    long currTime = ClockStamp();
                    
                    // if the timestamp on the message is expired...
                    if( currTime >= msgTime)
                    {
                        // schedule it to be sent to destination...
#ifdef MUM_MACOSX
                        SendMIDIMessage(msg,player->midiOutPort, player->midiDest);
#endif
                        
#ifdef MUM_LINUX
                        SendMIDIMessage(msg,player->midiout);
#endif
                        // advance event counter...
                        pool[i].next += 1;
                        // if this is the last event in the buffer,
                        // this queue needs to be reset...
                        if(pool[i].next >= pool[i].buffer.count)
                        {
                            // reset queue
                            delete [] pool[i].buffer.data;
                            pool[i].buffer.data = NULL;
                            pool[i].buffer.count = 0;
                            pool[i].buffer.max = 0;
                            pool[i].paused = false;
                            pool[i].next = 0;
                            pool[i].queueThread = 0;
                            pool[i].loadingTime = 0;
                            // lastly deactivate queue
                            pool[i].active = false;
                            pool[i].loading = false;
                        }
                    }
                }
            } // end MAX_QUEUES loop
            usleep(10); // idle for a moment...
        } // end if(!pause)
        else
        {
            usleep(100); // idle for a moment...
        }
    } // end infinite loop
    
    
    pthread_exit(NULL);
}

#ifdef MUM_MACOSX
void MuPlayer::SendMIDIMessage(MuMIDIMessage msg, MIDIPortRef outPort, MIDIEndpointRef dest)
{
    //pthread_mutex_lock(&sendMIDIlock);
    int byteCount;
    Byte msgBuff[MESSAGE_LENGTH];
    
    if((outPort != 0) && (dest != 0))
    {
        msgBuff[0] = msg.status;
        msgBuff[1] = msg.data1;
        msgBuff[2] = msg.data2;
        
        // If this is a program change or pitchbend message, send 2 bytes...
        if(((msgBuff[0] & 0xF0) == 0xC0) || ((msgBuff[0] & 0xF0) == 0xE0) )
        {
            byteCount = 2;
        }
        else // for all other voice messages send three bytes...
        {
            byteCount = 3;
        }
        
        MIDITimeStamp timestamp = 0.0;
        Byte buffer[1024]; // storage space for MIDI Packets
        MIDIPacketList * packetlist = (MIDIPacketList*)buffer;
        MIDIPacket * packet = MIDIPacketListInit(packetlist);
        packet = MIDIPacketListAdd( packetlist, sizeof(buffer),
                                   packet, timestamp,
                                   byteCount, msgBuff );
        MIDISend(outPort, dest, packetlist);
    }
    
    //pthread_mutex_unlock(&sendMIDIlock);
}
#endif

#ifdef MUM_LINUX
void MuPlayer::SendMIDIMessage(MuMIDIMessage msg, RtMidiOut * midiOut)
{
    //pthread_mutex_lock(&sendMIDIlock);
    int byteCount;
    uByte msgBuff[MESSAGE_LENGTH];
    
    if(midiOut != NULL)
    {
        msgBuff[0] = msg.status;
        msgBuff[1] = msg.data1;
        msgBuff[2] = msg.data2;
        
        // If this is a program change or pitchbend message, send 2 bytes...
        if(((msgBuff[0] & 0xF0) == 0xC0) || ((msgBuff[0] & 0xF0) == 0xE0) )
        {
            byteCount = 2;
        }
        else // for all other voice messages send three bytes...
        {
            byteCount = 3;
        }
        
      midiOut->sendMessage( msgBuff, 3);
    }
    
    //pthread_mutex_unlock(&sendMIDIlock);
}

#endif

void MuPlayer::Pause(bool T_F)
{
    pause = T_F;
}

void MuPlayer::Stop(void)
{
    stop = true;
}




