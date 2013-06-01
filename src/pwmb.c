/*
 * pwm.c
 *
 * Created: 6/1/2013 10:39:56 AM
 *  Author: ViDAR
 */ 
#include <asf.h>
#include <board.h>
#include <sysclk.h>
#include <stdio_usb.h>
#include "pwmb.h"

static pwm_channel_t g_pwm_channel_led;
static uint32_t pwm_count = 0;  /* PWM counter value */
static uint8_t pwm_state = 0;

void PWM_init(void)
{
	/////////////////
	/* Enable PWM peripheral clock */
	pmc_enable_periph_clk(ID_PWM);
	
	/* Disable PWM channels for LEDs */
	pwm_channel_disable(PWM, PIN_PWM_LED0_CHANNEL);
	
	/* Set PWM clock A as PWM_FREQUENCY * PERIOD_VALUE (clock B is not used) */
	pwm_clock_t clock_setting = {
		.ul_clka = 200000,
		.ul_clkb = 0,
		.ul_mck = sysclk_get_cpu_hz()
	};
	pwm_init(PWM, &clock_setting);
	
	/* Initialize PWM channel for LED0 (period is left-aligned and output waveform starts at a low level) */
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;  /* Use PWM clock A as source clock */
	g_pwm_channel_led.ul_period = 100;  /* Period value of output waveform */
	g_pwm_channel_led.ul_duty = 0;  /* Duty cycle value of output waveform */
	g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
	pwm_channel_init(PWM, &g_pwm_channel_led);

	/* Enable channel counter event interrupt */
	pwm_channel_enable_interrupt(PWM, PIN_PWM_LED0_CHANNEL, 0);

	/* Configure interrupt and enable PWM interrupt */

	NVIC_DisableIRQ(PWM_IRQn);
	NVIC_ClearPendingIRQ(PWM_IRQn);
	NVIC_SetPriority(PWM_IRQn, 0);
	NVIC_EnableIRQ(PWM_IRQn);

	/* Enable PWM channels for LEDs */
	pwm_channel_enable(PWM, PIN_PWM_LED0_CHANNEL);

	/////////////////
}

void PWM_Handler(void)
{
	uint32_t events = pwm_channel_get_interrupt_status(PWM);
	pwm_count++;
	g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
	
	if (pwm_state == 1)
	{
		if(pwm_count < 500)
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, 50);
		else
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, 0);
	}
	
	if (pwm_state == 2)
	{
		if(pwm_count > 500 && pwm_count < 1000)
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, 50);
		else
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, 0);
		
		if (pwm_count > 1000)
			pwm_count = 0;
	}
}

void Beep(uint8_t mode)
{
	pwm_count = 0;
	switch (mode)
	{
		case 0x00: pwm_state = 0; pwm_channel_update_duty(PWM, &g_pwm_channel_led, 0); break;
		case 0x01: pwm_state = 1; break;
		case 0x02: pwm_state = 2; break;
		case 0x03: pwm_state = 3; break;
	}
}