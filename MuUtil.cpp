//*********************************************
//***************** NCM-UnB *******************
//******** (c) Carlos Eduardo Mello ***********
//*********************************************
// This softwre may be freely reproduced, 
// copied, modified, and reused, as long as 
// it retains, in all forms, the above credits.
//*********************************************

/** @file MuUtil.cpp
 *
 * @brief Utility Functions Implementation
 *
 * @author Carlos Eduardo Mello
 * @date 3/5/2009
 *
 * @details
 * MuM utilities includes functions which are used by MuM classes
 * to perform routine tasks such as initializing arrays ( Set() ), 
 * picking random values within a range ( Between() ), etc.
 **/


#include "MuUtil.h"

void MuInit(void)
{
	srand((int)time((0)));
}

void Set(int * array, int n, int value)
{
	int i;
	if(n > 0)
	{
		for(i = 0; i < n; i++)
		{
			array[i] = value;
		}
	}
	else
	{
		if(n == 0)
		{
			for(i = 0; i < value; i++)
			{
				array[i] = i;
			}
			
		}
	}
}

extern int Between( int low, int high )
{
	int temp;
	int range;
	
	if(high == low)
		return high;
	else
	{
		if(high < low)
		{
			temp = low;
			low = high;
			high = temp;
		}
	}
	
	high++; // to include upper limit
	range = high - low;
	
	return ( ( rand()% range ) + low );
}


extern float Between(float min, float max)
{
    return (rand()*((max-min)/RAND_MAX)) + min;
}


// Tries to find a short inside an array of shorts
// returns the index number of found value, or -1 
// if value not found
extern short Inside(short value, short * array, short n )
{
	short index = -1;
	int i = 0;
	for( i = 0; i < n; i++ )
	{
		if( value == array[i] )
		{
			index = i;
			break;
		}
	}
	return index;
}

// Mixes the input array
extern void MixInts( int * array, int n)
{
	int i, j, pick;
	int * bag;
	int limit = n-1;
	
	bag = new int[n];
	if(bag)
	{
		for(i = 0; i < n; i++)
			bag[i] = array[i];
		
		// fill new array one item at a time...
		for(i = 0 ; i < n; i++)
		{
			// picking a random position in input array...
			pick = Between( 0, limit );
			array[i] = bag[pick];
			
			// shrinking the number of items in input array.
			for(j = pick; j < limit; j++)
				bag[j] = bag[j+1];
			
			// eliminate selected option
			limit--;
		}
		delete [] bag;
	}
}

// Sorts (bubble) an array of  ints
extern void SortInts( int * array, int n)
{
	int i, j;
	
	for(i = n; i >= 1; i-- )
	{
		for( j = 0; j < i-1; j++ )
		{
			if( array[j] > array[j+1] )
			{
				int temp;
				temp = array[j];
				array[j] = array[j+1]; 
				array[j+1] = temp;
			}
		}
	}
}

// Sorts (bubble) an array of  floats
extern void SortFloats( float * array, int n)
{
    int i, j;
    
    for(i = n; i >= 1; i-- )
    {
        for( j = 0; j < i-1; j++ )
        {
            if( array[j] > array[j+1] )
            {
                int temp;
                temp = array[j];
                array[j] = array[j+1]; 
                array[j+1] = temp;
            }
        }
    }
}


// Shows the contents of an integer array
extern void ShowInts( int * array, int n )
{
	int i;
	for( i = 0; i < n; i++ )
		cout << array[i] << " ";
	cout << endl;
}
