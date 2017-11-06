/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __TYPES_H__
#define __TYPES_H__

#include "common.h"
#include "currency.h"
#include "datetime.h"
#include "idarray.h"
#include "vec.h"
#include "mem.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ids */
typedef int16_t  sid_t;
typedef int32_t  lid_t;

#define PID_MAX 1024

/* atomic type */
typedef intptr_t atomic_t;

typedef struct ctx_s ctx_t;

typedef struct event_s event_t;
//typedef struct event_tick_s tick_event_t;

typedef struct uxe_logger_s event_logger_t;

typedef struct instrument_s instrument_t;
typedef struct order_s order_t;

typedef struct fill_s fill_t;
typedef struct transaction_s transaction_t;

typedef struct position_s position_t;
typedef struct account_s account_t;
typedef struct portfolio_s portfolio_t;

typedef struct provider_s provider_t;
typedef struct data_provider_s data_provider_t;
typedef struct fundamental_provider_s fundamental_provider_t;
typedef struct news_provider_s news_provider_t;
typedef struct execution_provider_s execution_provider_t;
typedef struct historical_provider_s historical_provider_t;

typedef struct stop_s stop_t;
typedef struct strategy_s strategy_t;

typedef struct indicator_s indicator_t;

typedef struct fill_series_s fill_series_t;
typedef struct time_series_s time_series_t;
typedef struct tick_series_s tick_series_t;
typedef struct bar_series_s bar_series_t;

/* list all error code */
#define OK   0
#define ERR -1

#ifdef __cplusplus
}
#endif

#endif // __TYPES_H__
