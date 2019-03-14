//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced,
// copied, modified, and reused, as long as
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuMIDI.h
 *
 * @brief MIDI definitions
 *
 * @author Carlos Eduardo Mello
 * @date 3/3/2019
 *
 * @details
 *
 * This file describes the MIDI related data structures and definitions
 * which are use throughout the MuM Library for input and output within the
 * new Realtime playback and input functionality. Some of the definitions
 * here were previously found in other class header files. They were
 * transfered to this place to facilitate project organization and #include 
 * directives.
 *
 **/

#ifndef MuMIDI_H
#define MuMIDI_H

/**
 * @brief MIDI event structure
 *
 * @details
 *
 * This structure is used to describe a typical MIDI event
 * associated with a time stamp. MIDI events are ued by
 * MuM to output note-on and note-off info, for playback and
 * sequencing. See MuNote::MIDIOn() and  MuNote::MIDIOff()
 * for more details. This structure is also used by MuPlayer
 * in output queues and by MuRecorder in input ring buffers
 **/
struct MuMIDIMessage
{
    //! @brief MIDI status byte: [1XXXCCCC]
    unsigned char status;
    //! @brief MIDI data byte: pitch number (0-127) [0VVVVVVV]
    unsigned char data1;
    //! @brief MIDI data byte: key velocity (0-127) [0VVVVVVV]
    unsigned char data2;
    //! @brief time stamp in seconds
    float time;
};
typedef struct MuMIDIMessage MuMIDIMessage;

/**
 * @brief MIDI Buffer structure
 *
 * @details
 *
 * MuMIDIBuffer is a structure containing a buffer of
 * MuMIDIMessages. The also structure contains  two
 * variables to keep track of the number of messages
 * in the buffer.
 *
 * The 'data' field in this structure is a pointer to
 * receive a dynamically allocated array of type MuMIDIMessage.
 * Normally, code passing this struture is responsible for
 * allocating this memory, while code in the receiving end
 * should free the buffer when it is no longer needed. The
 * other two fields, 'max' and 'count' control buffer size.
 * 'max' should contain the maximum number of elements in
 * the array. This value should be modified only once, when
 * memory is allocated. 'count' stores the number of elements
 * currently in use. For obvious reasons, 'count' should
 * allways be less or equal to 'max'. This simple
 * vector of MIDI messages is used to copy and pass
 * blocks of MIDI data between various parts of MuM,
 * particularly in the input class (see MuRecorder).
 *
 **/
struct MuMIDIBuffer
{
    //! @brief pointer to the first message in the buffer
    MuMIDIMessage * data;
    //! @brief maximum number of messages allowed in the buffer
    long max;
    //! @brief number of used/valid messages in the buffer
    long count;
};
typedef struct MuMIDIMessage MuMIDIMessage;

//! @brief default size for MIDI message buffers
const long DEFAULT_BUFFER_SIZE = 1024;




#endif /* MuMIDI_H */
