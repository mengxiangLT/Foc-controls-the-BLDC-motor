#include "lowpass_filter.h"
#include <math.h>
#include "includes.h"

#if 0
extern uint32_t _micros(void);  /* ?????? */

void LowPassFilter_Init(LowPassFilter *filter, float time_constant)
{
    filter->Tf = time_constant;
    filter->y_prev = 0.0f;
    filter->timestamp_prev = _micros();
}

float LowPassFilter_Update(LowPassFilter *filter, float x)
{
    uint32_t timestamp = _micros();
    float dt = (timestamp - filter->timestamp_prev) * 1e-6f;
    float alpha;
    float y;
    
    /* ??????? */
    if(dt < 0.0f) {
        dt = 1e-3f;
    } else if(dt > 0.3f) {
        /* ?????,??????? */
        filter->y_prev = x;
        filter->timestamp_prev = timestamp;
        return x;
    }
    
    /* ??????:y = alpha * y_prev + (1-alpha) * x */
    /* ?? alpha = Tf / (Tf + dt) */
    alpha = filter->Tf / (filter->Tf + dt);
    y = alpha * filter->y_prev + (1.0f - alpha) * x;
    
    filter->y_prev = y;
    filter->timestamp_prev = timestamp;
    
    return y;
}
#endif


void LowPassFilter_Init(LowPassFilter *filter, float alpha)
{
    filter->alpha = alpha;
    filter->y_prev = 0.0f;
}

float LowPassFilter_Update(LowPassFilter *filter, float input)
{
    float output = filter->alpha * input + (1.0f - filter->alpha) * filter->y_prev;
    filter->y_prev = output;
    return output;
}

