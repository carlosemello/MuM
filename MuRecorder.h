//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced,
// copied, modified, and reused, as long as
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuRecorder.h
 *
 * @brief MuRecorder Class Interface
 *
 * @author Carlos Eduardo Mello
 * @date 3/3/2019
 *
 * @details
 *
 * MuRecorder introduces MIDI input to the MuM library.
 * The class starts an independent thread that constantly
 * looks for incomming MIDI data an adds it to a pair
 * of input buffers that the rest of the class can access.
 * When user code wants to check for available data, it
 * calls GetData(), which copies available MIDI messages
 * from one of the buffers, leaving the other one free
 * to keep receiving other messages. Normally only a 
 * single object of this class needs to be instantiated 
 * within a MuM based application.
 *
 * For more details about how to use an MuRecorder object
 * see the MuRecorder Class Documentation.
 *
 **/

#ifndef MuRecoder_H
#define MuRecoder_H

#include <vector>
using namespace std;

#include <pthread.h>
#include "MuUtil.h"
#include "MuMIDI.h"

#ifdef MUM_MACOSX
#include <CoreMIDI/MIDIServices.h>
#endif

#ifdef MUM_LINUX
#include "RtMidi.h"
#include <unistd.h>
#endif

/**
 * @class MuRecorder
 *
 * @brief MuRecorder Class
 *
 * @details
 * INTRO:
 *
 * MuRecorder is the class responsible for MIDI input in the
 * MuM Library. The class listens to system MIDI connections
 * from devices and applications and stores received MIDI events
 * in a pair of input buffers. From there, these events can be 
 * retrieved by calling code using MuRecorder's methods.
 *
 * INITIALIZATION:
 *
 * Before being used, an MuRecorder needs to be
 * initialized. This initialization creates the necessary
 * infrastructure for the player to interact with the MIDI
 * system in the current platform. Initialization is done
 * with a call to Init(). When using CoreMIDI, Init()
 * creates a MIDI client and an input port associated with
 * it. It also verifies the available MIDI sources at the
 * moment of the call and displays a list of sources in
 * standard output (std::cout). By default, Init() selects the
 * first available source, but this choice can be overruled
 * with a subsequent call to SelectMIDISource(). ResetMIDI()
 * releases all MIDI resources created by Init(). After this call,
 * the Recorder needs to be initialized again in order be usable.
 *
 * Linux implementation of Init() performs similar tasks as its 
 * Mac (CoreMIDI) counterpart but using the equivalent RtMidi data
 * structures and methods, with an underlying ALSA infrastructure.
 *
 * Once MIDI connections are in place, Init() starts a listener thread
 * which is responsible for actually receiving MIDI events. The listener
 * pols system resources frequently looking for  MIDI messages. When 
 * a message is received, it gets copied to the current input buffer
 * and stamped with current system time with ClockStamp().
 *
 * USING RECORDERS:
 *
 * Once initialized, the recorder object will immediately start
 * listening to incomming events. Each event received from
 * the system is timestamped and stored in the currently
 * available buffer. 
 *
 * Whenever client code needs to get MIDI data it
 * calls GetData(). GetData() toggles current buffer
 * redirecting input to the other buffer, while copying data.
 * This way the listener thread doesn't have to wait to  
 * store new messages. GetData() makes a copy of the
 * available data, puts it into a MIDI buffer structure
 * (MuMIDIBuffer) and returnes it to the caller.
 * It is important to note that these buffer
 * structures depend on dynamic memory
 * allocation and that CALLING CODE IS RESPONSIBLE FOR
 * DEALLOCATING this memory when no longer needed.
 *
 * Internally the MIDI buffers returned by GetData() are very simple
 * structures. They contain three fields: 'data' (which is a pointer
 * a dynamic array of MIDI message structures, 'max' which is
 * the number of elements allocated for this array and 'count'
 * which contains the number of valid elements (elements that are
 * in use). For more details about the buffer structure see MuMIDIBuffer.
 * The buffers returned by GetData() can be added into larger buffers
 * with MuRecorder::JoinMIDIBuffers() so user code can access 
 * all incomming that is continuously stored by the recorder.
 * This data can also be converted to a music material for use in MuM
 * with a call to MuMaterial::LoadMIDIBuffer();
 *
 * UNDER THE HOOD
 *
 * In order to keep working continuously and without generating wrong timestamps,
 * the listener works in a separate thread. Also it stores the data collected
 * from MIDI system in two separate, alternating buffers. Whenever GetData()
 * gets called, the listener stops writing in the current buffer and moves
 * to the other one. Meanwhile, GetData(), operating in the main thread,
 * copies data from the first buffer and returns it to calling code. The
 * two input buffers are pre-allocated when the recorder is initialized,
 * so that when getting new data, the listener never has to allocate memory, 
 * or move data around. It just copies a MIDI message structure into a new 
 * address in one of the arrays. Switching arrays is also a very cheap
 * operation (changing a pointer address) which is done by GetData(), so
 * that, as far as the listener is concerned it is just storing another
 * message to a given address.
 *
 * SAMPLE:
 *
 * There are many ways to use an MuRecorder in a MuM Application.
 * It all depends on how we need to collect input and what we want
 * to do with the it. For example, the recorder could be called
 * continuosly in a tight loop to get small blocks of data and
 * interpret them as they come. Or the app could listen for a
 * certain amount of time or until a certain event arrives, before
 * getting the data out. Whatever the strategy, using MuRecorder
 * usually involves:
 *
 * @code {.cpp}
 *
 * // instantiating a recorder object
 * MuPlayer rec;
 *
 * // initializing it...
 * rec.Init();
 *
 * // giving it some time to record events...
 * usleep(ONE_SECOND);
 *
 * // getting some data from the recorder...
 * MuMIDIBuffer buffer = GetData();
 *
 * // puting it inside a music material so it can be manipulated...
 * MuMaterial mat.LoadMIDIBuffer(buffer, MIDI_BUFFER_MODE_PURGE);
 *
 * // releasing buffer memory...
 * if(buffer.data)
 * delete [] buffer.data;
 *
 * @endcode
 *
 * @note
 * Currently, only PLAYBACK_MODE_NORMAL is implemented.
 *
 * @warning
 *MIDI BUFFERS MUST BE DEALLOCATED BY CALLING CODE to avoid memory leaks.
 *
 */

class MuRecorder
{
    private:
    
    // DATA...
    MuMIDIBuffer buff1;
    MuMIDIBuffer buff2;
    MuMIDIBuffer * currentBuffer; // points to one of the input buffers
    
#ifdef MUM_MACOSX
    // MIDI CONNECTIONS...
    MIDIClientRef midiClient;       // MIDI Client (CoreMIDI)
    MIDIPortRef midiInPort;         // Input Port (CoreMIDI)
    MIDIEndpointRef midiSource;     // Source Endpoint (CoreMIDI)
#endif
    
#ifdef MUM_LINUX
    RtMidiIn * midiIn;
    int selectedPort;
#endif
    
    long initialStamp;
    
    /**
     * @brief toggles the current input buffer to be used by MIDI input
     * callback function
     *
     * @details
     * MuRecorder uses two alternating input buffers to store incomming MIDI
     * events. When client code requests data, the MIDI callback has to be
     * redirected to a different buffer in order to keep running smoothly
     * and avoid conflicts with the data reading routine. So whenever GetData() 
     * is called, it runs ToggleCurrentBuffer() to point the listener to
     * the next available buffer.
     *
     *
     */
    void ToggleCurrentBuffer(void);
    
public:
    // Constructor/Destructor
    
    /**
     * @brief Default Constructor
     *
     * @details
     * This constructor sets internal player data fields to reasonable default values
     *
     **/
    MuRecorder(void);
    
    /**
     * @brief Destructor
     *
     * @details
     * MuRecorder destructor releases memory from the input buffers and 
     * disconnects the input port MIDI sources.
     **/
    ~MuRecorder(void);
    
    /**
     * @brief Initializes the MuRecorder MIDI configuration and installs
     * MIDI callback function
     *
     * @details
     * Init() is responsible for initializing the MIDI environment for the Recorder
     * This part of the library contains some of the most platform dependent code.
     * MuM provides implementations for two platforms which utilize a similar model
     * for capturing MIDI information: CoreMIDI on Mac OS X and RtMidi (ALSA) on Linux.
     * Code dependencies are handled here with conditional compilation macros.
     * Bellow is a brief decription of MIDI initialization for each platform.
     *
     * More details
     *
     * CoreMIDI implementation of this method starts out by creating a
     * MIDI client  and an associated MIDI input port, so the Library
     * can receive MIDI events from the system.
     *
     * RtMidi implementation allocates similar structures, except
     * that the client already has an implict local port. So in RtMidi
     * terms, ports refer to external MIDI entities to which the client
     * connects, which, in CoreMIDI, correspond to MIDI Endpoints (sources
     * or destinations).
     *
     * After creating the input client, Init() installs a MIDI input callback
     * which is later called by the system whenever MIDI data is available
     * for retrieval. After that, the method requests the list of current MIDI
     * sources/ports to the system and displays that list to standard output.
     * This part of initialization is pretty much the same on both platforms.
     *
     * Init() always selects the first available source for
     * input, but this choice can be changed by a subsequent call to
     * SelectMIDISource(), using one of the source numbers displayed by Init().
     *
     * Normally there shouldn't be any problems with initialization, but it
     * is always safer to check the return value for this method. If Init()
     * for any reason returns 'false', it means one or more of the CoreMIDI/RtMidi
     * calls failed, in which case the MuRecorder object should not be used.
     * Init() will also fail if it cannot allocate memory for the input buffers.
     *
     * @param
     * buffSize (long) - size of input buffers; each buffer will be
     * 'buffSize' events long.
     *
     * @return
     * bool - true for success, false for error in initializing the MIDI
     * environment or allocating the input buffers.
     *
     **/
    bool Init(long buffSize);
    
    /**
     * @brief Selects a source for MIDI input
     *
     * @details
     * SelectMIDISurce() takes a source number and stores it
     * for use by the player, replacing any prior selections.
     * Valid source numbers are supplied by CoreMIDI and can be
     * verified with a call to DisplaySources() or by checking
     * Init()'s console output.
     *
     * @param
     * destNumber (int) - number of the desired MIDI source 
     *
     * @return
     * void
     *
     **/
    bool SelectMIDISource(int sourceNumber);
    
    /**
     * @brief returns a buffer structure containing the latest input MIDI events
     *
     * @details
     *
     * GetData() returns recent input data collected by the MIDI input callback
     * in one of the input buffers. Immediately after starting and before copying
     * any data, GetData() redirects the current buffer pointer to a different
     * input buffer, so that the MIDI callback thread can keep doing its job
     * while data is being copied without any conflicts. Once all data is copied, 
     * the method resets the input buffer count to zero so the callback can use 
     * the buffer from the begining next time around.
     *
     *@attention
     *
     * Subsequent calls to GetData() will keep toggling from one buffer to 
     * the other, so that it will never be reading from the same place the 
     * Recorder is storing data. This means that once a block of MIDI data
     * is copied from the input buffer by GetData() it will no longer be 
     * available, as the data will be overwritten by the callback after 
     * the next call to GetData(). Hence client code should store that 
     * data if it intends to reuse it.
     *
     * @return
     *
     * MuMIDIBuffer - GetData() returns a MIDI buffer structure containing a
     * pointer to the data copied from the input buffers. The structure also
     * has a 'max' field with data array size and a 'count' field reporting 
     * the number of elements actually used in the array.
     * Obs.: the two length fields ('max' and 'count') can be used to populate
     * only part of a buffer, when necessary. When returned by GetData(), however,
     * these two fields will always contain the same value. The buffers returned
     * by GetData() can be appended to a larger buffer with a call to
     * JoinMIDIBuffers().
     *
     **/
    MuMIDIBuffer GetData(void);
    
#ifdef MUM_MACOSX
    /**
     * @brief gets called by MIDI system when there is MIDI data available
     *
     * @details
     *
     * MIDIInputCallback() is a readProc function that needs to be
     * provided by client code when ceating a MIDI input port with CoreMIDI.
     * It gets called directly by the system in a special high priority
     * thread, providing MuRecorder with the latest received MIDI data.
     * Data is received in the form of a MIDI packet list, which
     * needs to be parsed in a specific manner in order to retrieve the
     * MIDI packets and ultimately the MIDI events inside it. 
     * This entire process is implemented by MuRecorder in this function
     * (for details on how to parse a MIDIPacketList, please see CoreMIDI
     * documentation)
     *
     * @note
     *
     * MIDIInputCallback is a static method of the MuRecorder class but should
     * not be called by client code.
     *
     * @param
     *
     * list (MIDIPacketList) - a CoreMIDI struture containing packets of MIDI data.
     * THis list is parsed by the function to extract MIDI events which are in turn
     * stored into MuRecorder's input buffers.
     *
     * @param
     *
     * procRef (void *) - This is a context pointer. It contains a pointer to
     * the MuRecorder object where the callback was registered. This is needed
     * to access the input buffers, since the function is static and has no direct
     * connection to the object.
     *
     * @param
     *
     * srcRef (void *) - This other pointer points to the MIDI source CoreMIDI
     * object to which the recorder's input port was conected. It may be used
     * to retrieve information from that source, if necessary.
     *
     **/
    static void MIDIInputCallback(const MIDIPacketList *list, void *procRef,void *srcRef);
#endif
    
#ifdef MUM_LINUX
    /**
     * @brief gets called by MIDI system when there is MIDI data available
     *
     * @details
     *
     * MIDIInputCallback() is a user callback function which gets called by
     * RtMidi whenever there is MIDI data available for reading by MuRecorder.
     * The first argument informs the time (delta) when data has arrived.
     * The Linux/RTMidi version of this function provides the actual MIDI data
     * through a pointer to an an std::vector object (second argument). The final
     * argument is a pointer to the MuRecorder object which is cast to the
     * correct type for retrieving context information (allowing acess to the
     * recorder's internal data and associated methods)
     *
     * @note
     * 
     * MIDIInputCallback is a static method of the MuRecorder class but should
     * not be called by client code. This method is only called by the MIDI system.
     *
     * @param
     *
     * timeStamp (double) - a delta timestamp: when the message was received.
     *
     * @param
     *
     * message (vector<unsigned char> *) - pointer to a standard C++ vector object
     * allocated by RtMidi to store arriving MIDI data and pass it to client code.
     *
     * @param
     *
     * userData (void *) - This is a context pointer. It contains a pointer to
     * the MuRecorder object where the callback was registered. This is needed
     * to access the input buffers, since the function is static and has no direct
     * connection to the object.
     *
     **/

    static void MIDIInputCallback(double timeStamp, vector< unsigned char > * message, void *userData);
#endif
    
    /**
     * @brief stores a single MIDI message in the current input buffer
     *
     * @details
     *
     * AddMessageToBuffer() stores the requested MuMIDIMessage in the next
     * available position of the current input buffer. If the buffer is
     * full, AddMessageToBuffer() fails silently. AddMessageToBuffer() is 
     * called by the MIDIInputCallback when it needs to add a new MIDI 
     * event to one of the input buffers.
     *
     * @warning
     *
     * MIDIInputCallback is a static method of the MuRecorder class but should
     * NEVER be called directly by client code, as this would completely disrupt
     * data count and possibly mess up array boundary controls in the input buffers.
     *
     * @param
     *
     * msg (MuMIDIMessage) - the message being stored by the function. This
     * structure is provided by the MIDI callback function.
     *
     *
     * @return
     *
     * void
     *
    **/
    void AddMessageToBuffer(MuMIDIMessage msg);
    
    /**
     * @brief adds two MIDI buffers and returns a larger one with the joined data
     *
     * @details
     *
     * JoinMIDIBuffers() takes two MuMIDIBuffers as input and returns a larger
     * buffer containing data from both. The method creates a new buffer and
     * copies the contents of 'buff1' and then 'buff2' to it. 
     * 
     * @note
     *
     * This new larger buffer must be released by calling code when it is no 
     * longer needed.
     *
     * @param
     *
     * buff1 (MuMIDIBuffer) - first buffer to be added; data from this buffer
     * goes at the begining of the resulting buffer.
     *
     * @param
     *
     * buff1 (MuMIDIBuffer) - second buffer to be added; data from this buffer
     * goes at the end of the resulting buffer;
     *
     * @return
     *
     * MuMIDIBuffer - the return value is an MuMIDIBuffer structure containing
     * the joined MIDI data. The memory allocated for the 'data' field in the
     * returning buffer structure must be released to avoid memory leaks. If
     * calling code needs a copy of this buffer data, the 'data' field should
     * be deep copied.
     *
     **/
    static MuMIDIBuffer JoinMIDIBuffers(MuMIDIBuffer buff1, MuMIDIBuffer buff2);
    
    /**
     * @brief returns invalid notes extracted from the input buffer      
     *
     * @details
     *
     * ExtractInvalidNotes() extracts from the input buffer all noteOn events that do not have a corresponding noteOff and returns them in another MIDI buffer structure.
     *
     * @note
     *
     * The extracted buffer must be released by calling code when it is no
     * longer needed.
     *
     * @param
     *
     * buff (MuMIDIBuffer) - input buffer containing the notes to be extracted
     *
     *
     * @return
     *
     * MuMIDIBuffer - the return value is an MuMIDIBuffer structure containing
     * the extracted MIDI data. The memory allocated for the 'data' field in
     * the returning buffer structure must be released to avoid memory leaks.
     * If calling code needs a copy of this buffer data, the 'data' field
     * should be deep copied.
     *
     **/
    static MuMIDIBuffer ExtractInvalidNotes(MuMIDIBuffer buff);
    
    /**
     * @brief returns all events of a given type found in the input buffer
     *
     * @details
     *
     * ExtractEventsOfType() extracts from the input buffer all events of the 
     * requested type and returns them in another MIDI buffer structure. The
     * desired event type is indicated by the first parameter (eventType).
     * However, although event types are specified as a status byte mask, only
     * the types defined by specific constants may be requested.
     *
     * @note
     *
     * The extracted buffer must be released by calling code when it is no
     * longer needed.
     *
     * @param eventType (unsigned char) - status byte mask constant used to
     * determine what kind of events should be extracted from input buffer
     *
     * @param buff (MuMIDIBuffer) - input buffer containing the events to be extracted
     *
     * @return
     *
     * MuMIDIBuffer - the return value is an MuMIDIBuffer structure containing
     * the extracted MIDI data. The memory allocated for the 'data' field in
     * the returning buffer structure must be released to avoid memory leaks.
     * If calling code needs a copy of this buffer data, the 'data' field
     * should be deep copied.
     *
     **/
    MuMIDIBuffer ExtractEventsOfType(unsigned char eventType, MuMIDIBuffer buff);
};



#endif /* MuRecoder_H */
