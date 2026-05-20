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
#include "timer.h"
#include "foc_deng.h"
#include "as5600_i2c.h"
#include "gd32f303c_eval.h"
//#include "bldc_motor.h"


/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
#if 1
int main(void)
{
//    int16_t i = 0;
//    FlagStatus breathe_flag = SET;
    /* configure systick */
    systick_config();
    /* configure the GPIO ports */
    gpio_config();
	  gd_eval_com_init(EVAL_COM1);
    Moto0_U_Enable();
//	  i2c_gpio_config();
//    i2c_config();
	  as5600_i2c_init();
	  delay_1ms(500);
    if(as5600_check_magnet()) {
        /* ??????,???? */
    }
//	  Moto0_V_Enable();
//	  Moto0_W_Enable();
    /* configure the TIMER peripheral */
    timer_config();
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TC)){
    }
		DFOC_M0_SET_VEL_PID(0.005,0.00,0,0);
		DFOC_Vbus(12);
    while (1){
	    DFOC_M0_setVelocity(500);
//			printf("\r\n as5600 test! \r\n");
//        /* delay a time in milliseconds */
//        delay_1ms(40);
//        if (SET == breathe_flag){
//             i = i + 10;
//        }else{
//            i = i - 10;
//        }
//        if(500 < i){
//            breathe_flag = RESET;
//        }
//        if(0 >= i){
//            breathe_flag = SET;
//        }
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
#endif

#if 0
int main(void)
{
    /* ??????? (????????) */
    /* system_clock_config(); */
    /* configure systick */
    systick_config();
    /* ??????? */
    bldc_motor_init();
    /* configure EVAL_COM1 */
    gd_eval_com_init(EVAL_COM1);
    /* ??:?? AS5600 ???? */
    if(as5600_check_magnet()) {
        /* ??????,???? */
    }
		/* output a message on hyperterminal using printf function */
    printf("\r\n USART printf example: please press the Tamper key \r\n");
    while(RESET == usart_flag_get(EVAL_COM1, USART_FLAG_TC)){
    }
    while(1) {
        /* ??1: ???? (????,??? PWM ?????) */
        bldc_run(500);  /* 50% ??? */
        delay_ms(10);   /* ?????? */
        printf("as5600 test");
        /* ??2: ??????
        bldc_run_speed(300.0f);  // ???? 300 RPM
        delay_ms(10);
        */
        
        /* ??3: ??????
        static uint16_t target = 0;
        bldc_run_position(target);
        target += 10;
        if(target >= 4096) target = 0;
        delay_ms(10);
        */
    }
}
#endif

