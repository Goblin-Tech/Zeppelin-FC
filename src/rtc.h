/*
 * rtc.h
 *
 * Created: 6/1/2013 7:33:29 AM
 *  Author: ViDAR
 */ 


#ifndef RTC_H_
#define RTC_H_

/* Edited hour. */
static uint32_t gs_ul_new_hour;
/* Edited minute. */
static uint32_t gs_ul_new_minute;
/* Edited second. */
static uint32_t gs_ul_new_second;

/* Edited year. */
static uint32_t gs_ul_new_year;
/* Edited month. */
static uint32_t gs_ul_new_month;
/* Edited day. */
static uint32_t gs_ul_new_day;
/* Edited day-of-the-week. */
static uint32_t gs_ul_new_week;

/* Indicate if alarm has been triggered and not yet cleared */
static uint32_t gs_ul_alarm_triggered;

/* Time string */
static uint8_t gs_uc_rtc_time[8 + 1];
/* Date string */
static uint8_t gs_uc_date[10 + 1];
/* Week string */
static uint8_t gs_uc_day_names[7][4];



#endif /* RTC_H_ */