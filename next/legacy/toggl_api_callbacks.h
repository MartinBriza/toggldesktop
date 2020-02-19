// Copyright 2020 Toggl Desktop developers.

#ifndef SRC_TOGGL_API_CALLBACKS_H_
#define SRC_TOGGL_API_CALLBACKS_H_

#include "toggl_api_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Configure the UI callbacks. Required.

TOGGL_EXPORT void toggl_on_show_app(
    void *context,
    TogglDisplayApp cb);

TOGGL_EXPORT void toggl_on_sync_state(
    void *context,
    TogglDisplaySyncState cb);

TOGGL_EXPORT void toggl_on_unsynced_items(
    void *context,
    TogglDisplayUnsyncedItems cb);

TOGGL_EXPORT void toggl_on_error(
    void *context,
    TogglDisplayError cb);

TOGGL_EXPORT void toggl_on_overlay(
    void *context,
    TogglDisplayOverlay cb);

TOGGL_EXPORT void toggl_on_update(
    void *context,
    TogglDisplayUpdate cb);

TOGGL_EXPORT void toggl_on_update_download_state(
    void *context,
    TogglDisplayUpdateDownloadState cb);

TOGGL_EXPORT void toggl_on_message(
    void *context,
    TogglDisplayMessage cb);

TOGGL_EXPORT void toggl_on_online_state(
    void *context,
    TogglDisplayOnlineState cb);

TOGGL_EXPORT void toggl_on_url(
    void *context,
    TogglDisplayURL cb);

TOGGL_EXPORT void toggl_on_login(
    void *context,
    TogglDisplayLogin cb);

TOGGL_EXPORT void toggl_on_reminder(
    void *context,
    TogglDisplayReminder cb);

TOGGL_EXPORT void toggl_on_pomodoro(
    void *context,
    TogglDisplayPomodoro cb);

TOGGL_EXPORT void toggl_on_pomodoro_break(
    void *context,
    TogglDisplayPomodoroBreak cb);

TOGGL_EXPORT void toggl_on_autotracker_notification(
    void *context,
    TogglDisplayAutotrackerNotification cb);

TOGGL_EXPORT void toggl_on_time_entry_list(
    void *context,
    TogglDisplayTimeEntryList cb);

TOGGL_EXPORT void toggl_toggle_entries_group(
    void *context,
    const char_t *name);

TOGGL_EXPORT void toggl_on_timeline(
    void *context,
    TogglDisplayTimeline cb);

TOGGL_EXPORT void toggl_on_mini_timer_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb);

TOGGL_EXPORT void toggl_on_help_articles(
    void *context,
    TogglDisplayHelpArticles cb);

TOGGL_EXPORT void toggl_on_time_entry_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb);

TOGGL_EXPORT void toggl_on_project_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb);

TOGGL_EXPORT void toggl_on_workspace_select(
    void *context,
    TogglDisplayViewItems cb);

TOGGL_EXPORT void toggl_on_client_select(
    void *context,
    TogglDisplayViewItems cb);

TOGGL_EXPORT void toggl_on_tags(
    void *context,
    TogglDisplayViewItems cb);

TOGGL_EXPORT void toggl_on_time_entry_editor(
    void *context,
    TogglDisplayTimeEntryEditor cb);

TOGGL_EXPORT void toggl_on_settings(
    void *context,
    TogglDisplaySettings cb);

TOGGL_EXPORT void toggl_on_timer_state(
    void *context,
    TogglDisplayTimerState cb);

TOGGL_EXPORT void toggl_on_idle_notification(
    void *context,
    TogglDisplayIdleNotification cb);

TOGGL_EXPORT void toggl_on_autotracker_rules(
    void *context,
    TogglDisplayAutotrackerRules cb);

TOGGL_EXPORT void toggl_on_project_colors(
    void *context,
    TogglDisplayProjectColors cb);

TOGGL_EXPORT void toggl_on_countries(
    void *context,
    TogglDisplayCountries cb);

TOGGL_EXPORT void toggl_on_promotion(
    void *context,
    TogglDisplayPromotion cb);

TOGGL_EXPORT void toggl_on_obm_experiment(
    void *context,
    TogglDisplayObmExperiment cb);

#ifdef __cplusplus
}
#endif

#endif // SRC_TOGGL_API_CALLBACKS_H_

