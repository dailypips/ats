/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __DATE_TIME_H__
#define __DATE_TIME_H__

#include "common.h"
#include <sys/time.h> // timeval
#include <time.h>   // struct tm

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t datetime_t;
typedef int64_t timespan_t;

/* datetime module */
/* datedatetime_t type represents a date and time.
   that stores the date and time as the number of microsecond intervals since
   12:00 AM January 1, year 1 A.D (as Day 0). in the proleptic Gregorian Calendar.
*/

#define EPOCH_DATE_TIME UINT64_C(62135596800000000)  /* 1970-01-01T00:00:00:00 */
#define MIN_DATE_TIME UINT64_C(0)                    /* 0001-01-01T00:00:00:00 */
#define MAX_DATE_TIME UINT64_C(315537897599999999)   /* 9999-12-31T23:59:59.999999*/

#define TICKS_PER_SECOND UINT64_C(1000000)
#define TICKS_PER_DAY UINT64_C(86400000000)

EXTERN datetime_t datetime_from_ymd(uint16_t year, uint16_t month, uint16_t day);
EXTERN datetime_t datetime_from_hmsu(uint16_t hour, uint16_t minute, uint16_t second, uint32_t usec);
EXTERN datetime_t datetime_from_timeval(struct timeval *t);
/* result = 0 means ok, other means error */
EXTERN int datetime_from_iso8601(const char *str, size_t len, datetime_t *dt);
EXTERN void datetime_decode(datetime_t dt, uint16_t *year, uint16_t *month, uint16_t *day, uint16_t *hour,
                               uint16_t *minute, uint16_t *second, uint32_t *usec);
EXTERN int datetime_to_tm(const datetime_t dt, struct tm *tmp);
EXTERN size_t datetime_format(char *dst, size_t len, datetime_t dt, long offset /* see timeout_offset */);
/* return timezone offset from utc, in minutes [-1439,1439] */
EXTERN long get_timezone_offset(void);
EXTERN void timezone_update(void);
EXTERN datetime_t datetime_now(void);


#ifdef __cplusplus
}
#endif

#endif /*__DATE_TIME_H__*/
