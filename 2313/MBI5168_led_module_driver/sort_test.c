#include <stdio.h>
#include <stdint.h>

typedef struct {
	uint8_t number;
	uint8_t dutycycle;
} led_t;

led_t glob_brightness[8] = { {0, 1}, {1, 2}, {2, 6}, {3, 5}, {4, 4}, {5, 7}, {6, 3}, {7, 8} };
led_t * sorted[8];

led_t * min(led_t * array, uint8_t size);
void sort(led_t * array, led_t ** buffer, uint8_t size);
int main(void);


int main(void)
{
	uint8_t index;
	sort(&glob_brightness[0],&sorted[0],8);

	for(index = 0; index < 8; index++)
	{
		printf("\nmin: %d \n", (*min(&glob_brightness[0],8)).dutycycle);
		printf("sorted: %d \n", (*sorted[index]).dutycycle);

	}
}

led_t * min(led_t * array, uint8_t size)
{
	uint8_t index;
	led_t * minimum = &array[0]; // start with the first element
	for(index = 1; index < size; index++)
	{
		if( (*minimum).dutycycle < array[index].dutycycle )
	       	{
			// keep it
		}
		else 
		{
			minimum = &array[index];
		}
	}
	return minimum;
}

void sort(led_t * array, led_t ** buffer, uint8_t size)
{
	uint8_t index;
	for(index = 0; index < size; index++)
	{
		buffer[index] = min(array,size);
	}
}
