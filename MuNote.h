//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuNote.h
 *
 * @brief Note Class Interface
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 * 
 * @details
 *
 * This file describes the interface for MuM's Note Class. Notes are used in
 * MuM to represent discrete sonic events (MuNotes) which are contained by 
 * music material objects (MuMaterial), organized in voices (MuVoice) and modified 
 * by the material's methods.
 *
 **/

#ifndef _MU_NOTE_H_
#define _MU_NOTE_H_

#include "MuParamBlock.h"


// Constants


//!@brief difference in octaves between MIDI and Csound formats
/**
* @details
*
* This is used to convert between MIDI and Csound pitch formats.
*
* CS-OCTAVE = ( MIDI-VALUE / ONE_OCTAVE ) + OCTAVE_OFFSET
*
* CS-PITCH-CLASS = MIDI-VALUE % ONE_OCTAVE
*
**/
const short OCTAVE_OFFSET = 3;

// Defs

/** 
 * @brief Pitch information in Csound format
 *
 * @details
 * This structure defines the pitch for a given note by separating
 * pitch-class and octave content. It is used to help MuM deal with
 * Csound score information (reading and writing) and to make internal 
 * calculations in which this separation is necessary.
 *
 * @warning
 *
 * The fields contained in this structure are meant for use in Csound 
 * and their values should not exceed documented limitations to avoid  
 * undesired musical output.
 **/
struct cs_pitch
{	
    //! @brief octave defines an octave index, with the central octave (middle-C) octave set to 8
	short octave;
	//! @brief pitch is a pitch class integer between 0 and 11
    short pitch;
};
typedef struct cs_pitch cs_pitch;

/** 
 * @brief Note information as MIDI events
 *
 * @details
 *
 * This structure is used to describe a note as MIDI data.
 * An MuNote object can output note-on and note-off info,
 * using this struct. See MuNote::MIDIOn() and  MuNote::MIDIOff()
 * for more details.
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
 * @class MuNote
 *
 * @brief Note Class
 *
 * @details
 *
 * MuNote is the most basic musical unit of the MuM composition library. 
 * It is used by every class/layer in MuM to represent discrete sonic events 
 * which can be manipulated by the library's various methods. 
 
 * An MuNote is designed to be output with a standard Csound line layout, 
 * where instrument choice, start time and duration are mandatory; 
 * besides these three pieces of data, two more fields constitute regular data members:
 * pitch and amplitude. Pitch is represented as an integer in the the form of a 
 * MIDI note number (Middle-C = 60) in order to facilitate interval manipulation. 
 * At "string output time", each pitch is converted to Csound pitch notation using 
 * the CsPitch() method and output as the fourth parameter (p4) in every note line. 
 * Amplitude is used to designate overall intensity for the note.
 * Amplitude is stored as a float value between 0 and 1.0. 
 * When output to a Csound score this data member is placed on the 
 * fifth parameter (p5) of each note line. When MIDI output is used, 
 * this value is converted to a 7 bit integer (0-127) and used as 
 * key velocity for each NoteOn event.
 * 
 * Other parameters are defined as floats and placed in an MuParamBlock as needed.
 * However, decisions as to the meaning of each parameter in the block are left to 
 * higher levels on the layer hierarchy (see voice.h, material.h). 
 * Notes only carry these parameters around so they can be used by other classes.
 * 
 **/
class MuNote
{
	private:	
		MuNote *	next;
		uShort		instr;
		float		start;
		float		dur;
		short		pitch;
		float		amp;
		MuParamBlock param;
	
	public:
	
	// Constructors
	
	/** 
	 * @brief Default Constructor
	 *
	 * @details
	 * This constructor sets internal note fields to reasonable default values
	 * 
	**/	
	MuNote(void);
	
	/** 
	 * @brief Initializing Constructor
	 *
	 * @details
	 * This constructor sets internal note fields to the values
	 * obtained from input note object.
	 *
	 * @param
	 * inNote note object being copied
	 *
	 **/	
	MuNote( const MuNote & inNote );
	
	// Operators
	
	/** 
	 * @brief Assignment Operator
	 *
	 * @details
	 * Copies content of assigned object to local note, field
	 * by field
	 *
	 * @param
	 * inNote - note object being assigned 
	 *
	 **/	
	MuNote & operator=( const MuNote & inNote );
	
	/** 
	 * @brief Equality Operator
	 *
	 * @details
	 * Compares the note's content to that of input object. Returns true
	 * if every field contains the same value.
	 *
	 * @param 
	 * inNote - note object being compared 
	 *
	 * @return
	 * <ul>
	 * <li> true - if objects are identical
	 * <li> false - otherwise
	 * </ul>
	 **/	
	bool operator==( const MuNote & inNote );
	
	/** 
	 * @brief Inequality Operator
	 *
	 * @details
	 * Compares the note's content to that of input object. Returns true
	 * if any field contains a different value.
	 *
	 *
	 * @param
	 * inNote - note object being compared 
	 *
	 * @return
	 * <ul>
	 * <li> true - if any field is different
	 * <li> false - otherwise
	 * </ul>
	 **/	
	bool operator!=( const MuNote & inNote );
	
	// Methods
	
	/**
	 * @brief Returns next note in note list
	 *
	 * @details 
	 * Returns the address of the next note, when note
	 * is contained in an MuVoice's note list; When dealing with
	 * isolated notes, this value should be ignored.
	 **/	
	MuNote *	Next(void);
	
	/**
	 * @brief Sets next note in note list
	 *
	 * @details
	 * Replaces address for the next note, with the input
	 * pointer when note is contained in a voice's note list. 
	 * This method and the corresponding field in the MuNote class are 
	 * only used for an MuVoice's internal note list control. When dealing
	 * with isolated notes, user code should ignore this method.
	 *
	 **/	
	void		SetNext(MuNote * nextNote);
	
	/**
	 * @brief Resets note fields to defaults
	 *
	 * @details
	 *
	 * Removes every parameter value from the note's fields, 
	 * returning them to default values (0). It also clears the 
	 * paramblock inside the note.  
	 *
	 **/
	void		Clear(void);
	
	/**
	* @brief Returns the instrument number chosen for the note.
	*
	* @details 
	*
	* At score output time, instrument numbers are used to call an instrument
	* module with a Csound Score's 'i' opcode (p1) or a Program Change Number (PC#)
	* in a MIDI track; when MIDI is used for output, this Program Change Event is 
	* inserted as the first event in the MIDI channel corresponding to the voice to
	* which the note belongs.
	*
	* @return an unsigned 16 bit integer designating an instrument number.
	*
	**/
	uShort		Instr(void);
	
	/**
	 * @brief Sets the instrument choice for the note.
	 *
	 * @details 
	 *
	 * When using notes individually it is possible to assign unique instrument
	 * numbers to each note. However, when a note is inserted in a musical material
	 * (MuMaterial), its instrument choice is changed to reflect that of the voice
	 * which contains it. Therefore, when working with materials it is necessary to 
	 * define instruments for each voice, before adding notes to them (see MuMaterial.h).  
	 * At score output time, the instrument numbers are used to call an instrument module  
	 * in a Csound Score's 'i' opcode (p1) or a Program Change Number (PC#) in a MIDI track;  
	 * when MIDI is used for output, this Program Change Event is inserted as the first  
	 * event in the MIDI channel corresponding to the voice to which the note belongs.  
	 * (obs.: instrument selections intended for MIDI output should limit choices to a   
	 * range of 0 - 127).
	 *
	 * @param newInstr uShort (unsigned 16 bit integer) - instrument number
	 *
	 **/
	void		SetInstr(uShort newInstr);
	
	/**
	 * @brief Returns the note's start time in seconds.
	 *
	 * @details 
	 *
	 * Start() returns the note's start time in seconds. This is the moment  
	 * at which the note begins to sound. A note's start time is relative 
	 * to the MuMaterial object in which it is inserted. In other words, a
	 * start time of zero means the note starts sounding at the begining of 
	 * the material. When a material is moved or inserted the start times for
	 * every note it contains are offset to the new position. See MuMaterial
	 * for more details.
	 *
	 * @return float - time in seconds
	 *
	 **/
	float		Start(void);
	
	/**
	 * @brief Sets the note's start time to the input value.
	 *
	 * @details 
	 *
	 * SetStart() replaces the note's start time with the value passed to it. 
	 * A note's start time is relative to the MuMaterial object in which
	 * it is inserted. When a note is inserted in an MuMaterial object
	 * its position in time will be given by the note's internal start time
	 * field, which is set by this method. Once the note is contained in a
	 * material, its start time may be changed by the material's various
	 * transformation methods. If a note's start time is negative, it is converted 
	 * to zero when inserted in an MuMaterial object. 
	 * 
	 * @param 
	 * newStart float - start time in seconds
	 *
	 **/
	void		SetStart(float newStart);
	
	/**
	 * @brief Returns the note's duration in seconds.  
	 *
	 * 
	 * @return float - duration in seconds
	 *
	 **/
	float		Dur(void);
	
	/**
	 * @brief Sets the note's duration in seconds.  
	 *
	 * 
	 * @param  newDur float - duration in seconds
	 *
	 **/
	void		SetDur(float newDur);
	
	/**
	 * @brief Returns the note's ending point in time (seconds)  
	 *
	 * @details
	 *
	 * End() returns the note's ending point in time (seconds). This corresponds  
	 * to Start() + Dur(). Similarly to its start time, a note's end time is relative 
	 * to the MuMaterial object in which it is inserted.
	 *
	 * @return  float - end time for the note in seconds
	 *
	**/
	float		End(void);
	
	/**
	 * @brief Returns the note's pitch 
	 *
	 * @details
	 *
	 * Pitch() returns the note's pitch. This is an integer designating a key in  
	 * a standard MIDI keyboard, where middle-C = 60. In MIDI output this value  
	 * becomes the first data byte (7 bits) in the coresponding NoteOn event.  
	 * For Csound output, the integer is converted to Csound pitch notation (X.YY) 
	 * ( see CsPitch(), PitchString() and CsString() ).
	 *
	 * @return  short - MIDI pitch code for the note 
	 *
	 **/	
	short		Pitch(void);
	
	/**
	 * @brief Sets the note's pitch 
	 *
	 * @details
	 *
	 * SetPitch() sets the note's pitch. This is an integer designating a key in 
	 * a standard MIDI keyboard, where middle-C = 60. In MIDI output this value  
	 * becomes the first data byte (7 bits) in the coresponding NoteOn event.  
	 * For Csound output, the integer is converted to Csound pitch notation (X.YY) 
	 * ( see CsPitch(), PitchString() and CsString() ). 
	 *
	 * @param 
	 * newPitch short - MIDI pitch value
	 * 
	 **/	
	void		SetPitch(short newPitch);
	
	/**
	 * @brief Sets the note's pitch with cs_pitch
	 *
	 * @details
	 *
	 * SetPitch() sets the note's pitch with the contents of newPitch. 
	 * This is a structure containing separate values for octave and pitch-class,
	 * according to Csound format. This data is converted on assignment to a MIDI
	 * note number.
	 *
	 * @param 
	 * newPitch cs_pitch - a structure containing separate pitch-class and octave values
	 * 
	 **/	
	void		SetPitch(cs_pitch newPitch);
	
	/**
	 * @brief Sets the note's pitch to its pitch-class in middle octave  
	 *
	 * @details
	 *
	 * This method effectively transposes the note's pitch to the
	 * middle octave, from whichever octave it was in.
	 *
	 *
	 **/	
	void		ColapsePitch(void);
	
	/**
	 * @brief Returns the note's amplitude  
	 *
	 * @details
	 *
	 * Amp() returns the note's amplitude - a value between 0.0 and 1.0. This value  
	 * is converted,at output time, to an appropriate format, depending on output target:  
	 * dB (0 - 120) for Csound; key velocity (0-127) for MIDI NoteOn events.  
	 *
	 * @return  float - amplitude (0.0 - 1.0) 
	 *
	 **/	
	float		Amp(void);
	
	/**
	 * @brief Sets the amplitude for the note  
	 *
	 * @details
	 *
	 * SetAmp() sets the amplitude for the note. Input argument should be a value  
	 * between 0.0 and 1.0. This value is converted, at output time, to an appropriate  
	 * format, depending on  output target: dB ( 0 - 120) for Csound; key velocity 
	 * (0-127) for MIDI NoteOn events. if Input argument is negative, SetAmp()  
	 * converts it to 0 (zero) on assignement.  
	 *
	 * @param
	 * newAmp float - amplitude value (0.0 to 1.0)
	 *
	 **/	
	void		SetAmp(float newAmp);
	
	/**
	 * @brief Returns a copy of the note's internal parameter block  
	 *
	 * @details
	 *
	 * Params() returns a copy of the note's internal parameter block. This is  
	 * an instance of the MuParamBlock class, which contains an array of floats  
	 * used to represent a note's extra parameters. Any information needed by  
	 * the note beyond the standard fields -- instrument, start, duration, pitch  
	 * and amplitude -- should be placed in the note's parameter block. User code
	 * is responsible for coordinating parameter values with instrument pFields in
	 * Csound.
	 *
	 * @return  MuParamBlock - the note's extra parameters
	 *
	 **/	
	MuParamBlock 	Params(void);
	
	/**
	 * @brief Stores a copy of the MuParamBlock object passed to it 
	 *
	 * @details
	 *
	 * SetParams() stores a copy of the MuParamBlock object contained in inParams. This is
	 * an instance of the MuParamBlock class, which contains an array of floats  
	 * used to represent a note's extra parameters. Any information needed by  
	 * the note beyond the standard fields -- instrument, start, duration, pitch  
	 * and amplitude -- should be placed in the note's parameter block.	 *
	 *
	 * @param  inParams MuParamBlock - parameter object to be stored
	 *
	 **/	
	void		SetParams(MuParamBlock inParams);
	
	/**
	 * @brief Returns the note's pitch as a cs_pitch structure 
	 *
	 * @details
	 *
	 * CsPitch() returns the note's pitch as a cs_pitch structure. This is a standard C  
	 * strucure containing pitch information separated in two fields: octave and  
	 * pitch class. This is used by the note's internal methods to convert pitch data  
	 * to Csound format, and can be employed by user code when necessary.  
	 *
	 * @return  cs_pitch - a structure containing separate pitch-class and octave values
	 *
	 **/	
	cs_pitch 	CsPitch();
	
	/**
	 * @brief Returns the note's pitch as a string in Csound format 
	 *
	 * @details
	 *
	 * PitchString() returns the note's pitch as a standard C++ string object, containing 
	 * a character string in Csound pitch format: Octave designation  (3-11) followed 
	 * by a dot and pitch class (0-11). This is the string used by the note's internal  
	 * methods to convert pitch data to Csound format; it can be employed by user code 
	 * when necessary.
	 * 
	 * @return  string object - Csound format pitch representation as C++ string ("X.YY")
	 *
	 **/	
	string		PitchString(void);
    
    /**
     * @brief Returns the note's pitch name
     *
     * @details
     *
     * PitchName() returns the note's pitch name for the requested language.
     * Currently supported languages are English (e.g.: A, B, C, D, ...) and
     * Portuguese (e.g.: La, Si, Do, Re, ...). Accidentals are represented by
     * characters '#' (sharp) and 'b' (flat) in both languages
     * (e.g.: F#, Sib, ...)
     *
     * @param  
     * languageChoice (int) - a constant indicating the choice of language to be used;
     * current values are: ENGLISH or PORTUGUESE.
     * @param
     * accidentals (int) - enharmonic choice for altered notes; current values are:
     * ACC_FAVOR_FLATS (1) and ACC_FAVOR_SHARPS (2)
     *
     * @return  string object containing the name of the note
     *
     **/	
    string		PitchName(int languageChoice, int accidentals);
	
	/**
	 * @brief Returns the note's Csound score representation (i-line)  
	 *
	 * @details
	 *
	 * CsString() returns the note's data, as a complete Csound score line,  
	 * in a standard C++ string object. The note's internal fields are incorporated  
	 * into the note line as described bellow: 
	 *
	 *<ul>
	 * <li> instr - instrument number becomes p1  
	 * <li> start - start time becomes p2  
	 * <li> dur - duration becomes p3  
	 * <li> pitch - pitch is converted to a pitch string (see PitchString() ) and placed in p4 
	 * <li> amp - amplitude is converted to dB and placed in p5 
	 * <li> params - the parameters contained in the note's parameter block (if any) are appended sequentially to the 
	 * note line, starting with p6 and moving on till the end of the block 
	 * </ul>
	 * @return string object containing Csound score line representation of the note's data
	 *
	 **/
	string		CsString(void);
	
	/**
	 * @brief Returns an activation event for the note as an MuMIDIMessage struct
	 *
	 * @details
	 *
	 * MIDIOn() converts the note's data to MIDI format and returns the
	 * note-on event for the note. Note data is assigned as follows:
	 * <ul>
	 * <li>Start - becomes time stamp in seconds (time field)
	 * <li>Instr - becomes channel choice in range 0-F (status field - bits 0 through 3)
	 * <li>Pitch - becomes data1 field
	 * <li>Amp - becomes data2 field (range 0 through 127)
	 * </ul>
	 * @return  MuMIDIMessage structure
	 *
	 **/
	MuMIDIMessage MIDIOn(void);
	
	
	/**
	 * @brief Returns a deactivation event for the note as an MuMIDIMessage struct
	 *
	 * @details
	 *
	 * MIDIOff()  converts the note's data to MIDI format and returns the
	 * note-off event for the note. Note data is assigned as follows:
	 * <ul>
	 * <li>::End() - becomes time stamp in seconds (time field)
	 * <li>Instr - becomes channel choice in range 0-F (status field - bits 0 through 3)
	 * <li>Pitch - becomes data1 field
	 * <li>data2 field receives 0 (zero)
	 * </ul>
	 * @return  MuMIDIMessage structure
	 *
	 **/
	MuMIDIMessage MIDIOff(void);
};
#endif
