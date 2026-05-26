#include "gd32f30x.h"
#include "gd32f303c_eval.h"
#include "systick.h"
#include "gpio.h"
#include "timer.h"
#include "foc.h"
#include "as5600_i2c.h"
#include "uart.h"
#include <stdio.h>
#include <math.h>
#include "pid.h"
#include <string.h>
#include "lowpass_filter.h"
#include <stdint.h>
#include <stdlib.h>


#define DEBUG_PRINT
#define MAX_RECORD_BUFF    5000


void RecordPrintLog(uint8_t en, uint16_t num, uint8_t ctl_flag, float a, float b);
void DebugPrint_log(uint8_t en, uint16_t print_delay, float a, float b);



