/*
 * rtc.c
 *
 * Created: 6/1/2013 7:28:37 AM
 *  Author: ViDAR
 */ 

#include <asf.h>
#include <board.h>
#include "rtc.h"


/**
 * \brief Interrupt handler for the RTC. Refresh the display.
 */
void RTC_Handler(void)
{
	uint32_t ul_status = rtc_get_status(RTC);

	/* Second increment interrupt */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		/* Disable RTC interrupt */
		rtc_disable_interrupt(RTC, RTC_IDR_SECDIS);

		rtc_clear_status(RTC, RTC_SCCR_SECCLR);

		rtc_enable_interrupt(RTC, RTC_IER_SECEN);
	} else {
		/* Time or date alarm */
		if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
			/* Disable RTC interrupt */
			rtc_disable_interrupt(RTC, RTC_IDR_ALRDIS);

			gs_ul_alarm_triggered = 1;
			rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
			rtc_enable_interrupt(RTC, RTC_IER_ALREN);
		}
	}
}

/**
 * \brief Calculate week from year, month, day.
 */
static uint32_t calculate_week(uint32_t ul_year, uint32_t ul_month,
		uint32_t ul_day)
{
	uint32_t ul_week;

	if (ul_month == 1 || ul_month == 2) {
		ul_month += 12;
		--ul_year;
	}

	ul_week = (ul_day + 2 * ul_month + 3 * (ul_month + 1) / 5 + ul_year +
			ul_year / 4 - ul_year / 100 + ul_year / 400) % 7;

	++ul_week;

	return ul_week;
}