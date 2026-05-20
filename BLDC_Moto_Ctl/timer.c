#include "includes.h"

void Moto0_U_Set_Val(uint16_t val)
{
	   timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,val);
}

void Moto0_V_Set_Val(uint16_t val)
{
	   timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,val);
}

void Moto0_W_Set_Val(uint16_t val)
{
	   timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_2,val);
}


void Moto1_U_Set_Val(uint16_t val)
{
	   timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,val);
}

void Moto1_V_Set_Val(uint16_t val)
{
	   timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,val);
}

void Moto1_W_Set_Val(uint16_t val)
{
	   timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,val);
}

//void Moto0_U_Channel_Close(void)
//{
//		 timer_channel_output_state_config(TIMER0,TIMER_CH_0,TIMER_CCX_DISABLE);
//}

//void Moto0_U_Channel_Open(void)
//{
//		 timer_channel_output_state_config(TIMER0,TIMER_CH_0,TIMER_CCX_ENABLE);
//}

void Moto0_U_Disable(void)
{
    GPIO_BC(GPIOE) = GPIO_PIN_6;
}

void Moto0_U_Enable(void)
{
    GPIO_BOP(GPIOE) = GPIO_PIN_6;
}

void Moto0_V_Disable(void)
{
    GPIO_BC(GPIOE) = GPIO_PIN_3;
}

void Moto0_V_Enable(void)
{
    GPIO_BOP(GPIOE) = GPIO_PIN_3;
}

void Moto0_W_Disable(void)
{
    GPIO_BC(GPIOE) = GPIO_PIN_4;
}

void Moto0_W_Enable(void)
{
    GPIO_BOP(GPIOE) = GPIO_PIN_4;
}

void Moto1_U_Disable(void)
{
    GPIO_BC(GPIOE) = GPIO_PIN_5;
}

void Moto1_U_Enable(void)
{
    GPIO_BOP(GPIOE) = GPIO_PIN_5;
}

void Moto1_V_Disable(void)
{
    GPIO_BC(GPIOE) = GPIO_PIN_6;
}

void Moto1_V_Enable(void)
{
    GPIO_BOP(GPIOE) = GPIO_PIN_6;
}

void Moto1_W_Disable(void)
{
    GPIO_BC(GPIOE) = GPIO_PIN_7;
}

void Moto1_W_Enable(void)
{
    GPIO_BOP(GPIOE) = GPIO_PIN_7;
}

void Moto0_UV_Run(uint16_t val)
{
	  Moto0_U_Enable();
		Moto0_U_Set_Val(val);
	
	  Moto0_V_Enable();
		Moto0_V_Set_Val(0);
	
	  Moto0_W_Disable();
//	  timer_channel_output_state_config(TIMER0,TIMER_CH_2,TIMER_CCX_DISABLE);
		Moto0_W_Set_Val(0);
}

void Moto0_UW_Run(uint16_t val)
{
	  Moto0_U_Enable();
		Moto0_U_Set_Val(val);
	
	  Moto0_W_Enable();
		Moto0_W_Set_Val(0);
	
	  Moto0_V_Disable();
		Moto0_V_Set_Val(0);
}

void Moto0_VW_Run(uint16_t val)
{
	  Moto0_V_Enable();
		Moto0_V_Set_Val(val);
	
	  Moto0_W_Enable();
		Moto0_W_Set_Val(0);
	
	  Moto0_U_Disable();
		Moto0_U_Set_Val(0);
}

void Moto0_VU_Run(uint16_t val)
{
	  Moto0_V_Enable();
		Moto0_V_Set_Val(val);
	
	  Moto0_U_Enable();
		Moto0_U_Set_Val(0);
	
	  Moto0_W_Disable();
		Moto0_W_Set_Val(0);
}

void Moto0_WU_Run(uint16_t val)
{
	  Moto0_W_Enable();
		Moto0_W_Set_Val(val);
	
	  Moto0_U_Enable();
		Moto0_U_Set_Val(0);
	
	  Moto0_V_Disable();
		Moto0_V_Set_Val(0);
}

void Moto0_WV_Run(uint16_t val)
{
	  Moto0_W_Enable();
		Moto0_W_Set_Val(val);
	
	  Moto0_V_Enable();
		Moto0_V_Set_Val(0);
	
	  Moto0_U_Disable();
		Moto0_U_Set_Val(0);
}

void Moto1_UV_Run(uint16_t val)
{
	  Moto1_U_Enable();
		Moto1_U_Set_Val(val);
	
	  Moto1_V_Enable();
		Moto1_V_Set_Val(0);
	
	  Moto1_W_Disable();
		Moto1_W_Set_Val(0);
}

void Moto1_UW_Run(uint16_t val)
{
	  Moto1_U_Enable();
		Moto1_U_Set_Val(val);
	
	  Moto1_W_Enable();
		Moto1_W_Set_Val(0);
	
	  Moto1_V_Disable();
		Moto1_V_Set_Val(0);
}

void Moto1_VW_Run(uint16_t val)
{
	  Moto1_V_Enable();
		Moto1_V_Set_Val(val);
	
	  Moto1_W_Enable();
		Moto1_W_Set_Val(0);
	
	  Moto1_U_Disable();
		Moto1_U_Set_Val(0);
}

void Moto1_VU_Run(uint16_t val)
{
	  Moto1_V_Enable();
		Moto1_V_Set_Val(val);
	
	  Moto1_U_Enable();
		Moto1_U_Set_Val(0);
	
	  Moto1_W_Disable();
		Moto1_W_Set_Val(0);
}

void Moto1_WU_Run(uint16_t val)
{
	  Moto1_W_Enable();
		Moto1_W_Set_Val(val);
	
	  Moto1_U_Enable();
		Moto1_U_Set_Val(0);
	
	  Moto1_V_Disable();
		Moto1_V_Set_Val(0);
}

void Moto1_WV_Run(uint16_t val)
{
	  Moto1_W_Enable();
		Moto1_W_Set_Val(val);
	
	  Moto1_V_Enable();
		Moto1_V_Set_Val(0);
	
	  Moto1_U_Disable();
		Moto1_U_Set_Val(0);
}

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
void timer0_config(void)
{
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
       TIMER0CLK = SystemCoreClock / 120 = 1MHz */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

	  rcu_periph_clock_enable(RCU_TIMER0);
	  timer_deinit(TIMER0);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 256;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0,&timer_initpara);

     /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0,TIMER_CH_0,&timer_ocintpara);
    timer_channel_output_config(TIMER0,TIMER_CH_1,&timer_ocintpara);
    timer_channel_output_config(TIMER0,TIMER_CH_2,&timer_ocintpara);
		
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_0,250);
		timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_1,250);
    timer_channel_output_pulse_value_config(TIMER0,TIMER_CH_2,250);

    timer_channel_output_mode_config(TIMER0,TIMER_CH_0,TIMER_OC_MODE_PWM0);
		timer_channel_output_mode_config(TIMER0,TIMER_CH_1,TIMER_OC_MODE_PWM0);
		timer_channel_output_mode_config(TIMER0,TIMER_CH_2,TIMER_OC_MODE_PWM0);
		
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
    timer_channel_output_shadow_config(TIMER0,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER0,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    timer_enable(TIMER0);
}

void timer1_config(void)
{
    /* TIMER0 configuration: generate PWM signals with different duty cycles:
       TIMER0CLK = SystemCoreClock / 120 = 1MHz */
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

	  rcu_periph_clock_enable(RCU_TIMER1);
	  timer_deinit(TIMER1);

    /* TIMER0 configuration */
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

     /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1,TIMER_CH_3,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
    timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocintpara);
		
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,250);
		timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,250);
    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,250);

    timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
		timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
		timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
		
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER1,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);
}

void timer_config(void)
{
    timer0_config();
    timer1_config();
}
