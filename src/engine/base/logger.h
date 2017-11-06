/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "ats_type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct uxe_logger_s {
    void (*init)(ctx_t *ctx, const char *name);
    void (*start)(event_logger_t *logger);
    void (*stop)(event_logger_t *logger);
    void (*on_event)(event_logger_t *logger, event_t *e);
    void (*close)(event_logger_t *logger);
};


#ifdef __cplusplus
}
#endif

#endif // __LOGGER_H__
