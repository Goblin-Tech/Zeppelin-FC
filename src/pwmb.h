/*
 * pwm.h
 *
 * Created: 6/1/2013 11:04:37 AM
 *  Author: ViDAR
 */ 


#ifndef PWM_H_
#define PWM_H_

/** PWM frequency in Hz */
#define PWM_FREQUENCY      2000
/** Period value of PWM output waveform */
#define PERIOD_VALUE       100
/** Initial duty cycle value */
#define INIT_DUTY_VALUE    0

/* PWM channel instance for LEDs */
static pwm_channel_t g_pwm_channel_led;
static uint32_t pwm_count;
static uint8_t pwm_state;

void PWM_init(void);
void Beep(uint8_t mode);

#endif /* PWM_H_ */