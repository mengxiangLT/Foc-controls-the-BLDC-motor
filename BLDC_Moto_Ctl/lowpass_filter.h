#ifndef __LOWPASS_FILTER_H
#define __LOWPASS_FILTER_H

#include <stdint.h>
#include "includes.h"

#if 0
/* ???????? */
typedef struct {
    float Tf;                   /* ????(?) */
    float y_prev;               /* ?????? */
    uint32_t timestamp_prev;    /* ???????(??) */
} LowPassFilter;

/* ???? */
void LowPassFilter_Init(LowPassFilter *filter, float time_constant);
float LowPassFilter_Update(LowPassFilter *filter, float x);
#endif

typedef struct {
    float alpha;    /* ???? 0-1,???????? */
    float y_prev;   /* ????? */
} LowPassFilter;

/* ???:alpha = dt / (Tf + dt),dt ?????(?) */
void LowPassFilter_Init(LowPassFilter *filter, float alpha);

/* ???? */
float LowPassFilter_Update(LowPassFilter *filter, float input);

#endif /* __LOWPASS_FILTER_H */


