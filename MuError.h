//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuError.h
 *
 * @brief Error Class Interface
 *
 * @details
 * MuM's Error information facility
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 **/

#ifndef _MU_ERROR_H_
#define _MU_ERROR_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

//!@brief No Error - task successfully completed.
const short MuERROR_NONE = 0;
//!@brief Insuficient memory to complete task!
const short MuERROR_INSUF_MEM = 1;
//!@brief Parameter number is out of range!!
const short MuERROR_INVALID_PARAMETER = 2;
//!@brief Parameter Block is not initialized!
const short MuERROR_PARAM_BLOCK_NOT_INITIALIZED = 3;
//!@brief Parameter Block contains data!
const short MuERROR_CANNOT_INIT = 4;
//!@brief Invalid size for Parameter Block!
const short MuERROR_INVALID_PARAMBLOCK_SIZE = 5;
//!@brief Note list is empty!
const short MuERROR_NOTE_LIST_IS_EMPTY = 6;
//!@brief Couldn't find requested note!
const short MuERROR_NOTE_NOT_FOUND = 7;
//!@brief Inuficient memory to complete task!
const short MuERROR_COULDNT_INIT_MATERIAL = 8;
//!@brief Material contains no data!
const short MuERROR_MATERIAL_IS_EMPTY = 9;
//!@brief Requested voice doesn't exist!
const short MuERROR_INVALID_VOICE_NUMBER = 10;
//!@brief Couldn't open input file!
const short MuERROR_COULDNT_OPEN_INPUT_FILE = 11;
//!@brief Couldn't open output file!
const short MuERROR_COULDNT_OPEN_OUTPUT_FILE = 12;
//!@brief Invalid scale degree!
const short MuERROR_INVALID_SCALE_DEGREE = 13;
//!@brief Invalid note range!
const short MuERROR_INVALID_NOTE_RANGE = 14;

/**
* @brief Error Class
*
* @details
*
* MuError is the error messaging class in MuM. It is used to pass error codes and access 
* error information from many methods within the library. Every internal MuM method 
* that uses memory allocation anywhere returns an MuError object. Methods which need to 
* verify data ranges also return an MuError. An MuError object is typically initialized with 
* an error constant code (see below) and returned, by value,  by the various methods. Then, 
* from anywhere in the code, this object can provide a textual description of the error 
* through a standard C++ string object. Obs.: User code can/should only access error objects 
* through MuMaterial's error handling facilities (see MuMaterial::LastError() and 
* MuMaterial::ClearError() ). 
**/
class MuError
{
	private:
	
	// Error code
	short e;
	
	public:
	
	// Constructors
	
	/** 
	 * @brief Default Constructor
	 *
	 * @details
	 * This constructor merely sets internal error code to MuERROR_NONE
	 **/
	MuError(void);
	
	/** 
	 * @brief Initializing Constructor
	 *
	 * @details
	 * This constructor sets internal error code to input value
	 * @param errorCode initialization value
	 **/
	MuError(short errorCode);
	
	// Methods
	
	/** 
	 * @brief Returns error value
	 *
	 * @details
	 * This method returns the error object's 
	 * internal error value
	 **/
	short	Get(void);
	
	/** 
	 * @brief Sets error code to input value
	 *
	 * @details
	 * This method sets the error object's internal 
	 * error value to input value
	 * @param errorCode input error value
	 **/
	void	Set(short errorCode);
	
	/** 
	 * @brief Sets error code with input object
	 *
	 * @details
	 * This method sets the error object's internal 
	 * error value to the value contained in input error object
	 * @param inError input error object
	 **/	
	void	Set(MuError inError);
	
	/** 
	 * @brief Returns error description
	 *
	 * @details
	 * This method returns a standard C++ string 
	 * containing a verbal description of the error
	 **/	
	string	Message(void);
	
};

#endif
