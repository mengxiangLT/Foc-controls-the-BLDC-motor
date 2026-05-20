#ifndef __TIMER_H__
#define __TIMER_H__
#include "gd32f30x.h"
#include "gd32f303c_eval.h"

void timer_config(void);
void Moto0_U_Enable(void);
void Moto0_V_Enable(void);
void Moto0_W_Enable(void);
void Moto0_U_Set_Val(uint16_t val);
void Moto0_V_Set_Val(uint16_t val);
void Moto0_W_Set_Val(uint16_t val);
void Moto1_U_Set_Val(uint16_t val);
void Moto1_V_Set_Val(uint16_t val);
void Moto1_W_Set_Val(uint16_t val);

#endif


