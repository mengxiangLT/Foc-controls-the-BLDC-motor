/*!
    \file    main.c
    \brief   TIMER Breath LED demo

    \version 2025-08-20, V3.0.2, demo for GD32F30x
*/

/*
    Copyright (c) 2025, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "includes.h"



float set_lowpass_para = 0;

//extern uint16_t g_print_buffer[2][5000];
extern uint8_t g_start_flag;

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
	  uint16_t set_speed = 0;
	
//    FlagStatus breathe_flag = SET;
    /* configure systick */
    systick_config();
    /* configure the GPIO ports */
    gpio_config();
	  /* USART interrupt configuration */
	  Uart_Init(EVAL_COM1);
    Moto0_U_Enable();
	  as5600_i2c_init();
	  delay_1ms(200);
    if(as5600_check_magnet()) {
        /* ??????,???? */
    }
    /* configure the TIMER peripheral */
    timer_config();
		DFOC_M0_SET_VEL_PID(0.01,0.1,0,0);
		DFOC_Vbus(12);
//		DFOC_M0_SET_ANGLE_PID(0.1,0.01,0,0);
//		DFOC_alignSensor(Motor_PP, Sensor_DIR);
    while (1){
			if(rx_counter > 1)
			{
			    set_speed = rx_buffer[0];
				  g_start_flag = rx_buffer[1];
				  set_lowpass_para = rx_buffer[1];
				  set_lowpass_para = (float)set_lowpass_para / 100000;
				  LowPassFilter_Init(&M0_Vel_Flt, set_lowpass_para);
				  printf("\r\n set_speed = %x set_lowpass_para = %f, time_constant = %f \r\n", set_speed, set_lowpass_para, M0_Vel_Flt.Tf);
				  rx_counter = 0;
			}
//			DFOC_M0_set_Velocity_Angle(1, 6.28);
//			DFOC_M0_set_Force_Angle(1, set_speed);
//			DFOC_M0_setVelocity(1, 150);
	    DFOC_M0_setVelocity(1, set_speed);
        /* configure TIMER channel output pulse value */
//        timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,i);
//				timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,i);
//        timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_2,i);
//				
//				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,i);
//        timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,i);
//				timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,i);
//        velocityOpenloop(5);
    }
}

