#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * this sorts an array of pointers with qsort
 * the pointers point to elements of another array, which are structs of 2 numbers
 * the sorting is done with regard to the 2nd number of each struct
 *
 * purpose: leave the original data alone and still access it in a sorted way
 *
 */

typedef struct {
	uint8_t number;
	uint8_t dutycycle;
} led_t;

led_t glob_brightness[8] = { {0, 1}, {1, 2}, {2, 6}, {3, 5}, {4, 4}, {5, 7}, {6, 3}, {7, 8} };
led_t * sorted[8]; // array of pointers

void init(void);
void bubbleSort(led_t ** array, uint8_t size);
int main(void);

int main(void)
{
	init();

	bubbleSort(&sorted[0],8);
	
	uint8_t index;
	for(index = 0; index < 8; index++)
	{
		printf("sorted: %d \n", (*sorted[index]).dutycycle);
	}
	
	for(index = 0; index < 8; index++)
	{
		printf("original: %d \n", (glob_brightness[index]).dutycycle);
	}
}


void init(void)
{
	uint8_t index;
	for(index = 0; index < 8; index++)
	{
		/* load the array of pointers with the unsorted data */
		sorted[index] = &glob_brightness[index];
	}
}

void bubbleSort(led_t ** array, uint8_t length)
{
	uint8_t i;
	uint8_t j;
	led_t * temp;
	uint8_t test;
	for(i = (length - 1); i > 0; i--)
	{
		test = 0;
		for(j = 0; j < i; j++)
		{
			if( (*array[j]).dutycycle > (*array[j+1]).dutycycle )
			{
				temp = array[j];
				array[j] = array[j+1];
				array[j+1] = temp;
				test = 1;
			}
		}
		if(test = 0)
		{
			break;
		}
	}
}
