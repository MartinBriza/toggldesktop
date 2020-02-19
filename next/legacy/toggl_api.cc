// Copyright 2020 Toggl Desktop developers.

#include "toggl_api.h"
#include "toggl_api_callbacks_private.h"

#include "context.h"

#include "model/country.h"
#include "misc/error.h"
#include "toggl_api_tools.h"

using namespace toggl;

// Initialize/destroy an instance of the app

void *toggl_context_init(
    const char_t *app_name,
    const char_t *app_version) {
    return new Context(app_name, Context::Callbacks{});
}

void toggl_context_clear(
    void *context) { }

// Set environment. By default, production is assumed. Optional.

void toggl_set_environment(
    void *context,
    const char_t *environment) { }

// You must free() the result
char_t *toggl_environment(
    void *context) {
    return nullptr;
}

// Optionally, disable update check

void toggl_disable_update_check(
    void *context) { }

// CA cert bundle must be configured from UI

void toggl_set_cacert_path(
    void *context,
    const char_t *path) { }

// DB path must be configured from UI

bool_t toggl_set_db_path(
    void *context,
    const char_t *path) {
    return false;
}

// Configure update download path for silent updates
// Need to configure only if you have
// enabled update check and have not set the
// display update callback
void toggl_set_update_path(
    void *context,
    const char_t *path) { }

// you must free the result
char_t *toggl_update_path(
    void *context) {
    return nullptr;
}

// Log path must be configured from UI

void toggl_set_log_path(
    const char_t *path) { }

// Log level is optional

void toggl_set_log_level(
    const char_t *level) { }

// Various parts of UI can tell the app to show itself.

void toggl_show_app(
    void *context) { }

// Configure the UI callbacks. Required.

void toggl_toggle_entries_group(
    void *context,
    const char_t *name) { }

// After UI callbacks are configured, start pumping UI events

bool_t toggl_ui_start(
    void *context) {
    reinterpret_cast<Context*>(context)->UiStart();
    return true;
}

// User interaction with the app

bool_t toggl_login(
    void *context,
    const char_t *email,
    const char_t *password) {
    reinterpret_cast<Context*>(context)->Login(email, password);
    return true;
}

bool_t toggl_login_async(
    void *context,
    const char_t *email,
    const char_t *password) {
    // both are actually async
    return toggl_login(context, email, password);
}

bool_t toggl_signup(
    void *context,
    const char_t *email,
    const char_t *password,
    const uint64_t country_id) {
    return false;
}

bool_t toggl_signup_async(
    void *context,
    const char_t *email,
    const char_t *password,
    const uint64_t country_id) {
    return false;
}

bool_t toggl_google_signup(
    void *context,
    const char_t *access_token,
    const uint64_t country_id) {
    return false;
}

bool_t toggl_google_signup_async(
    void *context,
    const char_t *access_token,
    const uint64_t country_id) {
    return false;
}

bool_t toggl_google_login(
    void *context,
    const char_t *access_token) {
    return false;
}

bool_t toggl_google_login_async(
    void *context,
    const char_t *access_token) {
    return false;
}

void toggl_password_forgot(
    void *context) { }

void toggl_tos(
    void *context) { }

void toggl_privacy_policy(
    void *context) { }

void toggl_open_in_browser(
    void *context) { }

bool_t toggl_accept_tos(
    void *context) {
    return false;
}

void toggl_get_support(
    void *context,
    const int type) { }

bool_t toggl_feedback_send(
    void *context,
    const char_t *topic,
    const char_t *details,
    const char_t *filename) {
    return false;
}

void toggl_search_help_articles(
    void *context,
    const char_t *keywords) { }

void toggl_view_time_entry_list(
    void *context) { }

void toggl_view_timeline_data(
    void *context) { }

void toggl_view_timeline_prev_day(
    void *context) { }

void toggl_view_timeline_next_day(
    void *context) { }

void toggl_view_timeline_current_day(
    void *context) { }

void toggl_view_timeline_set_day(
    void *context,
    const int64_t unix_timestamp) { }

void toggl_edit(
    void *context,
    const char_t *guid,
    const bool_t edit_running_time_entry,
    const char_t *focused_field_name) {
    auto ctx = reinterpret_cast<Context*>(context);
    if (contextMap.contains(ctx) && contextMap[ctx].OnTimeEntryEditor) {
        TogglTimeEntryView *view { nullptr };
        if (edit_running_time_entry) {
            auto te = const_cast<const UserData*>(ctx->GetData())->RunningTimeEntry();
            if (!te)
                return;
            view = time_entry_view_item_init(te);
        }
        else {
            locked<const TimeEntryModel> te = ctx->GetData()->TimeEntries.byGUIDconst(uuid_t(guid));
            if (!te)
                return;
            view = time_entry_view_item_init(te);
        }
        contextMap[ctx].OnTimeEntryEditor(true, view, focused_field_name);
    }
}

void toggl_edit_preferences(
    void *context) { }

bool_t toggl_continue(
    void *context,
    const char_t *guid) {
    return false;
}

bool_t toggl_continue_latest(
    void *context,
    const bool_t prevent_on_app) {
    return false;
}

bool_t toggl_delete_time_entry(
    void *context,
    const char_t *guid) {
    return false;
}

bool_t toggl_set_time_entry_duration(
    void *context,
    const char_t *guid,
    const char_t *value) {
    return false;
}

bool_t toggl_set_time_entry_project(
    void *context,
    const char_t *guid,
    const uint64_t task_id,
    const uint64_t project_id,
    const char_t *project_guid) {
    return false;
}

bool_t toggl_set_time_entry_date(
    void *context,
    const char_t *guid,
    const int64_t unix_timestamp) {
    return false;
}

bool_t toggl_set_time_entry_start(
    void *context,
    const char_t *guid,
    const char_t *value) {
    return false;
}

bool_t toggl_set_time_entry_start_timestamp(
    void *context,
    const char_t *guid,
    const int64_t start) {
    return false;
}

bool_t toggl_set_time_entry_end(
    void *context,
    const char_t *guid,
    const char_t *value) {
    return false;
}

bool_t toggl_set_time_entry_end_timestamp(
    void *context,
    const char_t *guid,
    const int64_t end) {
    return false;
}

// value is '\t' separated tag list
bool_t toggl_set_time_entry_tags(
    void *context,
    const char_t *guid,
    const char_t *value) {
    return false;
}

bool_t toggl_set_time_entry_billable(
    void *context,
    const char_t *guid,
    bool_t value) {
    return false;
}

bool_t toggl_set_time_entry_description(
    void *context,
    const char_t *guid,
    const char_t *value) {
    return false;
}

bool_t toggl_stop(
    void *context,
    const bool_t prevent_on_app) {
    auto ctx = reinterpret_cast<Context*>(context);
    ctx->Stop();
}

bool_t toggl_discard_time_at(
    void *context,
    const char_t *guid,
    const int64_t at,
    const bool_t split_into_new_entry) {
    return false;
}

bool_t toggl_discard_time_and_continue(
    void *context,
    const char_t *guid,
    const int64_t at) {
    return false;
}

void toggl_set_key_start(
    void *context,
    const char_t *value) { }

// You must free() the result
char_t *toggl_get_key_start(
    void *context) {
    return nullptr;
}

void toggl_set_key_show(
    void *context,
    const char_t *value) { }

// You must free() the result
char_t *toggl_get_key_show(
    void *context) {
    return nullptr;
}

void toggl_set_key_modifier_show(
    void *context,
    const char_t *value) { }

// You must free() the result
char_t *toggl_get_key_modifier_show(
    void *context) { }

void toggl_set_key_modifier_start(
    void *context,
    const char_t *value) { }

// You must free() the result
char_t *toggl_get_key_modifier_start(
    void *context) {
    return nullptr;
}

bool_t toggl_logout(
    void *context) {
    return false;
}

bool_t toggl_clear_cache(
    void *context) {
    return false;
}

// returns GUID of the started time entry. you must free() the result
char_t *toggl_start(
    void *context,
    const char_t *description,
    const char_t *duration,
    const uint64_t task_id,
    const uint64_t project_id,
    const char_t *project_guid,
    const char_t *tags,
    const bool_t prevent_on_app,
    const uint64_t started,
    const uint64_t ended) {
    Context *ctx = reinterpret_cast<Context*>(context);

    uuid_t taskUuid;
    uuid_t projectUuid;
    if (task_id) {
        taskUuid = ctx->GetData()->Tasks.byId(task_id)->GUID();
    }
    if (project_id) {
        projectUuid = ctx->GetData()->Tasks.byId(project_id)->GUID();
    }
    else if (project_guid) {
        projectUuid = uuid_t(project_guid);
    }

    return copy_string(ctx->Start(description, duration, taskUuid, projectUuid, tags, started, ended));
}

// returns GUID of the new project. you must free() the result
char_t *toggl_add_project(
    void *context,
    const char_t *time_entry_guid,
    const uint64_t workspace_id,
    const uint64_t client_id,
    const char_t *client_guid,
    const char_t *project_name,
    const bool_t is_private,
    const char_t *project_color) {
    return nullptr;
}

// returns GUID of the new client. you must free() the result
char_t *toggl_create_client(
    void *context,
    const uint64_t workspace_id,
    const char_t *client_name) {
    return nullptr;
}

bool_t toggl_add_obm_action(
    void *context,
    const uint64_t experiment_id,
    const char_t *key,
    const char_t *value) {
    return false;
}

void toggl_add_obm_experiment_nr(
    const uint64_t nr) { }

bool_t toggl_set_default_project(
    void *context,
    const uint64_t pid,
    const uint64_t tid) {
    return false;
}

void toggl_get_project_colors(
    void *context) { }

void toggl_get_countries(
    void *context) {
    reinterpret_cast<Context*>(context)->GetCountries();
}

void toggl_get_countries_async(
    void *context) {
    reinterpret_cast<Context*>(context)->GetCountries();
}

// You must free() the result
char_t *toggl_get_default_project_name(
    void *context) {
    return nullptr;
}

uint64_t toggl_get_default_project_id(
    void *context) { }

uint64_t toggl_get_default_task_id(
    void *context) { }

bool_t toggl_set_update_channel(
    void *context,
    const char_t *update_channel) {
    return false;
}

// You must free() the result
char_t *toggl_get_update_channel(
    void *context) {
    return nullptr;
}

// You must free() the result
char_t *toggl_get_user_fullname(
    void *context) {
    return nullptr;
}

// You must free() the result
char_t *toggl_get_user_email(
    void *context) {
    return nullptr;
}

void toggl_sync(
    void *context) { }

void toggl_fullsync(
    void *context) { }

bool_t toggl_timeline_toggle_recording(
    void *context,
    const bool_t record_timeline) {
    return false;
}

bool_t toggl_timeline_is_recording_enabled(
    void *context) {
    return false;
}

void toggl_set_sleep(
    void *context) { }

void toggl_set_wake(
    void *context) { }

void toggl_set_locked(
    void *context) { }

void toggl_set_unlocked(
    void *context) { }

void toggl_os_shutdown(
    void *context) { }

// Notify lib that client is online again.
void toggl_set_online(
    void *context) { }

void toggl_set_idle_seconds(
    void *context,
    const uint64_t idle_seconds) { }

bool_t toggl_set_promotion_response(
    void *context,
    const int64_t promotion_type,
    const int64_t promotion_response) {
    return false;
}

// Shared helpers

// You must free() the result
char_t *toggl_format_tracking_time_duration(
    const int64_t duration_in_seconds) {
    return nullptr;
}

// You must free() the result
char_t *toggl_format_tracked_time_duration(
    const int64_t duration_in_seconds) {
    return nullptr;
}

int64_t toggl_parse_duration_string_into_seconds(
    const char_t *duration_string) { }

// Write to the lib logger
void toggl_debug(
    const char_t *text) { }

// Check if sizeof view struct matches those in UI
// Else stuff blows up when Marshalling in C#
// Will return error string if size is invalid,
// you must free() the result
char_t *toggl_check_view_struct_size(
    const int time_entry_view_item_size,
    const int autocomplete_view_item_size,
    const int view_item_size,
    const int settings_size,
    const int autotracker_view_item_size) {
    return nullptr;
}

// You must free() the result
char_t *toggl_run_script(
    void *context,
    const char_t *script,
    int64_t *err) {
    return nullptr;
}

int64_t toggl_autotracker_add_rule(
    void *context,
    const char_t *term,
    const uint64_t project_id,
    const uint64_t task_id) { }

bool_t toggl_autotracker_delete_rule(
    void *context,
    const int64_t id) {
    return false;
}

// Testing helpers. May change any time
void testing_sleep(
    const int seconds) { }

// For testing only
bool_t testing_set_logged_in_user(
    void *context,
    const char *json) {
    return false;
}

void toggl_load_more(
    void *context) { }

void track_window_size(
    void *context,
    const uint64_t width,
    const uint64_t height) { }

void track_edit_size(
    void *context,
    const uint64_t width,
    const uint64_t height) { }

void toggl_iam_click(
    void *context,
    const uint64_t type) { }
