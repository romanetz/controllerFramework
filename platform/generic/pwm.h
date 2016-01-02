#ifndef __PWM_H__
#define __PWM_H__

#include <generic.h>

typedef uint8_t pwm_channel_index_t;
typedef uint32_t pwm_value_t;

#ifdef USE_CLASSES

typedef struct PWMClass PWMClass;

typedef void (*PWMClassSetValueFunc)(PWMClass *pwm, pwm_channel_index_t channel, pwm_value_t value);
typedef void (*PWMClassEnableFunc)(PWMClass *pwm, pwm_channel_index_t channel);
typedef void (*PWMClassDisableFunc)(PWMClass *pwm, pwm_channel_index_t channel);
typedef pwm_channel_index_t (*PWMClassCountFunc)(PWMClass *pwm);
typedef pwm_value_t (*PWMClassMinValueFunc)(PWMClass *pwm, pwm_channel_index_t channel);
typedef pwm_value_t (*PWMClassMaxValueFunc)(PWMClass *pwm, pwm_channel_index_t channel);

struct PWMClass {
	PWMClassSetValueFunc setValue;
	PWMClassEnableFunc enable;
	PWMClassDisableFunc disable;
	PWMClassCountFunc count;
	PWMClassMinValueFunc minValue;
	PWMClassMaxValueFunc maxValue;
};

#define PWM_CLASS(obj) ((PWMClass*)(obj))

#define pwmSetValue(pwm, channel, value) PWM_CLASS(pwm)->setValue(PWM_CLASS(pwm), channel, value)
#define pwmEnable(pwm, channel) PWM_CLASS(pwm)->enable(PWM_CLASS(pwm), channel)
#define pwmDisable(pwm, channel) PWM_CLASS(pwm)->disable(PWM_CLASS(pwm), channel)
#define pwmCount(pwm) PWM_CLASS(pwm)->count(PWM_CLASS(pwm))
#define pwmMinValue(pwm, channel) PWM_CLASS(pwm)->minValue(PWM_CLASS(pwm), channel)
#define pwmMaxValue(pwm, channel) PWM_CLASS(pwm)->maxValue(PWM_CLASS(pwm), channel)

#endif

#endif
