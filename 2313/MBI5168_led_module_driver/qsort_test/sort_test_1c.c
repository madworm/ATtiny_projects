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
static int my_compare_func(const void * p1, const void * p2);
int main(void);

int main(void)
{
	init();

	qsort(&sorted[0],8,sizeof(sorted[0]),my_compare_func);
	
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

static int my_compare_func(const void * p1, const void * p2)
{
	/* see 'sort_test_1a.c' as well for an
	 * equivalent version.
	 * pointer magic... ugh...
	 */
	 
	led_t * first = (led_t *)(*(const *)(p1));
	led_t * second = (led_t *)(*(const *)(p2));

	if( (*first).dutycycle > (*second).dutycycle )
	{
		return 1;
	}
	else if ( (*first).dutycycle < (*second).dutycycle )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
