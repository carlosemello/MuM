//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuNote.cpp
 *
 * @brief Note Class Implementation
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 **/

#include "MuNote.h"

MuNote::MuNote(void)
{
	instr = 0;
	start = 0.0;
	dur = 0.0;
	pitch = 0;
	amp = 0.0;
	next = NULL;
}

MuNote * MuNote::Next(void)
{
    return next;
}

// Copy Constructor
MuNote::MuNote( const MuNote & inNote )
{
	instr = inNote.instr;
	start = inNote.start;
	dur = inNote.dur;
	pitch = inNote.pitch;
	amp = inNote.amp;
	param = inNote.param;
}


// Assignment...
MuNote & MuNote::operator=( const MuNote & inNote )
{
	instr = inNote.instr;
	start = inNote.start;
	dur = inNote.dur;
	pitch = inNote.pitch;
	amp = inNote.amp;
	param = inNote.param;
	
	return *this;
}

// Equality test...
bool MuNote::operator==( const MuNote & inNote )
{
	bool result = false;
	if(	
		( instr == inNote.instr ) &&
		( start == inNote.start )  &&
		( dur == inNote.dur )  &&
		( pitch == inNote.pitch )  &&
		( amp == inNote.amp ) &&
	    ( param == inNote.param )
	   )
		result = true;
	
	return result;
}

// Inequality test...
bool MuNote::operator!=( const MuNote & inNote )
{
	return !( *this == inNote );
}

void MuNote::SetNext(MuNote * nextNote)
{
    next = nextNote;
}

void MuNote::Clear(void)
{
	instr = 0;
	start = 0.0;
	dur = 0.0;
	pitch = 0;
	next = NULL;
}

uShort MuNote::Instr(void)
{
	return instr;
}

void MuNote::SetInstr(uShort newInstr)
{
	instr = newInstr;
}

float MuNote::Start(void)
{
	return start;
}

void MuNote::SetStart(float newStart)
{
	start = newStart;
}

float MuNote::Dur(void)
{
	return dur;
}

void MuNote::SetDur(float newDur)
{
    dur = newDur;
}

float MuNote::End(void)
{
    return (start + dur);
}

short MuNote::Pitch(void)
{
	return pitch;
}

void MuNote::SetPitch(short newPitch)
{
	pitch = newPitch;
    if (pitch < 0)
        pitch = 0;
}

void MuNote::SetPitch(cs_pitch newPitch)
{
	pitch = newPitch.pitch + ( ( newPitch.octave - 3 ) * 12 );
    if(pitch < 0)
        pitch = 0;
}

void MuNote::ColapsePitch(void)
{
	pitch = ( pitch % 12 ) + MIDDLE_C;
}

float MuNote::Amp(void)
{
	return amp;
}

void MuNote::SetAmp(float newAmp)
{
    amp = newAmp;
}


MuParamBlock MuNote::Params(void)
{
	return param;
}

void MuNote::SetParams(MuParamBlock inParams)
{
	param = inParams;
}

cs_pitch MuNote::CsPitch(void)
{
	cs_pitch csp;
    if(pitch == 0)
    {
        csp.octave = 0;
        csp.pitch = 0;
    }
    else
    {
        csp.octave = ((pitch / 12) + OCTAVE_OFFSET);
        csp.pitch = ((pitch % 12));
    }
    
    return csp;
}

string MuNote::PitchString(void)
{
	string pitchString;
    cs_pitch p;
	char buff[10];
	
    p = CsPitch();
    sprintf(buff, "%d.", p.octave);
    pitchString += buff;
    if(p.pitch < 10)
        pitchString += "0";
    sprintf(buff, "%d", p.pitch);
    pitchString += buff;
    return pitchString;
}

string MuNote::CsString(void)
{
	string cs_string;
	char buff[10];
	MuParamBlock pBlock;
	float paramVal = 0;
	short numberOfParams = 0;
	MuError err(MuERROR_NONE);
	
	cs_string += "i";
	sprintf(buff,"%d",Instr());
	cs_string += buff;
	cs_string += "\t";
	sprintf(buff,"%.3f",Start());
	cs_string += buff;
	cs_string += "\t";
	sprintf(buff,"%.3f",Dur());
	cs_string += buff;
	cs_string += "\t";
	cs_string +=  PitchString();
	cs_string += "\t\t";
	sprintf(buff,"%.3f",Amp());
	cs_string += buff;
	cs_string += "\t";
	
	pBlock = Params();
	numberOfParams = pBlock.Num();
	if( numberOfParams != 0)
	{
		for(short k = 0; k < numberOfParams; k++)
		{
			err = pBlock.Val(k, &paramVal);
			if(err.Get() != MuERROR_NONE)
				cout << err.Message();
			else
			{
				sprintf(buff,"%.3f",paramVal);
				cs_string += buff;
				cs_string += "\t";
			}
		}
	}
	
	return cs_string;
}

string MuNote::PitchName(int languageChoice, int accidentals)
{
    string name;
    string englishSharps[12]    = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    string englishFlats[12]     = {"C","Db","D","Eb","E","F","Gb","G","Ab","A","Bb","B"};
    string portSharps[12]       = {"DO","DO#","RE","RE#","MI","FA","FA#","SOL","SOL#","LA","LA#","B"};
    string portFlats[12]        = {"DO","REb","RE","MIb","MI","FA","SOLb","SOL","LAb","LA","SIb","SI"};
    
    // zero pitch indicates a rest
    if(this->Pitch() == 0)
        name = " -- ";
    else
    {
        int semitone = Pitch()%12;
        
        switch (languageChoice)
        {
            case ENGLISH:
            {
                switch (accidentals)
                {
                    case ACC_FAVOR_FLATS:
                        name = englishFlats[semitone];
                        break;
                        
                    case ACC_FAVOR_SHARPS:
                        name = englishSharps[semitone];
                        break;
                }
                break;
            }
                
                
            case PORTUGUESE:
            {
                switch (accidentals)
                {
                    case ACC_FAVOR_FLATS:
                        name = portFlats[semitone];
                        break;
                        
                    case ACC_FAVOR_SHARPS:
                        name = portSharps[semitone];
                        break;
                }
                break;
            }
        }
    }
    
    return name;
}

MuMIDIMessage MuNote::MIDIOn(void)
{
	MuMIDIMessage noteOn;
	noteOn.status = 0x90 + ( instr - 1 );
	noteOn.data1 = pitch;
	noteOn.data2 = amp * 127;
	noteOn.time = start;
	return noteOn;
}

MuMIDIMessage MuNote::MIDIOff(void)
{
	MuMIDIMessage noteOff;
	noteOff.status = 0x80 + ( instr - 1 );
	noteOff.data1 = pitch;
	noteOff.data2 = 0;
	noteOff.time = ( start + dur );
	return noteOff;
}
