/*
 * (C) Copyright 1992, ..., 2014 the "DOSEMU-Development-Team".
 *
 * for details see file COPYING.DOSEMU in the DOSEMU distribution
 */

#ifndef COOPTH_H
#define COOPTH_H

#define COOPTH_TID_INVALID (-1)

typedef void (*coopth_func_t)(void *arg);
typedef void (*coopth_hndl_t)(int tid, void *arg, void *arg2);
enum { COOPTH_SL_YIELD, COOPTH_SL_WAIT, COOPTH_SL_SLEEP };
typedef void (*coopth_sleep_hndl_t)(int tid, int sl_state);

void coopth_init(void);
int coopth_create(const char *name, coopth_func_t func);
int coopth_create_multi(const char *name, int len, coopth_func_t func);
int coopth_create_vm86(const char *name, coopth_func_t func,
	void (*post)(void), uint16_t *hlt_off);
int coopth_start(int tid, void *arg);
int coopth_set_permanent_post_handler(int tid, coopth_hndl_t func);
int coopth_set_ctx_handlers(int tid, coopth_hndl_t pre, coopth_hndl_t post,
	void *arg);
int coopth_set_sleep_handlers(int tid, coopth_sleep_hndl_t pre,
	coopth_hndl_t post);
int coopth_add_post_handler(coopth_func_t func, void *arg);
void coopth_join_vm86(int tid);
int coopth_flush_vm86(void);
int coopth_set_detached(int tid);
int coopth_unsafe_detach(int tid, const char *who);
void coopth_unsafe_shutdown(void);
int coopth_set_sleep_handler(coopth_func_t func, void *arg);
int coopth_set_cleanup_handler(coopth_func_t func, void *arg);
void coopth_push_user_data(int tid, void *udata);
void coopth_push_user_data_cur(void *udata);
void *coopth_pop_user_data(int tid);
void *coopth_pop_user_data_cur(void);
int coopth_get_tid(void);
void coopth_ensure_sleeping(int tid);
void coopth_ensure_single(int tid);
void coopth_yield(void);
void coopth_wait(void);
void coopth_sleep(void);
int coopth_sched(void);
int coopth_sched_cond(void);
void coopth_detach(void);
void coopth_attach_to_cur(int tid);
void coopth_attach(void);
void coopth_leave(void);
void coopth_abandon(void);
void coopth_exit(void);
void coopth_wake_up(int tid);
void coopth_cancel(int tid);
void coopth_done(void);
void coopth_run(void);
void coopth_run_tid(int tid);
int coopth_wants_sleep_vm86(void);
void coopth_set_ctx_checker_vm86(int (*checker)(void));
void coopth_cancel_disable(void);
void coopth_cancel_enable(void);
void coopth_dump(int all);
void coopth_set_nothread_notifier(void (*notifier)(void));

#endif
