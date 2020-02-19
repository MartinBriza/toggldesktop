// Copyright 2020 Toggl Desktop developers.

#include "toggl_api_callbacks.h"
#include "toggl_api_callbacks_private.h"

#include "toggl_api_tools.h"
#include "context.h"

using namespace toggl;

void toggl_on_show_app(
    void *context,
    TogglDisplayApp cb) {
    reinterpret_cast<Context*>(context)->GetCallbacks()->OnShowApp = [cb](bool open) {
        cb(open);
    };
}

void toggl_on_sync_state(
    void *context,
    TogglDisplaySyncState cb) { }

void toggl_on_unsynced_items(
    void *context,
    TogglDisplayUnsyncedItems cb) { }

void toggl_on_error(
    void *context,
    TogglDisplayError cb) {
    reinterpret_cast<Context*>(context)->GetCallbacks()->OnError = [cb](const std::string &err, bool user_error) {
        auto errCopy = copy_string(err);
        cb(errCopy, user_error);
        free(errCopy);
    };
}

void toggl_on_overlay(
    void *context,
    TogglDisplayOverlay cb) { }

void toggl_on_update(
    void *context,
    TogglDisplayUpdate cb) { }

void toggl_on_update_download_state(
    void *context,
    TogglDisplayUpdateDownloadState cb) { }

void toggl_on_message(
    void *context,
    TogglDisplayMessage cb) { }

void toggl_on_online_state(
    void *context,
    TogglDisplayOnlineState cb) { }

void toggl_on_url(
    void *context,
    TogglDisplayURL cb) { }

void toggl_on_login(
    void *context,
    TogglDisplayLogin cb) {
    reinterpret_cast<Context*>(context)->GetCallbacks()->OnLogin = [cb](bool open, uint64_t user_id) {
        cb(open, user_id);
    };
}

void toggl_on_reminder(
    void *context,
    TogglDisplayReminder cb) { }

void toggl_on_pomodoro(
    void *context,
    TogglDisplayPomodoro cb) { }

void toggl_on_pomodoro_break(
    void *context,
    TogglDisplayPomodoroBreak cb) { }

void toggl_on_autotracker_notification(
    void *context,
    TogglDisplayAutotrackerNotification cb) { }

void toggl_on_time_entry_list(
    void *context,
    TogglDisplayTimeEntryList cb) {
    auto ctx = reinterpret_cast<Context*>(context);
    ctx->GetCallbacks()->OnTimeEntryList = [ctx, cb]() {
        TogglTimeEntryView *result = nullptr, *previous = nullptr;
        // TODO try cleaning this up, why doesn't foreach work with const_iterator?
        for (auto it = ctx->GetData()->TimeEntries.cbegin(); it != ctx->GetData()->TimeEntries.cend(); ++it) {
            auto i = *it;
            if (i->IsTracking())
                continue;
            auto current = time_entry_view_item_init(i);
            if (!result)
                result = current;
            if (previous)
                previous->Next = current;
            previous = current;
        }
        cb(true, result, true);
        while (result) {
            auto next = reinterpret_cast<TogglTimeEntryView*>(result->Next);
            free(result->Description);
            delete result;
            result = next;
        }
    };
}

void toggl_on_timeline(
    void *context,
    TogglDisplayTimeline cb) { }

void toggl_on_mini_timer_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb) { }

void toggl_on_help_articles(
    void *context,
    TogglDisplayHelpArticles cb) { }

void toggl_on_time_entry_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb) { }

void toggl_on_project_autocomplete(
    void *context,
    TogglDisplayAutocomplete cb) { }

void toggl_on_workspace_select(
    void *context,
    TogglDisplayViewItems cb) { }

void toggl_on_client_select(
    void *context,
    TogglDisplayViewItems cb) { }

void toggl_on_tags(
    void *context,
    TogglDisplayViewItems cb) { }

void toggl_on_time_entry_editor(
    void *context,
    TogglDisplayTimeEntryEditor cb) {
    auto ctx = reinterpret_cast<Context*>(context);
    if (contextMap.contains(ctx))
        contextMap.insert({ctx, LegacyContext()});
    contextMap[ctx].OnTimeEntryEditor = cb;
}

void toggl_on_settings(
    void *context,
    TogglDisplaySettings cb) { }

void toggl_on_timer_state(
    void *context,
    TogglDisplayTimerState cb) {
    auto ctx = reinterpret_cast<Context*>(context);
    ctx->GetCallbacks()->OnTimerState= [ctx, cb]() {
        auto runningTE = ctx->GetData()->RunningTimeEntry();
        if (runningTE) {
            auto view = new TogglTimeEntryView();
            view->ID = runningTE->ID();
            view->Description = copy_string(runningTE->Description());
            cb(view);
            free(view->Description);
            delete view;
        }
        else {
            cb(nullptr);
        }
    };
}

void toggl_on_idle_notification(
    void *context,
    TogglDisplayIdleNotification cb) { }

void toggl_on_autotracker_rules(
    void *context,
    TogglDisplayAutotrackerRules cb) { }

void toggl_on_project_colors(
    void *context,
    TogglDisplayProjectColors cb) { }

void toggl_on_countries(
    void *context,
    TogglDisplayCountries cb) {
    auto ctx = reinterpret_cast<Context*>(context);
    ctx->GetCallbacks()->OnCountries = [ctx, cb]() {
        TogglCountryView *result = nullptr, *previous = nullptr;
        for (auto i : ctx->GetData()->Countries) {
            auto current = new TogglCountryView;
            current->ID = i->ID();
            current->Name = copy_string(i->Name());
            current->Code = copy_string(i->CountryCode());
            current->Next = nullptr;
            current->VatRegex = nullptr;
            current->VatPercentage = nullptr;
            current->VatApplicable = 0;
            if (!result)
                result = current;
            if (previous)
                previous->Next = current;
            previous = current;
        }
        cb(result);
        while (result) {
            auto next = reinterpret_cast<TogglCountryView*>(result->Next);
            free(result->Name);
            free(result->Code);
            delete result;
            result = next;
        }
    };
}

void toggl_on_promotion(
    void *context,
    TogglDisplayPromotion cb) { }

void toggl_on_obm_experiment(
    void *context,
    TogglDisplayObmExperiment cb) { }
