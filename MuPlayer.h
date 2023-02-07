 //
//  MuPlayer.hpp
//  MuMRT
//
//  Created by Carlos Eduardo Mello on 2/17/19.
//  Copyright © 2019 Carlos Eduardo Mello. All rights reserved.
//

/** @file MuPlayer.h
 *
 * @brief MuPlayer Class Interface
 *
 * @author Carlos Eduardo Mello
 * @date 2/17/2019
 *
 * @details
 *
 * MuPlayer orquestrates the realtime playback facilities
 * in the MuM library. It handles everything from scheduling
 * materials for playback to managing working threads
 * and playback controls. Normally only a single object
 * of this class should be instantiated for a MuM based
 * application.
 *
 * For more details about how to use an MuPlayer object to
 * play Musical Materials dynamically during progam execution
 * see the MuPlayer Class Documentation.
 *
 **/


#ifndef MU_PLAYER_H
#define MU_PLAYER_H

#include <pthread.h>
#include <iostream>
#include <string>
#include "MuMaterial.h"
using namespace std;

#ifdef MUM_MACOSX
#include <CoreMIDI/MIDIServices.h>
#endif

#ifdef MUM_LINUX
#include "RtMidi.h"
#include <unistd.h>
#endif

#define MUM_CLIENT_NAME "MuM Playback"
#define MUM_PORT_NAME "MuM Output"

//!@brief Maximum number of queue objects in the playback pool
const int MAX_QUEUES = 10;

//!@brief Normal Playback Mode: imediate playback of scheduled materials
const int PLAYBACK_MODE_NORMAL = 1;

//!@brief Game Playback Mode: materials requested through callback
const int PLAYBACK_MODE_GAME = 2;

//!@brief size of midi message 
const int MESSAGE_LENGTH = 3;

/**
 * @brief Event Queue - MIDI events to be played
 *
 * @details
 *
 * This structure contais a pointer to an array of MIDI events
 * and the number of events in this array. It is used to pass
 * a sequence of events to a player event thread so it can quickly
 * access the necessary data for playback. The structure is only
 * a convenient wrapper for a data buffer and its size. It is assumed
 * by the receiving end that the buffer pointer actually points to
 * a valid vector.
 *
 **/
struct EventQueue
{
    //! @brief buffer of messages to be sent
    MuMIDIBuffer buffer;
    //! @brief index of next message to be sent
    long next;
    //! @brief activation flag: true == active, false == inactive;
    // a queue should not be picked for playback when it is active.
    bool active;
    //! @brief loading flag: set while the working thread is filling up the queue;
    // a queue should not be picked for playback when it is loading.
    bool loading;
    //! @brief pause flag: true == paused, false == running
    bool paused;
    //! @brief queue thread: fills event queue with events from input MuMaterial
    pthread_t queueThread;
    //! @brief reference to input material to be associated with this queue
    MuMaterial material;
    //! @brief time in microseconds when the event queue is loaded and ready to be played
    long loadingTime;
};
typedef struct EventQueue EventQueue;

/**
 * @class MuPlayer
 *
 * @brief MuPlayer Class
 *
 * @details
 *
 * INTRO:
 *
 * MuPlayer is the only class in the MuM Library realtime
 * playback module. Playback is  done with MIDI and
 * can be directed to any enabled MIDI destinations in the system.
 * Normally only a single Player object is needed to play various
 * materials within the lifetime of a MuM based application.
 * MuPlayer assigns MuMaterial objects to playback queues and
 * schedules them for playback. A queue can be scheduled
 * using one of two ways: normal mode or game mode. In normal mode
 * queues are scheduled for immediate playback. In game mode, client
 * code registers a callback function which the Player will call
 * when it needs a new material to keep playback going.
 *
 * Alternatively, any MuMaterial can be played from code with
 * Csound by calling PlaybackWithCsound() on the object to be
 * heard. However this call is synchronous and it starts
 * a system level process which will play the entire length
 * of the material before returning control to the Library.
 * MIDI playback with MuPlayer,on the other hand, can
 * be used at any time during without blocking an application
 * execution flow.
 *
 * @note
 * currently only normal playback mode is implemented
 * Game Mode is TBD. (check the MuM Library
 * page on GitHub frequently for new functionality).
 *
 * INITIALIZATION:
 *
 * Before being used, an MuPlayer needs to be
 * initialized. This initialization creates the necessary
 * infrastructure for the player to interact with the MIDI
 * system in the current platform. Initialization is done
 * with a call to Init(). When using CoreMIDI (Mac OS), Init()
 * creates a MIDI client and an output port associated with
 * it. It also verifies the available MIDI destinations at the
 * moment of the call and displays a list of destinations in
 * standard output (std::cout). By default, Init() selects the
 * first available destination, but this choice can be overruled
 * with a subsequent call to SelectMIDIDestination(). Init() also
 * starts the scheduler thread which is responsible for actually
 * sending the MIDI events. ResetMIDI() releases all MIDI
 * resources created by Init(). After this call, the Player
 * needs to be initialized again in order be usable.
 * Linux implementation behaves ina similar manner
 * but using RtMIDI (ALSA). To opt between Mac/Linux
 * compilation, there is a macro definition in MIDI.h.
 * Currently there is no Windows implementation of MuPlayer.
 *
 * Once initialized, the player can receive requests
 * to play musical materials with calls to Play(). This method
 * takes an MuMaterial as argument and passes the material to
 * a working queue. It also takes a choice of playback
 * mode, which determines when/how the materials will be played.
 * Assuming the player has been initialized beforehand,
 * Play() can be called at any time during program execution.
 *
 * PLAYBACK CONTROLS:
 *
 * MuPlayer can be used to pause, resume or stop
 * playback. It is possible to pause/resume/stop an individual
 * (playback queue) or the entire playback system. See Pause()
 * and Stop() for more details on how to use the playback
 * controls of MuPlayer.
 *
 * UNDER THE HOOD
 *
 * In order to allow many materials to playback simultaneously,
 * MuPlayer employs Posix Threads. The class contains a scheduler thread
 * and several working queue threads. The queue threads are initiated
 * when a request for playback is made. They strip the input materials
 * extracting MIDI events, put them in chronological order inside a
 * MIDI event queue and flag the queue as active. The scheduler, on
 * the other hand, starts when the player is initialized and keeps 
 * looking for active queues in the pool. For every active queue, it finds
 * the next pending event and checks its timestamp. If it is expired the
 * scheduler sends it. Then it moves to the next active queue and so on,
 * until the applications terminates or the player is paused, stopped or
 * reset. When the scheduler reaches the last event in a queue, it
 * resets the queue and marks it as inactive, so it can be used again
 * by the player.
 *
 * The player comunicates to its threads through one-way flags.
 * Only the working queue thread can set the active flag and it only
 * does that once, when the event queue is filled up. After that the 
 * working queue thread will terminate. Only the scheduler thread
 * will read this flag and turn it off when the event queue is completely
 * empty. Only the player will look for inactive queue so it can 
 * play another material. The playback controls are
 * implemented using this same type of mechanism. Each queue has a
 * pause flag which can only be set by the player and read by the
 * scheduler thread. When the scheduler detects an acitve queue,
 * it checks to see if it is paused. If it is, and there is a pending event
 * to be played, it discards it, unless it is a noteOff event.
 * if the entire player is paused, the scheduler ignores all queues
 * and just idles for a few microseconds before checking again.
 *
 * USAGE:
 *
 * When using MuPlayer for playback with MuM, it is necessary
 * to define a MIDI channel for each voice to be played, which can
 * be done with SetChannel(). Once the channel is set, instrument
 * choices can be made with a call to SendProgramChange()
 * to each individual channel, before calling  Play(). This
 * assumes  the synthesizer on the other side of the MIDI
 * setup understands GM requests. Any other required messages
 * during MIDI playback can be delivered by SendMIDIEvents().
 *
 * Normal workflow for playback with MuM can be summarized by
 * the following code sample:
 *
 * @code {.cpp}
 *
 * MuPlayer player;
 * player.Init();
 * //...
 * MuMaterial mat;
 * mat.MajorScale(0.5);
 * mat.SetChannel(0,1);
 * player.SendProgramChange(0,72); // flute (GM)
 * player.Play(mat, PLAYBACK_MODE_NORMAL);
 * //...
 * player.Pause(true); // pause playback
 * //...
 * player.Pause(false); // resume playback
 * //...
 * @endcode
 *
 * @note
 * Currently, only PLAYBACK_MODE_NORMAL is implemented.
 *
 * @warning
 *
 * MuPlayer's playback queue pool is allocated at compile time.
 * Its size is limited by the MAX_QUEUES constant. Depending
 * on expected density of materials in the application, it may
 * be necessary to increase this value before compiling. Otherwise,
 * once all queues in the pool are in use, requests to play new
 * materials may not be honored.
 *
 **/


class MuPlayer
{
    private:
    
    EventQueue eqPool[MAX_QUEUES];  // Playback Pool
    
#ifdef MUM_MACOSX
    MIDIClientRef midiClient;       // MIDI Client (CoreMIDI)
    MIDIPortRef midiOutPort;        // OUTPUT Port (CoreMIDI)
    MIDIEndpointRef midiDest;       // Destination Endpoint (CoreMIDI)
#endif

#ifdef MUM_LINUX
    RtMidiOut * midiout;
    int selectedPort;
#endif
    
    pthread_t schedulerThread;
    static pthread_mutex_t sendMIDIlock;
    
    static bool pause; // flag to communicate pause command to scheduler
    static bool stop;  // flag to communicate stop command to scheduler
    
    public:
    
    // Constructor/Destructor
    
    /**
     * @brief Default Constructor
     *
     * @details
     * This constructor sets internal player data fields to reasonable default values
     *
     **/

    MuPlayer(void);
    
    /**
     * @brief Destructor
     *
     * @details
     * currently, the MuPlayer Destructor does not handle any specific tasks.
    **/
    ~MuPlayer(void);
    
    /**
     * @brief clears all pool data (buffers, materials) and zeroes fields
     *
     * @details
     * This method clears all data from playback pool. It goes through each
     * queue releasing memory buffers, zeroeing structure fields and emptying
     * materials. It is called once by the Player's constructor and is reused
     * when necessary, by other methods.
     *
     **/
    void CleanPlaybackPool(void);
    
    /**
     * @brief Initializes the MuPlayer MIDI configurations and starts event scheduler thread
     *
     * @details
     *
     * Init() is responsible for initializing the MIDI environment
     * for the Player. The CoreMIDI implementation of this method
     * starts out by creating a MIDI client and an associated MIDI output
     * port, so the Library can send MIDI events to the system. After that
     * it requests the list of current destinations to CoreMIDI and displays
     * the list to standard output (std::cout). Init() always selects the
     * first available destination for playback, but this choice can be
     * changed by a subsequent call to SelectMIDIDestination(), using one
     * of the destination numbers displayed by Init(). When using RTMidi
     * Init() will check for available MIDI ports for playback in a similar way.
     *
     * Normally there shouldn't be any problems with initialization, but it
     * is always safer to check the return value for this method. If Init()
     * for any reason returns 'false', it means one or more CoreMIDI/RTMidi
     * calls failed, in which case the MuPlayer object should not be used.
     * Init() may also return false if for some reason it cannot start the
     * scheduler thread.
     *
     * @return
     * bool - true for success, false for error in initializing the MIDI
     * environment or starting the scheduler thread.
     *
     **/
    bool Init(void);
    
    /**
     * @brief Selects a MIDI destination  for playback
     *
     * @details
     *
     * SelectMIDIDestination() takes a destination number and stores it
     * for use by the player, replacing any prior selections.
     * Valid destination numbers are supplied by CoreMIDI /RTMidiand can be
     * verified with a call to DisplayDestinations() or by checking
     * Init()'s console output.
     *
     * @param
     * destNumber (int) - number of the desired MIDI destination 
     *
     * @return
     * void
     *
     **/
    bool SelectMIDIDestination(int destNumber);
    
    /**
     * @brief Lists MIDI destinations available for playback in the system
     *
     * @details
     *
     * ListDestinations() shows the avialable MIDI destinations at the time
     * of the call. The numbers in the list can be used by 
     * SelectMIDIDestination() to choose a target for playback. It should
     * be noted that this information is inherently dynamic. MIDI devices
     * or applications may be started or finished right after a call
     * to ListDestinations(). Therefore it is important to allways check
     * the return value from SelectMIDIDestination().
     *
     * @return
     *
     * string: a standard C++ string containing a list of possible MIDI
     * destinations. This string needs to be parsed to extract each
     * destination. Data in the string is organized according to the
     * following scheme:
     * 
     * line 0: Description for Destination 0
     * line 1: Description for Destination 1
     * line 2: Description for Destination 2
     * line n: Description for Destination n
     *
     * Destinations are separated by a carriage return ("\n").
     * Destination numbers are implied by the line position in 
     * the string. The lines are numbered starting from 0, so the
     * very first line describes destination 0, the second line
     * describes destination 1, and so forth. This string may
     * simply be sent to standart output (std::cout) for visual
     * verification at the console or parsed into discrete units
     * for further manipulation.
     **/
    string  ListDestinations(void);

    /**
     * @brief cancels MIDI setup, stops scheduler and releases all resources
     * used by MuPlayer
     *
     * @details
     *
     * Reset() releases all resources created by Init()
     * and zeroes all  internal variables associated with them.
     * It also stops the scheduler thread and throws away any active
     * queues. Reset() effectively puts MuPlayer back at its original
     * uninitialized state. After a call to ResetMIDI(), an MuPlayer 
     * object cannot be used until it is initialized again.
     *
     * @return
     * void
     *
     **/
    void Reset(void);
    
    /**
     * @brief initiates a playback queue for a requested material and mode
     *
     * @details
     *
     * Play() takes the input MuMaterial object contained in the 'inMat'
     * argument and assigns an event queue from the Player's playback pool
     * to handle that input material.
     *
     * Play() goes through the playback pool only once,
     * looking for inactive queues to use. Once it finds one, it calls
     * StartQueueThread() with a reference to the MuMaterial to be played.
     * That method stores a copy of the material inside the queue structure
     * and starts the working thread, which, in turn, extracts data from the
     * material and activates the queue. If Play() cannot find an inactive
     * queue to use, it returns false, in which case the playback request
     * is not honored.
     *
     * The playback pool is just an array of EventQueue structures, which
     * can be used and reused during the course of the application.
     * The pool has a fixed size which is determined at compile time. 
     * Since not all queues are in use all the time,
     * recycling them allows more efficient use of resources.
     * If for any reason the pool size turns out to be too small, it can be
     * increased by changing the value of MAX_QUEUES at the begining of
     * MuPlayer's header file.      
     *
     * @code {.cpp}
     *
     * const int MAX_QUEUES = 10;
     *
     * @endcode
     *
     * @param
     * inMat (MuMaterial&) - material to be played
     *
     * @param
     * mode (int) - playback mode to be used
     * (currently, only PLAYBACK_MODE_NORMAL is implemented)
     *
     * @return
     *
     * bool - Play() returns false if (a) it couldn't find an idle event
     * queue in the pool or (b) the call to StartQueueThread() fails,
     *  otherwise it returns true (see  StartQueueThread() for details).
     *
     **/
    bool Play(MuMaterial & inMat, int mode);
    
    /**
     * @brief initiates a playback queue for the requested buffer of MIDI
     * events.
     *
     * @details
     *
     * SendEvents() takes the input MIDI event buffer contained in argument 
     * 'inEvents' and assigns an event queue from the Player's playback pool
     * to handle those events.
     *
     * SendEvents() goes through the playback pool only once,
     * looking for inactive queues to use. it calls StartQueueThread() with 
     * the adress of the MIDI buffer to be sent. That method stores a copy
     * of that address inside the queue structure and starts a working thread, 
     * which, in turn, extracts data from the buffer and activates the queue.  
     * If SendEvents() cannot find an inactive queue to use, it returns false, 
     * in which case the send request is not honored.
     *
     * The playback pool is just an array of EventQueue structures, which
     * can be used and reused during the course of the application.
     * The pool has a fixed size which is determined at compile time.
     * Since not all queues are in use all the time,
     * recycling them allows more efficient use of resources.
     * If for any reason the pool size turns out to be too small, it can be
     * increased by changing the value of MAX_QUEUES at the begining of
     * MuPlayer's header file.
     *
     * @code {.cpp}
     *
     * const int MAX_QUEUES = 10;
     *
     * @endcode
     *
     * @note     
     *
     * SendEvents() does not create a separate working thread. Instead, it
     * enqueues MIDI events directly to the playback pool. Therefore, MIDI
     * buffers for SendEvents() should typically be short, so they don't
     * significantly impact performance and, consequently, music 
     * synchronization.
     *
     * @param
     *
     * inEvents (MuMIDIBuffer) - events to be sent. this buffer should be
     * allocated by calling code and is released inside SendEvents().
     *
     * @return
     *
     * bool - SendEvents() returns false if (a) it couldn't find an
     * idle event queue in the pool, otherwise it returns true.
     *
     **/
    bool SendEvents(MuMIDIBuffer events);
    
    /**
     * @brief sends a program change (MIDI instrument (GM) request) to the
     * desired MIDI channel.
     *
     * @details
     *
     * SendProgramChange() calls SendEvents(), with a single MIDI
     * event, which is a Program Change event to the requested MIDI channel.
     * In order to do that, this method allocates a buffer which should be released
     * by SendEvents().
     *
     * @param
     *
     * channel (int) - MIDI channel (0-15) to send event to;
     * pc (int) - Program Change (0 - 127) to be sent;
     *
     * @return
     *
     * bool - SendProgramChange() returns false if either the MID buffer
     * allocation or the call to SendEvents() fail.
     **/
    bool SendProgramChange(unsigned char channel, unsigned char pc);
    
    /**
     * @brief starts an event queue working thread to playback notes
     *
     * @details
     *
     * Each queue has a working thread associated with it. It is
     * used to fill up the queue with MIDI events extracted from
     * the material being played. StartQueueThread() initiates this
     * thread. The thread terminates automatically when all events
     * from the input material are enqueued for playback.
     *
     * @param
     * inMat (MuMaterial &): reference to the material to be enqueued
     *
     * @param
     * queueIdx (int): index of the selected queue
     *
     * @return
     * bool: StartQueueThread() returns false if it cannot start
     * the working thread and true otherwise
     *
     **/
    bool StartQueueThread(MuMaterial & inMat, int queueIdx);

    
    /**
     * @brief starts an event queue working thread for MIDI events
     *
     * @details
     *
     * This overloaded version of StartQueueThread() works with
     * a MIDI buffer instead of an MuMaterial.
     * Each queue has a working thread associated with it. It is
     * used to fill up the queue with MIDI events extracted from
     * the input buffer. StartQueueThread() initiates this
     * thread. The thread terminates automatically when all events
     * from the input buffer are enqueued for playback.
     *
     * @param
     * inMat (MuMIDIBuffer): buffer of events to be enqueued
     *
     * @param
     * queueIdx (int): index of the selected queue
     *
     * @return
     * bool: StartQueueThread() returns false if it cannot start
     * the working thread and true otherwise
     *
     **/
    bool StartQueueThread(MuMIDIBuffer events, int queueIdx);
    
    /**
     * @brief extracts MIDI events from input material and puts them
     * in the corresponding playback event queue.
     *
     * @details
     *
     * EnqueueMaterial() is the thread function for a queue's working
     * thread. It is initiated by StartQueueThread() and is responsible
     * for getting each note from the input material converted to MIDI
     * events and placed in the queue in chronological order, so they
     * can be scheduled for playback by the scheduler thread. When this
     * method concludes its work, it sets the queue's 'active' flag to
     * true, so its events can be accessd  by the scheduler.
     *
     * @param
     * arg (void*) - this argument should receive the address of
     * the event queue this working thread will be operating on.
     *
     * @return
     * void *:  EnqueueMaterial() terminates when the tread exits
     *
     **/
    static void * EnqueueMaterial(void* arg);
    
    
    /**
     * @brief copies MIDI events from input buffer to the
     * corresponding playback event queue.
     *
     * @details
     *
     * EnqueueEvents() is a thread function for a queue's working
     * thread. It is initiated by the buffer verion ofStartQueueThread() 
     * and is responsible for copying each MIDI event in the buffer to
     * be placed in the queue in chronological order, so they can be
     * scheduled for playback by the scheduler thread. When this method
     * concludes its work, it sets the queue's 'active' flag to true,
     * so its events can be accessd  by the scheduler.
     *
     * @param
     * arg (void*) - this argument should receive the address of
     * the event queue this working thread will be operating on.
     *
     * @return
     * void *:  EnqueueEvents() terminates when the tread exits
     *
     **/
    static void * EnqueueEvents(void* arg);
    
    /**
     * @brief starts the event scheduling thread
     *
     * @details
     *
     * StartScheduler() initiates the MIDI event scheduling thread
     * within an MuPlayer. Once successfully started, the scheduler
     * will keep looking for pending events on every active queue
     * untill it is stopped or paused, or the Player is reset/destroyed.
     *
     * @return
     * bool: StartScheduler() returns false if it cannot start
     * the scheduler thread and true otherwise
     *
     **/
    bool StartScheduler(void);
    
    /**
     * @brief 
     * scheduler thread function: gets data from queues and sends to MIDI
     * system at the appropriate time
     *
     * @details
     *
     * ScheduleEvents() is the scheduler thread function. It is started
     * from StartScheduler() and runs continuously until the Player is
     * stopped. Before starting its main loop, ScheduleEvents() checks 
     * if the pause flag is set by the Player, in which case it will
     * just idle until the next turn. If the player is not paused,
     * it goes through each queue in the pool, checking if they are active. 
     * For any active queues, ScheduleEvents() will look for the next event 
     * in the queue, check its timestamp, and send it to the MIDI system 
     * if the stamp is expired.
     *
     * @param
     * pool (void *): pointer to the player object; as the scheduler thread
     * function is static, it needs to have access the playback pool to allow
     * access to the event queues. This void pointer should contain the 
     * address of the player object which started the event scheduler.
     * @return
     * void *:  ScheduleEvents() terminates when the tread exits
     *
     **/
    static void * ScheduleEvents(void * pl);
    
    /**
     * @brief sends MIDI messages to the MIDI System
     *
     * @details
     *
     * SendMIDIMessage() is called by ScheduleEvents() to deliver
     * a single MIDI message at a time to its destination.
     * The time stamp within 'msg' is always ignored.
     * SendMIDIMessage() always delivers every message immediately.
     * Keeping track of time between events is done by calling code.
     *
     * @note
     *
     * This method should NOT be called directly by client code. It is
     * meant to be called by MuPlayer internal code running on another
     * thread.
     *
     * Mac OS:
     *
     * @param
     * msg (MuMIDIMessage) - MIDI event to be delivered
     *
     * @param
     * outPort (MIDIPortRef) - MIDI Output Port associated with the
     * Playback Manager's MIDI Client (CoreMIDI)
     *
     * @param
     * dest (MIDIEndpointRef) - Destination Endpoint selected by the
     * Playback Manager from the lst of available destinations in the
     * MIDI system (CoreMIDI)
     *
     * Linux:
     *
     * @param
     * msg (MuMIDIMessage) - MIDI event to be delivered
     *
     * @param
     * midiOut (RtMidiOut *) - output port (RtMidi)
     *
     *
     * @return
     * void
     *
     **/
#ifdef MUM_MACOSX
    static void SendMIDIMessage(MuMIDIMessage msg, MIDIPortRef outPort, MIDIEndpointRef dest);
#endif
    
#ifdef MUM_LINUX
    static void SendMIDIMessage(MuMIDIMessage msg, RtMidiOut * midiOut);
#endif
    
    /**
     * @brief pauses playback for all active queues in the playback pool
     *
     * @details
     *
     * Pause() can be used to pause and resume playback for all event queues
     * controlled by the Player. Its single argument defines which
     * action will take place after the call. If 'T_F' contains 'true', the
     * Player pauses all queues. If it contains 'false', playback is resumed
     * in all queues. 
     *
     * @param
     * T_F (bool) - true == pause, false == resume
     *
     * @return
     * void
     *
     **/
    void Pause(bool T_F);
    
    /**
     * @brief stops all playback and cancels all event queues
     *
     * @details
     *
     * Stop() can be used to stop playback. When Stop() is called,
     * all event queues are deactivated. It is not possible to resume
     * previously scheduled playback once the Player issues a
     * stop command. It is possible, however to make new requests, as
     * long as the player is not Reset().
     *
     * @return
     * void
     *
     **/
    void Stop(void);
};

#endif /* MU_PLAYER_H */
