//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuUtil.h
 *
 * @brief Utility Functions
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 * @details
 * MuM utilities includes functions which are used by MuM classes
 * to perform routine tasks such as initializing arrays ( Set() ), 
 * picking random values within a range ( Between() ), etc.
 **/

#ifndef _MU_UTIL_H_
#define _MU_UTIL_H_

#include "MuError.h"

// CONSTANTS

//!@brief MIDI pitch value for middle C
const short MIDDLE_C = 60;

//!@brief One octave defined as 12 half-steps
const short ONE_OCTAVE = 12;

//!@brief direction for sorting and transposing: ascending order
const short ASCENDING = 1;

//!@brief direction for sorting and transposing: descending order
const short DESCENDING = -1;

//!@brief Language Preference: English
const short ENGLISH = 0;

//!@brief Language Preference: Portuguese
const short PORTUGUESE = 1;

//!@brief acidentals to use for altered notes: mixed
const short ACC_USE_BOTH = 0; // currently not supported

//!@brief acidentals to use for altered notes: flats
const short ACC_FAVOR_FLATS = 1;

//!@brief acidentals to use for altered notes: sharps
const short ACC_FAVOR_SHARPS = 2;

// PROTOTYPES


/** 
 * @brief Various initializations needed by the framework
 *
 * @details
 * This function needs to be called in a program before using any 
 * methods in the MuM framework, so that subsequent calls work
 * properly.
**/
void MuInit(void);


/** 
 * @brief Sets array of 'n' ints to 'value'
 *
 * @details
 * This function sets 'n' elements of 'array' to 'value'; but
 * if n == 0, 'value' is treated as the array size, and 
 * elements are incremented from 0. (i.e.: 0, 1, 2, 3,...)
 * @param array address of int buffer to be set
 * @param n number of elements in array (0 flags increment mode)
 * @param value value to set array to (if n == 0, number of elements)
**/
void Set(int * array, int n, int value);

/** 
 * @brief Random integer between low and high
 *
 * @details
 * This function returns a randomly picked integer 
 * value within given limits (obs.: upper and lower 
 * limits are included in range)
 * @param low lower limit
 * @param high upper limit
 **/
extern int Between( int low, int high );

/** 
 * @brief Random float between low and high
 *
 * @details
 * This function returns a randomly picked float  
 * value within given limits (obs.: upper and lower 
 * limits are included in range)
 * @param min lower limit
 * @param max upper limit
 **/
extern float Between(float min, float max);

/** 
 * @brief Searches short value in array
 *
 * @details
 * This function tries to find a short inside 
 * an array of shorts. If value is found, it returns
 * its index in the array; otherwise it returns -1
 * @param value search key value
 * @param array buffer to be searched
 * @param n number of elements in array
 **/
extern short Inside(short value, short * array, short n );

/** 
 * @brief Mixes (scrambles) an array of ints
 *
 * @details
 * This function mixes the values in 'array'
 * @param array buffer to be scrambled
 * @param size number of elements in array
 **/
extern void MixInts( int * array, int size);

/** 
 * @brief Sorts an array of ints
 *
 * @details
 * This function sorts every element of 'array'
 * in ascending order
 * @param array buffer to be sorted
 * @param size number of elements in array
 **/
extern void SortInts( int * array, int size);

/**
 * @brief Sorts an array of floats (Bubble Sort)
 *
 * @details
 * This function sorts every element of 'array'
 * in ascending order
 * @param array buffer to be sorted
 * @param size number of elements in array
 **/
extern void SortFloats( float * array, int size);

/** 
 * @brief Displays an array of ints (Bubble Sort)
 *
 * @details
 * This function prints the contents of 'array'
 * to standard output for debugging
 * @param array buffer to be displayed
 * @param size number of elements in array
 **/
extern void ShowInts( int * array, int size );

#endif
