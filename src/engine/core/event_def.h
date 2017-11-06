/******************************************************************************
 * Automated Trading System                                                   *
 *                                                                            *
 * Copyright (C) 2017 Xiaojun Gao                                             *
 *                                                                            *
 * Distributed under the terms and conditions of the MIT License.             *
 ******************************************************************************/

#ifndef EVENT_DEF_H
#define EVENT_DEF_H

#ifdef __cplusplus
extern "C" {
#endif


#define REMINDER_EVENT                  0
#define ASK_EVENT                       1
#define BID_EVENT                       2
#define TRADE_EVENT                     3
#define LEVEL2_SNAPSHOT_EVENT           4
#define LEVEL2_UPDATE_EVENT             5
#define BAR_EVENT                       6
#define BAR_OPEN_EVENT                  7
#define BAR_SLICE_EVENT                 8
#define NEWS_EVENT                      9
#define FUNDAMENTAL_EVENT               10

#define INSTRUMENT_DEFINE_EVENT         20
#define INSTRUMENT_DEFINE_END_EVENT     21
#define HISTORICAL_DATA_EVENT           22
#define HISTORICAL_DATA_END_EVENT       23

#define ACCOUNT_DATA_EVENT              30
#define ACCOUNT_REPORT_EVENT            31
#define EXECUTION_REPORT_EVENT          32


#define QUEUE_OPEN_EVENT                40
#define QUEUE_CLOSE_EVENT               41
#define SIMULATOR_START_EVENT           42
#define SIMULATOR_STOP_EVENT            43
#define SIMULATOR_PROGRESS_EVENT        44

/* event module */
#define EVENTDEF(_)                                                            \
  /* UppercaseName        LowcaseName         destroy            clone         \
   * dispatch*/                                                                \
  /* timer event */                                                            \
  _(REMINDER, reminder, default, default, reminder)                            \
  /* market events */                                                          \
  _(ASK, ask, default, default, ask)                                           \
  _(BID, bid, default, default, bid)                                           \
  _(TRADE, trade, default, default, trade)                                     \
  _(LEVEL2_SNAPSHOT, level2_snapshot, level2_snapshot, level2_snapshot,        \
    level2_snapshot)                                                           \
  _(LEVEL2_UPDATE, level2_update, level2_update, level2_update, level2_update) \
  _(BAR, bar, default, default, bar)                                           \
  _(BAR_OPEN, bar_open, default, default, default)                            \
  _(BAR_SLICE, bar_slice, default, default, bar_slice)                          \
  _(NEWS, news, news, news, news)                                              \
  _(FUNDAMENTAL, fundamental, default, default, fundamental)                   \
  /* instrument */                                                             \
  _(INSTRUMENT_DEF, instrument_def, default, default, default)          \
  _(INSTRUMENT_DEF_END, instrument_def_end, default, default,                  \
    default)                                                        \
  _(INSTRUMENT_ADDED, instrument_added, default, default, default)    \
  _(INSTRUMENT_REMOVED, instrument_removed, default, default,                  \
    default)                                                          \
  /* historical event */                                                       \
  _(HISTORICAL_DATA, historical_data, default, default, historical_data)       \
  _(HISTORICAL_DATA_END, historical_data_end, default, default,                \
    historical_data_end)                                                       \
  /* execution events */                                                       \
  _(ACCOUNT_DATA, account_data, default, default, account_data)                \
  _(ACCOUNT_REPORT, account_report, account_report, account_report,            \
    account_report)                                                            \
  _(EXECUTION_REPORT, execution_report, execution_report, execution_report,    \
    execution_report)                                                          \
  _(EXECUTION_COMMAND, execution_command, execution_command,                   \
    execution_command, default)                                      \
  /* order event */                                                            \
  _(ORDER_STATUS_CHANGED, order_status_changed, default, default,              \
    order_status_changed)                                                      \
  _(ORDER_FILLED, order_filled, default, default, order_filled)                \
  _(ORDER_PENDING_NEW, order_pending_new, default, default, order_pending_new) \
  _(ORDER_PARTIALLY_FIILED, order_partially_filled, default, default,          \
    order_partially_filled)                                                    \
  _(ORDER_CANCELLED, order_cancelled, default, default, order_cancelled)       \
  _(ORDER_REPLACED, order_replaced, default, default, order_replaced)          \
  _(ORDER_REJECTED, order_rejected, default, default, order_rejected)          \
  _(ORDER_EXPIRED, order_expired, default, default, order_expired)             \
  _(ORDER_CANCEL_REJECTED, order_cancel_rejected, default, default,            \
    order_cancel_rejected)                                                     \
  _(ORDER_REPLACE_REJECTED, order_replace_rejected, default, default,          \
    order_replace_rejected)                                                    \
  _(ORDER_DONE, order_done, default, default, order_done)                      \
  _(ORDER_SENT, order_sent, default, default, order_sent)                      \
  /* portfolio event */                                                        \
  _(PORTFOLIO_ADDED, portfolio_added, default, default, portfolio_added)       \
  _(PORTFOLIO_REMOVED, portfolio_removed, default, default, portfolio_removed) \
  _(PORTFOLIO_PARENT_CHANGED, portfolio_parent_changed, default, default,      \
    portfolio_parent_changed)                                                  \
  _(PORTFOLIO_UPDATE, portfolio_update, default, default, portfolio_update)    \
  /* position event */                                                         \
  _(POSITION_OPENED, position_opened, default, default, position_opened)       \
  _(POSITION_CLOSED, position_closed, default, default, position_closed)       \
  _(POSITION_CHANGED, position_changed, default, default, position_changed)    \
  /* fill */                                                                   \
  _(ON_FILL, on_fill, default, default, on_fill)                               \
  _(ON_TRANSACETION, on_transaction, default, default, on_transaction)         \
  /* stop events */                                                            \
  _(STOP_EXECUTED, stop_executed, default, default, default)             \
  _(STOP_CANCELLED, stop_cancelled, default, default, default)          \
  _(STOP_STATUS_CHANGED, stop_status_changed, default, default,                \
    stop_status_changed)                                                       \
  /* command event */                                                          \
  _(COMMAND, command, default, default, command)                               \
  _(USER_COMMAND, user_command, default, default, user_command)                \
  /* internal events */                                                        \
  _(QUEUE_OPENED, queue_opened, default, default, default)                     \
  _(QUEUE_CLOSED, queue_closed, default, default, default)                     \
  /* */                                                                        \
  _(SIMULATOR_START, simulator_start, default, default, simulator_start)               \
  _(SIMULATOR_STOP, simulator_stop, default, default, simulator_stop)                 \
  _(SIMULATOR_PROGRESS, simulator_progress, default, default, default)         \
  /* */                                                                        \
  _(OPTIMIZATION_START, optimization_start, default, default, optimization_start)         \
  _(OPTIMIZATION_STOP, optimization_stop, default, default, optimization_stop)           \
  _(OPTIMIZATION_PROGRESS, optimization_progress, default, default, optimization_progress)   \
  /* provider event */                                                         \
  _(PROVIDER_ADDED, provider_added, default, default, default)          \
  _(PROVIDER_REMOVED, provider_removed, default, default, default)    \
  _(PROVIDER_STATUS_CHANGED, provider_status_changed, default, default,        \
    default)     \
  _(PROVIDER_CONNECTED, provider_connected, default, default,                  \
    provider_connected)                                                        \
  _(PROVIDER_DISCONNECTED, provider_disconnected, default, default,            \
    provider_disconnected)                                                     \
  _(PROVIDER_ERROR, provider_error, default, default, provider_error)          \
  /* output event */                                                           \
  _(OUTPUT, output, default, default, default)                                 \
  /* group event */                                                            \
  _(GROUP, group, default, default, group)                                   \
  _(GROUP_UPDATE, group_update, default, default, default)                     \
  _(GROUP_EVENT, group_event, default, default, group_event)                       \
  /* exception */                                                              \
  _(EXCEPTION, exception, exception, exception, exception)

#ifdef __cplusplug
}
#endif

#endif // EVENT_DEF_H
