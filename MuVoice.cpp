//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuVoice.cpp
 *
 * @brief Voice Class Implementation
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 **/

#include "MuVoice.h"

MuVoice::MuVoice(void)
{
    noteList = NULL;
    numOfNotes = 0;
    instrumentNumber = 0;
    numOfParameters = 0;
}

// copy constructor
MuVoice::MuVoice(const MuVoice & inVoice)
{
    MuNote * temp = NULL;
    noteList = NULL;
    numOfNotes = 0;
    instrumentNumber = 0;
    numOfParameters = 0;
    
    if(inVoice.noteList != NULL)
    {
        temp = inVoice.noteList;
        while(temp)
        {
            AddNote(*temp);
            temp = temp->Next();
        }
                numOfNotes = inVoice.numOfNotes;
    }
	instrumentNumber = inVoice.instrumentNumber;
	numOfParameters = inVoice.numOfParameters;

}

// destructor
MuVoice::~MuVoice(void)
{
    Clear();
}

// assignment
MuVoice & MuVoice::operator=(const MuVoice & inVoice)
{
    MuNote * temp = NULL;
	// if self assigning,  do nothing...
	if(this == &inVoice)
		return *this;
    // wipe off old data...
    Clear();
	
    if(inVoice.noteList != NULL)
    {
        temp = inVoice.noteList;
        while(temp)
        {
            AddNote(*temp);
            temp = temp->Next();
        }
        
        numOfNotes = inVoice.numOfNotes;
    }
	
	instrumentNumber = inVoice.instrumentNumber;
	numOfParameters = inVoice.numOfParameters;
    
    return *this;
}

// Equality test
bool MuVoice::operator==(const MuVoice & inVoice)
{
    long i, n;
	MuNote * thisNote;
	MuNote * thatNote;
	
	// instrumentCode is not considered for equality...
	
	if( 
		( numOfNotes != inVoice.numOfNotes ) ||
		( instrumentNumber != inVoice.instrumentNumber ) ||
		( numOfParameters != inVoice.numOfParameters )
	   )
		return false;
		
	n = numOfNotes;
	if( ( noteList != NULL ) && ( inVoice.noteList != NULL ) )
	{
		thisNote = noteList;
		thatNote = inVoice.noteList;
		for(i = 0; i < n; i++)
		{
			if( (*thisNote) != (*thatNote) )
				return false;
			thisNote = thisNote->Next();
			thatNote = thatNote->Next();
		}
	}
	
	return true;
}

// Inequality test...
bool MuVoice::operator!=(const MuVoice & inVoice) //const 
{
    return !( *this == inVoice );
}

void MuVoice::Clear(void)
{
    if(noteList)
    {
        MuNote * temp = noteList;
        MuNote * prev = temp;
        while(temp)
        {
            temp = temp->Next();
            delete prev;
            prev = temp;
        }
    }
    numOfNotes = 0;
    instrumentNumber = 0;
    numOfParameters = 0;
    noteList = NULL;
}

long MuVoice::NumberOfNotes(void) const
{
    return numOfNotes;
}

float MuVoice::Start(void)
{
	float voiceStart = 0.0;
	// obs.: returning the start time of the first note in note list
	// this code only works if note list is allways ordered!!
	if(noteList)
		voiceStart = noteList->Start(); 
	return voiceStart;
}

float MuVoice::End(void)
{
    float voiceEnd = 0.0;
	if(noteList)
	{
		if(noteList->Next() == NULL) 
		{
			return noteList->End();
		}
		else // if there is more than one note...
		{
			MuNote * temp;
			temp = noteList;
			
			// get first candidate for voiceEnd...
			voiceEnd = temp->End();
			
			// if this is not the end of the list...
			while(temp->Next())
			{
				// go to next note...
				temp = temp->Next();
				// and compare current voiceEnd to last candidate...
				if(temp->End() > voiceEnd)
					voiceEnd = temp->End();
			}
		}
	}
    
    return voiceEnd;
}

float MuVoice::Dur(void)
{
	return End() - Start();
}

MuError MuVoice::AddNote(MuNote inNote)
{
    MuError err;
	MuNote * prev, * curr;
    MuNote * newNote = new MuNote;
	bool slotFound = false;
	
    if(!newNote)
    {
		err.Set(MuERROR_INSUF_MEM);
    }
    else
    {
        // copy note data to new link
        *newNote = inNote;
		
        // if list is empty...
        if(!noteList) 
        {
            noteList = newNote; // ... add first note
            newNote->SetNext(NULL);	// and terminate list
		}
        else
        {
            curr = noteList;
			
			if( curr->Next() == NULL ) // if there is only one note on the list...
			{
				// check which one comes first...
				// and place it accordingly.
				if( newNote->Start() < curr->Start() )
				{
					noteList = newNote;
					newNote->SetNext(curr);
				}
				else
				{
					curr->SetNext( newNote );
					newNote->SetNext(NULL);
				}
			}
			else // if there is more than one note on the list, ...
			{
				// if the note should go before the first note...
				if( newNote->Start() < curr->Start() )
				{
					// set the list address to the new note and...
					noteList = newNote;
					// make the list continue from there
					newNote->SetNext(curr);
				}
				else // otherwise...
				{
					// Keep searching until you find the...
					// correct place for the new note
					prev = curr;
					
					while(curr)
					{
						// if this is the place
						if(newNote->Start() < curr->Start())
						{
							prev->SetNext(newNote);
							newNote->SetNext(curr);
							slotFound = true;
							break;
						}
						prev = curr;
						curr = curr->Next();
					}
					// if we reached the end of the list and couldn't
					// find a slot for the note, it means it should be
					// the last note
					if(!slotFound)
					{
						prev->SetNext(newNote);
						newNote->SetNext(NULL);
					}
				}
			}
        }
		
		// set the note's instrument choice according to
		// the voice's instrument number, if instrument number is set,
		// and the note has no instrument definition yet.
		// otherwise, leave the note's instrument choice untouched...
		if(( InstrumentNumber() > 0) && (newNote->Instr() == 0))
			newNote->SetInstr(InstrumentNumber());
		
		// increment note count
		// OBS.: (IMPORTANT) this member variable must not be modified
		// anywhere else, except for when removing notes!
        numOfNotes++;
    }
	
	return err;
}

MuError MuVoice::IncludeNote(MuNote inNote)
{
    MuError err;
    MuNote * prev, * curr;
    
    MuNote * newNote = new MuNote;
    
    if(!newNote)
    {
        err.Set(MuERROR_INSUF_MEM);
    }
    else
    {
        // copy note data to new link
        *newNote = inNote;
        
        // if list is empty...
        if(!noteList)
        {
            noteList = newNote; // ... add first note
            newNote->SetNext(NULL);	// and terminate list
        }
        else
        {
            curr = noteList;
            prev = curr;
            
            while (curr)
            {
                prev = curr;
                curr = curr->Next();
            }
            
            // at this point prev points to the last valid link...
            prev->SetNext(newNote);
        }
        
        // set the note's instrument choice according to
        // the voice's instrument number, if instrument number is set,
        // and the note has no instrument definition yet.
        // otherwise, leave the note's instrument choice untouched...
        if(( InstrumentNumber() > 0) && (newNote->Instr() == 0))
            newNote->SetInstr(InstrumentNumber());
        
        // increment note count
        // OBS.: (IMPORTANT) this member variable must not be modified
        // anywhere else, except for when removing notes!
        numOfNotes++;
    }
    
    return err;
}


MuError MuVoice::RemoveNote(long num)
{
    MuError err(MuERROR_NONE);
    // if list is empty...
    if(!noteList) 
    {
        err.Set(MuERROR_NOTE_LIST_IS_EMPTY);
    }
    else // otherwise find the note...
    {
        long count = FIRST_NOTE_INDEX;
        // if there is only one note in the list...
        if(noteList->Next() == NULL) 
        {
            if(num == count)	// if this is the note...
            {
                delete noteList; // Remove it...
                noteList = NULL; // ...reset pointer...
                numOfNotes--;
            }
            else
            {
                err.Set(MuERROR_NOTE_NOT_FOUND);
            }
        }
        else // if there is more than one note...
        {
            MuNote * temp, * prev;
            bool foundIt = false;
            
            // go through the list...
            temp = noteList;
            prev = temp;
            while(temp)
            {
                if(count == num) // if this is the note...
                {
                    foundIt = true;
                    break;
                }
                prev = temp;
                temp = temp->Next();
                count++;
            }
            
            if(foundIt)
            {
                if(count == 0)			// we are deleting the first note
                {
                    noteList = temp->Next();	// point to the rest of the list
                    delete temp;			// Remove current link...
                }
                else
                {
                     prev->SetNext(temp->Next());	// reconect previous link...
                     delete temp;			// Remove current link...
                }
                numOfNotes--;			// update note count

            }
            else
                err.Set(MuERROR_NOTE_NOT_FOUND);
        }
    }
    
    return err;
}

MuError MuVoice::RemoveLastNote(void)
{
    MuError err(MuERROR_NONE);
    // if list is empty...
    if(!noteList) 
    {
        err.Set(MuERROR_NOTE_LIST_IS_EMPTY);
    }
    else // otherwise find last note...
    {
        if(noteList->Next() == NULL) // if there is only one
        {
                delete noteList; // Remove it...
                noteList = NULL; // ...reset pointer...
                numOfNotes--;
        }
        else // if there is more than one note...
        {
            MuNote * temp, * prev;
            
            // go through the list...
            prev = temp = noteList;
            while(temp->Next())
            {
                prev = temp;
                temp = temp->Next();
            }
            
            delete temp;
            prev->SetNext(NULL);
            numOfNotes--;
        }
    }
    
    return err;

}

MuError MuVoice::GetNote(long num, MuNote * outNote) const
{
     MuError err(MuERROR_NONE);
    // if list is empty...
    if(!noteList) 
    {
        err.Set(MuERROR_NOTE_LIST_IS_EMPTY);
    }
    else // otherwise find the note...
    {
        long count = FIRST_NOTE_INDEX;
        // if there is only one note in the list...
        if(noteList->Next() == NULL) 
        {
            if(num == count)	// if this is the note...
            {
                *outNote = *noteList;
            }
            else
            {
                err.Set(MuERROR_NOTE_NOT_FOUND);
            }
        }
        else // if there is more than one note...
        {
            MuNote * temp, * prev;
            bool foundIt = false;
            
            // go through the list...
            temp = noteList;
            while(temp)
            {
                if(count == num) // if this is the note...
                {
                    foundIt = true;
                    break;
                }
                prev = temp;
                temp = temp->Next();
                count++;
            }
            
            if(foundIt)
            {
                *outNote = *temp;
                outNote->SetNext(NULL);
            }
            else
                 err.Set(MuERROR_NOTE_NOT_FOUND);
        }
    }
    
    return err;

}

MuError MuVoice::SetNote(long num, MuNote newNote)
{
    MuError err(MuERROR_NONE);
    // if list is empty...
    if(!noteList) 
    {
        err.Set(MuERROR_NOTE_LIST_IS_EMPTY);
    }
    else // otherwise find the note...
    {
        long count = FIRST_NOTE_INDEX;
        // if there is only one note in the list...
        if(noteList->Next() == NULL) 
        {
            if(num == count)	// if this is the note...
            {
                *noteList = newNote;
                noteList->SetNext(NULL);
            }
            else
            {
                err.Set(MuERROR_NOTE_NOT_FOUND);
            }
        }
        else // if there is more than one note...
        {
            MuNote * temp, * prev;
            bool foundIt = false;
            
            // go through the list...
            temp = noteList;
            while(temp)
            {
                if(count == num) // if this is the note...
                {
                    foundIt = true;
                    break;
                }
                prev = temp;
                temp = temp->Next();
                count++;
            }
            
            if(foundIt)
            {
                newNote.SetNext(temp->Next());
                *temp = newNote;
            }
            else
                 err.Set(MuERROR_NOTE_NOT_FOUND);
        }
    }
    
    return err;
}

// Bubble-sorts notes by start time
void MuVoice::Sort( void ) 
{
    MuNote * previous = NULL;
    MuNote * current = NULL;
    MuNote * next = NULL;
    MuNote * last = NULL;
    
    if(noteList) // if note list is not empty...
    {
        while(last != noteList) // stop when the last note is the first
        {
            // start again
            current = noteList;
            previous = current;
            
            while(current->Next() != last) // walk the list until we reach the last note
            {
                // update next
                next = current->Next();
                // compare notes...
                if(current->Start() > next->Start())
                {
					current->SetNext(next->Next());
					next->SetNext(current);
					if(previous == current)
						noteList = next;
					else
						previous->SetNext(next);

					// since we swapped the notes, we need to update the previous pointer
					previous  = next;
				}
                else // if notes were not swapped...
                {
                    // we move to next note...
                    previous = current;
                    current = next;
                }

            }
            // update last so that every pass through the list...
            //  unsorted section decreases by one note
            last = current; 
        }
    }
}

// Bubble-sorts notes by other fields (dur, instr, pitch, amp)
void MuVoice::SortBy( short field ) 
{
    MuNote * previous = NULL;
    MuNote * current = NULL;
    MuNote * next = NULL;
    MuNote * last = NULL;
	
	if(noteList) // if note list is not empty...
    {
        while(last != noteList) // stop when the last note is the first
        {
            // start again
            current = noteList;
            previous = current;
            
            while(current->Next() != last) // walk the list until we reach the last note
            {
                // update next
                next = current->Next();
				
                // compare notes...
				switch(field)
				{
					case SORT_FIELD_INSTR:
					{
						if(current->Instr() > next->Instr())
						{
							current->SetNext(next->Next());
							next->SetNext(current);
							if(previous == current)
								noteList = next;
							else
								previous->SetNext(next);
							
							// since we swapped the notes, we need to update the previous pointer
							previous  = next;
						}
						else // if notes were not swapped...
						{
							// we move to next note...
							previous = current;
							current = next;
						}
						break; // to jump the switch
					}
						
					case SORT_FIELD_DUR:
					{
						if(current->Dur() > next->Dur())
						{
							current->SetNext(next->Next());
							next->SetNext(current);
							if(previous == current)
								noteList = next;
							else
								previous->SetNext(next);
							
							// since we swapped the notes, we need to update the previous pointer
							previous  = next;
						}
						else // if notes were not swapped...
						{
							// we move to next note...
							previous = current;
							current = next;
						}
						break; // to jump the switch
					}
						
					case SORT_FIELD_PITCH:
					{
						if(current->Pitch() > next->Pitch())
						{
							current->SetNext(next->Next());
							next->SetNext(current);
							if(previous == current)
								noteList = next;
							else
								previous->SetNext(next);
							
							// since we swapped the notes, we need to update the previous pointer
							previous  = next;
						}
						else // if notes were not swapped...
						{
							// we move to next note...
							previous = current;
							current = next;
						}
						break; // to jump the switch
					}
					
					case SORT_FIELD_AMP:
					{
						if(current->Amp() > next->Amp())
						{
							current->SetNext(next->Next());
							next->SetNext(current);
							if(previous == current)
								noteList = next;
							else
								previous->SetNext(next);
							
							// since we swapped the notes, we need to update the previous pointer
							previous  = next;
						}
						else // if notes were not swapped...
						{
							// we move to next note...
							previous = current;
							current = next;
						}
						break; // to jump the switch
					}
				}
                				
            }
            // update last so that every pass through the list...
            //  unsorted section decreases by one note
            last = current; 
        }
    }	
}


MuVoice	MuVoice::Extract(float beg, float end)
{
    MuVoice excerpt;
    MuNote * temp;
    MuNote clipped;
    // go through the list...
    temp = noteList;
    while(temp)
    {
        clipped = *temp;
        // if note is completely within segment bounds...
        if((clipped.Start() >= beg) && (clipped.Start() < end) && (clipped.End() > beg) && (clipped.End() <= end))
            excerpt.AddNote(clipped); // ... just add it
            
        // if note exceeds in the end...
        else if((clipped.Start() >= beg) && (clipped.Start() < end) && (clipped.End() > end))
        {
            clipped.SetDur(end - clipped.Start()); 	// clip note's end...
            excerpt.AddNote(clipped);			// before inserting it
        }
        // if note exceeds at the begining...
        else if((clipped.Start() < beg) && (clipped.End() > beg) && (clipped.End() <= end))
        {
            clipped.SetDur(clipped.Dur() - (beg - clipped.Start()));	// calculate duration and...
            clipped.SetStart(beg);					// clip note's start...
            excerpt.AddNote(clipped);					// before inserting it...
        }
        else if((clipped.Start() < beg) && (clipped.End() > end))
        {
            clipped.SetStart(beg);					// clip both ends...
            clipped.SetDur(end - beg);					// ... then
            excerpt.AddNote(clipped);					// insert note.
        }
        temp = temp->Next();
    }
    return excerpt;
}

uShort	MuVoice::InstrumentNumber(void)
{
    return instrumentNumber;
}

MuError	MuVoice::SetInstrumentNumber(uShort inInstrNum)
{
    MuError err;
	MuNote temp;
	
	if(inInstrNum > 0 && inInstrNum <= 128)
		instrumentNumber = inInstrNum;
	else
		instrumentNumber = 1;
	
	long n = NumberOfNotes();
	long i;
	
	for(i = 0; i < n; i++)
	{
		err = GetNote( i, &temp);
		if(err.Get() == MuERROR_NONE)
		{
			temp.SetInstr( inInstrNum );
			err = SetNote( i, temp );
			if(err.Get() != MuERROR_NONE)
				break;
		}
		else
			break;
	}
	
	return err;
}

MuError MuVoice::Transpose(short interval)
{
    short thePitch = 0;
    int i = 0;
    long n = numOfNotes;
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    for(i = 0; i < n; i++)
    {
        err = GetNote(i, &tempNote);
        if(err.Get() != MuERROR_NONE)
            return err;
        thePitch = tempNote.Pitch();
        tempNote.SetPitch( thePitch + interval);
        err = SetNote(i,tempNote);
        if(err.Get() != MuERROR_NONE)
            return err;
    }
    return err;
}

MuError MuVoice::Transpose(long noteNumber, short interval)
{
    long n = 0;
    short thePitch = 0;
    MuNote tempNote;
    MuError err(MuERROR_NONE);
    
    n = NumberOfNotes();
    if((noteNumber >= 0) && (noteNumber < n))
    {
        err = GetNote(noteNumber, &tempNote);
        thePitch = tempNote.Pitch();
        tempNote.SetPitch( thePitch + interval);
        err = SetNote(noteNumber,tempNote);
    }
    
    return err;
}

MuError MuVoice::Move(float time)
{
    int i = 0;
    long n = 0;
    float timeOffset;
    MuNote tempNote;
    MuError err(MuERROR_NONE);
	
	// negative positions in time are not allowed
	if(time < 0)
		return MuERROR_INVALID_PARAMETER;
    
    n = NumberOfNotes();
	
    // fetch starting point for first note
    err = GetNote(0, &tempNote);
    if(err.Get() != MuERROR_NONE) return err;
	
	// calculate distance between new voice start and first note position
    timeOffset =  time - tempNote.Start();
    
	// now go through note list,...
    for(i = 0; i < n; i++)
    {
		err = GetNote(i, &tempNote);
        if(err.Get() != MuERROR_NONE) 
			return err;
		else
		{
			// add offset to each note start...
			tempNote.SetStart(tempNote.Start() + timeOffset );
			//  and replace note with modified version.
			err = SetNote(i,tempNote);
			if(err.Get() != MuERROR_NONE) return err;
		}
    }
    return err;
}

MuError MuVoice::RemoveBlankNotes(void)
{
    MuError err(MuERROR_NONE);
    MuNote * curr, * previous, * temp;
    curr = previous = noteList;
    bool noteWasDeleted = false;
    
    while(curr)
    {
        if( (curr->Pitch() == 0) || (curr->Amp() == 0) )
        {
            temp = curr;
            previous->SetNext(curr->Next());
            curr = curr->Next();
            delete temp;
            noteWasDeleted = true;
            numOfNotes--;
        }
        
        if (noteWasDeleted == false)
        {
            previous = curr;
            curr = curr->Next();
        }
        
        noteWasDeleted = false;
    }
    
    return err;
}


