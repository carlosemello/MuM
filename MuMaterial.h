//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuMaterial.h
 *
 * @brief Material Class Interface
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 **/

/** @mainpage MuM
 * @details 
 MuM is an algorithmic composition framework.  This framework was developed to 
 facilitate the manipulation of music information, when implementing music composition 
 algorithms. The framework is designed around the concept of “Musical Material”, an open 
 designation for any length of musical information which can be manipulated in order to 
 create more complex musical information. In this sense, music material can be a motive, a 
 melody, a chord, a multi-voiced choral, a complete orchestral piece or a single musical 
 interval. In another words, any data from which a composer could extract musical ideas 
 for new compositions. MuM models this idea in a class of objects which can store any 
 combination of notes and voices. 
 
 MuM is based on a discrete concept of sonic events, in which a composition is ultimately 
 represented  as organized groups of notes. Furthermore, even though there is support for 
 manipulating and storing timbre information, the main focus is on pitch and rhythm 
 organization. Thus, the framework is not particularly useful for music which is centered 
 on continuous transformation of sound. The framework is built with standard (ANSI) 
 C++. Therefore it is easily portable to various platforms in source code form. Its output 
 may consist of either Csound score files or Standard MIDI Files. 
 
 MuM is free software. It may be copied, modified and reused in any way, as long as it 
 retains the credits for the original developers (as noted in this document and in source 
 code) in the derived work's source and documentation files. The original MuM 
 implementation was writen by Carlos Eduardo Mello, at the University of Brasilia. 
 
* @version 1.0
* @author Carlos Eduardo Mello
* @date 3/5/2009
**/
 
#ifndef _MU_MATERIAL_H_
#define _MU_MATERIAL_H_

#include "MuVoice.h"


// MUSIC CONSTANTS:

const short LOWEST_C = 24; 
const short HIGHEST_C = 108; 
const short NUM_OF_SCALE_DEGREES = 7;
const short NUM_OF_OCTAVES = 8;
const short FULL_SCALE_SIZE = NUM_OF_SCALE_DEGREES * NUM_OF_OCTAVES; 
const short OCTAVE_IN_DEGREES = 8;
const short MAJOR_MODE = 0;
const short MINOR_MODE = 1;

// NOTES
const short C_NAT = 0;
const short C_SHARP = 1; 
const short D_FLAT = 1;
const short D_NAT = 2;
const short D_SHARP = 3; 
const short E_FLAT = 3; 
const short E_NAT = 4;
const short F_NAT = 5;
const short F_SHARP = 6; 
const short G_FLAT = 6; 
const short G_NAT = 7;
const short G_SHARP = 8; 
const short A_FLAT = 8; 
const short A_NAT = 9;
const short A_SHARP = 10; 
const short B_FLAT = 10; 
const short B_NAT = 11;

// DEGREES

const short FIRST_DEGREE = 1;
const short SECOND_DEGREE = 2;
const short THIRD_DEGREE = 3;
const short FOURTH_DEGREE = 4;
const short FIFTH_DEGREE = 5;
const short SIXTH_DEGREE = 6;
const short EIGHTH_DEGREE = 7;

// FILE PATHS

#define CSOUND_PATH "/usr/local/bin/csound "

// SCORE LOADING MODES:
const short LOAD_MODE_TIME = 0;
const short LOAD_MODE_DIRECT = 1;

/**
 * @class MuMaterial
 *
 * @brief Musical Material Class
 *
 * @details
 * MuMaterial is the most important class in MuM. It embodies the main design idea for the 
 * framework, that of a flexible object which may contain any number/combination of notes, 
 * distributed in voices. It is through this object that the user accesses, modifies, copies and 
 * otherwise manipulates the notes, in order to create specific algorithms for composition. 
 * MuMaterial supports a number of methods for inserting notes, copying entire voices, 
 * loading score files. It offers several basic transfomations of its internal data, such as 
 * transposition, time scaling, inversion, retrogradation, concatenation, mixing, and more. 
 * The entries in this reference manual explain in detail the interface for each of these 
 * methods. The remaining of this section will clarify some general aspects of the class's 
 * design and its overall features. 
 * 
 * Voices inside Materials 
 *  
 * MuMaterial features methods which allows the user to organize its internal structure in 
 * separate voices, for use with diferent instruments or tracks. However, while these 
 * methods include facilities for most desirable routine operations, like  creating, removing, 
 * copying and clearing each voice independently, the API use is often more intuitive when 
 * these tasks are handled transparently by the object. MuMaterial was designed to required 
 * as little specification as possible from the user, when acessing each of its features. In order 
 * for this to be acomplished, most methods are overloaded so that there is usually a general 
 * call which applies transformations to the entire contents of the object (or to voice 0) and 
 * another, or others, which direct action to specific voices within the object. This way, if user 
 * code uses only "general-flavor" calls, it will be operating on the more intuitive scope - 
 * either everything, or when it makes sense, a single default voice.  
 * 
 * Errors inside Materials 
 * 
 * MuM classes handle internal errors by pasing error objects along the object hierarchy. 
 * Every call that involves memory allocation, or which may be subject to questionable 
 * requests (e.g.: invalid indexes and such) return error codes within an MuError object. 
 * Normaly, this kind of design should be incorporated into every part of a framework so 
 * that calling code can consistently keep track of what is happening inside it. However, in 
 * order to comply with the original idea of making the framework intuitive and easy to use, 
 * MuMaterial was built to handle errors diferently. Every time and error condition is 
 * reached within an MuMaterial method, this condition is temporarily reported by storing 
 * the corresponding error code in an internal MuError instance. Every new call clears this 
 * error condition. So if user code needs to check if a given method returned an error, it 
 * simply calls the LastError() method, which returns a copy of the stored error object. This 
 * way, when things are working, the code becomes more readable and intuitive, allowing 
 * algorithms to be uncluterred by constant error checks. On the other hand, quick 
 * debugging is made easy through the insertion of a simple safe call right after the 
 * suspicious one. 
 * 
 * Time references in MuM 
 * 
 * Time in MuM may be approached in two different ways. Let's call these strategies relative 
 * and absolute. While the library is flexible enough to allow mixing them together, it is 
 * probably wiser to choose one or the other for a given project, in order to avoid confusion 
 * and extra maintenance work. For each one of these approaches, MuM provides a group of 
 * methods that facilitate the handling of time related issues. 
 * 
 * The first option is to treat each material object as having its own independent time (relative 
 * time reference). In order to do this we need to assign the first note of our materials a start 
 * time of 0 (zero). The following notes then take the necessary start values to create the 
 * desired rhythmic patterns. In other words, each material should be treated as a whole 
 * which exists in isolation. These "standalone" pieces can then be pasted into other pieces 
 * with the help of methods such as Insert(), Append() and others like them, in order to build 
 * larger materials, which eventually merge into one composition. This approach is 
 * somewhat easier to deal with as it models a more intuitive version of music materials, one 
 * which we are used to see in traditional composition. For instance, we may want to take a 
 * certain melody and use it in various places in the composition. So we store this 
 * progression using start time information which places every note in the 'correct' place in 
 * relation to the first note. Then, when assembling the composition, we copy that melody to 
 * the desired spots, by inserting the material into a main material which serves as main 
 * storage area for the composition. 
 * 
 * The second approach to timing is to adopt an absolute reference. This means thinking of all 
 * materials as sharing the same time line. To handle this kind of time treatment, we need to 
 * use a group of methods to move each material to the right position in time, within itself, 
 * and then mix all the materials together in order to build the complete work.  *
 **/
class MuMaterial
{
    private:
	
    MuError lastError;
    MuVoice * voices;	
    int numOfVoices;
	
    static string orchestra;
    static string functionTables;
    string csOptions;
	
	public:
    
    // Constructors
	
	/** 
	 * @brief Default Constructor
	 *
	 * @details
	 * This constructor sets internal material fields to reasonable default values
	 * 
	 **/	
    MuMaterial( void );
	
    /** 
	 * @brief Copy Constructor
	 *
	 * @details
	 * This constructor copies input material's data to internal 
	 * member fields, performing deep copies when necessary
	 * 
	 * @param
	 * inMaterial (MuMaterial&) - material object being copied
	 *
	 **/	
    MuMaterial( const MuMaterial & inMaterial );
	
	/** 
	 * @brief Init with specific voice from inMaterial
	 *
	 * @details
	 * This constructor initializes material with the voice requested by
	 * fromVoice, from inMaterial
	 * 
	 * @param
	 * inMaterial (MuMaterial&) - material object being copied
	 * @param
	 * fromVoice (int) - index of voice to be copied
	 *
	 **/
    MuMaterial( const MuMaterial & inMaterial, int fromVoice );
	
    // Operators
	
	/** 
	 * @brief Assignment Operator
	 *
	 * @details
	 * Copies content of assigned object to local material, field by field,
	 * performing deep copy when necessary
	 *
	 * @param
	 * inMaterial (MuMaterial&) - material object being assigned
	 *
	 * @return
	 * MuMaterial& - the resulting material
	 *
	 **/		
    MuMaterial & operator=( const MuMaterial & inMaterial);
	
	/** 
	 * @brief Equality Operator
	 *
	 * @details
	 * Compares the material's content to that of input object. Returns true
	 * if every field contains the same values.
	 *
	 * @param 
	 * inMaterial  (MuMaterial&) - material object being compared 
	 *
	 * @return
	 * <ul>
	 * <li> true - if objects are identical
	 * <li> false - otherwise
	 * </ul>
	 **/			
    bool operator==( const MuMaterial & inMaterial);	// overloaded equality operator
	
	/** 
	 * @brief Inequality Operator
	 *
	 * @details
	 * Compares the material's content to that of input object. Returns true
	 * if any field contains a different value.
	 *
	 * @param
	 * inMaterial (MuMaterial&) - material object being compared  
	 *
	 * @return
	 * <ul>
	 * <li> true - if any field is different
	 * <li> false - otherwise
	 * </ul>
	 **/		
	bool operator!=(const MuMaterial & inMaterial); // overloaded inequality operator
	
	/** 
	 * @brief Addition Operator - append
	 *
	 * @details
	 * Appends contents of input object to the end of receiving material
	 *
	 * @param
	 * inMaterial (MuMaterial&) - material object being appended
	 *
	 * @return
	 * MuMaterial& - the resulting material
	 *
	 **/
	MuMaterial operator+( const MuMaterial & inMaterial );
    
    /**
     * @brief Addition and Assignment Operator - append to existing material content
     *
     * @details
     * Appends contents of input object to the end of receiving material
     *
     * @param
     * inMaterial (MuMaterial&) - material object being appended
     *
     * @return
     * MuMaterial& - the resulting material
     *
     **/
    MuMaterial & operator+=( const MuMaterial & inMaterial );
    
    /**
     * @brief Appends inNote to voice 0 of receiving material
     *
     * @details
     * input note is appended to the end of receiving material. This
     * insertiion point is calculated from the ending time of the
     * last sounding note in the material, with a call to Dur(). The
     * note is allways appended to voice 0.
     *
     * @param
     * inNote (MuNote &) - note to be appended
     * @return
     * MuMaterial & - reference to receiving material
     *
     **/
    MuMaterial & operator+=( const MuNote & inNote );
    
	/** 
	 * @brief Multiplication Operator - mix
	 *
	 * @details
	 * Mixes the contents of input object to that of receiving material, voice by voice.
	 *
	 * @param
	 * inMaterial (MuMaterial&) - material object being mixed
	 *
	 * @return
	 * MuMaterial& - the resulting material
	 *
	 **/
	MuMaterial & operator*( const MuMaterial & inMaterial  );
	
	/** 
	 * @brief Multiplication Operator - transpose
	 *
	 * @details
	 * transposes the contents of receiving material to the requested interval.
	 * Negative intervals result in downwards transposition; positive values
	 * transpose the notes up.
	 
	 * @param
	 * interval (short) - distance in halfsteps
	 *
	 * @return
	 * MuMaterial& - the resulting material
	 *
	 **/
	MuMaterial & operator*( short interval );
    
    /** 
	 * @brief Destructor Constructor
	 *
	 * @details
	 * This constructor releases dynamically allocated memory
	 * 
	 **/	
    ~MuMaterial(void);
	
	// BASIC INFO
	
   /** 
    * @brief Returns the earliest note starting point in this material
    *
    * @details
	* Returns the start time of the very first note inside the material, taking all voices into 
	* account. This is effectively the starting point for the first sound within the material. If the 
	* material is appended to another, this value will be the offset (silence) between them in the
	* resulting material.
	*
	* @return
	* float - time in seconds
	*
	**/
	float Start(void);
	
	/** 
	 * @brief Returns the total duration for the material (same as its "End")
	 *
	 * @details
	 * Returns the total duration of the material, taking all voices into account. This value 
	 * includes any offset between time zero and the first note's start time. In other words, Dur() 
	 * value starts in time 0.0 and runs through the end of the last sounding note in the material.
	 *
	 * @return
	 * float - time in seconds
	 *
	 **/
	float Dur(void);
	
    // VOICES 
	
	/**
	* @brief Returns number of voices in this material
	*
	* @details
	* Returns the number of voices inside material. Voices are referenced by a zero-based index 
	* number. So the last voice index is ( NumberOfVoices() - 1 ).
	*
	* @return
	* int - number of voices
	* 
	**/
	int NumberOfVoices(void);
	
	/**
	 * @brief Verifies the existence of requested voice
	 *
	 * @details
	 * Voice() returns true if 'voiceNumber' is a valid voice index; 
	 * Obs.: a voice may exist and be empty of notes.
	 *
	 * @param
	 * voiceNumber (int) - index of the requested voice
	 *
	 * @return
	 * bool - true if voice exists inside material. false otherwise
	 * 
	 **/
	bool Voice(int voiceNumber);
	
	/**
	 * @brief Returns the index of voice with requested instrument number
	 *
	 * @details
	 * GetVoiceNumberForInstrument() Returns the index number of the first 
	 * voice in the material which has its internal instrument number set to instrNum.  
	 *
	 * @param
	 * instrNum (int) - requested instrument number
	 *
	 * @return
	 * int - voice index
	 * 
	 **/
	int GetVoiceNumberForInstrument(int instrNum);
    
    /**
     * @brief Returns instrument number set for for requested voice
     *
     * @details
     * InstrumentNumber() Returns the internal instrument number choice for voice voiceNumber
     * If material is empty or if voiceNumber is not a valid voice index, InstrumentNumber()
     * issues and error and terminates. 
     *
     * @param
     * voiceNumber (int) - index of voice to be queried
     * 
     **/
    uShort InstrumentNumber(int voiceNumber);
	
	/**
	 * @brief Sets instrument number for requested voice
	 *
	 * @details
	 * SetInstrument() Sets the internal instrument number choice for voice voiceNumber to instrument .
	 * instrNum. If the material is empty or if voiceNumber is not a valid voice index, 	 
     * SetInstrument() issues an error and terminates.
     * If SetInstrument() is called on a material that contains previous notes, ALL notes will have their
     * instrument set to the new number. Every note which gets added after this call and does not contain 
     * an instrument definition, will have its instrument field set to the number provided by 
     * SetInstrument(). However, new added notes which do contain an instrument choice preserve 
     * their instrument definition when added to material. Therefore, if there is a need for multiple 
     * instruments in the same voice, it is best to issueinstrument assignments manually, that is, 
     * on a note to note basis, instead of using SetInstrument().
     *
	 * @param
	 * voiceNumber (int) - index of voice to be set
	 * @param
	 * instrNum (int) - requested instrument number
	 *
	 * 
	 **/
	void SetInstrument(int voiceNumber, uShort instrNum);
	
	/**
	 * @brief Returns a copy of requested voice inside an MuMaterial object
	 *
	 * @details
	 * GetVoice() returns a copy of the requested voice inside an MuMaterial object.
	 * The requested voice is allways returned as voice 0 (zero) in resulting object.
	 * If voiceNumber is not a valid voice index, GetVoice() issues and error and 
	 * returns an  empty material. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 *
	 * @return
	 * MuMaterial - material object containing requested voice
	 * 
	 **/
	MuMaterial GetVoice(int voiceNumber);
	
	/**
	 * @brief Sets target voice in this material with requested voice of input material
	 *
	 * @details
	 * SetVoice() sets voice voiceNumber of material to voice inVoice of inMaterial. This replaces the 
	 * contents of target voice with notes copied from source voice. If material is empty or either 
	 * voice indexes are not valid in their corresponding materials, an error is issued 
	 * and SetVoice() terminates.
	 *
	 * @param
	 * voiceNumber (int) - index of target voice
	 * @param
	 * inMaterial (MuMaterial) - music material to copy voice from
	 * @param
	 * inVoice (int) - index of requested voice
	 *
	 * 
	 **/	
	void SetVoice(int voiceNumber, const MuMaterial & inMaterial, int inVoice);
	
	/**
	 * @brief Adds empty voice(s) to the end of this material
	 *
	 * @details
	 * AddVoices() Adds 'n' empty voices to the materials voice list. If memory allocation 
	 * is not possible, an error is issued and method terminates. 
	 *
	 * @param
	 * n (int) - number of voice to add
	 *
	 * 
	 **/	
	void AddVoices(int n);
	
	/**
	 * @brief Inserts empty voice(s) after requested voice of this material
	 *
	 * @details
	 * InsertVoices() inserts 'n' empty voices after voice voiceNumber. If memory allocation 
	 * is not possible, or voiceNumber  is not a valid voice index number, an error is issued 
	 * and method terminates. 
	 *
	 * @param
	 * voiceNumber (int) - requested voice index
	 * @param
	 * n (int) - number of voice to add
	 *
	 **/		
	void InsertVoices(int voiceNumber, int n);
	
	/**
	 * @brief Removes requested voice  and re-indexes internal array of voices
	 *
	 * @details
	 * RemoveVoice() removes voice voiceNum from this materialand re-indexes voice array. 
	 * If voiceNumber  is not a valid voice index number, an error is issued and method terminates. 
	 *
	 * @param
	 * voiceNumber (int) - index of voice to be removed
	 *
	 **/		
	void RemoveVoice(int voiceNumber);
	
	/**
	 * @brief Clears requested voice
	 *
	 * @details
	 * ClearVoice() empties all notes from requested voice and resets its internal 
	 * data to default values.
	 *
	 * @param
	 * voiceNumber (int) - index of voice to be Cleared
	 *
	 **/		
	void ClearVoice(int voiceNumber);
	
	/**
	 * @brief Checks if requested voice contains notes
	 *
	 * @details
	 * IsEmptyVoice() Returns true if voice 'voiceNumber' is empty
	 * (contains no notes)
	 *
	 * @param
	 * voiceNumber (int) - index of requested voice
	 *
	 * @return
	 * bool - true if voice is empty; false otherwise.
	 *
	 **/		
	bool IsEmptyVoice( int voiceNumber );

	/**
	 * @brief Exchanges content of two voices in this material
	 *
	 * @details
	 * SwapVoices() exchanges the contents of voice oneVoice and voice otherVoice. 
	 * If either oneVoice and/or otherVoice are not valid voice index numbers, 
	 * an error is issued and method terminates. 
	 *
	 * @param
	 * oneVoice (int) - voice index
	 * @param
	 * otherVoice (int) - voice index
	 *
	 **/			
	void SwapVoices(int oneVoice, int otherVoice);
	
	/**
	 * @brief Mixes this material with input material
	 *
	 * @details
	 * Mixes every voice of material with the corresponding voice of inMaterial. 
	 * Notes in every voice are inserted at the original start time.
	 * If any voices are missing in either objects, they are created so the mix 
	 * can work properly.  
	 *
	 * @param
	 * inMaterial (MuMaterial) - input music material
	 *
	 **/			
	void Mix( const MuMaterial & inMaterial);
	
	/**
	 * @brief Mixes specific voices
	 *
	 * @details
	 * This version of Mix() Mixes voice voiceNumber of this material with voice 
	 * inVoice of inMaterial. If voiceNumber  and/or inVoice are not valid voice 
	 * index numbers in their corresponding materials, an error is issued and method 
	 * terminates. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * inMaterial (MuMaterial) - input music material
	 * @param
	 * inVoice (int) - voice index
	 *
	 **/			
	void Mix(int voiceNumber, const MuMaterial & inMaterial, int inVoice);
	
	/**
	 * @brief Appends one voice from input material to another voice of this material
	 *
	 * @details
	 * Append() takes every note of voice inVoice of inMaterial and appends them
	 * to voice voiceNumber of this material.
	 * If either oneVoice and/or otherVoice are not valid voice index numbers in 
	 * their corresponding materials, an error is issued and method terminates. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * inMaterial (MuMaterial) - input music material
	 * @param
	 * inVoice (int) - voice index
	 *
	 **/			
	void Append(int voiceNumber, const MuMaterial & inMaterial, int inVoice);
	
    /**
     * @brief Appends input note to the requested voice in material
     *
     * @details
     * Append() takes note 'inNote' and appends it to voice 'voiceNumber' 
     * of this material. If 'voiceNumber' does not exist it is created. 
     * If 'voiceNumber' is negative, an error is issued (see lastError)
     *
     * @param
     * voiceNumber (int) - voice index
     * @param
     * inNote (MuNote) - note to be appended
     *
     **/
    void Append(int voiceNumber, MuNote inNote);
    
    /**
     * @brief adds input note to the end of requested voice
     *
     * @details
     * Include() takes note 'inNote' and adds it to the end of 'voiceNumber'
     * regardless of the note's start time. If the requested voice does not 
     * exist it is created. If 'voiceNumber' is negative, an error is issued 
     * (see lastError)
     *
     * @param
     * voiceNumber (int) - voice index
     * @param
     * inNote (MuNote) - note to be included
     *
     **/
    void IncludeNote(int voiceNumber, MuNote inNote);

    // NOTES
	
	/**
	 * @brief Returns total number of notes for this material
	 *
	 * @details
	 * NumberOfNotes() returns total number of notes in material. If material is empty, 
	 * an error is issued and NumberOfNotes() returns 0 (zero).
	 *
	 *
	 * * @return
	 * long - number of notes in material
	 *
	 **/			
	long NumberOfNotes(void);
	
	/**
	 * @brief Returns the number of notes contained in voice 'voiceNumber'
	 *
	 * @details
	 * NumberOfNotes() returns total number of notes in material. If material is empty, 
	 * an error is issued and NumberOfNotes() returns 0 (zero).
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 *
	 * @return
	 * long - number of notes in requested voice
	 *
	 **/			
	long NumberOfNotes( int voiceNumber );
	
	/**
	 * @brief Sets note 'noteNumber' of voice '0' to 'inNote'
	 *
	 * @details
	 * SetNote() sets note 'noteNumber' of voice '0' to note 'inNote'. If material is empty, 
	 * or noteNumber is not a valid note number for voice 0, an error is issued and 
	 * method terminates.
	 *
	 * @param
	 * noteNumber (long) - note index
	 * @param
	 * inNote (MuNote) - note object
	 *
	 **/			
    void SetNote(long noteNumber, MuNote inNote);
	
	/**
	 * @brief Sets note 'noteNumber' of voice 'voiceNumber' to 'inNote'
	 *
	 * @details
	 * SetNote() sets note 'noteNumber' of voice 'voiceNumber' to note 'inNote'. If material 
	 * is empty, or voiceNumber  is not a valid voice index number, or noteNumber 
	 * is not a valid note number for voice voiceNumber, an error is issued and 
	 * method terminates.	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * noteNumber (long) - note index
	 * @param
	 * inNote (MuNote) - note object
	 *
	 **/
    void SetNote(int voiceNumber, long noteNumber, MuNote inNote);
	
	/**
	 * @brief Gets note 'noteNumber' of voice '0'
	 *
	 * @details
	 * GetNote()gets a copy of note 'noteNumber' of voice '0'. If material is empty, 
	 * or noteNumber is not a valid note number for voice voiceNumber, an error 
	 * is issued and GetNote() returns an empty (zeroed) note.	 
	 *
	 * @param
	 * noteNumber (long) - note index
	 *
	 * @return
	 * MuNote - requested note object
	 **/
    MuNote GetNote(long noteNumber);
	
	/**
	 * @brief Gets note 'noteNumber' of voice 'voiceNumber'
	 *
	 * @details
	 * GetNote() gets a copy of note 'noteNumber' of voice 'voiceNumber'. If material is 
	 * empty, or voiceNumber  is not a valid voice index number, or noteNumber is not 
	 * a valid note number for voice voiceNumber, an error is issued and GetNote() 
	 * returns an empty (zeroed) note.	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * noteNumber (long) - note index
	 *
	 * @return
	 * MuNote - requested note object
	 *
	 **/
    MuNote GetNote(int voiceNumber, long noteNumber);
	
	/**
	 * @brief Returns a copy of the first note in the material
	 *
	 * @details
	 * GetNote() Returns a copy of the first note in material. The first note 
	 * is determined as the note ( of any voice ) which has the earliest starting 
	 * time in material. If more than one note have the exact same start time, 
	 * the first one found is returned. If material is empty, an error is issued 
	 * and GetFirstNote() returns an empty (zeroed) note.	 
	 *
	 * @return
	 * MuNote - requested note object
	 *
	 **/
	MuNote GetFirstNote( void );
	
	/**
	 * @brief Returns a group of notes inside material object
	 *
	 * @details
	 * GetNotes() returns a material object containing (in voice 0) a range
	 * of notes from voice 'voiceNumber' between 'from' and 'through'; both upper 
	 * and lower limits are inclusive.	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * from (long) - note index, lower limit
	 * @param
	 * through (long) - note index, upper limit
	 *
	 * @return
	 * MuMaterial - material containing requested notes
	 *
	 **/
	MuMaterial GetNotes( int voiceNumber, long from, long through );
	
	/**
	 * @brief Returns a group of notes starting at a given time
	 *
	 * @details
	 * GetNotesStartingAt() returns a material object containing (in voice 0) every  
	 * note, from voice 'voiceNumber' of this material, which starts at point 'time"	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * time (float) - starting point in time
	 *
	 * @return
	 * MuMaterial - material containing requested notes
	 *
	 **/
	MuMaterial GetNotesStartingAt(int voiceNumber, float time);
	
	/**
	 * @brief Returns a group of notes sounding at a given time
	 *
	 * @details
	 * GetNotesSoundingAt() returns a material object containing (in voice 0) every note, 
	 * from voice 'voiceNumber' of this material, which is active at point 'time". In other
	 * words, this will get any notes which have: Start() >= time AND time < End().
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * time (float) - sounding point in time
	 *
	 * @return
	 * MuMaterial - material containing requested notes
	 *
	 **/
	MuMaterial GetNotesSoundingAt(int voiceNumber, float time);
	
	/**
	 * @brief returns true if material contains input note (pitch) at voice 'voiceNumber'
	 *
	 * @details
	 * Contains() checks whether or not a given pitch value (MIDI pitch) is found
	 * within material. This means that, if ANY note in voice 'voiceNumber' of this 
	 * material has its pitch field set to the requested pitch number, this will 
	 * return true.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * pitch (int) - pitch (MIDI note number) to search for 
	 *
	 * @return
	 * bool - true if pitch is part of material
	 *
	 **/
	bool Contains( int voiceNumber, int pitch );
	
	/**
	 * @brief Adds note 'inNote' to voice '0'
    
	 * @details
	 * AddNote() adds note 'inNote' to voice '0'. If material is empty, voice '0' is 
     * automatically  created.Notes are stored in chronological order, so 'inNote' is
     * inserted at the correct point in time within the voice, according to its
     * start time. If there are other notes with the exact same starting time, 'inNote'
     * is always inserted after them.
 	 *
	 * @param
	 * inNote (MuNote) - note object to be added
	 *
	 **/
	void AddNote(MuNote inNote);
	
	/**
	 * @brief Adds note 'inNote' to voice 'voiceNumber'
	 *
	 * @details
	 * AddNote() adds note 'inNote' to voice 'voiceNumber'. If material is empty, or 
     * 'voiceNumber'  is not a valid voice index number, an error is issued and 
     * AddNote() terminates. Notes are stored in chronological order, so 'inNote' is
     * inserted at the correct point in time within the voice, according to its
     * start time. If there are other notes with the exact same starting time, 'inNote'
     * is always inserted after them.

	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * inNote (MuNote) - note object to be added
	 *
	 **/
    void AddNote(int voiceNumber, MuNote inNote);
	
	/**
	 * @brief Removes note 'noteNumber' of voice '0'
	 *
	 * @details
	 * RemoveNote() removes note 'noteNumber' from voice '0'. If material is empty, 
	 * or noteNumber is not a valid note number for voice '0', an error is issued 
	 * and RemoveNote()  terminates. 
	 *
	 * @param
	 * noteNumber (long) - note index
	 *
	 **/
    void RemoveNote(long noteNumber);
	
	/**
	 * @brief Removes note 'noteNumber' of voice 'voiceNumber'
	 *
	 * @details
	 * RemoveNote() removes note 'noteNumber' of voice 'voiceNumber'. If material 
	 * is empty, or 'voiceNumber'  is not a valid voice index number, or 'noteNumber' 
	 * is not a valid note number for voice voiceNumber, an error is issued and 
	 * RemoveNote() terminates. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * noteNumber (long) - note index
	 *
	 **/
    void RemoveNote(int voiceNumber, long noteNumber);				
    
	// TRANSFORMATIONS
	
    // Transposition
	
	/**
	 * @brief Transposes entire material by a certain interval
	 *
	 * @details
	 * Transpose() transposes entire material by given interval. If interval 
	 * is positive material is transposed up; if interval is negative, material 
	 * is transposed down. If material is empty, an error is issued and Transpose() 
	 * terminates.
	 *
	 * @param
	 * interval (short) - numer of halfsteps to transpose
	 *
	 **/	
    void Transpose(short interval);
    
	/**
	 * @brief 	Transposes requested voice
	 *
	 * @details
	 * Transpose() Transposes voice 'voiceNumber' by given interval. If interval is 
	 * positive the voice is transposed up; if interval is negative, the voice is 
	 * transposed down. If material is empty, or voiceNumber  is not a valid 
	 * voice index number, an error is issued and Transpose() terminates.	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * interval (short) - numer of halfsteps to transpose
	 *
	 **/	
	void Transpose(int voiceNumber, short interval);
    
	/**
	 * @brief Transposes requested note in voice
	 *
	 * @details
	 * Transpose() transposes note 'noteNumber' of voice 'voiceNumber' by given 'interval'. 
	 * If interval is positive the note is transposed up; if interval is negative, 
	 * the note is transposed down. If material is empty, or voiceNumber  is not 
	 * a valid voice index number, or noteNumber is not a valid note number for voice 
	 * voiceNumber, an error is issued and Transpose() terminates.	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * noteNumber (long) - note index
	 * @param
	 * interval (short) - numer of halfsteps to transpose
	 *
	 **/	
	void Transpose(int voiceNumber, long noteNumber, short interval);  
    
	/**
	 * @brief Transposes note range of requested voice
	 *
	 * @details
	 * Transpose() transposes a range of notes of voice voiceNumber by given interval. 
	 * If interval is positive the notes are transposed up; if interval is negative, 
	 * the notes are transposed down. If material is empty, or voiceNumber  is not a 
	 * valid voice index number, or startingNote and/or endingNote are not valid note 
	 * numbers for voice voiceNumber, an error is issued and Transpose() terminates.	 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * startingNote (long) - note index
	 * @param
	 * endingNote (long) - note index
	 * @param
	 * interval (short) - numer of halfsteps to transpose
	 *
	 **/	
	void Transpose(int voiceNumber, long startingNote, long endingNote, short interval); 
    
	/**
	 * @brief Transposes entire material by degree
	 *
	 * @details
	 * DiatonicTranspose() transposes entire material by degree, that is respecting 
	 * the intervals found in the key signature. User code provides a key 
	 * (MIDI note number) and mode (MAJOR_MODE or MINOR_MODE). DiatonicTranspose() 
	 * translates each note in material to scale degrees. Degrees are then transposed 
     * based on the "degree  distance" between the lowest note of material and the 
     * requested degree of transposition. Direction of transposition
     * can be set with the 'direction' argument (possible values bellow).
     * If any notes within material do not belong to the requested key/mode, 
     * DiatonicTranspose() issues an error and returns. This may happen at any point
     * along the transposition process, therefore it is advisable to allways check
     * the materials last error, before proceding after using DiatonicTranspose().
     *
	 * @param
	 * key (short) - key center (a MIDI pitch number)
	 * @param
	 * mode (short) - major or minor
	 * @param
	 * degree (short) - target scale degree (degree to transpose to)
	 * @param
	 * direction (short) - ASCENDING or DESCENDING
	 *
	 **/	
	void DiatonicTranspose( short key, short mode, short degree, short direction );
 	
	
	// Pitch Class
	
	/**
	 * @brief Transposes every note in material to the same pitch class in middle octave
	 *
	 * @details
	 * ColapsePitch() transposes every note in material from its original octave to the
	 * middle octave; original pitch-classes are retained. This method, simply calls
	 * ColapsePitch(int) for each voice; if any of these calls fail, ColapsePitch()
	 * aborts and a corresponding error is issued.
	 *
	 **/	
	void ColapsePitch(void);
	
	/**
	 * @brief Transposes every note in requested voice to the same pitch class in middle octave
	 *
	 * @details
	 * ColapsePitch() transposes every note in requested voice from its original octave
	 * to the middle octave; original pitch-classes are retained. If 'voiceNumber' is note
	 * a valid voice number for this material, the methods aborts and an error is issued.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 *
	 **/	
	void ColapsePitch(int voiceNumber);
    
	// Moving
	
	/**
	 * @brief Moves entire material to the desired point in time 
	 *
	 * @details
	 * Move() Moves entire material to the requested point in time. timePosition will 
	 * be the start time of the first note in material. Remaining notes will be moved 
	 * acoordingly, in order to maintain the same original distance with each other. 
	 * If material is empty, an error is issued and Move() terminates.
	 *
	 * @param
	 * timePoint (float) - time in seconds
	 *
	 **/	
    void Move(float timePoint);
	
	/**
	 * @brief 	Moves requested voice to the desired point in time 
	 *
	 * @details
	 * Move() Moves voice voiceNumber to the requested point in time. timePosition 
	 * will be the start time of the first note in voice. Remaining notes will be 
	 * moved acoordingly, in order to maintain the same original distance with each 
	 * other. If material is empty, or voiceNumber  is not a valid voice index number, 
	 * an error is issued and Move() terminates.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * timePoint (float) - time in seconds
	 *
	 **/	
    void Move(int voiceNumber, float timePoint); 
    
    // Inverting
	
	/**
	 * @brief 	Inverts entire material 
	 *
	 * @details
	 * Invert() switches the direction of every melodic interval in material, 
	 * thus producing an inverted "image" of original melodic content. 	 
	 *
	 **/	
    void Invert(void);
	
	/**
	 * @brief 	Inverts requested voice
	 *
	 * @details
	 * Invert() switches the direction of every melodic interval in requested voice, 
	 * thus producing an inverted "image" of original melodic content. If 'voiceNumber'
	 * is not a valid voice number, or if material is empty, Invert aborts and an error
	 * is issued.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 *
	 **/	
    void Invert(int voiceNumber);
    
    // Retrograding
	
	/**
	 * @brief 	Retrogrades entire material
	 *
	 * @details
	 * Retro() moves material backwards, so that notes run from last to first
	 * in every voice.
	 *
	 **/	
    void Retro(void);
	
	/**
	 * @brief 	Retrogrades requested voice
	 *
	 * @details
	 * Retro() moves requested voice backwards, so that notes run from last to first
	 * If material is empty or 'voiceNumber' is not a valid voice index for this material,
	 * Retro() terminates and an error is issued.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 *
	 **/	
    void Retro(int voiceNumber);
    
	
	// Pitch Clycling
	
	/**
	 * @brief 	Shifts pitches from one note the the next.
	 *
	 * @details
	 * CyclePitch() shifts pitch content of notes in material. Every note takes on the
	 * pitch of the following note in a round robin sequence. This process is repeated
	 * the requested number of times.
	 *
	 * @param
	 * times (int) - number of times to cycles pitches
	 *
	 **/
	void CyclePitch( int times );
	
	/**
	 * @brief 	Shifts pitches of requested voice
	 *
	 * @details
	 * CyclePitch() shifts pitch content of notes in requested voice. Every note 
	 * takes on the pitch of the following note in a round robin sequence. This 
	 * process is repeated the requested number of times.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * times (int) - number of times to cycles pitches
	 *
	 **/
	void CyclePitch(int voiceNumber, int times);
	
	// Interval Modification
	
	/**
	 * @brief 	Expands every interval in material
	 *
	 * @details
	 * ExpandInterval() adds the desired number of halfsteps to every
	 * interval in material.
	 *
	 * @param
	 * halfSteps (int) - number of halfsteps to add to intervals
	 *
	 **/	
	void ExpandInterval(int halfSteps);
	
	/**
	 * @brief Expands every interval in requested voice
	 *
	 * @details
	 * ExpandInterval() adds the desired number of halfsteps to every
	 * interval in requested voice. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * halfSteps (int) - number of halfsteps to add to intervals
	 *
	 **/	
	void ExpandInterval(int voiceNumber, int halfSteps);
	
	/**
	 * @brief 	Contracts every interval in material
	 *
	 * @details
	 * ContractInterval() subtracts the desired number of halfsteps to every
	 * interval in material. 
	 *
	 * @param
	 * halfSteps (int) - number of halfsteps to subtract from intervals
	 *
	 **/	
	void ContractInterval(int halfSteps);
	
	/**
	 * @brief 	Contracts every interval in requested voice
	 *
	 * @details
	 * ContractInterval() the desired number of halfsteps to every
	 * interval in requested voice. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * halfSteps (int) - number of halfsteps to subtract from intervals
	 *
	 **/	
	void ContractInterval(int voiceNumber, int halfSteps);
	
    // Time changes
	/**
	 * @brief 	Fits entire material into given duration
	 *
	 * @details
	 * Fit() rescales note durations and repositions start-times for every note in order
	 * to make material proportionally expand or contract in time to fit into new total 
	 * duration
	 *
	 * @param
	 * dur (float) - final duration in seconds
	 *
	 **/	
    void Fit(float dur);
	
	/**
	 * @brief Scales entire material by given factor
	 *
	 * @details
	 * Scale() rescales note durations and repositions start-times for every note in order
	 * to make material proportionally expand or contract in time according to requested
	 * ratio of original total duration
	 *
	 * @param
	 * factor (float) - size scale
	 *
	 **/
	void Scale(float factor);
	
	/**
	 * @brief 	Shifts note durations in material
	 *
	 * @details
	 * CyclePitch() shifts durations of notes in requested voice. Every note 
	 * takes on the duration of the following note in a round robin sequence. Start
	 * times are fixed to retain rhythmic flow. This process is repeated 
	 * the requested number of times.
	 *
	 * @param
	 * times (int) - number of times to cycles rhythms
	 *
	 **/	
	void CycleRhythm( int times);
	
	/**
	 * @brief 	Shifts note durations in requested voice
	 *
	 * @details
	 * CycleRhythm() shifts durations of notes in requested voice. Every note 
	 * takes on the duration of the following note in a round robin sequence. Start
	 * times are fixed to retain rhythmic flow. This process is repeated 
	 * the requested number of times.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * times (int) - number of times to cycles pitches
	 *
	 **/	
	void CycleRhythm(int voiceNumber, int times);
    
    // Segmentation
	
	/**
	 * @brief 	Extracts segments of same length
	 *
	 * @details
	 * Segments() Divides material into n segments of equal-length and returns 
	 * them in another MuMaterial object. Each segment thus constructed is placed 
	 * in an adjacent voice of the returned material (i.e.: segment 0 goes to 
	 * voice 0, segment 1 to voice 1, etc.)
	 *
	 * @param
	 * n (int) - number of segments
	 *
	 **/	
    MuMaterial Segments(int n);	
	
	/**
	 * @brief 	Extracts segments with lengths according to ratios
	 *
	 * @details
	 * Segments() Divides material into n segments and returns them in another MuMaterial
	 * object. Each segment thus constructed is placed in an adjacent voice of the 
	 * returned material (i.e.: segment 0 goes to voice 0, segment 1 to voice 1, etc.).
	 * The length of each segment will be calculated according to the value of the 
	 * corresponding element in the ratios array and the total duration of the material.
	 *
	 * @param
	 * n (int) - number of segments
	 * @param
	 * ratios (float*) - address to array of segments
	 *
	 * @return
	 * MuMaterial - material object
	 **/	
    MuMaterial Segments(int n, float * ratios);	
    
    // DATA GENERATION
	
	// Chords
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with major triad in time 0
	 *
	 * @details
	 * Builds a major triad from middle C within material. All three notes start 
	 * in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
    void MajorTriad(float dur);
	
	/**
	 *
	 * @brief
	 * Fills requested voice with major triad in time 0
	 *
	 * @details
	 * Builds a major triad from middle C within requested voice. All three notes start 
	 * in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/	
	void MajorTriad(int voiceNumber, float dur);
    
    /**
     *
     * @brief
     * Fills voice '0' with arpeggiated major triad in time 0
     *
     * @details
     * Builds a major triad from middle C within material. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void MajorTriadArpeg(float dur);
    
    /**
     *
     * @brief
     * Fills requested voice with arpeggiated major triad in time 0 of requested voice
     *
     * @details
     * Builds a major triad from middle C within requested voice. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * voiceNumber (int) - voice index
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void MajorTriadArpeg(int voiceNumber, float dur);
    
	/**
	 *
	 * @brief
	 * Fills first 3 voices with notes from a major triad in time 0
	 *
	 * @details
	 * Builds a major triad from middle C placing each note in one of the first
	 * three voices in material. All three notes start in time 0 (zero) and are 
	 * assigned a length of dur. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/	
	void MajorTriadSplit(float dur);			
   
	/**
	 *
	 * @brief
	 * Fills voice '0' with minor triad in time 0
	 *
	 * @details
	 * Builds a minor triad from middle C within material. All three notes start 
	 * in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MinorTriad(float dur);	
	
	/**
	 *
	 * @brief
	 * Fills requested voice with minor triad in time 0
	 *
	 * @details
	 * Builds a minor triad from middle C within requested voice. All three notes start 
	 * in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MinorTriad(int voiceNumber, float dur);
    
    /**
     *
     * @brief
     * Fills voice '0' with arpeggiated minor triad in time 0
     *
     * @details
     * Builds a minor triad from middle C within material. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void MinorTriadArpeg(float dur);
    
    /**
     *
     * @brief
     * Fills requested voice with arpeggiated minor triad in time 0 of requested voice
     *
     * @details
     * Builds a major minor from middle C within requested voice. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * voiceNumber (int) - voice index
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void MinorTriadArpeg(int voiceNumber, float dur);

	/**
	 *
	 * @brief
	 * Fills first 3 voices with notes from a minor triad in time 0
	 *
	 * @details
	 * Builds a minor triad from middle C placing each note in one of the first
	 * three voices in material. All three notes start in time 0 (zero) and are 
	 * assigned a length of dur. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MinorTriadSplit(float dur);
    
	/**
	 *
	 * @brief
	 * Fills voice '0' with augmented triad in time 0
	 *
	 * @details
	 * Builds an augmented triad from middle C within material. All three notes start 
	 * in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void AugTriad(float dur);
    
	/**
	 *
	 * @brief
	 * Fills requested voice with augmented triad in time 0
	 *
	 * @details
	 * Builds an augmented triad from middle C within requested voice. All three 
	 * notes start in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds	 
	 *
	 **/
	void AugTriad(int voiceNumber, float dur);
    
    /**
     *
     * @brief
     * Fills voice '0' with arpeggiated augmented triad in time 0
     *
     * @details
     * Builds a augmented triad from middle C within material. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void AugTriadArpeg(float dur);
    
    /**
     *
     * @brief
     * Fills requested voice with arpeggiated augmented triad in time 0 of requested voice
     *
     * @details
     * Builds a augmented triad from middle C within requested voice. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * voiceNumber (int) - voice index
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void AugTriadArpeg(int voiceNumber, float dur);
    
	/**
	 *
	 * @brief
	 * Fills first 3 voices with notes from an augmented triad in time 0
	 *
	 * @details
	 * Builds an augmented triad from middle C placing each note in one of the first
	 * three voices in material. All three notes start in time 0 (zero) and are 
	 * assigned a length of dur. 
	 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void AugTriadSplit(float dur);
    
    
	/**
	 *
	 * @brief
	 * Fills voice '0' with diminished triad in time 0
	 *
	 * @details
	 * Builds a diminished triad from middle C within material. All three 
	 * notes start in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void DimTriad(float dur);
    
	/**
	 *
	 * @brief
	 * Fills requested voice with diminished triad in time 0
	 *
	 * @details
	 * Builds a diminished triad from middle C within material. All three 
	 * notes start in time 0 (zero) and are assigned a length of dur. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void DimTriad(int voiceNumber, float dur);
	
    /**
     *
     * @brief
     * Fills voice '0' with arpeggiated diminished triad in time 0
     *
     * @details
     * Builds a diminished triad from middle C within material. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void DimTriadArpeg(float dur);
    
    /**
     *
     * @brief
     * Fills requested voice with arpeggiated diminished triad in time 0 of requested voice
     *
     * @details
     * Builds a diminished triad from middle C within requested voice. The first note of
     * the chord starts at time 0 and subsequent notes are placed melodically from
     * that point on. All notes have a duration of 'dur'.
     *
     * @param
     * voiceNumber (int) - voice index
     * @param
     * dur (float) - duration in seconds
     *
     **/
    void DimTriadArpeg(int voiceNumber, float dur);
    
	/**
	 *
	 * @brief
	 * Fills first 3 voices with notes from a diminished triad in time 0
	 *
	 * @details
	 * Builds a diminished triad from middle C placing each note in one of the first
	 * three voices in material. All three notes start in time 0 (zero) and are 
	 * assigned a length of dur. 	 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void DimTriadSplit(float dur);
    
    /**
	 *
	 * @brief
	 * Fills requested voice with notes from a major seventh chord  in time 0
	 *
	 * @details
	 * Builds a major seventh chord  from middle C within material. All three notes start in
     * time 0 (zero) and are assigned a length of dur.
	 *
     * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
    void MajorSeventhChord(int voiceNumber, float dur);	// [PUBLIC]
	
	// Scales
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with a major scale
	 *
	 * @details
	 * Builds a major scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/	
	void MajorScale(float dur, bool addOctave = false);
	
	/**
	 *
	 * @brief
	 * Fills requested voice with a major scale
	 *
	 * @details
	 * Builds a major scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/		
	void MajorScale(int voiceNumber, float dur, bool addOctave = false);
    
	/**
	 *
	 * @brief
	 * Fills voice '0' with a minor scale
	 *
	 * @details
	 * Builds a minor scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MinorScale(float dur);	
    
	/**
	 *
	 * @brief
	 * Fills requested voice with a minor scale
	 *
	 * @details
	 * Builds a minor scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MinorScale(int voiceNumber, float dur);	
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with a harmonic minor scale
	 *
	 * @details
	 * Builds a harmonic minor scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void HarmonicMinorScale(float dur);	
    
	/**
	 *
	 * @brief
	 * Fills requested voice with a harmonic minor scale
	 *
	 * @details
	 * Builds a harmonic minor scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void HarmonicMinorScale(int voiceNumber, float dur);
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with a melodic minor scale
	 *
	 * @details
	 * Builds a melodic minor scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MelodicMinorScale(float dur);	
    
	/**
	 *
	 * @brief
	 * Fills requested voice with a melodic minor scale
	 *
	 * @details
	 * Builds a melodic minor scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void MelodicMinorScale(int voiceNumber, float dur);
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with a pentatonic scale
	 *
	 * @details
	 * Builds a pentatonic scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void PentatonicScale(float dur);
    
	/**
	 *
	 * @brief
	 * Fills requested voice with a pentatonic scale
	 *
	 * @details
	 * Builds a pentatonic scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void PentatonicScale(int voiceNumber, float dur);
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with a whole tone scale
	 *
	 * @details
	 * Builds a whole tone scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void WholeToneScale(float dur);	
    
	/**
	 *
	 * @brief
	 * Fills requested voice with a whole tone scale
	 *
	 * @details
	 * Builds a whole tone scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void WholeToneScale(int voiceNumber, float dur);
	
	/**
	 *
	 * @brief
	 * Fills voice '0' with a Chromatic scale
	 *
	 * @details
	 * Builds a Chromatic scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void ChromaticScale(float dur);
	
	/**
	 *
	 * @brief
	 * Fills requested voice with a Chromatic scale
	 *
	 * @details
	 * Builds a Chromatic scale from middle C within material. All notes have a 
	 * length of dur and start time incremented by this duration, producing a 
	 * melodic pattern. The octave above is not included. 
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void ChromaticScale(int voiceNumber, float dur);

    // Dynamics and Phrasing
    
    
    /**
	 *
	 * @brief
	 * sets amplitude of all notes in voice voiceNumber to value
	 *
	 * @details
	 * sets the amplitude of each note in voice 'voiceNumber' to 'amp'.
     * If voiceNumber is invalid, nothing is done.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * amp (float) - amplitude (0.0 - 1.0)
	 *
	 **/
	void SetAmp(int voiceNumber, float amp);

    
    /**
	 *
	 * @brief
	 * sets dynamics as a crescendo from begining to end of material
	 *
	 * @details
	 * Builds a Chromatic scale from middle C within material. All notes have a
	 * length of dur and start time incremented by this duration, producing a
	 * melodic pattern. The octave above is not included.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
	void Crescendo(int voiceNumber, float max);

    /**
	 *
	 * @brief
	 * sets dynamics as a decrescendo from begining to end of material
	 *
	 * @details
	 * Builds a Chromatic scale from middle C within material. All notes have a
	 * length of dur and start time incremented by this duration, producing a
	 * melodic pattern. The octave above is not included.
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * dur (float) - duration in seconds
	 *
	 **/
    void Decrescendo(int voiceNumber, float max);
    
    /**
     *
     * @brief
     * Quantizes durations and start times to conform to a  given tempo
     *
     * @details
     * QuantizeMelodyFor() finds the closest duration value in a list of
     * quantized fractions of a beat for each note in the material, and changes
     * the note's start time and duration accordingly. 
     *
     * IMPORTANT! This algorithm only works with melodic materials.
     * Also, every rest should be featured within the voices as notes of
     * invalid (0) pitch and amplitude. If material contains more than
     * one voice, they should be loaded into separate voices withing the
     * material. Failure to do so would most likely result in
     * undesired behaviour.
     *
     * @param
     *
     * tempo (MuParamBlock &) - the assumed tempo for the melody(ies) contained
     * in the material. If the wrong tempo is provided, quantization results in
     * wrong rythms.
     *
     **/
    void QuantizeMelodyFor(float tempo);
    
    /**
	 *
	 * @brief
	 * removes blank notes from selected voice
	 *
	 * @details
	 * Removes every note that contains pitch == 0 or amp == 0 from selected 
     * voice. These notes are usually used to mark pauses in applications
     * that require explicit rests. Since MuM uses explicit start times for
     * every note, deleting these notes won't affect rhythm or performance...
     *
	 * @param
	 * voiceNumber (int) - voice index
	 *
	 **/
    void RemoveBlankNotes(int voiceNumber);

    
    // File IO
	
	/**
	* @brief
	* Reads a Csound score (.sco) into material
	* 
	* @details
	* Loads a Csound score file into material. User code provides the location of the score as a 
	* standard C++ string (fileName). Notes in score are assigned to voices in material 
	* according to instrument designation in the score's note lines, so that: 
	* 
	* instr(n) ==>> voice(n-1) 
	* 
	* Thus, notes for instrument 1 go into voice 0, notes for instrument 2 go into voice 1, and so 
	* on. LoadScore() currently does not support Csound comments or any score abreviations 
	* such as ".", "+" or "<". Every note parameter needs to be provided explicitly.  
	* 
	* Normally fileName needs to reference a file which is contained in the same folder as the 
	* running program. Optionally, the string may contain the complete path to the file, in 
	* which case the file may be placed in a suitable location. If the score file cannot be found in 
	* the place referenced by fileName, or if by any other reason it fails to open (permissions?), 
	* LoadScore() issues an error and terminates silently.
    *
    * LoadScore() can operate in one of two modes, as requested by its second argument. If mode ==
    * LOAD_MODE_TIME (the default mode) notes are added according to their start time, so they will
    * be in crhonological order even if the score is not. If mode == LOAD_MODE_DIRECT notes are
    * loaded in the order they appear in the score, thus they may not be necessarily in order.
	*
	* @param
	* fileName (string) - path to file as a string object
    * @param
    * mode (short) - mode of operation. allowed values are LOAD_MODE_TIME (0)
    * and LOAD_MODE_DIRECT (1)
	*
	**/
    void LoadScore(string fileName, short mode = LOAD_MODE_TIME);
	
    /**
     * @brief
     * Generates orchestra definition for Csound playback
     *
     * @details
     * Creates an orchestra file with default instruments for testing output scores. The
     * orquestra contains four instruments with diverse wave forms and overall amplitude envelopes.
     * Instruments may vary with MuM versions, so it is wise to study and test the orchestra
     * before using it. In any case, this orchestra's instruments will allways be able to play
     * standard note output which contains only the basic five parameters per note (instr, start,
     * dur, pitch, amp).
     * Obs.: Instruments need to use the default function tables defined by SetDefaultFunctionTables().
     *
     * @return
     * standard C++ string object containing the entire Csound orchestra
     **/
     string Orchestra(void);
    
    /**
     * @brief
     * Loads an orchestra definition from a Csound Orchestra file (.orc)
     *
     * @details
     * LoadOrchestra() opens the provided Csound orquestra file and copies
     * it's data as text to the material's internal string object, so that 
     * it can be used by the material for playback purposes. This will 
     * completely replace the material's internal orchestra definition for
     * the remaining of the object's existence.
     *
     * Normally fileName needs to reference a file which is contained in the same folder as the
     * running program. Optionally, the string may contain the complete path to the file, in
     * which case the file may be placed in a suitable location. If the orchestra file cannot
     * be found in the place referenced by fileName, or if by any other reason it fails to open
     * (permissions?), LoadOrchestra() issues an error and terminates silently.
     *
     * @param
     * fileName (string) - pathname for the orchestra file to be loaded. File maust be a
     *  a valid Csound orchestra file (.orc)
     *
     **/
    void LoadOrchestra(string fileName);

	/**
	 * @brief
	 * Writes Csound orchestra to file
	 * 
	 * @details
	 * Creates an orchestra file with default instruments for testing output scores. The  
	 * orquestra contains four instruments with diverse wave forms and overall amplitude envelopes.
	 * Instruments may vary with MuM versions, so it is wise to study and test the orchestra 
	 * before using it. In any case, this orchestra's instruments will allways be able to play 
	 * standard note output which contains only the basic five parameters per note (instr, start, 
	 * dur, pitch, amp).
	 * Obs.: Instruments need to use the default function tables defined by SetDefaultFunctionTables().
	 *
	 *
     @param
     * fileName (string) - path to file, wrapped in a standard C++ string object
     *
	 **/
    void Orchestra(string fileName);
	
    
	/**
	 * @brief
	 * Generates score definition for Csound playback
	 * 
	 * @details
	 * Generates a Csound score with every note in material. Each note converts its five basic 
	 * parameters to a standard note line where p4 = pitch (in Csound cpspch() format) and p5 = 
	 * amplitude between 0.0 and 1.0. 
	 *
	 * Example: 
	 *
	 * [voice 0] ==> [note instr=1, start=0.0, dur=2.5, pitch=48, amp=0.5] 
	 *
	 * becomes 
	 *
	 * i1  0.0  2.5  7.00  0.5 
	 *
	 * @return
	 * standard C++ string object containing the entire csound score
	 *
	 **/
    string Score(void);
    
    /**
     * @brief
     * Writes Csound score to file
     *
     * @details
     * Writes a Csound score file with every note in material. Each note converts its five basic
     * parameters to a standard note line where p4 = pitch (in Csound cpspch() format) and p5 =
     * amplitude between 0.0 and 1.0.
     *
     * Example:
     *
     * [voice 0] ==> [note instr=1, start=0.0, dur=2.5, pitch=48, amp=0.5]
     *
     * becomes
     *
     * i1  0.0  2.5  7.00  0.5
     *
     * @param
     * fileName (string) - path to file inside string object
     *
     **/
     void Score(string fileName);
    
    /**
     * @brief
     * defines a string containing the rendering flags to be used with Csound
     *
     * @details
     * Any valid Csound option flags can be inserted here in string format.
     * These options are used with the Csound command and inside the Csd() output.
     * The call replaces any prior flag strings inside the material.
     *
     * @param
     * a string containing option flags to be used in csd file, for Csound rendering
     *
     **/
    void SetCsOptions(string options);

    
    /**
     * @brief
     * returns the material's data for playback in Csound .csd format
     *
     * @details
     * Generates Csound playback information from material using Orchestra() and
     * Score() functions. Both these functions return strings which are then used
     * to assemble the XML based .csd file.
     *
     * Obs.1: Csound rendering options SetCsOptions() should be called beore Csd()
     * Obs.2: SetFuntionTables() or Se DefaultFunctionTables() should be called
     * before Csd()
     *
     * @return
     * a string containing a csd formatted document.
     *
     **/
    string Csd(void);
    
    /**
     * @brief
     * writes the material's data to a file in .csd format
     *
     * @details
     * Generates Csound playback information from material using Orchestra() and
     * Score() functions. Both these functions return strings which are then used
     * to assemble the XML based .csd file.
     *
     * Obs.1: Csound rendering options SetCsOptions() should be called before Csd()
     * Obs.2: SetFuntionTables() or Se DefaultFunctionTables() should be called
     * before Csd()
     *
     * @return
     * a string containing a csd formatted document.
     *
     **/
    void Csd(string fileName);

    /**
     * @brief
     * creates a csd file and calls Csound to render the file
     *
     * @details
     *
     Generates Csound playback information from material using Orchestra() and
     * Score() functions. Both these functions return strings which are then used
     * to assemble the XML based .csd file. After that, this method makes a
     * system call to start Csound using the generate file.
     *
     * @return
     * a string containing a csd formatted document.
     *
     **/
    void PlaybackWithCsound(string fileName);

	 
    // UTILITIES
	
	// General
	
	/**
	 * @brief
	 * Sorts entire material
	 * 
	 * @details
	 *
	 * @param
	 * field (short) - note parameter to search by
	 *
	 **/		
	void Sort( short field );
	
	/**
	 * @brief
	 * Sorts requested voice
	 * 
	 * @details
	 *
	 * @param
	 * voiceNumber (int) - voice index
	 * @param
	 * field (short) - note parameter to search by
	 *
	 **/		
    void Sort(int voiceNumber, short field);
	
	/**
	 * @brief
	 * Clears contents of material
	 * 
	 * @details
	 * Removes every voice and every note within the target object;  
	 * it also erases functiontables, if any exists. 
	 *
	 **/		
	void Clear(void);
	
	// Debug
	
	/**
	 * @brief
	 * Sends description of material to standard output
	 * 
	 * @details
	 * Displays information to screen describing material for debugging purposes;    
	 *
	 **/			
	void Show( void );
	
	// Csound
	
	/**
	 * @brief
	 * Converts pitches from csound to MuM native
	 * 
	 * @details
	 * Converts pitch information from Csound format to MuM format 
	 * (MIDI note number). Takes a cs_pitch structure and returns
	 * a 16 bit integer with the corresponding pitch number.
	 *
	 * @param
	 * inPitch (short) - note parameter to search by
	 *
	 * @return
	 * short - pitch value (MIDI note number)
	 *
	 **/		
    short CsoundToLocalPitch(cs_pitch inPitch);
	
	/**
	 * @brief
	 * Returns string containing function tables
	 * 
	 * @details
	 * Returns a standard C++ string containing the function tables assigned to material.
	 * An empty string, if no tables were assigned. Function tables are used by score 
	 * output files to provide data to be used by Csound instruments. Thus, they are 
	 * not necessary for MIDI output or for simple instruments. Tables are separated by 
	 * new lines, so that the entire block can be inserted at the begining of a file or 
	 * other output stream
	 *
	 * @return
	 * string - C++ string with a copy of internal function tables string
	 *
	 **/
    string FunctionTables(void);
	
	/**
	 * @brief
	 * Replaces function tables in material
	 * 
	 * @details
	 * Stores the string inTables, which must contain valid Csound function table
	 * declarations for use in score output files. This may be a single function 
	 * table line, or several declarations separated by a line break ( "\n" ). 
	 * Previous contents of internal function table string will be replace by the 
	 * content of inTables.
	 *
	 * @param
	 * inTables (string) - C++ string with valid Csound function tables
	 *
	 **/
    void SetFunctionTables(string inTables);
	
	/**
	 * @brief
	 * Defines a set of default function tables for built in orchestra
	 * 
	 * @details
	 * Creates and stores four function tables for use with
	 * the four instruments contained in a material's default
	 * orchestra ( see Orchestra() ). When processing scores with
	 * the material's default orchestra, this method should be called 
	 * before a call to Score(), in order to ensure that instruments 
	 * in the default orchestra will have access to the corresponding 
	 * default tables (f1, f2, f3, f4), when the score is processed by Csound.
	 * 
	 **/
	void SetDefaultFunctionTables(void);
	
	/**
	 * @brief
	 * Appends input string to function tables string
	 * 
	 * @details
	 * Stores the string inTables, which must contain valid Csound function table
	 * declarations for use in score output files. This may be a single function 
	 * table line, or several declarations separated by a line break ( "\n" ). 
	 * The string passed to AddFunctionTables() is appended to current function
	 * tables string inside material.
	 *
	 * @param
	 * inTables (string) - C++ string with valid Csound function tables
	 *
	 **/
	void AddFunctionTables(string inTables);
    
    /**
     * @brief
     * Loads function tables from file
     *
     * @details
     * LoadFunctionTables() opens the provided file and copies
     * it's data as text to the material's internal string object, so that
     * it can be used by the material for playback with Csound. This will
     * completely replace the material's internal function table definitions for
     * the  object.
     *
     * Normally fileName needs to reference a file which is contained in the same folder as the
     * running program. Optionally, the string may contain the complete path to the file, in
     * which case the file may be placed in a suitable location. If the function table file cannot
     * be found in the place referenced by fileName, or if by any other reason it fails to open
     * (permissions?), LoadFunctionTables() issues an error and terminates silently, leaving
     * the internal table definitions untouched.
     *
     * @param
     * fileName (string) - pathname for the orchestra file to be loaded. File maust be a
     *  a valid Csound orchestra file (.orc)
     *
     **/

    void LoadFunctionTables(string fileName);
	
	/**
	 * @brief
	 * Converts csound line to note object
	 * 
	 * @details
	 * Returns an MuNote object with the parameters of the note defined by inLine string.
	 * This is used internally by LoadScore(), but may be used directly by user code. 
	 * If imput string does not contain a perfectly defined Csound note line 
	 * (see Score() ), the results are undefined! 
	 * 
	 * @param
	 * inLine (char*) - voice index
	 *
	 * @return
	 * MuNote - resulting note object
	 *
	 **/
     MuNote CreateNoteFromCsoundLine(char * inLine);
    
    // ERROR MANAGEMENT
	
	/**
	 * @brief
	 * Returns internal error object
	 * 
	 * @details
	 * Returns an MuError object containing error information for the last action 
	 * performed by material. Since every method of the MuMaterial class clears 
	 * the internal error flag before executing, the error condition should be
	 * cheked right after a given method call. This error may then be viewed with
	 * the Message() method (see MuError, earlier in this reference manual).  
	 *
	 * Example: 
	 *
	 * MuMaterial m; MuNote n; 
	 * m.AddNote(n); 
	 * cout << (m.LastError()).Message();
	 *
	 * @return
	 * MuError - copy of internal error object
	 *
	 **/
    MuError LastError(void);
	
	/**
	 * @brief
	 * Clears internal error object
	 * 
	 * @details
	 * Resets internal error object to its default value (MuERROR_NONE)
	 *
	 *
	 **/
    void ClearError(void);
};

#endif
