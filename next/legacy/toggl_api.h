// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_TOGGL_API_H_
#define SRC_TOGGL_API_H_

// Models
#include "toggl_api_types.h"
// Callback setters
#include "toggl_api_callbacks.h"
// Settings access
#include "toggl_api_settings.h"

#ifdef __cplusplus
extern "C" {
#endif

    // Initialize/destroy an instance of the app

    TOGGL_EXPORT void *toggl_context_init(
        const char_t *app_name,
        const char_t *app_version);

    TOGGL_EXPORT void toggl_context_clear(
        void *context);

    // Set environment. By default, production is assumed. Optional.

    TOGGL_EXPORT void toggl_set_environment(
        void *context,
        const char_t *environment);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_environment(
        void *context);

    // Optionally, disable update check

    TOGGL_EXPORT void toggl_disable_update_check(
        void *context);

    // CA cert bundle must be configured from UI

    TOGGL_EXPORT void toggl_set_cacert_path(
        void *context,
        const char_t *path);

    // DB path must be configured from UI

    TOGGL_EXPORT bool_t toggl_set_db_path(
        void *context,
        const char_t *path);

    // Configure update download path for silent updates
    // Need to configure only if you have
    // enabled update check and have not set the
    // display update callback
    TOGGL_EXPORT void toggl_set_update_path(
        void *context,
        const char_t *path);

    // you must free the result
    TOGGL_EXPORT char_t *toggl_update_path(
        void *context);

    // Log path must be configured from UI

    TOGGL_EXPORT void toggl_set_log_path(
        const char_t *path);

    // Log level is optional

    TOGGL_EXPORT void toggl_set_log_level(
        const char_t *level);

    // Various parts of UI can tell the app to show itself.

    TOGGL_EXPORT void toggl_show_app(
        void *context);

    TOGGL_EXPORT bool_t toggl_ui_start(
        void *context);

    // User interaction with the app

    TOGGL_EXPORT bool_t toggl_login(
        void *context,
        const char_t *email,
        const char_t *password);

    TOGGL_EXPORT bool_t toggl_login_async(
        void *context,
        const char_t *email,
        const char_t *password);

    TOGGL_EXPORT bool_t toggl_signup(
        void *context,
        const char_t *email,
        const char_t *password,
        const uint64_t country_id);

    TOGGL_EXPORT bool_t toggl_signup_async(
        void *context,
        const char_t *email,
        const char_t *password,
        const uint64_t country_id);

    TOGGL_EXPORT bool_t toggl_google_signup(
        void *context,
        const char_t *access_token,
        const uint64_t country_id);

    TOGGL_EXPORT bool_t toggl_google_signup_async(
        void *context,
        const char_t *access_token,
        const uint64_t country_id);

    TOGGL_EXPORT bool_t toggl_google_login(
        void *context,
        const char_t *access_token);

    TOGGL_EXPORT bool_t toggl_google_login_async(
        void *context,
        const char_t *access_token);

    TOGGL_EXPORT void toggl_password_forgot(
        void *context);

    TOGGL_EXPORT void toggl_tos(
        void *context);

    TOGGL_EXPORT void toggl_privacy_policy(
        void *context);

    TOGGL_EXPORT void toggl_open_in_browser(
        void *context);

    TOGGL_EXPORT bool_t toggl_accept_tos(
        void *context);

    TOGGL_EXPORT void toggl_get_support(
        void *context,
        const int type);

    TOGGL_EXPORT bool_t toggl_feedback_send(
        void *context,
        const char_t *topic,
        const char_t *details,
        const char_t *filename);

    TOGGL_EXPORT void toggl_search_help_articles(
        void *context,
        const char_t *keywords);

    TOGGL_EXPORT void toggl_view_time_entry_list(
        void *context);

    TOGGL_EXPORT void toggl_view_timeline_data(
        void *context);

    TOGGL_EXPORT void toggl_view_timeline_prev_day(
        void *context);

    TOGGL_EXPORT void toggl_view_timeline_next_day(
        void *context);

    TOGGL_EXPORT void toggl_view_timeline_current_day(
        void *context);

    TOGGL_EXPORT void toggl_view_timeline_set_day(
        void *context,
        const int64_t unix_timestamp);

    TOGGL_EXPORT void toggl_edit(
        void *context,
        const char_t *guid,
        const bool_t edit_running_time_entry,
        const char_t *focused_field_name);

    TOGGL_EXPORT void toggl_edit_preferences(
        void *context);

    TOGGL_EXPORT bool_t toggl_continue(
        void *context,
        const char_t *guid);

    TOGGL_EXPORT bool_t toggl_continue_latest(
        void *context,
        const bool_t prevent_on_app);

    TOGGL_EXPORT bool_t toggl_delete_time_entry(
        void *context,
        const char_t *guid);

    TOGGL_EXPORT bool_t toggl_set_time_entry_duration(
        void *context,
        const char_t *guid,
        const char_t *value);

    TOGGL_EXPORT bool_t toggl_set_time_entry_project(
        void *context,
        const char_t *guid,
        const uint64_t task_id,
        const uint64_t project_id,
        const char_t *project_guid);

    TOGGL_EXPORT bool_t toggl_set_time_entry_date(
        void *context,
        const char_t *guid,
        const int64_t unix_timestamp);

    TOGGL_EXPORT bool_t toggl_set_time_entry_start(
        void *context,
        const char_t *guid,
        const char_t *value);

    TOGGL_EXPORT bool_t toggl_set_time_entry_start_timestamp(
        void *context,
        const char_t *guid,
        const int64_t start);

    TOGGL_EXPORT bool_t toggl_set_time_entry_end(
        void *context,
        const char_t *guid,
        const char_t *value);

    TOGGL_EXPORT bool_t toggl_set_time_entry_end_timestamp(
        void *context,
        const char_t *guid,
        const int64_t end);

    // value is '\t' separated tag list
    TOGGL_EXPORT bool_t toggl_set_time_entry_tags(
        void *context,
        const char_t *guid,
        const char_t *value);

    TOGGL_EXPORT bool_t toggl_set_time_entry_billable(
        void *context,
        const char_t *guid,
        bool_t value);

    TOGGL_EXPORT bool_t toggl_set_time_entry_description(
        void *context,
        const char_t *guid,
        const char_t *value);

    TOGGL_EXPORT bool_t toggl_stop(
        void *context,
        const bool_t prevent_on_app);

    TOGGL_EXPORT bool_t toggl_discard_time_at(
        void *context,
        const char_t *guid,
        const int64_t at,
        const bool_t split_into_new_entry);

    TOGGL_EXPORT bool_t toggl_discard_time_and_continue(
        void *context,
        const char_t *guid,
        const int64_t at);

    TOGGL_EXPORT void toggl_set_key_start(
        void *context,
        const char_t *value);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_key_start(
        void *context);

    TOGGL_EXPORT void toggl_set_key_show(
        void *context,
        const char_t *value);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_key_show(
        void *context);

    TOGGL_EXPORT void toggl_set_key_modifier_show(
        void *context,
        const char_t *value);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_key_modifier_show(
        void *context);

    TOGGL_EXPORT void toggl_set_key_modifier_start(
        void *context,
        const char_t *value);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_key_modifier_start(
        void *context);

    TOGGL_EXPORT bool_t toggl_logout(
        void *context);

    TOGGL_EXPORT bool_t toggl_clear_cache(
        void *context);

    // returns GUID of the started time entry. you must free() the result
    TOGGL_EXPORT char_t *toggl_start(
        void *context,
        const char_t *description,
        const char_t *duration,
        const uint64_t task_id,
        const uint64_t project_id,
        const char_t *project_guid,
        const char_t *tags,
        const bool_t prevent_on_app,
        const uint64_t started,
        const uint64_t ended);

    // returns GUID of the new project. you must free() the result
    TOGGL_EXPORT char_t *toggl_add_project(
        void *context,
        const char_t *time_entry_guid,
        const uint64_t workspace_id,
        const uint64_t client_id,
        const char_t *client_guid,
        const char_t *project_name,
        const bool_t is_private,
        const char_t *project_color);

    // returns GUID of the new client. you must free() the result
    TOGGL_EXPORT char_t *toggl_create_client(
        void *context,
        const uint64_t workspace_id,
        const char_t *client_name);

    TOGGL_EXPORT bool_t toggl_add_obm_action(
        void *context,
        const uint64_t experiment_id,
        const char_t *key,
        const char_t *value);

    TOGGL_EXPORT void toggl_add_obm_experiment_nr(
        const uint64_t nr);

    TOGGL_EXPORT bool_t toggl_set_default_project(
        void *context,
        const uint64_t pid,
        const uint64_t tid);

    TOGGL_EXPORT void toggl_get_project_colors(
        void *context);

    TOGGL_EXPORT void toggl_get_countries(
        void *context);

    TOGGL_EXPORT void toggl_get_countries_async(
        void *context);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_default_project_name(
        void *context);

    TOGGL_EXPORT uint64_t toggl_get_default_project_id(
        void *context);

    TOGGL_EXPORT uint64_t toggl_get_default_task_id(
        void *context);

    TOGGL_EXPORT bool_t toggl_set_update_channel(
        void *context,
        const char_t *update_channel);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_update_channel(
        void *context);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_user_fullname(
        void *context);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_get_user_email(
        void *context);

    TOGGL_EXPORT void toggl_sync(
        void *context);

    TOGGL_EXPORT void toggl_fullsync(
        void *context);

    TOGGL_EXPORT bool_t toggl_timeline_toggle_recording(
        void *context,
        const bool_t record_timeline);

    TOGGL_EXPORT bool_t toggl_timeline_is_recording_enabled(
        void *context);

    TOGGL_EXPORT void toggl_set_sleep(
        void *context);

    TOGGL_EXPORT void toggl_set_wake(
        void *context);

    TOGGL_EXPORT void toggl_set_locked(
        void *context);

    TOGGL_EXPORT void toggl_set_unlocked(
        void *context);

    TOGGL_EXPORT void toggl_os_shutdown(
        void *context);

    // Notify lib that client is online again.
    TOGGL_EXPORT void toggl_set_online(
        void *context);

    TOGGL_EXPORT void toggl_set_idle_seconds(
        void *context,
        const uint64_t idle_seconds);

    TOGGL_EXPORT bool_t toggl_set_promotion_response(
        void *context,
        const int64_t promotion_type,
        const int64_t promotion_response);

    // Shared helpers

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_format_tracking_time_duration(
        const int64_t duration_in_seconds);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_format_tracked_time_duration(
        const int64_t duration_in_seconds);

    TOGGL_EXPORT int64_t toggl_parse_duration_string_into_seconds(
        const char_t *duration_string);

    // Write to the lib logger
    TOGGL_EXPORT void toggl_debug(
        const char_t *text);

    // Check if sizeof view struct matches those in UI
    // Else stuff blows up when Marshalling in C#
    // Will return error string if size is invalid,
    // you must free() the result
    TOGGL_EXPORT char_t *toggl_check_view_struct_size(
        const int time_entry_view_item_size,
        const int autocomplete_view_item_size,
        const int view_item_size,
        const int settings_size,
        const int autotracker_view_item_size);

    // You must free() the result
    TOGGL_EXPORT char_t *toggl_run_script(
        void *context,
        const char_t *script,
        int64_t *err);

    TOGGL_EXPORT int64_t toggl_autotracker_add_rule(
        void *context,
        const char_t *term,
        const uint64_t project_id,
        const uint64_t task_id);

    TOGGL_EXPORT bool_t toggl_autotracker_delete_rule(
        void *context,
        const int64_t id);

    // Testing helpers. May change any time
    TOGGL_EXPORT void testing_sleep(
        const int seconds);

    // For testing only
    TOGGL_EXPORT bool_t testing_set_logged_in_user(
        void *context,
        const char *json);

    TOGGL_EXPORT void toggl_load_more(
        void *context);

    TOGGL_EXPORT void track_window_size(
        void *context,
        const uint64_t width,
        const uint64_t height);

    TOGGL_EXPORT void track_edit_size(
        void *context,
        const uint64_t width,
        const uint64_t height);

    TOGGL_EXPORT void toggl_iam_click(
        void *context,
        const uint64_t type);

#ifdef __cplusplus
}
#endif

#endif // SRC_TOGGL_API_H_
