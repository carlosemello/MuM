//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuParamBlock.h
 *
 * @brief Parameter Block Class Interface
 *
 * @author Carlos Eduardo Mello
 *
 * @date 3/5/2009
 *
 * @details
 * This class consists of floating point vector which is used by MuM to
 * hold and access extra parameters in a note.These parameters are optional,
 * and usually only used by notes with special argument needs, for Csound output.
 *
 **/

#ifndef _MU_PARAM_BLOCK_H_
#define _MU_PARAM_BLOCK_H_

//#include "MuError.h"
#include "MuUtil.h"
using namespace std;

//!@brief unsigned byte
typedef unsigned char uByte;
//!@brief unsigned 16 bit integer
typedef unsigned short uShort;
//!@brief unsigned 32 bit integer
typedef unsigned long uLong;

/** 
 * @brief Parameter Block Class
 *
 * @details
 *
 * The MuParamBlock class is used to wrap a vector of floating point values, so that these 
 * these values may be assigned, copied, and otherwise manipulated without the programmer 
 * having to deal with memory management. Parameters in an MuParamBlock are used to 
 * provide csound scores with note definitions other than the basic set of parameters 
 * which constitute a note object (see MuNote). Generally, every note inside a music  
 * material has an MuParamBlock whose size depends on the number of parameters required  
 * by its containing voice. A copy of this block may be obtained through the note’s   
 * methods (MuNote.h). The correct number of parameters is provided by the voice’s  
 * methods (MuVoice.h). When building an isolated note, client code tipically declares an 
 * MuParamblock object, modify its values and then hands the entire block over to the note. 
 * MuParamBlocks allow: 
 *
 * <ul>
 * <li>growing array size without previous data loss 
 * <li>reducing array size by cropping the upper elements 
 * <li>accessing values through [ ] operator or accessor functions
 * </ul>
 *
 **/
class MuParamBlock
{
	private:	
        
        float * values;
        uShort numValues;
	
	public:
	
	// Constructors
	
	/** 
	 * @brief Default Constructor
	 *
	 * @details
	 * This constructor zeroes internal variables
	 * 
	 **/	
	MuParamBlock(void);									
	
	/** 
	 * @brief Copy Constructor
	 *
	 * @details
	 * This constructor stores a copy of the parameters found in input block
	 * 
	 * @param 
	 * inBlock MuParamBlock object being copied
	 *
	 **/	
	MuParamBlock(const MuParamBlock & inBlock);
	
	/** 
	 * @brief Destructor
	 *
	 * @details Releases internal memory buffers
	 *
	 **/	
	~MuParamBlock(void);
	
	/** 
	 * @brief Assignment Operator
	 *
	 * @details
	 * Copies parameters from input block
	 * 
	 * @param 
	 * inBlock MuParamBlock object being atributed
	 *
	 * @return 
	 * MuParamBlock& - a reference to the resulting parameter block
	 *
	 **/	
	MuParamBlock & operator=(const MuParamBlock & inBlock);
	
	// Operators
	
	/** 
	 * @brief Access Operator
	 *
	 * @details 
	 * Allows access to individual parameters as elements of array
	 * 
	 * @param 
	 * i uShort (unsigned 16 bit integer) - index (subscript) to desired array element
	 * 
	 * @return 
	 * float - the value found at requested array position
	 *
	 **/
	float& operator[](uShort i);
	
	/** 
	 * @brief Equality Operator
	 *
	 * @details
	 * Compares the parameter block's content to that of input object. Returns true
	 * if every parameter contains the same value in each object.
	 *
	 * @param 
	 * inBlock MuParamBlock object being compared 
	 *
	 * @return 
	 * bool - true if parameters are identical
	 *
	 **/	
	bool operator==(const MuParamBlock & inBlock);
	
	/** 
	 * @brief Inequality Operator
	 *
	 * @details
	 * Compares the parameter block's content to that of input object. Returns true
	 * if any parameter contains a conflicting value
	 *
	 * @param 
	 * inBlock MuParamBlock object being compared 
	 *
	 * @return 
	 * bool - true if any parameter is different
	 *
	 **/	
	bool operator!=(const MuParamBlock & inBlock);
	
	// Methods
	
	/** 
	 * @brief Initializes internal array with n values
	 *
	 * @details
	 * Init() should only be used with an MuParamBlock object which has not been 
	 * initialized. MuParamBlock objects with previous data may be resized by either
	 * a call to the Clear() method followed by Init(), or a call to Grow() or Trunc(). 
	 *
	 * @param 
	 * n uShort - number of array elements to be allocated 
	 *
	 * @return
	 * <ul>
	 * <li> upon success, Init() returns MuERROR_NONE 
	 * <li> if n is zero, Init() returns MuERROR_INVALID_PARAMBLOCK_SIZE; 
	 * <li> if object contains previous data, Init() returns MuERROR_CANNONT_INIT;
	 * </ul>
	 *
	 **/	
	MuError Init(uShort n);
	
	/** 
	 * @brief releases internal array memory.
	 *
	**/		
	void   Clear(void);
	
	/** 
	 * @brief Returns number of array elements
	 *
	 * @details
	 * Informs the number of values in internal array. If the parameter block
	 * is uninitialized, Num() returns zero.
	 *
	 * @return
	 * uShort - number of internal array items
	 *
	 **/		
	uShort Num(void);
	
	/** 
	 * @brief Returns the value at requested array position
	 *
	 * @details
	 * returns the value of the item within position which_param of an MuParamBlock's   
	 * internal array. The value is returned through pointer outVal, which should 
	 * contain the address of a valid floating point variable. Num() should be  
	 * called before iterating through an MuParamBlock's values, in order to obtain 
	 * the correct bounds for the block.
	 *
	 * @return
	 * <ul>
	 * <li> upon success, Val() returns MuERROR_NONE 
	 * <li> if object is not initialized, Val() returns MuERROR_PARAM_BLOCK_NOT_INITIALIZED 
	 * <li> if which_param is out of bounds, Val() returns MuERROR_INVALID_PARAMETER
	 * </ul>
	 * 
	 **/
	MuError Val(uShort which_param, float * theValue);
	
	/** 
	 * @brief Modifies the value of the requested array element
	 *
	 * @details
	 * Replaces the value of the item within position which_param of an MuParamBlock's 
	 * internal array with newValue. Num() should be called beforehand, in order 
	 * to obtain the correct bounds for the ParamBlock. 
	 *
	 * @return
	 * <ul>
	 * <li> upon success, SetVal() returns MuERROR_NONE 
	 * <li> if object is not initialized, SetVal() returns MuERROR_PARAM_BLOCK_NOT_INITIALIZED 
	 * <li> if which_parameter is out of bounds, SetVal() returns MuERROR_INVALID_PARAMETER
	 * </ul>
	 * 
	 **/	
	MuError SetVal(uShort which_param, float newValue);
	
	/** 
	 * @brief Appends newValue to the end of an MuParamBlock's internal array
	 *
	 * @details
	 * Appends newValue to the end of an MuParamBlock's internal array. If object has no 
	 * previous data, AddParam() initializes array with one item and sets its value to newValue. 
	 * In case of memory allocation failure, AddParam() aborts with an error. 
	 *
	 * @return
	 * <ul>
	 * <li> upon success, AddParam() returns MuERROR_NONE 
	 * <li> if memory allocation fails, AddParam() returns MuERROR_INSUF_MEM 
	 * </ul>
	 * 
	 **/		
	MuError AddParam(float newValue);
	
	/** 
	 * @brief Appends n elements to the end of internal array
	 *
	 * @details
	 * If an MuParamBlock object is empty (not initialized), Grow() initializes it  
	 * with n elements. If it contains previous data, Grow() appends n elements to  
	 * the end of the object’s internal array and sets every element to zero. If  
	 * memory allocation fails, Grow() terminates with an error, leaving internal  
	 * array untouched.
	 *
	 * @return
	 * <ul>
	 * <li> upon success, Grow() returns MuERROR_NONE 
	 * <li> if memory allocation fails, Grow() returns MuERROR_INSUF_MEM  
	 * </ul>
	 * 
	 **/			
	MuError Grow(uShort n);
	
	/** 
	 * @brief Crops the end of internal array by n elements
	 *
	 * @details
	 * 
	 * Trunc() eliminates the upper n elements of an MuParamBlock object’s internal array. If n 
	 * is greater then current array size, or if object is not initialized, or yet if memory allocation 
	 * fails, Trunc() terminates with an error, leaving internal array untouched. If n equals 
	 * current array size, Trunc() calls Clear() thus reseting the MuParamBlock object to its 
	 * uninitialized state.
	 *
	 * @return
	 * <ul>
	 * <li> upon success, Trunc() returns MuERROR_NONE 
	 * <li> if n > current size, Trunc() returns MuERROR_INVALID_PARAMBLOCK_SIZE 
	 * <li> if memory allocation fails, Trunc() returns MuERROR_INSUF_MEM 
	 * </ul>
	 * 
	 **/			
	MuError Trunc(uShort n);
    
    /**
     * @brief displays data values for debugging
     *
     * @details
     * Show() goes through each parameter contained in the block and
     * sends its value to standard output. If Block is empty, nothing happens.
     *
     **/			
    void Show(void);
    
    /**
     * @brief Bubble sorts block's data
     *
     * @details
     * Sort() sorts the block's internal data values
     * in either ascending or descending order. A bubble
     * algorithm is used.
     *
     * @param
     * order (short) - this argument should contain one of two
     * constants: ASCENDING (1) or DESCENDING (-1), to inform 
     * the sorting order to be used.
     *
     **/
    void Sort(short order);
};
#endif
