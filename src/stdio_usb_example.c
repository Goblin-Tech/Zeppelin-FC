
#include <asf.h>
#include <board.h>
#include <sysclk.h>
#include <stdio_usb.h>
#include "zeppelin_fc.h"
#include "MPU9150.h"
#include "I2CdevWrapper.h"
#include "rtc.h"
#include "pwmb.h"

#define N_E 165000
#define N_G 196000
#define N_B 247000
#define N_E2 300000
#define N_C 261000
#define N_D 294000

void BootupTone(void);

/**
 * \brief main function
 */
int main (void)
{
	sysclk_init();
	board_init();
	
	/* Default RTC configuration, 12-hour mode */
	rtc_set_hour_mode(RTC, 1);
	
	
	
	/* Configure RTC interrupts */
	NVIC_DisableIRQ(RTC_IRQn);
	NVIC_ClearPendingIRQ(RTC_IRQn);
	NVIC_SetPriority(RTC_IRQn, 0);
	NVIC_EnableIRQ(RTC_IRQn);
	rtc_enable_interrupt(RTC, RTC_IER_SECEN | RTC_IER_ALREN);
	
	gpio_configure_pin(PIO_PB13_IDX, (PIO_OUTPUT_1 | PIO_DEFAULT));
	gpio_set_pin_high(PIO_PB13_IDX);
	// Initialize interrupt vector table support.
	irq_initialize_vectors();

	// Enable interrupts
	cpu_irq_enable();
	
	/* Set direction and pullup on the given button IOPORT */
	ioport_set_pin_dir(PIO_PA0_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIO_PA0_IDX, IOPORT_MODE_PULLUP);
	
	//if (!ioport_get_pin_level(PIO_PA0_IDX))
	//	flash_clear_gpnvm(1);
	
	/* Call a local utility routine to initialize C-Library Standard I/O over
	 * a USB CDC protocol. Tunable parameters in a conf_usb.h file must be
	 * supplied to configure the USB device correctly.
	 */
	stdio_usb_init();
	
	
	delay_s(3);
	
	
	ZP_Init();
	BootupTone();
	PWM_init();
	while (true) {
		
		ZP_Loop();
	}
}






void BootupTone(void)
{
	playNote(N_E, 100);
	playNote(N_G, 100);
	playNote(N_B, 100);
}
