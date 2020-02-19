// Copyright 2020 Toggl Desktop developers.

#ifndef SRC_TOGGL_API_SETTINGS_H_
#define SRC_TOGGL_API_SETTINGS_H_

#include "toggl_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

TOGGL_EXPORT bool_t toggl_set_settings_remind_days(
    void *context,
    const bool_t remind_mon,
    const bool_t remind_tue,
    const bool_t remind_wed,
    const bool_t remind_thu,
    const bool_t remind_fri,
    const bool_t remind_sat,
    const bool_t remind_sun);

TOGGL_EXPORT bool_t toggl_set_settings_remind_times(
    void *context,
    const char_t *remind_starts,
    const char_t *remind_ends);

TOGGL_EXPORT bool_t toggl_set_settings_use_idle_detection(
    void *context,
    const bool_t use_idle_detection);

TOGGL_EXPORT bool_t toggl_set_settings_autotrack(
    void *context,
    const bool_t value);

TOGGL_EXPORT bool_t toggl_set_settings_open_editor_on_shortcut(
    void *context,
    const bool_t value);

TOGGL_EXPORT bool_t toggl_set_settings_autodetect_proxy(
    void *context,
    const bool_t autodetect_proxy);

TOGGL_EXPORT bool_t toggl_set_settings_menubar_timer(
    void *context,
    const bool_t menubar_timer);

TOGGL_EXPORT bool_t toggl_set_settings_menubar_project(
    void *context,
    const bool_t menubar_project);

TOGGL_EXPORT bool_t toggl_set_settings_dock_icon(
    void *context,
    const bool_t dock_icon);

TOGGL_EXPORT bool_t toggl_set_settings_on_top(
    void *context,
    const bool_t on_top);

TOGGL_EXPORT bool_t toggl_set_settings_reminder(
    void *context,
    const bool_t reminder);

TOGGL_EXPORT bool_t toggl_set_settings_pomodoro(
    void *context,
    const bool_t pomodoro);

TOGGL_EXPORT bool_t toggl_set_settings_pomodoro_break(
    void *context,
    const bool_t pomodoro_break);

TOGGL_EXPORT bool_t toggl_set_settings_stop_entry_on_shutdown_sleep(
    void *context,
    const bool_t stop_entry);

TOGGL_EXPORT bool_t toggl_set_settings_show_touch_bar(
    void *context,
    const bool_t show_touch_bar);

TOGGL_EXPORT bool_t toggl_set_settings_idle_minutes(
    void *context,
    const uint64_t idle_minutes);

TOGGL_EXPORT bool_t toggl_set_settings_focus_on_shortcut(
    void *context,
    const bool_t focus_on_shortcut);

TOGGL_EXPORT bool_t toggl_set_settings_reminder_minutes(
    void *context,
    const uint64_t reminder_minutes);

TOGGL_EXPORT bool_t toggl_set_settings_pomodoro_minutes(
    void *context,
    const uint64_t pomodoro_minutes);

TOGGL_EXPORT bool_t toggl_set_settings_pomodoro_break_minutes(
    void *context,
    const uint64_t pomodoro_break_minutes);

TOGGL_EXPORT bool_t toggl_set_settings_manual_mode(
    void *context,
    const bool_t manual_mode);

TOGGL_EXPORT bool_t toggl_set_proxy_settings(
    void *context,
    const bool_t use_proxy,
    const char_t *proxy_host,
    const uint64_t proxy_port,
    const char_t *proxy_username,
    const char_t *proxy_password);

TOGGL_EXPORT bool_t toggl_set_window_settings(
    void *context,
    const int64_t window_x,
    const int64_t window_y,
    const int64_t window_height,
    const int64_t window_width);

TOGGL_EXPORT bool_t toggl_window_settings(
    void *context,
    int64_t *window_x,
    int64_t *window_y,
    int64_t *window_height,
    int64_t *window_width);

TOGGL_EXPORT void toggl_set_window_maximized(
    void *context,
    const bool_t value);

TOGGL_EXPORT bool_t toggl_get_window_maximized(
    void *context);

TOGGL_EXPORT void toggl_set_window_minimized(
    void *context,
    const bool_t value);

TOGGL_EXPORT bool_t toggl_get_window_minimized(
    void *context);

TOGGL_EXPORT void toggl_set_window_edit_size_height(
    void *context,
    const int64_t value);

TOGGL_EXPORT int64_t toggl_get_window_edit_size_height(
    void *context);

TOGGL_EXPORT void toggl_set_window_edit_size_width(
    void *context,
    const int64_t value);

TOGGL_EXPORT int64_t toggl_get_window_edit_size_width(
    void *context);

TOGGL_EXPORT void toggl_set_keep_end_time_fixed(
    void *context,
    const bool_t value);

TOGGL_EXPORT bool_t toggl_get_keep_end_time_fixed(
    void *context);

TOGGL_EXPORT bool_t toggl_get_show_touch_bar(
    void *context);

TOGGL_EXPORT void toggl_set_mini_timer_x(
    void *context,
    const int64_t value);

TOGGL_EXPORT int64_t toggl_get_mini_timer_x(
    void *context);

TOGGL_EXPORT void toggl_set_mini_timer_y(
    void *context,
    const int64_t value);

TOGGL_EXPORT int64_t toggl_get_mini_timer_y(
    void *context);

TOGGL_EXPORT void toggl_set_mini_timer_w(
    void *context,
    const int64_t value);

TOGGL_EXPORT int64_t toggl_get_mini_timer_w(
    void *context);

TOGGL_EXPORT void toggl_set_mini_timer_visible(
    void *context,
    const bool_t value);

TOGGL_EXPORT bool_t toggl_get_mini_timer_visible(
    void *context);

#ifdef __cplusplus
}
#endif

#endif // SRC_TOGGL_API_SETTINGS_H_
