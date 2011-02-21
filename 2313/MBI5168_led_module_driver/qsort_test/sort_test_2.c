#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * this sorts an array of structs with qsort
 *
 * the sorting is done with respect to the 2nd number in the structs
 *
 */

typedef struct {
	uint8_t number;
	uint8_t dutycycle;
} led_t;

led_t glob_brightness[8] = { {0, 1}, {1, 2}, {2, 6}, {3, 5}, {4, 4}, {5, 7}, {6, 3}, {7, 8} };

static int my_compare_func(const void * p1, const void * p2);
int main(void);

int main(void)
{
	qsort(&glob_brightness[0],8,sizeof(led_t),my_compare_func);
	
	uint8_t index;
	for(index = 0; index < 8; index++)
	{
		printf("sorted original: %d \n", (glob_brightness[index]).dutycycle);
	}
}

static int my_compare_func(const void * p1, const void * p2)
{
	const led_t * first = p1;
	const led_t * second = p2;

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
