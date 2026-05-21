#include "includes.h"

void DebugPrint_log(uint16_t print_delay, float a, float b)
{
		static uint16_t count = 0;
	
    count++;
	  if(count>=print_delay) {
		    printf("\r\n a = %lf, b = %lf \r\n", a, b);
			  count = 0;
		}
}

