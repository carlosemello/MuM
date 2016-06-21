//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuVoice.h
 *
 * @brief Voice Class Interface
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 * @details
 *
 * a voice  represents an individual sequence
 * of notes inside a music material object, which
 * in turn may contain any number of voices
 *
 * Besides the notes themselves, the voice contains
 * information about instrument choice:
 * an instrument number and an integer accounting for the
 * number of parameters required for each note in the sequence
 * to be correctly rendered by the referred instrument.
 * As explainned in note.h, the parameters are all floats
 * and should be contained in an MuParamBlock within each note
 *
 * Optionally, the voice may also contain a string with 
 * the instrument's code, for Csound rendering.
 *
 **/
#ifndef _MU_VOICE_H_
#define _MU_VOICE_H_

#include "MuNote.h"


const short FIRST_NOTE_INDEX = 0;

// Sorting fields
const short SORT_FIELD_INSTR = 0;
const short SORT_FIELD_START = 1;
const short SORT_FIELD_DUR = 2;
const short SORT_FIELD_PITCH = 3;
const short SORT_FIELD_AMP = 4;


/**
 * @class MuVoice
 *
 * @brief Voice Class
 *
 * @details
 * An MuVoice object represents an individual sequence of notes inside an MuMaterial 
 * (music material) object, which in turn may contain any number of voices. The voice, as 
 * defined here, is analogous to a part in a musical score, but it may contain any number of 
 * simultaneous notes. Therefore an MuVoice can contain the entire composition and may be 
 * the only voice within a given material (see MuMaterial below). Tipically, however, voices 
 * are used to store the notes to be played by individual instruments. Thus, besides the notes 
 * themselves, a voice contains information about instrument choice -- an instrument number 
 * and an integer accounting for the number of parameters required for each note in the 
 * sequence, so that these can be correctly rendered by the referred instrument. As explained 
 * in MuNote.h, parameters other than the ones directly defined in the note class,  are always 
 * floats and should be contained in an MuParamBlock within each note. 
 *
 * The MuVoice class is an internal implementation detail and should not be used directly by 
 * user code. It is documented here in oder to facilitate comprehension of MuM Library and 
 * maintenance of internal code. All the voice functionality is acessessible through the 
 * MuMaterial's interface. 
 *
 **/
class MuVoice
{
    private:	
    
    MuNote *	noteList;
    long	numOfNotes;
    uShort	instrumentNumber;
    uShort	numOfParameters;
    string	instrumentCode;
	
    public:
	
	// Constructors
	
	/** 
	 * @brief Default Constructor
	 *
	 * @details
	 * This constructor sets internal voice fields to reasonable default values
	 * 
	 **/	
	MuVoice(void);
	
	/** 
	 * @brief Copy Constructor
	 *
	 * @details
	 * This constructor copies input voice's data to internal 
	 * member fields, performing deep copies when necessary
	 * 
	 * @param
	 * inVoice - voice object being copied
	 *
	 **/
	MuVoice(const MuVoice & inVoice);
	
	/** 
	 * @brief Destructor Constructor
	 *
	 * @details
	 * This constructor releases dynamically allocated memory
	 * 
	 **/
	~MuVoice(void);
	
	/** 
	 * @brief Assignment Operator
	 *
	 * @details
	 * Copies content of assigned object to local voice, field by field,
	 * performing deep copy when necessary
	 *
	 * @param
	 * inVoice - voice object being assigned
	 *
	 * @return
	 * MuVoice& - the resulting voice
	 *
	 **/	
	MuVoice & operator=(const MuVoice & inVoice);
	
	/** 
	 * @brief Equality Operator
	 *
	 * @details
	 * Compares the voice's content to that of input object. Returns true
	 * if every field contains the same values.
	 *
	 * @param 
	 * inVoice - voice object being compared 
	 *
	 * @return
	 * <ul>
	 * <li> true - if objects are identical
	 * <li> false - otherwise
	 * </ul>
	 **/		
	bool operator==(const MuVoice & inVoice);
	
	/** 
	 * @brief Inequality Operator
	 *
	 * @details
	 * Compares the voice's content to that of input object. Returns true
	 * if any field contains a different value.
	 *
	 * @param
	 * inVoice - voice object being compared  
	 *
	 * @return
	 * <ul>
	 * <li> true - if any field is different
	 * <li> false - otherwise
	 * </ul>
	 **/		
	bool operator!=(const MuVoice & inVoice);
	
	/** 
	 * @brief Clears internal members
	 *
	 * @details
	 * Clear() resets voice members to default values and 
	 * releases note list
	 *
	**/			
    void	Clear(void);
	
	/** 
	 * @brief Returns number of notes
	 *
	 * @details
	 * NumberOfNotes() returns the number of note objects contained in 
	 * the voice's internal note list. User code should call it before
	 * attempting to access the voice's notes
	 *
	 * @return
	 * long - number of notes in voice
	 *
	 **/			
    long	NumberOfNotes(void) const;
	
	/**
	*
	* @brief returns starting point of voice in time
	*
	* @details
	* This method returns the start time for first note in note list
	* This works correctly as long as note list is allways ordered.
	*
	* @return
	* float - start time in seconds; zero if voice is empty.
	*
	**/
	float	Start(void);
	
	/**
	 *
	 * @brief returns ending point of voice in time
	 *
	 * @details
	 * This method returns the end time for last note in note list
	 * This works correctly as long as note list is allways ordered.
	 *
	 * @return
	 * float - end time in seconds; zero if voice is empty
	 *
	 **/	
    float	End(void);
	
	/**
	 *
	 * @brief returns total duration of voice
	 *
	 * @details
	 * This method returns the time length between the start of first note
	 * and the end of last note in voice's note list.
	 * This works correctly as long as note list is allways ordered.
	 *
	 * @return
	 * float - duration in seconds; zero if voice is empty.
	 *
	 **/	
	float	Dur(void);
	
	/**
	 *
	 * @brief Adds note to voice's note list
	 *
	 * @details
	 * This method adds input note to the voice's note list
	 * Notes are inserted in time order
	 *
	 * @param
	 * inNote (MuNote) - note to be added to voice
	 *
	 * @return
	 * MuError
	 * <ul>
	 * <li> MuERROR_NONE upon success
	 * <li> MuERROR_INSUF_MEM if memoery allocation fails
	 * </ul>
	 *
	 **/	
    MuError	AddNote(MuNote inNote);
	
    /**
     *
     * @brief places the new note at the end of voice's note list
     *
     * @details
     * This method places a copy of 'inNote' at the end of the
     * voice's note list, regardless of the note's start time
     *
     * @param
     * inNote (MuNote) - note to be included in voice
     *
     * @return
     * MuError
     * <ul>
     * <li> MuERROR_NONE upon success
     * <li> MuERROR_INSUF_MEM if memoery allocation fails
     * </ul>
     *
     **/	

    MuError IncludeNote(MuNote inNote);
    
	/**
	 *
	 * @brief Removes note from voice's note list
	 *
	 * @details
	 * This method removes requested note from the voice's note list
	 * If voice is empty, or if requested note index doesn't exist 
	 * in voice, RemoveNote() returns an error. Call NumberOfNotes() 
	 * before, in order to observe valid voice bounds.
	 *
	 * @attention
	 * After a call to RemoveNote(), other notes may change indexes, so
	 * placing this call in a sequential loop to remove notes may not work;
	 * For example in order to remove all notes in a voice, user code would
	 * need to either call RemoveNote(0) for NumberOfNotes() times, or do 
	 * the same calling to RemoveLastNote(); although in this particular case
	 * it may be more efficient to call Clear().
	 *
	 * @param
	 * num (long) - index of note to be removed
	 *
	 * @return
	 * MuError
	 * <ul>
	 * <li> MuERROR_NONE upon success
	 * <li> MuERROR_NOTE_LIST_IS_EMPTY if voice contains no notes
	 * <li> MuERROR_NOTE_NOT_FOUND if num contains invalid note index
	 * </ul>
	 *
	 **/		
    MuError	RemoveNote(long num);
	
	/**
	 *
	 * @brief Removes last note from voice's note list
	 *
	 * @details
	 * This method removes last note from the voice's note list
	 * If voice is empty, RemoveLastNote() returns an error.
	 *
	 * @return
	 * MuError
	 * <ul>
	 * <li> MuERROR_NONE upon success
	 * <li> MuERROR_NOTE_LIST_IS_EMPTY if voice contains no notes
	 * </ul>
	 *
	 **/		
    MuError	RemoveLastNote(void);
	
	/**
	 *
	 * @brief Returns a copy of requested note.
	 *
	 * @details
	 * GetNote() returns a copy of the requested note. num must be a valid
	 * note index (see NumberOfNotes()); outNote must contain the address
	 * of a valid MuNote object. If note list is empty or num is not valid
	 * GetNote() returns an error.
	 *
	 * @return
	 * MuError
	 * <ul>
	 * <li> MuERROR_NONE upon success
	 * <li> MuERROR_NOTE_LIST_IS_EMPTY if voice contains no notes
	 * <li> MuERROR_NOTE_NOT_FOUND if num contains invalid note index
	 * </ul>
	 *
	 **/		
    MuError	GetNote(long num, MuNote * outNote) const;
	
	/**
	 *
	 * @brief Replaces note at requested location with input note
	 *
	 * @details
	 * SetNote() copies input note to note list at requested index;
	 * num must be a valid note index (see NumberOfNotes()). If note 
	 * list is empty or num is not valid, SetNote() returns an error.
	 *
	 * @return
	 * MuError
	 * <ul>
	 * <li> MuERROR_NONE upon success
	 * <li> MuERROR_NOTE_LIST_IS_EMPTY if voice contains no notes
	 * <li> MuERROR_NOTE_NOT_FOUND if num contains invalid note index
	 * </ul>
	 *
	 **/			
    MuError	SetNote(long num, MuNote inNote);
	
	/**
	 *
	 * @brief Bubble-sorts the notes (according to start time)
	 *
	 *
	 **/			
    void	Sort(void);
	
	/**
	 *
	 * @brief Bubble-sorts the notes by requested field
	 *
	 * @details
	 * By default, notes inside a voice are ordered by start time.
	 * SortBy() orders the voice's note list by the parameter indicated
	 * in the field argument. This argument must be one of the 
	 * sort constants defined in MuVoice.h:
	 *
	 * <ul>
	 * <li> SORT_FIELD_INSTR
	 * <li> SORT_FIELD_DUR
	 * <li> SORT_FIELD_PITCH
	 * <li> SORT_FIELD_AMP
	 * </ul>
	 *
	 * @warning
	 * When notes are sorted by parameteres other than start time
	 * many methods in MuM will fail!
	 *
	 **/	
	void	SortBy( short field );
	
	/**
	 *
	 * @brief Extracts content of voice between times beg and end
	 *
	 * @details
	 * Extract() goes through the voice's note list looking for notes which
	 * occur within the requested time range and returns a copy of these notes
	 * inside an MuVoice object. Notes that either start before
	 * 'beg' or terminate after 'end', but are partially contained in the range,
	 * are clipped to fit the range and included in the resulting extraction.
	 * If voice is empty or anything goes wrong, Extract() returns an empty voice
	 * object.
	 *
	 * @param beg (float) - a starting point in time (seconds)
	 * @param end (float) - an ending point in time (seconds)
	 *
	 * @return
	 * MuVoice - voice object containing notes extracted from this voice
	 *
	 **/	
    MuVoice	Extract(float beg, float end);
	
	/** 
	 * @brief Returns the instrument number definition for this voice
	 *
	 * @details
	 * InstrumentNumber() returns the instrument assigned to this voice;
	 * a value of 0 means no instrument has been assigned to this voice yet.
	 * A voice's instrument number is used to assign instrument definitions
	 * to notes which are inserted in the voice and have no previous instrument
	 * number.
	 *
	 * @return
	 * uShort - instrument number
	 *
	 **/
    uShort	InstrumentNumber(void);
	
	/** 
	 * @brief Sets the instrument number definition for this voice
	 *
	 * @details
	 * SetInstrumentNumber() updates the instrument number assigned to the
	 * voice and changes the instrument choices for every note accordingly.
	 * SetInstrumentNumber() will force every note inside the note list to reset
     * its instrument choice to the requested number, therefore eliminating 
     * any previous instrument definitions in the existing notes.
     * Notes added after this call will also conform to the instrument number
     * defined by SetInstrumentNumber() if they had no prior instrument choice.
     *
	 * Valid instrument range for inInstrNum is 1 through 128, to allow compatibility
	 * with GM program change format used by MIDI output. If an instrument
	 * number outside this range is provided, SetInstrumentNumber will default 
	 * to value 1. If SetInstrumentNumber() fails to Get or Set a note, when updating
	 * instrument choices, it aborts and returns a corresponding error.
	 *
	 * param
	 * inInstrNum (uShort) - new instrument number for the voice
	 *
	 * @return
	 * MuError
	 *
	 **/
    MuError	SetInstrumentNumber(uShort inInstrNum);
	
	/** 
	 * @brief Transposes every note's pitch
	 *
	 * @details
	 * Transpose() transposes the pitch of every note inside voice
	 * according to the number of halfsteps requested in interval.
	 * Positive values signal an ascending transposition, while negative
	 * values mean the notes should be transposed down.If Transpose() 
	 * fails to Get or Set any notes, when transposing them,
	 * it aborts and returns a corresponding error.
	 *
	 * param
	 * inInstrNum (uShort) - new instrument number for the voice
	 *
	 * @return
	 * MuError
	 *
	 **/
    MuError	Transpose(short interval);
	
	/** 
	 * @brief Transposes a specific note's pitch
	 *
	 * @details
	 * This version of Transpose() changes the pitch of the requested note
	 * according to the number of halfsteps requested in interval.
	 * Positive values signal an ascending transposition, while negative
	 * values mean the notes should be transposed down. If Transpose() 
	 * fails to Get or Set any notes, when transposing them, or noteNumber
	 * does not contain a valid note index for this voice, it aborts and 
	 * returns a corresponding error.
	 *
	 * param noteNumber (uShort) - index of note to be transposed
	 * param interval (short) - interval to use in transposition
	 *
	 * @return
	 * MuError
	 *
	 **/
    MuError	Transpose(long noteNumber, short interval);
	
	/** 
	 * @brief Moves voice to start at point 'time'
	 *
	 * @details
	 * Moves first sounding note in note list to point time. The remaining
	 * notes are shifted by the same offset, in order to preserve rhythmic  
	 * distribuition. If Move() fails to Get or Set any notes, when 
	 * moving them, or time contains a negative value move aborts and 
	 * returns a corresponding error.
	 * 
	 * @param time (float) - index of note to be transposed
	 *
	 * @return
	 * MuError
	 * <ul>
	 * <li> MuERROR_INVALID_PARAMETER if time is negative
	 * </ul>
	 *
	 **/	
    MuError	Move(float time);
    
    /**
	 * @brief Removes blank notes (rests)
	 *
	 * @details
	 * Removes every note that contains pitch == 0 or amp == 0 from selected
     * voice. These notes are usually used to mark pauses in applications
     * that require explicit rests. Since MuM uses explicit start times for
     * every note, deleting these notes won't affect rhythm or performance...	 
     *
	 * @param time (float) - index of note to be transposed
	 *
	 * @return
	 * MuError
     *
	 **/
    MuError RemoveBlankNotes(void);
};
#endif
