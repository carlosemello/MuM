//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuError.cpp
 * @brief Error Class Interface
 * @details
 * MuM's Error information facility
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 **/

#include "MuError.h"

MuError::MuError(void)
{
	e = MuERROR_NONE;
}

MuError::MuError(short newError)
{
	e = newError;
}

short MuError::Get(void)
{
	return e;
}

void MuError::Set(short newError)
{
	e = newError;
}

void MuError::Set(MuError inError)
{
	e = inError.Get();
}

string MuError::Message(void)
{
	string message = "";
	switch(e)
	{
		case MuERROR_INSUF_MEM:
			message = "Insuficient memory to complete task!";
			break;
			
		case MuERROR_INVALID_PARAMETER:
			message = "Parameter number is out of range!";
			break;
			
		case MuERROR_PARAM_BLOCK_NOT_INITIALIZED:
			message = "Parameter Block is not initialized!";
			break;
			
		case MuERROR_CANNOT_INIT:
			message = "Parameter Block contains data!";
			break;
                        
		case MuERROR_INVALID_PARAMBLOCK_SIZE:
			message = "Invalid size for Parameter Block!";
			break;
                        
		case MuERROR_NOTE_LIST_IS_EMPTY:
			message = "Note list is empty!";
			break;
                        
		case MuERROR_NOTE_NOT_FOUND:
			message = "Couldn't find requested note!";
			break;
                
		case MuERROR_COULDNT_INIT_MATERIAL:
			message = "Couldn't allocate array of voices!";
			break;
                
		case MuERROR_MATERIAL_IS_EMPTY:
			message = "Material contains no data!";
			break;
                
		case MuERROR_INVALID_VOICE_NUMBER:
			message = "Requested voice doesnt exist!";
			break;
                        
		case MuERROR_COULDNT_OPEN_INPUT_FILE:
			message = "Couldn't open input file!";
			break;
			
		case MuERROR_COULDNT_OPEN_OUTPUT_FILE:
			message = "Couldn't open output file!";
			break;
			
		case MuERROR_INVALID_SCALE_DEGREE:
			message = "Invalid scale degree!";
			break;
			
		case MuERROR_INVALID_NOTE_RANGE:
			message = "Invalid note range!";
			break;
			
	}
	
	return message;
}
