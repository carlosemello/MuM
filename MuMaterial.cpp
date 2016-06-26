//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************


/** @file MuMaterial.cpp
 *
 * @brief Material Class Implementation
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 **/


#include "MuMaterial.h"

// Constructors
string MuMaterial::orchestra = "";
string MuMaterial::functionTables = "";

// Default
MuMaterial::MuMaterial(void)
{
    voices = NULL;
    numOfVoices = 0;
    lastError.Set(MuERROR_NONE);
    csOptions =  "-odac -d -O null";
}

// Copy
MuMaterial::MuMaterial(const MuMaterial & inMaterial)
{
    voices = NULL;
	numOfVoices = 0;
    lastError.Set(MuERROR_NONE);
	int n = 0;
	
    if(inMaterial.voices != NULL)
    {
        n = inMaterial.numOfVoices;
        voices = new MuVoice[n];
        if(voices)
        {
            for(int i = 0; i < n; i++)
				voices[i] = inMaterial.voices[i];
			numOfVoices = n;
			functionTables = inMaterial.functionTables;
            csOptions = inMaterial.csOptions;
        }
        else
        {
            lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
        }
    }
    else
    {
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
    }
}


// init with specific voice from inMaterial
MuMaterial::MuMaterial( const MuMaterial & inMaterial, int fromVoice )
{
    voices = NULL;
    numOfVoices = 0;
    lastError.Set(MuERROR_NONE);
	
	// obs.: since we are only interested in a specific voice,...
	// functionTables remain uninitialized
    
    if(inMaterial.voices != NULL)
    {
        if( ( fromVoice > 0 ) && ( fromVoice < inMaterial.numOfVoices) )
        {
            voices = new MuVoice[1];
            if(voices)
            {
                voices[0] = inMaterial.voices[fromVoice];
                numOfVoices = 1;
                functionTables = inMaterial.functionTables;
                csOptions = inMaterial.csOptions;
            }
            else
            {
                lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
            }
        }
        else
        {
            lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
        }
    }
    else
    {
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
    }
}

// Operators

// Assignment
MuMaterial & MuMaterial::operator=(const MuMaterial & inMaterial)		// [PUBLIC]
{
    int i;
	lastError.Set(MuERROR_NONE);
	
	// if self copy do nothing...
	if(this == &inMaterial)
		return *this;
	// delete old storage...
	delete [] voices;
	voices = NULL;
	int n = 0;
	// check for input content...
	if(inMaterial.voices != NULL)
    {
        n = inMaterial.numOfVoices;
        voices = new MuVoice[n];
        if(voices)
        {
            for(i = 0; i < n; i++)
			{
				voices[i] = inMaterial.voices[i];
			}
            numOfVoices = n;
			functionTables = inMaterial.functionTables;
            csOptions = inMaterial.csOptions;
        }
		else
		{
			lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
		}
    }
	else
	{
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	}

    return *this;
}

// Tests two materials for equality
// obs.: function tables are not considered for equality
bool MuMaterial::operator==( const MuMaterial & inMaterial)
{
	int i, n;
	
	if( numOfVoices != inMaterial.numOfVoices )
		 return false;
	
	n = numOfVoices;
	for( i = 0; i < n; i++ )
	{
		if( voices[i] != inMaterial.voices[i] )
			return false;
	}
	
	return true;
}

// Inequality test...
bool MuMaterial::operator!=(const MuMaterial & inMaterial) 
{
    return !( *this == inMaterial );
}

// Operator + => Appends material on the right of operator to the end of material on the left
MuMaterial  MuMaterial::operator+(const MuMaterial & inMaterial)		// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	lastError.Set(MuERROR_NONE);
	int i;
	float em = 0.0; // end of material
	float ev = 0.0;	// end of voice
	int n = numOfVoices;
	MuMaterial temp = inMaterial;
	
	// calculate end of material, comparing 
	//  ending point for each voice
	for(i = 0; i < n; i++)
	{
		ev = voices[i].End();
		if( ev > em )
			em = ev;
	}

	// Offset each voice of inMaterial to end of material...
	temp.Move(em);
		
	// MOVE EACH VOICE OF InMATERIAL TO END
	// MIX TWO MATERIALS
	temp.Mix(*this);
	
	return temp;
}

 MuMaterial & MuMaterial::operator+=( const MuMaterial & inMaterial )
{
    MuError err(MuERROR_NONE);
    lastError.Set(MuERROR_NONE);
    int i;
    float em = 0.0; // end of material
    float ev = 0.0;	// end of voice
    int n = numOfVoices;
    MuMaterial temp = inMaterial;
    
    // calculate end of material, comparing
    //  ending point for each voice
    for(i = 0; i < n; i++)
    {
        ev = voices[i].End();
        if( ev > em )
            em = ev;
    }
    
    // Offset each voice of inMaterial to end of material...
    temp.Move(em);
    
    // MOVE EACH VOICE OF InMATERIAL TO END
    // MIX TWO MATERIALS
    this->Mix(temp);
    
    return *this;
}

 MuMaterial & MuMaterial::operator+=( const MuNote & inNote )
{
    MuNote temp = inNote;
    
    temp.SetStart(this->Dur());
    AddNote(temp);
    
    return *this;
}

// Operator * => Mixes two materials
MuMaterial & MuMaterial::operator*( const MuMaterial & inMaterial  )		// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	Mix(inMaterial);
	return *this;
}

// Operator * => Transposes material by interval
MuMaterial & MuMaterial::operator*(short interval)		// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	Transpose(interval);
	return *this;
}

// Destructor
MuMaterial::~MuMaterial(void)
{
    lastError.Set(MuERROR_NONE);
	if(voices)
        delete [] voices;
}

// ======================================================================

float MuMaterial::Start(void) // [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	float last = 0.0;
	float curr = 0.0;
	int i;
	int n = numOfVoices;
	last = voices[0].Start();
	
	for(i = 0; i < n; i++)
	{
		curr = voices[i].Start();
		if( curr < last )
			last = curr;
	}
	
	return last;
}


float MuMaterial::Dur(void) // [PUBLIC]
{
	float dur = 0.0;
	float curr = 0.0;
	int i;
	int n = numOfVoices;
	
	for(i = 0; i < n; i++)
	{
		curr = voices[i].End();
		if( curr > dur )
			dur = curr;
	}
	
	return dur;
}

// Voices
int MuMaterial::NumberOfVoices(void)				// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	return numOfVoices;
}

bool MuMaterial::Voice(int voiceNumber)
{
	if( (voices != NULL) && ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		return true;
	
	return false;
}

int MuMaterial::GetVoiceNumberForInstrument(int instrNum)  // [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	if(voices != NULL)
    {
        for(int i = 0; i < numOfVoices; i++)
        {
            if(voices[i].InstrumentNumber() == instrNum)
                return i;
        }
    }
    else
    {
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
        return -1;
    }
    
    return -1;
}

uShort MuMaterial::InstrumentNumber(int voiceNumber)
{
    uShort instr = 0;
    lastError.Set(MuERROR_NONE);
    if(voices != NULL)
    {
        if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
        {
            instr = voices[voiceNumber].InstrumentNumber();
        }
        else
            lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
    }
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
    
    return instr;
}

void MuMaterial::SetInstrument(int voiceNumber, uShort instrNum)
{
	lastError.Set(MuERROR_NONE);
	if(voices != NULL)
	{
		if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		{
			voices[voiceNumber].SetInstrumentNumber(instrNum);
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
}

MuMaterial MuMaterial::GetVoice(int voiceNumber)
{
	lastError.Set(MuERROR_NONE);
	MuMaterial temp;
	if(voices != NULL)
	{
		if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		{ 
			temp.AddVoices(1);
			temp.SetVoice(0, *this, voiceNumber );
			return temp;
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
	return temp;
}

void MuMaterial::SetVoice(int voiceNum, const MuMaterial & inMaterial, int inVoice)
{
    lastError.Set(MuERROR_NONE);
	long i, n, numVoices;
	MuNote theNote;
	MuError err;
	
	// verify if requested voice exists inside inMaterial
	// (since inMaterial is const, we must check manually...)
	numVoices = inMaterial.numOfVoices;
	if( (inMaterial.voices == NULL) || ( inVoice < 0 ) || ( inVoice >= numVoices ) )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	// If inVoice is ok we get a handle to it
	MuVoice & tempVoice = inMaterial.voices[inVoice];
	
	// if the requested voice position in this material does not exist...
	// we add new voices up to the desired voice number...
	if( ( voiceNum >= numOfVoices ) )
		AddVoices(voiceNum - numOfVoices + 1);

	// then we wipe clean existing local voice, ...
	voices[voiceNum].Clear();
	// and add each note of the input voice to the target voice:
	n = tempVoice.NumberOfNotes();

	for(i = 0; i < n; i++)
	{
		err = tempVoice.GetNote( i, &theNote );
		if( err.Get() == MuERROR_NONE )
		{
			err = voices[voiceNum].AddNote( theNote );
			if( err.Get() != MuERROR_NONE )
			{
				lastError.Set( err );
				return;
			}
		}
		else
		{	
			lastError.Set( err );
			return;
		}
	}
}

void MuMaterial::AddVoices( int n ) // [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuVoice * tempVoices = NULL;
    int i = 0;
    
    if(voices == NULL)
    {
        voices = new MuVoice[n];
        if(voices != NULL)
            numOfVoices = n;
        else
            lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
    }
    else
    {
        tempVoices = new MuVoice[numOfVoices + n];
        if(tempVoices)
        {
            for(i = 0; i < numOfVoices; i++)
                tempVoices[i] = voices[i];
            // free old array...
            delete [] voices;
            // update address...
            voices = tempVoices;
            // update voice count
            numOfVoices += n;
        }
        else
            lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
    }
}

void MuMaterial::InsertVoices(int voiceNum, int n) 
{
   lastError.Set(MuERROR_NONE);
	MuVoice * tempVoices = NULL;
    int i = 0;
    
    if(voices != NULL)
    {
        if((voiceNum >= 0) && (voiceNum < numOfVoices))
        {
            // Get a bigger array... 
            tempVoices = new MuVoice[numOfVoices + n];
            if(tempVoices)
            {
                // copy voices up to  desired voice number...
                for(i = 0; i <= voiceNum; i++)
                    tempVoices[i] = voices[i];
				
				// copy remaining voices to subsequent spots... 
                for(i = (voiceNum + n + 1); i < (numOfVoices + n); i++)
                    tempVoices[i] = voices[i-n];
                // free old array...
                delete [] voices;
                // update address...
                voices = tempVoices;
                // update voice count
                numOfVoices += n;
            }
            else
                lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
        }
        else
            lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
    }
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
}


void MuMaterial::RemoveVoice(int voiceNum) // [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuVoice * tempVoices = NULL;
    int i = 0;
    
    if(voices != NULL)
    {
        if((voiceNum >= 0) && (voiceNum < numOfVoices))
        {
            tempVoices = new MuVoice[numOfVoices - 1];
            if(tempVoices)
            {
                for(i = 0; i < voiceNum; i++)
                    tempVoices[i] = voices[i];
                for(  ; i < (numOfVoices -1); i++)
                    tempVoices[i] = voices[i + 1];
                // free old array...
                delete [] voices;
                // update address...
                voices = tempVoices;
                // update voice count
                numOfVoices--;
            }
            else
                lastError.Set(MuERROR_COULDNT_INIT_MATERIAL);
        }
        else
            lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
    }
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
}

void MuMaterial::ClearVoice(int voiceNumber)
{
	lastError.Set(MuERROR_NONE);
	
	if(voices != NULL)
	{
		if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		{ 
			voices[voiceNumber].Clear();
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
}

bool MuMaterial::IsEmptyVoice( int voiceNumber )
{
	lastError.Set(MuERROR_NONE);
	// if voice doesn't exist, this will return as being empty
	bool isEmpty = true;
	
	if(voices != NULL)
	{
		if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		{ 
			if( voices[voiceNumber].NumberOfNotes() != 0 )
				isEmpty = false;
		}
	}
	return isEmpty;
}

void MuMaterial::SwapVoices(int oneVoice, int otherVoice) // [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	if(voices != NULL)
    {
        if((oneVoice >= 0) && (oneVoice < numOfVoices) && (otherVoice >= 0) && (otherVoice < numOfVoices))
        {
            // swap positions
            MuVoice temp = voices[oneVoice];
            voices[oneVoice] = voices[otherVoice];
            voices[otherVoice] = temp;
        }
        else
            lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
    }
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
}


void MuMaterial::Mix( const MuMaterial & inMaterial)
{
	lastError.Set(MuERROR_NONE);
	MuError err(MuERROR_NONE);
	MuNote note;
	long i, j, n;
	int targetSize = numOfVoices;				// number of voices in target material
	// [OLD CODE - WITHOUT CONST]
	// int sourceSize = inMaterial.NumberOfVoices();	// number of voices in source material
	int sourceSize = inMaterial.numOfVoices;	// number of voices in source material
	int voiceCount =  sourceSize - targetSize;	// if positive, we need to add voices to target
	
	// Add new voices to this material if needed...
	if( voiceCount > 0 )
		AddVoices(voiceCount);
	
	// For every voice in input material...
	for(i = 0; i < sourceSize; i++)
	{
		// verify number of notes for each voice
		MuVoice & tempVoice = inMaterial.voices[i];
		n = tempVoice.NumberOfNotes();
		
		// copy each note to corresponding voice in target
		for(j = 0; j < n; j++)
		{
			// [OLD CODE - WITHOUT CONST]
			//note = inMaterial.GetNote( i, j );
			//err = inMaterial.LastError();
			err = tempVoice.GetNote(j,&note);
			if(err.Get() == MuERROR_NONE)
				voices[i].AddNote( note );
			else
			{
				lastError.Set(err);
				return;
			}
		}
	}
}

void MuMaterial::Mix(int voiceNumber, const MuMaterial & inMaterial, int inVoice)
{
	lastError.Set(MuERROR_NONE);
	MuError err(MuERROR_NONE);
	MuVoice sourceVoice;
	MuNote note;
	long i, n, numVoices;
	
	// verify if requested voice exists inside inMaterial
	// (since inMaterial is const, we must check manually...)
	numVoices = inMaterial.numOfVoices;
	if( (inMaterial.voices == NULL) || ( inVoice < 0 ) || ( inVoice >= numVoices ) )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	// then verify if target voice is valid
	if( !Voice( voiceNumber ) )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	// verify number of notes
	MuVoice & tempVoice = inMaterial.voices[inVoice];
	n = tempVoice.NumberOfNotes();
	
	// copy each note to corresponding voice in target
	for(i = 0; i < n; i++)
	{
		err = tempVoice.GetNote(i,&note);
		if(err.Get() == MuERROR_NONE)
			voices[voiceNumber].AddNote( note );
		else
		{
			lastError.Set(err);
			return;
		}
	}
}

void MuMaterial::Append(int voiceNumber, const MuMaterial & inMaterial, int inVoice)
{
	lastError.Set(MuERROR_NONE);
	long i, n, numVoices;
	float end;
	MuNote theNote;
	MuError err;
	
	// verify if requested voice exists inside inMaterial
	// (since inMaterial is const, we must check manually...)
	numVoices = inMaterial.numOfVoices;
	if( (inMaterial.voices == NULL) || ( inVoice < 0 ) || ( inVoice >= numVoices ) )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	// If inVoice is ok we get a handle to it
	MuVoice tempVoice = inMaterial.voices[inVoice];
	
	// If target voice number is invalid (negative) we bail out
	if(voiceNumber < 0)
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	// if the requested voice position in this material does not exist...
	// we add new voices up to the desired voice number...
	if( ( voiceNumber >= numOfVoices ) )
		AddVoices(voiceNumber - numOfVoices + 1);
	
	// if we got this far, just find out where the voice ends,...
	end = voices[voiceNumber].End();
	
	// move the notes in input voice to that position...
	tempVoice.Move( end );
	
	// and add them to target voice.
	n = tempVoice.NumberOfNotes();
	for( i = 0; i < n; i++ )
	{
		err = tempVoice.GetNote(i, &theNote);
		if(err.Get() != MuERROR_NONE )
		{
			lastError.Set(err);
			return;
		}
		
		err = voices[voiceNumber].AddNote( theNote );
		if(err.Get() != MuERROR_NONE )
		{
			lastError.Set(err);
			return;
		}
	}
}

void MuMaterial::Append(int voiceNumber, MuNote inNote)
{
    lastError.Set(MuERROR_NONE);
    float end;
    
    // If target voice number is invalid (negative) we bail out
    if(voiceNumber < 0)
    {
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
        return;
    }

    // if the requested voice position in this material does not exist...
    // we add new voices up to the desired voice number...
    if( ( voiceNumber >= numOfVoices ) )
        AddVoices(voiceNumber - numOfVoices + 1);
    
    // then, find out where the voice ends,...
    end = voices[voiceNumber].End();
    
    // and make the new note start at that point...
    inNote.SetStart(end);
    
    // after that we just append the note to the requested voice...
    // and acknowledge possible errors...
    lastError.Set(voices[voiceNumber].AddNote(inNote));
}

void MuMaterial::IncludeNote(int voiceNumber, MuNote inNote)
{
    lastError.Set(MuERROR_NONE);
    
    // If target voice number is invalid (negative) we bail out
    if(voiceNumber < 0)
    {
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
        return;
    }
    
    // if the requested voice position in this material does not exist...
    // we add new voices up to the desired voice number...
    if( ( voiceNumber >= numOfVoices ) )
        AddVoices(voiceNumber - numOfVoices + 1);
    
    // after that we just include the note to the requested voice...
    // and acknowledge possible errors...
    lastError.Set(voices[voiceNumber].IncludeNote(inNote));
}

// NOTES

long MuMaterial::NumberOfNotes(void)
{
	lastError.Set(MuERROR_NONE);
	
	long num = 0;
	long i, n;
	
	// if material is not empty...
	if(voices != NULL)
	{
		// go through every voice e acumulate
		// note count for each one...
		n = numOfVoices;
		for( i = 0; i < n; i++ )
		{
			num += voices[i].NumberOfNotes();
		}
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
	return num;
	
}

long MuMaterial::NumberOfNotes(int voiceNumber)
{
	lastError.Set(MuERROR_NONE);
	
	long num = 0;
	
	if(voices != NULL)
	{
		if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		{
			num = voices[voiceNumber].NumberOfNotes();
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
	return num;
}

void MuMaterial::SetNote(long noteNumber, MuNote inNote)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	
	if(voices != NULL)
        lastError.Set( voices[0].SetNote( noteNumber, inNote ) );
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY); 
		
}	

void MuMaterial::SetNote(int voiceNumber, long noteNumber, MuNote inNote)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	
	if(voices != NULL)
	{
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
			lastError.Set( voices[voiceNumber].SetNote( noteNumber, inNote ) );
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
}

MuNote MuMaterial::GetNote(long noteNumber)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	MuNote theNote;
	
	if(voices != NULL)
        lastError.Set( ( voices[0].GetNote( noteNumber, &theNote ) ) );
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY); 
	
    return theNote;
}

MuNote MuMaterial::GetNote(int voiceNumber, long noteNumber)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
    MuNote theNote;

	if(voices != NULL)
	{
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
			lastError.Set( (voices[voiceNumber].GetNote(noteNumber, &theNote) ) );
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
    				  
    return theNote;
}

MuNote MuMaterial::GetFirstNote( void )
{
	lastError.Set(MuERROR_NONE);
	bool foundNote = false;
	MuError err;
    MuNote theNote, currNote;
	float start = 0;
	int n = numOfVoices;
	int i;
	
	for( i = 0; i < n; i++ )
	{
		err = voices[i].GetNote( 0, &currNote );
		if( err.Get() == MuERROR_NONE )
		{
			// if we still don't have a first note...
			if(!foundNote)
			{
				// remember this first found note and...
				start = currNote.Start();
				theNote = currNote;
				// acknowledge finding it
				foundNote = true;
			}
			else
			{
				// if we already have a note, compare it to 
				// the one we just found (we want the earliest one)
				if( currNote.Start() < start )
				{
					start = currNote.Start();
					theNote = currNote;
				}
			}
		}
	}
	
	// If we didn't find any notes in this material,
	// report error (this error should be documented...)
	if(!foundNote)
		lastError.Set(MuERROR_NOTE_NOT_FOUND);
	
	return theNote;
}

MuMaterial MuMaterial::GetNotes( int voiceNumber, long from, long through )
{
	lastError.Set(MuERROR_NONE);
	MuMaterial outMaterial;
    MuNote note;
	MuError err;
	
	if(voices != NULL)
	{
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
		{
			long i, n;
			n = voices[voiceNumber].NumberOfNotes();
			
			// if requested range is valid...
			if(		( (from >= 0) && (from < n) ) &&
					( (through >=0) && (through < n) ) &&
					( (through > from) )
				)
			{
				for( i = from; i <= through; i++ )
				{
					err = voices[voiceNumber].GetNote(i, &note);
					if(err.Get() == MuERROR_NONE )
					{
						outMaterial.AddNote(0, note);
						if( (outMaterial.LastError()).Get() != MuERROR_NONE )
							return outMaterial;
					}
				}
			}
			else
				lastError.Set(MuERROR_INVALID_NOTE_RANGE);
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
    return outMaterial;
	
}

MuMaterial MuMaterial::GetNotesStartingAt(int voiceNumber, float time)
{
	lastError.Set(MuERROR_NONE);
	MuMaterial outMaterial;
    MuNote note;
	MuError err;
	
	if(voices != NULL)
	{
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
		{
			long i, n;
			n = voices[voiceNumber].NumberOfNotes();
			for( i = 0; i < n; i++ )
			{
				err = voices[voiceNumber].GetNote(i, &note);
				if(err.Get() == MuERROR_NONE )
				{
					if(note.Start() == time)
					{
						outMaterial.AddNote(0, note);
						if( (outMaterial.LastError()).Get() != MuERROR_NONE )
							return outMaterial;
					}
				}
			}
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
    return outMaterial;
}

MuMaterial MuMaterial::GetNotesSoundingAt(int voiceNumber, float time)
{
	lastError.Set(MuERROR_NONE);
	MuMaterial outMaterial;
    MuNote note;
	MuError err;
	
	if(voices != NULL)
	{
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
		{
			long i, n;
			n = voices[voiceNumber].NumberOfNotes();
			for( i = 0; i < n; i++ )
			{
				err = voices[voiceNumber].GetNote(i, &note);
				if(err.Get() == MuERROR_NONE )
				{
					if( time >= note.Start() && time < note.End() )
					{
						outMaterial.AddNote(0, note);
						if( (outMaterial.LastError()).Get() != MuERROR_NONE )
							return outMaterial;
					}
				}
			}
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
    return outMaterial;
}

bool MuMaterial::Contains( int voiceNumber, int pitch )
{
	lastError.Set(MuERROR_NONE);
	long n, i;
	bool answer = false;
	MuNote note;
	
	if(voices != NULL)
	{
		if( ( voiceNumber >= 0 ) && ( voiceNumber < numOfVoices ) )
		{
			n = NumberOfNotes( voiceNumber );
			for(i = 0; i < n; i++ )
			{
				note = GetNote(voiceNumber, i);
				if(note.Pitch() == pitch)
					return true;
			}
		}
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
	
	return answer;
}

void MuMaterial::AddNote(MuNote inNote)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	if(voices != NULL)
        lastError.Set( voices[0].AddNote(inNote) );
    else
	{
		AddVoices(1);
		if(lastError.Get() == MuERROR_NONE)
		{
			lastError.Set( voices[0].AddNote(inNote) );
		}
	}
}

void MuMaterial::AddNote(int voiceNumber, MuNote inNote)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	if(voiceNumber < 0)
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	if(voices != NULL)
    {
        // If requested voice already exists, add note and be done
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
            lastError.Set( voices[voiceNumber].AddNote(inNote) );
        else // If not, add necessary voices...
		{
            if( ( voiceNumber >= numOfVoices ) )
			{
				AddVoices( voiceNumber - numOfVoices + 1);
				// if new voice is ok...
				if(lastError.Get() == MuERROR_NONE)
				{
					// then add new note
					lastError.Set( voices[voiceNumber].AddNote(inNote) );
				}
			}
		}
    }
	else // if there are no voices in material
	{
		AddVoices( voiceNumber + 1);
		// if new voice is ok...
		if(lastError.Get() == MuERROR_NONE)
		{
			// then add new note
			lastError.Set( voices[voiceNumber].AddNote(inNote) );
		}
	}
}


void MuMaterial::RemoveNote(long noteNumber)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	
	if(voices != NULL)
		lastError.Set( voices[0].RemoveNote( noteNumber ) );
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);  
}

void MuMaterial::RemoveNote(int voiceNumber, long noteNumber)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	
	if(voices != NULL)
	{
		if((voiceNumber < numOfVoices) && (voiceNumber >= 0))
			lastError.Set( voices[voiceNumber].RemoveNote( noteNumber ) );
		else
			lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
	}
	else
		lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
}

// Transposing
void MuMaterial::Transpose(short interval)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	int n;
	int i = 0;
	
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    n = numOfVoices;
	
    for(i = 0; i < n; i++)
    {
        err = voices[i].Transpose(interval);
        lastError.Set(err);
    }
}

void MuMaterial::Transpose(int voiceNumber, short interval)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    if((voiceNumber >= 0) && (voiceNumber < numOfVoices))
    {
		err = voices[voiceNumber].Transpose(interval);
		lastError.Set(err);
    }
    else
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
}

void MuMaterial::Transpose(int voiceNumber, long noteNumber, short interval)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    if((voiceNumber >= 0) && (voiceNumber < numOfVoices))
    {
            err = voices[voiceNumber].Transpose(noteNumber,interval);  
            lastError.Set(err);
    }
    else
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
}

void MuMaterial::Transpose(int voiceNumber, long startingNote, long endingNote, short interval)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	long i = 0;
    long n = 0;
    short thePitch = 0;
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    if((voiceNumber >= 0) && (voiceNumber < numOfVoices))
    {
        n = voices[voiceNumber].NumberOfNotes();
        if((startingNote >= 0) && (startingNote < n) && (endingNote >= 0) && (endingNote < n))
        {
            // swap limits if needed...
            if(startingNote > endingNote)
            {
                long temp = startingNote;
                startingNote = endingNote;
                endingNote = temp;
            }
            for(i = startingNote; i <= endingNote; i++)
            {
                err = voices[voiceNumber].GetNote(i, &tempNote);
                thePitch = tempNote.Pitch();
                tempNote.SetPitch( thePitch + interval);
                err = voices[voiceNumber].SetNote(i,tempNote);
                lastError.Set(err);
            }
        }
        else
            lastError.Set(MuERROR_NOTE_NOT_FOUND);
    }
    else
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
}

// ARGUMENTS:  ==================================
// key ? { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }
// mode ? { MAJOR_MODE, MINOR_MODE }
// degree ? { 1, 2, 3, 4, 5, 6, 7 }
// direction ? { ASCENDING, DESCENDING }
// ==============================================
void MuMaterial::DiatonicTranspose( short key, short mode, short targetDegree, short direction )
{	
	MuError err( MuERROR_NONE );
	lastError.Set( MuERROR_NONE );
	short major[ NUM_OF_SCALE_DEGREES ] = { 0, 2, 4, 5, 7, 9, 11 };
	short minor[ NUM_OF_SCALE_DEGREES ] = { 0, 2, 3, 5, 7, 8, 10 };
	short scale[ FULL_SCALE_SIZE ];
	short * modePattern = NULL;
	long n, j, k;
    int v,i;
    MuMaterial tempMat;
	MuNote tempNote;
	short pitch;
	short sourceDegree, currDegree, degreeChange;
	
	switch (mode) 
	{
		case MAJOR_MODE:
			modePattern = major;
			break;
			
		case MINOR_MODE:
			modePattern = minor;
			break;
	}
	
	// Generate full scale for degree transposition...
	for( i = 0, j = LOWEST_C; ( i < FULL_SCALE_SIZE ); i += NUM_OF_SCALE_DEGREES, j += 12 )
	{
		for( k = 0; k < NUM_OF_SCALE_DEGREES; k++ )
			scale[ i + k ] = j + modePattern[ k ] + key;
	}
	
	targetDegree--;
    tempMat = *this;
    tempMat.Sort(SORT_FIELD_PITCH);
	tempNote = tempMat.GetFirstNote();
	pitch = tempNote.Pitch();
	sourceDegree = Inside( pitch, scale, FULL_SCALE_SIZE );
	sourceDegree %= NUM_OF_SCALE_DEGREES;
	degreeChange = targetDegree - sourceDegree;
	degreeChange *= direction;
	
	v = numOfVoices;
	for( i = 0; i < v; i++ )
	{
		n = NumberOfNotes( i );
		for( j = 0; j < n; j++ )
		{
			tempNote = GetNote( i, j );
			if( lastError.Get() == MuERROR_NONE )
			{
				currDegree = Inside(tempNote.Pitch(), scale, FULL_SCALE_SIZE );
                if(currDegree == -1)
                {
                    // if pitch is not found in scale, we cannot proceed,
                    // because we are using currDegree as an array index.
                    lastError = MuERROR_INVALID_SCALE_DEGREE;
                    return;
                }
				currDegree += degreeChange;
				tempNote.SetPitch( scale[ currDegree ] );
				SetNote( i, j, tempNote );
			}
		}
	}
}

void MuMaterial::ColapsePitch(void)
{
	lastError.Set(MuERROR_NONE);
	
	int n = numOfVoices;
	int i;
	for(i = 0; i < n; i++)
	{
		ColapsePitch(i);
		if(lastError.Get() != MuERROR_NONE)
			return;
	}
}

void MuMaterial::ColapsePitch(int voiceNumber)
{
	lastError.Set(MuERROR_NONE);
	MuError err;
	MuNote note;
	long v, n, i;
	
	v = numOfVoices;
    if((voiceNumber >= 0) && (voiceNumber < v))
    {
		n = voices[voiceNumber].NumberOfNotes();
		for(i = 0; i < n; i++)
		{
			note = GetNote( voiceNumber, i);
			note.ColapsePitch();
			SetNote( voiceNumber, i, note );
		}
		
    }
    else
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
}

// Moving
void MuMaterial::Move(float timePoint) 	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	int i = 0;
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    for(i = 0; i < numOfVoices; i++)
    {
        err = voices[i].Move(timePoint);
		if(err.Get() != MuERROR_NONE)
			lastError.Set(err);
    }
}

void MuMaterial::Move(int voiceNumber, float timePoint)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    if((voiceNumber >= 0) && (voiceNumber < numOfVoices))
    {
        err = voices[voiceNumber].Move(timePoint);
        lastError.Set(err);
    }
    else
        lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
}

// Inverting
void MuMaterial::Invert(void)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	int i, n;
	n = numOfVoices;
	for(i = 0; i < n; i++)
	{
		Invert(i);
		if( lastError.Get() != MuERROR_NONE )
			return;
	}
}

void MuMaterial::Invert(int voiceNumber)	// [PUBLIC]
{
    long i, n;
	MuNote tempNote;
	MuError err;
	short firstPitch, currPitch, interval;
	lastError.Set(MuERROR_NONE);
	
	if( voiceNumber < 0 || voiceNumber >= numOfVoices )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	// first note is the pivot for invertion...
	err = voices[voiceNumber].GetNote(0, &tempNote);
	if(err.Get() != MuERROR_NONE)
	{
		lastError.Set(err);
		return;
	}
	firstPitch = tempNote.Pitch();
	
	n = voices[voiceNumber].NumberOfNotes();
	for( i = 0; i < n; i++ )
	{
		err = voices[voiceNumber].GetNote(i, &tempNote);
		if(err.Get() == MuERROR_NONE)
		{
			// Get pitch for current note...
			currPitch = tempNote.Pitch();
			// calculate distance to first note...
			interval = currPitch - firstPitch;
			// invert interval...
			interval *= -1;
			// redefine note based on inverted interval...
			tempNote.SetPitch(firstPitch + interval);
			// update note...
			err = voices[voiceNumber].SetNote( i, tempNote);
			if(err.Get() != MuERROR_NONE)
			{
				lastError.Set(err);
				return;
			}
		}
	}
}

// Retrograding
void MuMaterial::Retro(void)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	int i, n;
	n = numOfVoices;
	for(i = 0; i < n; i++)
	{
		Retro(i);
		if( lastError.Get() != MuERROR_NONE )
			return;
	}
}

void MuMaterial::Retro(int voiceNumber)	// [PUBLIC]
{
    long i, n;
	MuNote currNote, prevNote;
	MuError err;
	float currTime = 0;
	float prevNoteEnd, prevNoteOffset;
	lastError.Set(MuERROR_NONE);
	
	if( voiceNumber < 0 || voiceNumber >= numOfVoices )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	n = voices[voiceNumber].NumberOfNotes();
	//calculate offset from end of material
	currTime = Dur() - voices[voiceNumber].End();	
	// Goes backwards through note list 
	// repositioning every note in reverse order.
	for( i = n-1; i >= 0; i--)
	{
		err = voices[voiceNumber].GetNote(i, &currNote);
		if(err.Get() == MuERROR_NONE)
		{
			prevNoteOffset = 0;
			if( i > 0 )
			{
				err = voices[voiceNumber].GetNote( (i-1), &prevNote);
				if(err.Get() == MuERROR_NONE)
				{
					prevNoteEnd = prevNote.Start() + prevNote.Dur();
					prevNoteOffset = currNote.Start() - prevNoteEnd;
				}
			}
			currNote.SetStart( currTime );
			err = voices[voiceNumber].SetNote( i, currNote);
			if(err.Get() != MuERROR_NONE)
			{
				lastError.Set(err);
				return;
			}
			currTime += (currNote.Dur() + prevNoteOffset);
		}
	}
	voices[voiceNumber].Sort();
}

void MuMaterial::CyclePitch(int voiceNumber, int times)
{
	lastError.Set(MuERROR_NONE);
	
	if( voiceNumber < 0 || voiceNumber >= numOfVoices )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	if(times < 0)
	{
		lastError.Set(MuERROR_INVALID_PARAMETER);
		return;
	}
	
	long n = NumberOfNotes( voiceNumber );
	int i;
	int tempPitch;
	MuNote note1, note2;
	
	while(times--)
	{
		tempPitch = (GetNote(voiceNumber, 0) ).Pitch();
		for( i = 0; i < (n-1); i++)
		{
			note1 = GetNote(voiceNumber, i);
			note2 = GetNote(voiceNumber, i+1);
			note1.SetPitch(note2.Pitch());
			SetNote(voiceNumber, i, note1);
		}
		
		note2 = GetNote(voiceNumber, i);
		note2.SetPitch(tempPitch);
		SetNote(voiceNumber, i, note2);
	}
}

void  MuMaterial::ExpandInterval(int halfSteps)
{
	lastError.Set(MuERROR_NONE);
	int i;
	int n = NumberOfVoices();
	
	for( i = 0; i < n; i++)
		ExpandInterval( i, halfSteps );
}

void  MuMaterial::ExpandInterval(int voiceNumber, int halfSteps)
{
	if( voiceNumber < 0 || voiceNumber >= numOfVoices )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	long n, i;
	MuNote curr, next;
	int currPitch, nextPitch, interval;
	
	curr = GetNote(voiceNumber, 0);
	currPitch = curr.Pitch();
	n = NumberOfNotes(voiceNumber);
	for( i = 0; i < (n-1); i++ )
	{
		currPitch = curr.Pitch();
		next = GetNote(voiceNumber, i+1);
		nextPitch = next.Pitch();
		interval = nextPitch - currPitch;
		// save next note for later
		curr = next;
		// negative == descending
		if(interval < 0)
			nextPitch -= halfSteps;
		else
			nextPitch += halfSteps;
		// now save modification to second note
		next.SetPitch(nextPitch);
		SetNote(voiceNumber, i+1, next);
	}
}

void  MuMaterial::ContractInterval(int halfSteps)
{
	lastError.Set(MuERROR_NONE);
	int i;
	int n = NumberOfVoices();
	
	for( i = 0; i < n; i++)
		ContractInterval( i, halfSteps );
}

void  MuMaterial::ContractInterval(int voiceNumber, int halfSteps)
{
	if( voiceNumber < 0 || voiceNumber >= numOfVoices )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	long n, i;
	MuNote curr, next;
	int currPitch, nextPitch, interval;
	
	curr = GetNote(voiceNumber, 0);
	currPitch = curr.Pitch();
	n = NumberOfNotes(voiceNumber);
	for( i = 0; i < (n-1); i++ )
	{
		currPitch = curr.Pitch();
		next = GetNote(voiceNumber, i+1);
		nextPitch = next.Pitch();
		interval = nextPitch - currPitch;
		// save next note for later
		curr = next;
		// negative == descending
		if(interval < 0)
			nextPitch += halfSteps;
		else
			nextPitch -= halfSteps;
		// now save modification to second note
		next.SetPitch(nextPitch);
		SetNote(voiceNumber, i+1, next);
		
	}
}

// time  stretching
void MuMaterial::Fit(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
    int i = 0;
    long j = 0;
    long n = 0;
    float end = 0;
    float tempEnd = 0;
    float stretch = 0;
    float tempStart = 0;
    float tempDur = 0; 
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    // First calculate duration of entire material
    for(i = 0; i < numOfVoices; i++)
    {
        n = voices[i].NumberOfNotes();
        
        for(j = 0; j < n; j++)
        {
            err = voices[i].GetNote(j, &tempNote);
            tempEnd = tempNote.Start() + tempNote.Dur();
            if (tempEnd > end)
                end = tempEnd;
        }
    }
    
    // Now that we know when it ends
    // we calculate what is the time stretch factor
    stretch = dur/end;
    
    // Then we go back through the material
    // rescaling starts and durations
    
    for(i = 0; i < numOfVoices; i++)
    {
        n = voices[i].NumberOfNotes();
        
        for(j = 0; j < n; j++)
        {
            err = voices[i].GetNote(j, &tempNote);
            tempStart = tempNote.Start();
            tempNote.SetStart( tempStart * stretch);
            tempDur = tempNote.Dur();
            tempNote.SetDur( tempDur * stretch);
            err = voices[i].SetNote(j,tempNote);
            lastError.Set(err);
        }
    }
}

void MuMaterial::Scale(float factor)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	int i = 0;
    long j = 0;
    long n = 0;
    float tempStart = 0;
    float tempDur = 0; 
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    for(i = 0; i < numOfVoices; i++)
    {
        n = voices[i].NumberOfNotes();
        
        for(j = 0; j < n; j++)
        {
            err = voices[i].GetNote(j, &tempNote);
            tempStart = tempNote.Start();
            tempNote.SetStart( tempStart * factor);
            tempDur = tempNote.Dur();
            tempNote.SetDur( tempDur * factor);
            err = voices[i].SetNote(j,tempNote);
            lastError.Set(err);
        }
    }

}

void MuMaterial::CycleRhythm( int times)
{
	lastError.Set(MuERROR_NONE);
	int i;
	int n = NumberOfVoices();
	
	for( i = 0; i < n; i++)
		CycleRhythm( i, times );
}

void MuMaterial::CycleRhythm(int voiceNumber, int times)
{
	lastError.Set(MuERROR_NONE);
	
	if( voiceNumber < 0 || voiceNumber >= numOfVoices )
	{
		lastError.Set(MuERROR_INVALID_VOICE_NUMBER);
		return;
	}
	
	if(times < 0)
	{
		lastError.Set(MuERROR_INVALID_PARAMETER);
		return;
	}
	
	long n = NumberOfNotes( voiceNumber );
	int i;
	float currDur, nextDur;
	float currSt, nextSt;
	MuNote currNote, nextNote;
	
	while(times--)
	{
		
		currNote = GetNote(voiceNumber, 0);
		currSt = currNote.Start();
		currDur = currNote.Dur();
		
		for( i = 0; i < (n-1); i++)
		{
			
			currNote = GetNote(voiceNumber, i);
			
			nextNote = GetNote(voiceNumber, i+1);
			nextSt = nextNote.Start();
			nextDur = nextNote.Dur();
			
			// then change it
			currNote.SetStart(nextSt);
			currNote.SetDur(nextDur);
		}
		
		currNote = GetNote(voiceNumber, i);
		currNote.SetStart(currSt);
		currNote.SetDur(currDur);
	}
	
	// CAREFULL: PERFORMANCE ALLERT !!!
	Sort(0);
}


// Segmentation

// Divides the data contained in voice 0 into n segments of equal size
// according to total duration of the original voice.
// Each  new segment is placed in a different voice of output material (m).
// Notes are clipped to acommodate correct segment durations.
MuMaterial MuMaterial::Segments(int n)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuError err(MuERROR_NONE);
	MuMaterial m;
    MuVoice v;
    float beg, end;
	long numNotes;
	int i, j;
    
    if(voices)
    {
        // get duration for voice 0
		float totalDur = voices[0].Dur();
		// calculate time duration for each segment
        float segmentLength = (totalDur / n);
        // create a new voice for every segment in material 'm'
		m.AddVoices(n);
        
        for(i = 0; i < n; i++)
        {
            // calculate segment bounds
            beg = i * segmentLength;
            end = beg + segmentLength;
            // get the desired segment
            v = voices[0].Extract(beg, end);
            v.Move(0);
			// copy segment content to a voice in output material
			MuNote note;
			numNotes = v.NumberOfNotes();
			for(j = 0; j < numNotes; j++)
			{
				err = v.GetNote(j,&note);
				if(err.Get() == MuERROR_NONE)
					m.AddNote( i, note );
			}
		}
        // don't forget to copy the tables...
        m.functionTables = functionTables;
    }
    return m;
}

// Divides the data contained in voice 0 into n segments, 
// whose sizes are proportional to the lengths provided in ratios, 
// according to total duration of the original voice.
// Each new segment is placed in a different voice of material object m.
// Notes are clipped to acommodate correct segment durations.
MuMaterial MuMaterial::Segments(int n, float * ratios)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuError err(MuERROR_NONE);
	MuMaterial m;
    MuVoice tempVoice;
    float beg = 0;
    float end = 0;
	long numNotes;
	int i, j;
    float * segmentDur; // array of durations for each segment
    
    if(voices)
    {
        float totalDur = voices[0].Dur();
        float totalRatios = 0.0;
		
		// create a new voice for every segment in material 'm'
		m.AddVoices(n);
		
        segmentDur = new float[n];
        if(segmentDur)
        {
            for( i = 0; i < n; i++)
            {
                totalRatios += ratios[i];
            }
            for( i = 0; i < n; i++)
            {
                segmentDur[i] = (ratios[i] * totalDur)/totalRatios;
                // calculate segment bounds
                beg  = end;
                end = beg + segmentDur[i];
                // get segment
                tempVoice = voices[0].Extract(beg, end);
				// move segment content to beginning of tempVoice
                tempVoice.Move(0);
				// copy segment content to a voice in output material
				MuNote note;
				numNotes = tempVoice.NumberOfNotes();
				for(j = 0; j < numNotes; j++)
				{
					err = tempVoice.GetNote(j,&note);
					if(err.Get() == MuERROR_NONE)
						m.AddNote( i, note );
				}
            }
            // don't forget to copy the tables...
            m.functionTables = functionTables;
        }
    }
    
    return m;
}


// data generating
void MuMaterial::MajorTriad(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MajorTriad( 0, dur );
}

void MuMaterial::MajorTriad(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
    note.SetPitch(60);
	AddNote(voiceNumber, note);
    note.SetPitch(64);
	AddNote(voiceNumber, note);
	note.SetPitch(67);
	AddNote(voiceNumber, note);
}

void MuMaterial::MajorTriadArpeg(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    MajorTriadArpeg( 0, dur );
}


void MuMaterial::MajorTriadArpeg(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    float st = 0.0;
    
    MuNote note;
    note.SetInstr(1);
    note.SetStart(st);
    note.SetDur(dur);
    note.SetAmp(1.0);
    
    note.SetPitch(60);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(64);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(67);
    AddNote(voiceNumber, note);
}


void MuMaterial::MajorTriadSplit( float dur )	// [PUBLIC]
{
    lastError.Set( MuERROR_NONE );
	
	MuNote note;
	note.SetInstr( 1 );
	note.SetStart( 0.0 );
	note.SetDur( dur );
	note.SetAmp( 1.0 );
	
	note.SetPitch( 60 );
	AddNote( 0, note );
	note.SetPitch( 64 );
	AddNote( 1, note );
	note.SetPitch( 67 );
	AddNote( 2, note );
}

void MuMaterial::MinorTriad(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MinorTriad( 0, dur );
}

void MuMaterial::MinorTriad(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	note.SetPitch(60);
	AddNote(voiceNumber, note);
	note.SetPitch(63);
	AddNote(voiceNumber, note);
	note.SetPitch(67);
	AddNote(voiceNumber, note);
}

void MuMaterial::MinorTriadArpeg(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    MinorTriadArpeg( 0, dur );
}

void MuMaterial::MinorTriadArpeg(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    float st = 0.0;
    
    MuNote note;
    note.SetInstr(1);
    note.SetStart(st);
    note.SetDur(dur);
    note.SetAmp(1.0);
    
    note.SetPitch(60);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(63);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(67);
    AddNote(voiceNumber, note);
}

void MuMaterial::MinorTriadSplit(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	note.SetPitch(60);
	AddNote( 0, note);
	note.SetPitch(63);
	AddNote( 1, note);
	note.SetPitch(67);
	AddNote( 2, note);	
}

void MuMaterial::AugTriad(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	MuNote note;
	AugTriad( 0, dur);
}

void MuMaterial::AugTriad(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	note.SetPitch(60);
	AddNote(voiceNumber, note);
	note.SetPitch(64);
	AddNote(voiceNumber, note);
	note.SetPitch(68);
	AddNote(voiceNumber, note);
}

void MuMaterial::AugTriadArpeg(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    AugTriadArpeg( 0, dur );
}

void MuMaterial::AugTriadArpeg(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    float st = 0.0;
    
    MuNote note;
    note.SetInstr(1);
    note.SetStart(st);
    note.SetDur(dur);
    note.SetAmp(1.0);
    
    note.SetPitch(60);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(64);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(68);
    AddNote(voiceNumber, note);
}

void MuMaterial::AugTriadSplit(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	note.SetPitch( 60 );
	AddNote( 0, note );
	note.SetPitch( 64 );
	AddNote( 1, note );
	note.SetPitch( 68 );
	AddNote( 2, note );
	
}

void MuMaterial::DimTriad(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	DimTriad( 0, dur );	
}


void MuMaterial::DimTriad(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	note.SetPitch(60);
	AddNote(voiceNumber, note);
	note.SetPitch(63);
	AddNote(voiceNumber, note);
	note.SetPitch(66);
	AddNote(voiceNumber, note);
}

void MuMaterial::DimTriadArpeg(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    DimTriadArpeg( 0, dur );
}

void MuMaterial::DimTriadArpeg(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    float st = 0.0;
    
    MuNote note;
    note.SetInstr(1);
    note.SetStart(st);
    note.SetDur(dur);
    note.SetAmp(1.0);
    
    note.SetPitch(60);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(63);
    AddNote(voiceNumber, note);
    
    st += dur;
    note.SetStart(st);
    note.SetPitch(66);
    AddNote(voiceNumber, note);
}


void MuMaterial::DimTriadSplit(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuNote note;
	note.SetInstr(1);
	note.SetStart(0.0);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	note.SetPitch(60);
	AddNote( 0, note);
	note.SetPitch(63);
	AddNote( 1, note);
	note.SetPitch(66);
	AddNote( 2, note);
}

void MuMaterial::MajorSeventhChord(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	
	MuNote note;
	note.SetInstr(1);
	note.SetDur(dur);
	note.SetAmp(1.0);
    note.SetStart(0.0);
	
    note.SetPitch(60);
	AddNote(voiceNumber, note);
    note.SetPitch(64);
	AddNote(voiceNumber, note);
    note.SetPitch(67);
	AddNote(voiceNumber, note);
    note.SetPitch(71);
	AddNote(voiceNumber, note);
}

void MuMaterial::MajorScale(float dur, bool addOctave)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MajorScale( 0, dur, addOctave);
}

void MuMaterial::MajorScale(int voiceNumber, float dur, bool addOctave)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	short scalePattern[8] = {0, 2, 4, 5, 7, 9, 11, 12};
	int i,n;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
    // if the user wants the first dergree repeated
    // at the end of the scale...
    if (addOctave)
        n = 8;  // we simply let the loop run to the end of the array...
    else
        n = 7;  // otherwise we stop one turn short
    
	for(i = 0; i < n; i++)
	{
		note.SetPitch(60 + scalePattern[i]);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}
}

void MuMaterial::MinorScale(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MinorScale( 0, dur );	
}

void MuMaterial::MinorScale(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	short scalePattern[7] = {0, 2, 3, 5, 7, 8, 10};
	int i;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	for(i = 0; i < 7; i++)
	{
		note.SetPitch(60 + scalePattern[i]);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}	
}

void MuMaterial::HarmonicMinorScale(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	HarmonicMinorScale( 0 , dur );
}

void MuMaterial::HarmonicMinorScale(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	short scalePattern[7] = {0, 2, 3, 5, 7, 8, 11};
	int i;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	for(i = 0; i < 7; i++)
	{
		note.SetPitch(60 + scalePattern[i]);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}	
}

void MuMaterial::MelodicMinorScale(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MelodicMinorScale( 0 , dur);
}

void MuMaterial::MelodicMinorScale(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	short scalePattern[7] = {0, 2, 3, 5, 7, 9, 11};
	int i;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	for(i = 0; i < 7; i++)
	{
		note.SetPitch(60 + scalePattern[i]);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}
}

void MuMaterial::PentatonicScale(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	PentatonicScale( 0 , dur );
}

void MuMaterial::PentatonicScale(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	short scalePattern[5] = {0, 2, 4, 7, 9};
	int i;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	for(i = 0; i < 5; i++)
	{
		note.SetPitch(60 + scalePattern[i]);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}	
}

void MuMaterial::WholeToneScale(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	WholeToneScale( 0, dur );
}

void MuMaterial::WholeToneScale(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	short scalePattern[6] = {0, 2, 4, 6, 8, 10};
	int i;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	for(i = 0; i < 6; i++)
	{
		note.SetPitch(60 + scalePattern[i]);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}
}

void MuMaterial::ChromaticScale(float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	ChromaticScale(0, dur);
}

void MuMaterial::ChromaticScale(int voiceNumber, float dur)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	int i;
	float nextStart = 0;
	
	MuNote note;
	note.SetInstr(1);
	note.SetStart(nextStart);
	note.SetDur(dur);
	note.SetAmp(1.0);
	
	for(i = 0; i < 12; i++)
	{
		note.SetPitch(60 + i);
		AddNote(voiceNumber, note);
		nextStart += dur;
		note.SetStart(nextStart);
	}
}

// DYNAMICS

void MuMaterial::SetAmp(int voiceNumber, float amp)
{
    lastError.Set(MuERROR_NONE);
    MuNote note;
    
    long n = NumberOfNotes(voiceNumber);
    long i;
    
    
    for(i = 0; i < n; i++)
    {
        note = GetNote(voiceNumber, i);
        note.SetAmp(amp);
        SetNote(voiceNumber, i, note);
    }
}

void MuMaterial::Crescendo(int voiceNumber, float max)
{
    lastError.Set(MuERROR_NONE);
    MuNote note;
    float totalDur = 0;
    float increment = 0;
    long n = NumberOfNotes(voiceNumber);
    long i;
    
    note = GetNote(voiceNumber, (n - 1));
    totalDur = note.Start() + note.Dur();
    increment = max/totalDur;
    
    for(i = 0; i < n; i++)
    {
        note = GetNote(voiceNumber, i);
        note.SetAmp(increment * note.Start());
        SetNote(voiceNumber, i, note);
    }
}

void MuMaterial::Decrescendo(int voiceNumber, float max)
{
    lastError.Set(MuERROR_NONE);
    MuNote note;
    float totalDur = 0;
    float increment = 0;
    long n = NumberOfNotes(voiceNumber);
    long i;
    
    note = GetNote(voiceNumber, (n - 1));
    totalDur = note.Start() + note.Dur();
    increment = max/totalDur;
    
    for(i = 0; i < n; i++)
    {
        note = GetNote(voiceNumber, i);
        note.SetAmp(max - (increment * note.Start()));
        SetNote(voiceNumber, i, note);
    }
}

void MuMaterial::QuantizeMelodyFor(float tempo)
{
    const int SIXTEENTH = 3;
    const int TRIPLET = 4;
    const int EIGHTH = 6;
    const int TWO_TRIPLETS = 8;
    const int DOTTED_EIGHTH = 9;
    const int ONE_BEAT = 12;
    
    MuNote note;
    MuParamBlock durations, tempValues;
    float noteLength;
    int wholePart, currTime;
    int i,j,k,nVoices,nDurs;
    long nNotes;
    bool binDiv,tripDiv;
    float originalBeat = 60.0 / tempo;
    float scale = ONE_BEAT / originalBeat;
    
    Scale(scale);
   
    durations.Init(6);
    durations[0] = SIXTEENTH;
    durations[1] = TRIPLET;
    durations[2] = EIGHTH;
    durations[3] = TWO_TRIPLETS;
    durations[4] = DOTTED_EIGHTH;
    durations[5] = ONE_BEAT;
    durations.Show();
    
    nDurs = durations.Num();
    tempValues.Init(nDurs);
    noteLength = currTime = wholePart = 0;
    binDiv = tripDiv = false;
    
    nVoices = NumberOfVoices();
    
    for(i = 0; i < nVoices; i++)
    {
        // see how many notes there are in this voice...
        nNotes = NumberOfNotes(i);
        
        // remember where the first note starts...
        note = GetNote(i,0);
        currTime = note.Start();
        
        for(j = 0; j < nNotes; j++)
        {
            note = GetNote(i,j);
            noteLength = note.Dur();
            cout << "[noteLength]: " << noteLength << endl;
            // 0) Calculate how many full beats fit inside current duration...
            int fullBeats = noteLength / ONE_BEAT;
            
            // and remove those beats, ...
            wholePart = (fullBeats * ONE_BEAT);
            noteLength -= wholePart;
            
            if(noteLength > 2)
            {
                switch (currTime % ONE_BEAT)
                {
                    // BEAT START
                    case 0:
                    {
                        binDiv = true;
                        tripDiv = true;
                        break;
                    }
                        
                    // DIVISAO BINARIA
                    case 3:
                    case 6:
                    case 9:
                    {
                        binDiv = true;
                        tripDiv = false;
                        break;
                    }
                    
                    // DIVISAO TERNARIA
                    case 4:
                    case 8:
                    {
                        binDiv = false;
                        tripDiv = true;
                        break;
                    }
                }

                
                // then we try to quantize the remainder...
                for(k = 0; k < nDurs; k++)
                {
                    // 1) Get the ratio between noteLength and each reference durations...
                    tempValues[k] = noteLength / durations[k];
                    
                    // 2) The value '1.0' would mean we found a hit. If we can't
                    // find a perfect match, the closest one should be it.
                    // So we subtract each candidate from 1.0 to see how close they
                    // are to the perfect match.
                    tempValues[k] -= 1.0;
                    
                    // 3) We want the smallest value, which is the closest to 1.0;
                    // negative values simply mean they off in the other diretion,
                    // so we get the absolute value (without the negative sign) in
                    // order to compare them all...
                    if(tempValues[k] < 0)
                        tempValues[k] *= -1;
                }
                
                // now we pick the smallest value...
                int index = 0;
                float value = ONE_BEAT + 1;
                
                for(k = 0; k < nDurs; k++)
                {
                   if( (((k % 2) == 0) || (k == 5)) && binDiv)
                   {
                        if(tempValues[k] < value)
                        {
                            value = tempValues[k];
                            index = k;
                        }
                   }
                    
                    if( ((k % 2) == 1) && tripDiv)
                    {
                        if(tempValues[k] < value)
                        {
                            value = tempValues[k];
                            index = k;
                        }
                    }
                }
                
                // 'index' points to the element in the 'durations' array
                // which has the smallest ratio diference when compared to
                // the tested note duration, so we use it to access the
                // correponding quantized duration, to which
                // we add the whole beats we removed in the beginning...
                noteLength = durations[index] + wholePart;

            }
            else
                noteLength = wholePart;
            
            note.SetStart(currTime);
            note.SetDur(noteLength);
            // and reinsert it...
            SetNote(i, j, note);
            currTime += noteLength;
        }
    }
}

void MuMaterial::RemoveBlankNotes(int voiceNumber)
{
    lastError.Set(MuERROR_NONE);
    
    if(voiceNumber < NumberOfVoices())
    {
        voices[voiceNumber].RemoveBlankNotes();
    }
}

// File IO
// reads a Csound score (.sco) into material object
void MuMaterial::LoadScore(string fileName, short mode)	// [PUBLIC]
{
	lastError.Set(MuERROR_NONE);
	int instrNumber = 0;
	int voiceNumber = 0;
	char inputLine[256];
	char tempLine[32];
	int i, j;
	MuNote theNote;
	
	// get rid of any previous data in this material
	Clear();
	
	// open input file...
	ifstream score_in(fileName.c_str());
	if(score_in)
	{
		while(!score_in.eof())
		{
			// retrieve each line from input file
			score_in.getline(inputLine, 256);
			
			// check begining of each line for valid opcodes
			switch(inputLine[0])
			{
				// if function table found, store it..
				case 'f':
					functionTables.append(inputLine);
					functionTables.append("\n");
					break;
				
				// if note line found, ...
				case 'i':
					// instrument number
					i = 1;
					j = 0;
					// copy parameter string
					while((inputLine[i]) && (inputLine[i] != ' ' && inputLine[i] != '\t'))
						tempLine[j++] = inputLine[i++];
					tempLine[j] = '\0';					
					instrNumber = atoi(tempLine);
					voiceNumber = GetVoiceNumberForInstrument(instrNumber);
					if(voiceNumber < 0)	// there is no voice with this instrument number...
					{
						// So we create one.
						AddVoices(1);
						// The new voice goes in the end of this material,
						// so we set the last voice's instrument number to 'instrNumber',...
						voiceNumber = (numOfVoices - 1);
						SetInstrument( voiceNumber, instrNumber);
						// Then we insert current note into the newly created voice...
						theNote = CreateNoteFromCsoundLine(inputLine);
                        
                        if(mode == LOAD_MODE_TIME)
                            AddNote(voiceNumber, theNote );
                        if(mode == LOAD_MODE_DIRECT)
                            IncludeNote(voiceNumber, theNote);
                        
						if(lastError.Get() != MuERROR_NONE)
							return;
					}
					else
					{
						theNote = CreateNoteFromCsoundLine(inputLine);
                        if(mode == LOAD_MODE_TIME)
                            AddNote(voiceNumber, theNote );
                        if(mode == LOAD_MODE_DIRECT)
                            IncludeNote(voiceNumber, theNote);
					}
					break;
			}
		}
	}
	else
	{
		lastError.Set(MuERROR_COULDNT_OPEN_INPUT_FILE);
	}
}


// Generates Orchestra Definitions...
string MuMaterial::Orchestra(void)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    MuError err(MuERROR_NONE);
    stringstream orc;

    if(orchestra == "")
    {
        orc << "; ====================" << endl;
        orc << "sr = 44100" << endl;
        orc << "kr = 441" << endl;
        orc << "ksmps = 100" << endl;
        orc << "nchnls = 2" << endl;
        orc << "; ====================" << endl;
        
        orc << endl;
        
        orc << "; =======================================" << endl;
        orc << "instr 1 ; (short atack, long release)" << endl;
        orc << "; =======================================" << endl;
        orc << "iamp = p5 * 25000" << endl;
        orc << "ifreq = cpspch(p4)" << endl;
        orc << "itable = 1" << endl;
        orc << endl;
        orc << "kamp linen 1.0, 0.05, p3, (0.9 * p3)" << endl;
        orc << "asig oscil iamp, ifreq, itable" << endl;
        orc << "aout = kamp * asig" << endl;
        orc << "outs aout, aout" << endl;
        orc << "; =======================================" << endl;
        orc << "endin" << endl;
        orc << "; =======================================" << endl;
        
        orc << "; =======================================" << endl;
        orc << "instr 2 ; (medium atack, short release)" << endl;
        orc << "; =======================================" << endl;
        orc << "iamp = p5 * 25000" << endl;
        orc << "ifreq = cpspch(p4)" << endl;
        orc << "itable = 2" << endl;
        orc << endl;
        orc << "kamp linen 1.0, 0.1, p3, (0.1 * p3)" << endl;
        orc << "asig oscil iamp, ifreq, itable" << endl;
        orc << "aout = kamp * asig" << endl;
        orc << "outs aout, aout" << endl;
        orc << "; =======================================" << endl;
        orc << "endin" << endl;
        orc << "; =======================================" << endl;
        
        orc << "; =======================================" << endl;
        orc << "instr 3 ; (long atack, long release)" << endl;
        orc << "; =======================================" << endl;
        orc << "iamp = p5 * 25000" << endl;
        orc << "ifreq = cpspch(p4)" << endl;
        orc << "itable = 3" << endl;
        orc << endl;
        orc << "kamp linen 1.0, (0.4 * p3), p3, (0.3 * p3)" << endl;
        orc << "asig oscil iamp, ifreq, itable" << endl;
        orc << "aout = kamp * asig" << endl;
        orc << "outs aout, aout" << endl;
        orc << "; =======================================" << endl;
        orc << "endin" << endl;
        orc << "; =======================================" << endl;
        
        orc << "; =======================================" << endl;
        orc << "instr 4 ; (short atack, medium release)" << endl;
        orc << "; =======================================" << endl;
        orc << "iamp = p5 * 25000" << endl;
        orc << "ifreq = cpspch(p4)" << endl;
        orc << "itable = 4" << endl;
        orc << endl;
        orc << "kamp linen 1.0, 0.03, p3, (0.4 * p3)" << endl;
        orc << "asig oscil iamp, ifreq, itable" << endl;
        orc << "aout = kamp * asig" << endl;
        orc << "outs aout, aout" << endl;
        orc << "; =======================================" << endl;
        orc << "endin" << endl;
        orc << "; =======================================" << endl;
        
        orchestra = orc.str();
    }
    
    return orchestra;
}

// Loads an orchestra from orchestra file (.orc)
void MuMaterial::LoadOrchestra(string fileName)
{
    char inputLine[256];
     stringstream orc;
    lastError.Set(MuERROR_NONE);
    
    ifstream orchestra_in(fileName.c_str());
    if(orchestra_in)
    {
        while(!orchestra_in.eof())
        {
            // retrieve each line from input file
            orchestra_in.getline(inputLine, 256);
            orc << inputLine << endl;
        }
        orchestra = orc.str();
        
        // DEBUG:
        //cout << "[ORCHESTRA FILE LOADED]:" << endl << endl;
        //cout << orchestra << endl << endl;
    }
    else
    {
        lastError.Set(MuERROR_COULDNT_OPEN_INPUT_FILE);
    }
}

// Writes orchestra file
void MuMaterial::Orchestra(string fileName)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
    MuError err(MuERROR_NONE);
    fileName.append(".orc");
    ofstream orc(fileName.c_str(), ios_base::out | ios_base::trunc);
    
    if(orc)
    {
        orc << Orchestra();
        orc.close();
    }
    else
        lastError.Set(MuERROR_COULDNT_OPEN_OUTPUT_FILE);
}


string MuMaterial::Score(void)
{
    lastError.Set(MuERROR_NONE);
    MuError err(MuERROR_NONE);
    stringstream score;
    string ftables = FunctionTables();
    
    if(ftables == "")
    {
        SetDefaultFunctionTables();
        ftables = FunctionTables();
    }
    
    // Write Function Tables...
    score << "; ========================================" << endl;
    score << "; Function Tables:" << endl;
    score << "; ========================================" << endl;
    score <<  ftables << endl;
    score << "; ========================================" << endl << endl;
    
    
    if( (voices != NULL) && (numOfVoices > 0) )
    {
        for(int i = 0; i < numOfVoices; i++)
        {
            MuNote theNote;
            long num = voices[i].NumberOfNotes();
            
            score << "; ========================================" << endl;
            score << "; VOICE: " << i << ", Instr.: " << voices[i].InstrumentNumber() << endl;
            score << "; ========================================" << endl;
            score << endl;
            
            for(long j = 0; j < num; j++)
            {
                err = voices[i].GetNote( j, &theNote );
                if(err.Get() != MuERROR_NONE)
                {
                    lastError.Set(err);
                    break;
                }
                else
                    score << theNote.CsString() << endl;
            }
            
            // if there was an error in the inner loop, bail the outer loop also:
            if(lastError.Get() != MuERROR_NONE)
                break;
            
            score << endl;
            score << "; ========================================" << endl;
            score << "; END VOICE " << i << endl;
            score << "; ========================================" << endl;
            score << endl;
        }
    }
    else
        lastError.Set(MuERROR_MATERIAL_IS_EMPTY);
    
    return score.str();
}

void MuMaterial::Score(string fileName)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
	MuError err(MuERROR_NONE);
    fileName.append(".sco");
    
	ofstream score(fileName.c_str(), ios_base::out | ios_base::trunc);
    if(score)
    {
        string scoString = Score();
        if(lastError.Get() == MuERROR_NONE)
            score << scoString;
        score.close();
    }
    else
        lastError.Set(MuERROR_COULDNT_OPEN_OUTPUT_FILE);
}

void MuMaterial::SetCsOptions(string options)
{
    csOptions = options;
}

string MuMaterial::Csd(void)
{
    stringstream csd;
    
    csd << "<CsoundSynthesizer>" << endl;
    
    csd << "<CsOptions>" << endl;
    csd << csOptions << endl;
    csd << "</CsOptions>" << endl;
    
    csd << "<CsInstruments>" << endl;
    csd << Orchestra();
    csd << "</CsInstruments>" << endl;
    
    csd << "<CsScore>" << endl;
    csd << Score();
    csd << "</CsScore>" << endl;
    
    csd << "</CsoundSynthesizer>" << endl;
    
    return csd.str();
}


void MuMaterial::Csd(string fileName)
{
    lastError.Set(MuERROR_NONE);
    MuError err(MuERROR_NONE);
    fileName.append(".csd");
    
    string tbls = FunctionTables();
    
    // cout << endl << "Creating Csound File: "<< fileName << endl << endl;
    
    ofstream csd(fileName.c_str(), ios_base::out | ios_base::trunc);
    if(csd)
    {
        csd << Csd();
        csd.close();
    }
    else
        lastError.Set(MuERROR_COULDNT_OPEN_OUTPUT_FILE);
}


void MuMaterial::PlaybackWithCsound(string fileName)
{
    // Start by creating the csd file...
    Csd(fileName);
    
    // If file creation succeeds, proceed to call Csound...
    if(lastError.Get() == MuERROR_NONE)
    {
        cout << endl << "Playing with Csound..." << endl << endl;
        string command;
        command = CSOUND_PATH;
        command += " ";
        command += csOptions;
        command += " ";
        command += fileName;
        command += ".csd ";
        system(command.c_str());
    }
}

// Utilities
void MuMaterial::Sort( short field )
{
	lastError.Set(MuERROR_NONE);
	
	int n = numOfVoices;
	int i;
	for(i = 0; i < n; i++)
	{
		Sort( i, field );
	}	
}

void MuMaterial::Sort( int voiceNumber, short field )	// [PUBLIC]
{
    MuError err( MuERROR_NONE );
	if( voices != NULL )
    {
        if( ( voiceNumber < numOfVoices ) && ( voiceNumber >= 0 ) )
		{
            if( field == SORT_FIELD_START )
				voices[voiceNumber].Sort();
			else
				voices[voiceNumber].SortBy( field );
		}
        else
            lastError.Set( MuERROR_INVALID_VOICE_NUMBER );
    }
    else
        lastError.Set( MuERROR_MATERIAL_IS_EMPTY );
}

void MuMaterial::Clear(void)
{
	MuError err(MuERROR_NONE);
	if(voices != NULL)
	{	
		delete [] voices;
		voices = NULL;
		numOfVoices = 0;
	}
	functionTables = "";
}

void MuMaterial::Show( void )
{
    string score = Score();
    
    cout << endl;
    
    if(lastError.Get() == MuERROR_NONE)
        cout << score;
    else
        cout << lastError.Message();
    
    cout << endl;
}

short MuMaterial::CsoundToLocalPitch(cs_pitch inPitch)	// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	return (((inPitch.octave -3) * 12) + inPitch.pitch);
}

string MuMaterial::FunctionTables(void)	// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	return functionTables;
}

void MuMaterial::SetDefaultFunctionTables(void)	// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	functionTables  = "f1 0 4096 10 1 .9 .1 .8 .2 .7 .3 .6 .4 .5\n";
	functionTables += "f2 0 4096 10 1 0 1 0 1 0 1 0 1\n";
	functionTables += "f3 0 4096 10 .1 .3 .5 .7 .5 .3 .1\n";
	functionTables += "f4 0 4096 10 .8 .6 .4 .2 .4 .6 .8\n";
}

void MuMaterial::SetFunctionTables(string inTables)	// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	functionTables = inTables;
}

void MuMaterial::AddFunctionTables(string inTables)
{
	MuError err(MuERROR_NONE);
	functionTables += "\n";
	functionTables += inTables;
}

void MuMaterial::LoadFunctionTables(string fileName)	// [PUBLIC]
{
    char inputLine[256];
    stringstream tables;
    lastError.Set(MuERROR_NONE);
    
    ifstream tables_in(fileName.c_str());
    if(tables_in)
    {
        while(!tables_in.eof())
        {
            // retrieve each line from input file
            tables_in.getline(inputLine, 256);
            tables << inputLine << endl;
        }
        SetFunctionTables(tables.str());
        // DEBUG:
        //cout << "[FUNCTION TABLES LOADED]:" << endl << endl;
        //cout << FunctionTables() << endl << endl;
    }
    else
    {
        lastError.Set(MuERROR_COULDNT_OPEN_INPUT_FILE);
    }
}

MuNote MuMaterial::CreateNoteFromCsoundLine(char * inLine)	// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	MuNote theNote;
    float theValue = 0.0;
    cs_pitch thePitch;
    MuParamBlock parameters;
    char tempLine[32];
    int i, j;
	
    
    // if input line is valid...
    if(inLine != NULL)
    {
        // Fill note with data...
        
        // instrument number
		i = 1;
        j = 0;
		// copy parameter string
        while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t'))
            tempLine[j++] = inLine[i++];
		tempLine[j] = '\0';
        theNote.SetInstr(atoi(tempLine));

        // start time
        j = 0;
        // skip whitespace
        while((inLine[i]) && (inLine[i] == ' ' || inLine[i] == '\t'))
            i++;
        // copy parameter string
        while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t'))
            tempLine[j++] = inLine[i++];
        // terminate copy
        tempLine[j] = '\0';
        // skip whitespace
        while((inLine[i]) && (inLine[i] == ' ' || inLine[i] == '\t'))
            i++;
        theValue = atof(tempLine);
        theNote.SetStart(theValue);

        // duration
        j = 0;
        // copy parameter string
        while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t'))
            tempLine[j++] = inLine[i++];
        // terminate copy
        tempLine[j] = '\0';
        // skip whitespace
        while((inLine[i]) && (inLine[i] == ' ' || inLine[i] == '\t'))
            i++;
        theValue = atof(tempLine);
        theNote.SetDur(theValue);
        
        // octave
        j = 0;
        // copy parameter string
        while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t') && (inLine[i] != '.'))
            tempLine[j++] = inLine[i++];
        // terminate copy
        tempLine[j] = '\0';
        thePitch.octave = atoi(tempLine);
        // skip the dot
        i++;
        
        // pitch
        j = 0;
        // copy parameter string
        while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t'))
            tempLine[j++] = inLine[i++];
        // terminate copy
        tempLine[j] = '\0';
        thePitch.pitch = atoi(tempLine);
        theNote.SetPitch(CsoundToLocalPitch(thePitch));
        // skip whitespace
        while((inLine[i]) && (inLine[i] == ' ' || inLine[i] == '\t'))
            i++;
        
		// Amplitude
		j = 0;
        // copy parameter string
        while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t'))
            tempLine[j++] = inLine[i++];
        // terminate copy
        tempLine[j] = '\0';
        theValue = atof(tempLine);
        theNote.SetAmp(theValue);
        // skip whitespace
        while((inLine[i]) && (inLine[i] == ' ' || inLine[i] == '\t'))
            i++;		
		
        // Parameters
        while(inLine[i])
        {
            j = 0;
            // copy parameter string
            while((inLine[i]) && (inLine[i] != ' ' && inLine[i] != '\t'))
                tempLine[j++] = inLine[i++];
            // terminate copy
            tempLine[j] = '\0';
            // skip whitespace
            while((inLine[i]) && (inLine[i] == ' ' || inLine[i] == '\t'))
                i++;
            theValue = atof(tempLine);
            parameters.AddParam(theValue);
        }
        theNote.SetParams(parameters);
    }
    
    return theNote;
}

// Error managing

// In order to make writing with MuM simpler and easier, none of the
// APIs in this library return errors directly. They either
// return requested objects/data or void. However, when implementing
// code which uses MuM, one may check, at any time, the error
// value of the last operation performed by an object of the
// MuMaterial class. It is also possible (though not common)
// to reset the error status to 0 or any other error code if so desired.

MuError MuMaterial::LastError(void)	// [PUBLIC]
{
    MuError err(MuERROR_NONE);
	return lastError;
}

void MuMaterial::ClearError(void)	// [PUBLIC]
{
    lastError.Set(MuERROR_NONE);
}
