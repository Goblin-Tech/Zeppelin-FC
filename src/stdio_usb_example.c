
#include <asf.h>
#include <board.h>
#include <sysclk.h>
#include <stdio_usb.h>
#include "zepplin_fc.h"
#include "MPU9150.h"
#include "I2CdevWrapper.h"

/* PWM frequency in Hz */
#define PWM_FREQUENCY      1000
/* Period value of PWM output waveform */
#define PERIOD_VALUE       100
/* Initial duty cycle value */
#define INIT_DUTY_VALUE    0

/* PWM channel instance for LEDs */
pwm_channel_t g_pwm_channel_led;

static long ZP_Altitude;
static long ZP_Max_Altitude;
static long ZP_Second_Deploment_Altitude = 400;
static long ZP_Temprature;
static long ZP_GroundLevel = 0;
static long ZP_Pressure;
static long ZP_Tick, ZP_Packet_Tick;
static int16_t ZP_Accel_X, ZP_Accel_Y, ZP_Accel_Z;
static int16_t ZP_Mag_X, ZP_Mag_Y, ZP_Mag_Z;
static int16_t ZP_Gyro_X, ZP_Gyro_Y, ZP_Gyro_Z;

#define ZP_Accel_X_B 0
#define ZP_Accel_Y_B 1
#define ZP_Accel_Z_B 2
#define ZP_Gyro_X_B 3
#define ZP_Gyro_Y_B 4
#define ZP_Gyro_Z_B 5
#define ZP_Mag_X_B 6
#define ZP_Mag_Y_B 7
#define ZP_Mag_Z_B 8

static uint8_t Packet_Buffer[32];
static uint8_t Packet_Buffer_Index = 0;


void Zepplin_SendPacket(void)
{
	uint8_t crc = 0;
	putchar('+');
	putchar('+');
	putchar('+');
	
	crc += ZP_Accel_X + ZP_Accel_Y + ZP_Accel_Z;
	putchar(ZP_Packet_Tick >> 8);
	putchar(ZP_Packet_Tick);
	
	crc += ZP_Packet_Tick;
	putchar(ZP_Accel_X >> 8);
	putchar(ZP_Accel_X);
	putchar(ZP_Accel_Y >> 8);
	putchar(ZP_Accel_Y);
	putchar(ZP_Accel_Z >> 8);
	putchar(ZP_Accel_Z);
	
	crc += ZP_Mag_X + ZP_Mag_Y + ZP_Mag_Z;
	putchar(ZP_Mag_X >> 8);
	putchar(ZP_Mag_X);
	putchar(ZP_Mag_Y >> 8);
	putchar(ZP_Mag_Y);
	putchar(ZP_Mag_Z >> 8);
	putchar(ZP_Mag_Z);
	
	crc += ZP_Gyro_X + ZP_Gyro_Y + ZP_Gyro_Z;
	putchar(ZP_Gyro_X >> 8);
	putchar(ZP_Gyro_X);
	putchar(ZP_Gyro_Y >> 8);
	putchar(ZP_Gyro_Y);
	putchar(ZP_Gyro_Z >> 8);
	putchar(ZP_Gyro_Z);
	
	int32_t tt = (int32_t) ZP_Altitude;
	
	putchar(tt >> 24);
	putchar(tt >> 16);
	putchar(tt >> 8);
	putchar(tt);
	
	putchar(0);
	
	putchar(crc);
	ZP_Packet_Tick++;
}

uint16_t Sensor[9];

void Zepplin_UpdateSensorData(void)
{
	MPU9150_getMotion9(&Sensor[ZP_Accel_X_B], &Sensor[ZP_Accel_Y_B], &Sensor[ZP_Accel_Z_B], &Sensor[ZP_Gyro_X_B], &Sensor[ZP_Gyro_Y_B], &Sensor[ZP_Gyro_Z_B], &Sensor[ZP_Mag_X_B], &Sensor[ZP_Mag_Y_B], &Sensor[ZP_Mag_Z_B]);
	// Get Accel Data
	//ZP_Accel_X = MPU9150_getAccelerationX();
	//ZP_Accel_Y = MPU9150_getAccelerationY();
	//ZP_Accel_Z = MPU9150_getAccelerationZ();
	
	// Get Mag Data
	//ZP_Mag_X = 0;
	//ZP_Mag_Y = 0;
	//ZP_Mag_Z = 0;
	
	// Get Gyro Data
	//ZP_Gyro_X = MPU9150_getRotationX();
	//ZP_Gyro_Y = MPU9150_getRotationY();
	//ZP_Gyro_Z = MPU9150_getRotationZ();
	
	// Get Pressure, Altitude, and Temprature Data
	ZP_Pressure = 0;
	ZP_Altitude = 0;
	ZP_Max_Altitude = ZP_Altitude;
	ZP_Temprature = 0;
}

/**
 * \brief main function
 */
int main (void)
{
	/* Initialize basic board support features.
	 * - Initialize system clock sources according to device-specific
	 *   configuration parameters supplied in a conf_clock.h file.
	 * - Set up GPIO and board-specific features using additional configuration
	 *   parameters, if any, specified in a conf_board.h file.
	 */
	sysclk_init();
	board_init();
	
	gpio_configure_pin(PIO_PB13_IDX, (PIO_OUTPUT_1 | PIO_DEFAULT));
	gpio_set_pin_high(PIO_PB13_IDX);
	// Initialize interrupt vector table support.
	irq_initialize_vectors();

	// Enable interrupts
	cpu_irq_enable();

	/* Call a local utility routine to initialize C-Library Standard I/O over
	 * a USB CDC protocol. Tunable parameters in a conf_usb.h file must be
	 * supplied to configure the USB device correctly.
	 */
	stdio_usb_init();
	
	delay_s(5);
	
	
	/* Set direction and pullup on the given button IOPORT */
	ioport_set_pin_dir(PIO_PA0_IDX, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(PIO_PA0_IDX, IOPORT_MODE_PULLUP);
	
	
	
	Zepplin_Init();
	
	ZP_CreateFile();
	ZP_Close();
	
	if (!ioport_get_pin_level(PIO_PA0_IDX))
		flash_clear_gpnvm(1);

	// Get and echo characters forever.

	uint8_t ch;
	ZP_Tick = 0;
	MPU9150_initialize();
	
	
	uint16_t ticki = 1;
	while (true) {
		//Zepplin_UpdateSensorData();
		//ZP_WriteToFile(&Sensor);
		//printf("Tick: %d\n", ticki);
		//ticki++;
		//if (ticki == 500 )
		//	ZP_Close();
		//Zepplin_SendPacket();
		Zepplin_Loop();
	}
}







/*


#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"

/ ** PWM frequency in Hz * /
#define PWM_FREQUENCY      1200
/ ** Period value of PWM output waveform * /
#define PERIOD_VALUE       100
/ ** Initial duty cycle value * /
#define INIT_DUTY_VALUE    0


/ ** PWM channel instance for LEDs * /
pwm_channel_t g_pwm_channel_led;

/ **
 * \brief Interrupt handler for the PWM controller.
 * /
void PWM_Handler(void)
{
	static uint32_t ul_count = 0;  / * PWM counter value * /
	static uint32_t ul_duty = INIT_DUTY_VALUE;  / * PWM duty cycle rate * /
	static uint8_t fade_in = 1;  / * LED fade in flag * /
	uint32_t events = pwm_channel_get_interrupt_status(PWM);

	/ * Interrupt on PIN_PWM_LED0_CHANNEL * /
	if ((events & (1 << PIN_PWM_LED0_CHANNEL)) == (1 << PIN_PWM_LED0_CHANNEL)) {

		ul_count++;

		/ * Fade in/out * /
		if (ul_count == (PWM_FREQUENCY / (PERIOD_VALUE - INIT_DUTY_VALUE))) {

			/ * Fade in * /
			if (fade_in) {

				ul_duty++;
				if (ul_duty == PERIOD_VALUE) {

					fade_in = 0;
				}
			}
			/ * Fade out * /
			else {

				ul_duty--;
				if (ul_duty == INIT_DUTY_VALUE) {

					fade_in = 1;
				}
			}

			/ * Set new duty cycle * /
			ul_count = 0;
			g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
			pwm_channel_update_duty(PWM, &g_pwm_channel_led, ul_duty);
		}
	}
}


/ **
 * \brief Application entry point for PWM with LED example.
 * Output PWM waves on LEDs to make them fade in and out.
 *
 * \return Unused (ANSI-C compatibility).
 * /
int main(void)
{
	/ * Initialize the SAM system * /
	sysclk_init();
	board_init();


	/ * Enable PWM peripheral clock * /
	pmc_enable_periph_clk(ID_PWM);

	/ * Disable PWM channels for LEDs * /
	pwm_channel_disable(PWM, PIN_PWM_LED0_CHANNEL);
	pwm_channel_disable(PWM, PIN_PWM_LED1_CHANNEL);

	/ * Set PWM clock A as PWM_FREQUENCY * PERIOD_VALUE (clock B is not used) * /
	pwm_clock_t clock_setting = {
		.ul_clka = PWM_FREQUENCY * PERIOD_VALUE,
		.ul_clkb = 0,
		.ul_mck = sysclk_get_cpu_hz()
	};
	pwm_init(PWM, &clock_setting);

	/ * Initialize PWM channel for LED0 (period is left-aligned and output waveform starts at a low level) * /
	g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;  / * Use PWM clock A as source clock * /
	g_pwm_channel_led.ul_period = PERIOD_VALUE;  / * Period value of output waveform * /
	g_pwm_channel_led.ul_duty = 50;  / * Duty cycle value of output waveform * /
	g_pwm_channel_led.channel = PIN_PWM_LED0_CHANNEL;
	pwm_channel_init(PWM, &g_pwm_channel_led);

	/ * Enable channel counter event interrupt * /
	pwm_channel_enable_interrupt(PWM, PIN_PWM_LED0_CHANNEL, 0);

	/ * Configure interrupt and enable PWM interrupt * /

	NVIC_DisableIRQ(PWM_IRQn);
	NVIC_ClearPendingIRQ(PWM_IRQn);
	NVIC_SetPriority(PWM_IRQn, 0);
	NVIC_EnableIRQ(PWM_IRQn);

	/ * Enable PWM channels for LEDs * /
	//pwm_channel_enable(PWM, PIN_PWM_LED0_CHANNEL);

	/ * Infinite loop * /
	while (1) {
		gpio_set_pin_high(PIO_PA16_IDX);
		delay_us(100);
		gpio_set_pin_low(PIO_PA16_IDX);
		delay_us(100);
	}
}*/