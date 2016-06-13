//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuParamBlock.cpp
 *
 * @brief ParamBlock Class Implementation
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 **/

#include "MuParamBlock.h"


// constructor
MuParamBlock::MuParamBlock(void)
{
	values = NULL;
	numValues = 0;
}

// copy constructor
MuParamBlock::MuParamBlock(const MuParamBlock & sourcePB)
{
	values = NULL;
	numValues = 0;
	
	uShort n = sourcePB.numValues;
	if(n > 0)
	{
		values = new float[n];
		if(values)
		{
			for(uShort i = 0; i < n; i++)
				values[i] = sourcePB.values[i];
			numValues = n;
		}
		else
			numValues = 0;
			
	}
}

// destructor
MuParamBlock::~MuParamBlock(void)
{
	if(values)
		delete [] values;
}

// assignment
MuParamBlock & MuParamBlock::operator=(const MuParamBlock & inBlock)
{
	uShort n, i;
	
	if(this != &inBlock)
	{
		delete [] values;
		n = inBlock.numValues;
		if(n > 0)
		{
			values = new float[n];
			if(values)
			{
				numValues = n;
				for( i = 0; i < n; i++)
					values[i] = inBlock.values[i];
			}
			else
				numValues = 0;
		}
	}
	
	return *this;
}

// subscript
float& MuParamBlock::operator[](uShort i)
{
	return values[i];
}

bool MuParamBlock::operator==(const MuParamBlock & inBlock)
{
	long i = 0;
	long n = numValues;
	
	if( numValues != inBlock.numValues )
		return false;
	
	for( i = 0; i < n; i++ )
		if( values[i] != inBlock.values[i] )
			return false;
	
	return true;
}

// inequality test...
bool MuParamBlock::operator!=(const MuParamBlock & inBlock)
{
	return !( *this == inBlock );
}

// Initialize Block with n values
MuError MuParamBlock::Init(uShort n)
{
	MuError err(MuERROR_NONE);
	
	if(n == 0)
	{
		err.Set(MuERROR_INVALID_PARAMBLOCK_SIZE);
		return err;
	}
	
	if(values)
	{
		err.Set(MuERROR_CANNOT_INIT);
		return err;
	}
        
	values = new float[n];
	if(values)
	{
		for(uShort i = 0; i < n; i++)
			values[i] = 0;
		numValues = n;
	}
	else
	{
		err.Set(MuERROR_INSUF_MEM);
		return err;
	}
		
	return err;
}

void MuParamBlock::Clear(void)
{
	if(values)
		delete [] values;
	values = NULL;
	numValues = 0;
}

// Number of values
uShort MuParamBlock::Num(void)
{
	return numValues;
}

// Read value
MuError MuParamBlock::Val(uShort which_param, float * theValue)
{
	MuError err(MuERROR_NONE);
	
	if(numValues == 0)
	{
		err.Set(MuERROR_PARAM_BLOCK_NOT_INITIALIZED);
		return err;
	}
	else
	{
		if(which_param >= numValues)
		{
			err.Set(MuERROR_INVALID_PARAMETER);
			return err;
		}
		else
		{
			if(values == NULL)
			{
				err.Set(MuERROR_PARAM_BLOCK_NOT_INITIALIZED);
				return err;
			}
			else
			{
					*theValue = values[which_param];
			}
		}
	}
	return err;
}

// write value
MuError MuParamBlock::SetVal(uShort which_param, float newValue)
{
	MuError err(MuERROR_NONE);
	
	if(numValues == 0)
	{
		err.Set(MuERROR_PARAM_BLOCK_NOT_INITIALIZED);
		return err;
	}
	else
	{
		if(which_param >= numValues)
		{
			err.Set(MuERROR_INVALID_PARAMETER);
			return err;
		}
		else
		{
			if(values == NULL)
			{
				err.Set(MuERROR_PARAM_BLOCK_NOT_INITIALIZED);
				return err;
			}
			else
			{
					values[which_param] = newValue;
			}
		}
	}
	return err;
}

// append value
MuError MuParamBlock::AddParam(float newValue)
{
	MuError err(MuERROR_NONE);
	
	if(numValues == 0)  // if Block is empty
	{
		values = new float[1]; // init with 1 param
		if(!values)
		{
			err.Set(MuERROR_INSUF_MEM);
			return err;
		}
		else
		{
				values[0] = newValue;
				numValues = 1;
		}
	}
	else
	{
		float * temp = new float[numValues + 1];
		if(!temp)
		{
			err.Set(MuERROR_INSUF_MEM);
			return err;
		}
		else
		{
			for(uShort i = 0; i < numValues; i++)
				temp[i] = values[i];
			temp[numValues] = newValue;
			delete [] values;
			values = temp;
			numValues++;
		}
	}
	
	return err;
}

MuError MuParamBlock::Grow(uShort n)
{
	MuError err(MuERROR_NONE);
	
	if(numValues == 0)  // if Block is empty
	{
		values = new float[n]; // init with n param
		if(!values)
		{
			err.Set(MuERROR_INSUF_MEM);
			return err;
		}
		else
		{
				for(uShort i = 0; i < n; i++)
					values[i] = 0;
				numValues = n;
		}
	}
	else
	{
		float * temp = new float[numValues + n];
		if(!temp)
		{
			err.Set(MuERROR_INSUF_MEM);
			return err;
		}
		else
		{
			for(uShort i = 0; i < numValues; i++)
				temp[i] = values[i];
			for(uShort i = numValues; i < numValues + n; i++)
				temp[i] = 0;
			delete [] values;
			values = temp;
			numValues += n;
		}
	}
	
	return err;
}

MuError MuParamBlock::Trunc(uShort n)
{
	MuError err(MuERROR_NONE);
	
	if(numValues == 0)  // if Block is empty
	{
		err.Set(MuERROR_PARAM_BLOCK_NOT_INITIALIZED);
		return err;
	}
	
	if(n > numValues)
	{
		err.Set(MuERROR_INVALID_PARAMBLOCK_SIZE);
		return err;
	}
	else
	{
		float * temp = new float[numValues - n];
		if(!temp)
		{
			err.Set(MuERROR_INSUF_MEM);
			return err;
		}
		else
		{
			for(uShort i = 0; i < numValues - n; i++)
				temp[i] = values[i];
			delete [] values;
			values = temp;
			numValues -= n;
		}
	}
	
	return err;
}


void MuParamBlock::Show(void)
{
    uShort i,n;
    n = numValues;
    
    cout.precision(6);
    cout << fixed;
    
    for(i = 0; i < n; i++)
    {
        
        cout << "[" << values[i] << "] ";
    }
    
    cout << endl;
}

void MuParamBlock::Sort(short order)
{
    int i, j, n;
    
    n = numValues;
    
    for(i = n; i >= 1; i-- )
    {
        for( j = 0; j < i-1; j++ )
        {
            if(order == ASCENDING)
            {
                if( values[j] > values[j+1] )
                {
                    float temp;
                    temp = values[j];
                    values[j] = values[j+1];
                    values[j+1] = temp;
                }
            }
            
            if(order == DESCENDING)
            {
                if( values[j] < values[j+1] )
                {
                    float temp;
                    temp = values[j];
                    values[j] = values[j+1];
                    values[j+1] = temp;
                }
            }
        }
    }

}