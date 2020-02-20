// Copyright 2014 Toggl Desktop developers.

#include "toggl_api_tools.h"

#include <cstdlib>

#include "client.h"
#include "context.h"
#include "formatter.h"
#include "project.h"
#include "time_entry.h"
#include "timeline_event.h"
#include "task.h"
#include "workspace.h"

#include <Poco/UnicodeConverter.h>

namespace toggl {

/*
TogglAutocompleteView *autocomplete_list_init(
    std::vector<view::Autocomplete> *items) {
    TogglAutocompleteView *first = nullptr;
    for (std::vector<view::Autocomplete>::const_reverse_iterator it =
        items->rbegin();
            it != items->rend();
            ++it) {
        TogglAutocompleteView *item = autocomplete_item_init(*it);
        item->Next = first;
        first = item;
    }
    return first;
}

TogglAutocompleteView *autocomplete_item_init(const view::Autocomplete &item) {
    TogglAutocompleteView *result = new TogglAutocompleteView();
    result->Description = copy_string(item.Description);
    result->Text = copy_string(item.Text);
    result->ProjectAndTaskLabel = copy_string(item.ProjectAndTaskLabel);
    result->TaskLabel = copy_string(item.TaskLabel);
    result->ProjectLabel = copy_string(item.ProjectLabel);
    result->ClientLabel = copy_string(item.ClientLabel);
    result->ProjectColor = copy_string(item.ProjectColor);
    result->WorkspaceName = copy_string(item.WorkspaceName);
    result->ProjectGUID = copy_string(item.ProjectGUID);
    result->TaskID = static_cast<unsigned int>(item.TaskID);
    result->ProjectID = static_cast<unsigned int>(item.ProjectID);
    result->WorkspaceID = static_cast<unsigned int>(item.WorkspaceID);
    result->Type = static_cast<unsigned int>(item.Type);
    result->Tags = copy_string(item.Tags);
    result->WorkspaceName = copy_string(item.WorkspaceName);
    result->ClientID = static_cast<unsigned int>(item.ClientID);
    result->Billable = item.Billable;
    result->Next = nullptr;
    return result;
}

void autocomplete_item_clear(TogglAutocompleteView *item) {
    if (!item) {
        return;
    }

    free(item->Text);
    item->Text = nullptr;

    free(item->ProjectAndTaskLabel);
    item->ProjectAndTaskLabel = nullptr;

    free(item->TaskLabel);
    item->TaskLabel = nullptr;

    free(item->ProjectLabel);
    item->ProjectLabel = nullptr;

    free(item->ClientLabel);
    item->ClientLabel = nullptr;

    free(item->Description);
    item->Description = nullptr;

    free(item->ProjectColor);
    item->ProjectColor = nullptr;

    free(item->ProjectGUID);
    item->ProjectGUID = nullptr;

    free(item->Tags);
    item->Tags = nullptr;

    free(item->WorkspaceName);
    item->WorkspaceName = nullptr;

    delete item;
}

void autocomplete_list_clear(TogglAutocompleteView *first) {
    while (first) {
        TogglAutocompleteView *next = reinterpret_cast<TogglAutocompleteView *>(first->Next);
        autocomplete_item_clear(first);
        first = next;
    }
}
*/

TogglGenericView *generic_to_view_item_list(const std::vector<view::Generic> &list) {
    TogglGenericView *first = nullptr;
    /*
    for (std::vector<view::Generic>::const_iterator
            it = list.begin();
            it != list.end();
            ++it) {
        TogglGenericView *item = generic_to_view_item(*it);
        item->Next = first;
        first = item;
    }
    */
    return first;
}

TogglGenericView *generic_to_view_item(const view::Generic &c) {
    TogglGenericView *result = new TogglGenericView();
    /*
    result->ID = static_cast<unsigned int>(c.ID);
    result->WID = static_cast<unsigned int>(c.WID);
    result->GUID = copy_string(c.GUID);
    result->Name = copy_string(c.Name);
    result->WorkspaceName = copy_string(c.WorkspaceName);
    result->Premium = c.Premium;
    */
    return result;
}

TogglAutotrackerRuleView *autotracker_rule_to_view_item(locked<const AutotrackerRuleModel> &model) {
    TogglAutotrackerRuleView *view = new TogglAutotrackerRuleView();
    // Autotracker settings are not saved to DB,
    // so the ID will be 0 always. But will have local ID
    view->ID = static_cast<int>(model->ID());
    view->Term = copy_string(model->Term());
    //view->ProjectAndTaskLabel = copy_string(model->ProjectName());
    return view;
}

void autotracker_view_item_clear(TogglAutotrackerRuleView *view) {
    if (!view) {
        return;
    }

    view->ID = 0;

    free(view->Term);
    view->Term = nullptr;

    free(view->ProjectAndTaskLabel);
    view->ProjectAndTaskLabel = nullptr;

    delete view;
}

void autotracker_view_list_clear(TogglAutotrackerRuleView *first) {
    while (first) {
        TogglAutotrackerRuleView *next = reinterpret_cast<TogglAutotrackerRuleView *>(first->Next);
        autotracker_view_item_clear(first);
        first = next;
    }
}

void view_item_clear(TogglGenericView *item) {
    if (!item) {
        return;
    }

    free(item->Name);
    item->Name = nullptr;

    free(item->GUID);
    item->GUID = nullptr;

    free(item->WorkspaceName);
    item->WorkspaceName = nullptr;

    delete item;
}

void view_list_clear(TogglGenericView *first) {
    while (first) {
        TogglGenericView *next = reinterpret_cast<TogglGenericView*>(first->Next);
        view_item_clear(first);
        first = next;
    }
}

void country_item_clear(TogglCountryView *item) {
    if (!item) {
        return;
    }

    item->ID = 0;

    free(item->Name);
    item->Name = nullptr;

    free(item->VatPercentage);
    item->VatPercentage = nullptr;

    free(item->VatRegex);
    item->VatRegex = nullptr;

    delete item;
}

void country_list_clear(TogglCountryView *first) {
    while (first) {
        TogglCountryView *next = reinterpret_cast<TogglCountryView *>(first->Next);
        country_item_clear(first);
        first = next;
    }
}

TogglCountryView *country_list_init(
    std::vector<TogglCountryView> *items) {

    TogglCountryView *first = nullptr;
    for (std::vector<TogglCountryView>::const_iterator
            it = items->begin();
            it != items->end();
            ++it) {
        TogglCountryView *item = new TogglCountryView();
        poco_check_ptr(item);

        item->ID = it->ID;
        item->Name = it->Name;
        item->VatApplicable = it->VatApplicable;
        item->VatRegex = it->VatRegex;
        item->VatPercentage = it->VatPercentage;
        item->Code = it->Code;

        item->Next = first;
        first = item;
    }
    return first;
}

TogglCountryView *country_view_item_init(
    const Json::Value v) {

    TogglCountryView *item = new TogglCountryView();
    poco_check_ptr(item);

    item->ID = v["id"].asInt64();
    item->Name = copy_string(v["name"].asString());
    item->VatApplicable = v["vat_applicable"].asBool();
    item->VatRegex = copy_string(v["vat_regex"].asString());
    item->VatPercentage = copy_string(v["vat_percentage"].asString());
    item->Code = copy_string(v["country_code"].asString());

    item->Next = nullptr;

    return item;
}

TogglTimeEntryView *time_entry_view_item_init(locked<const TimeEntryModel> &te) {
    auto ws = te->Workspace();
    auto project = te->Project();
    auto task = te->Task();
    locked<const ClientModel> client;
    if (project)
        client = project->Client();
    auto user = te->User();


    int quarter = 900;
    int tmp_rounded;

    tmp_rounded = ((int)(te->Start() / quarter) * quarter);
    // gets the percentage that is used to set margin from top
    auto RoundedStart = (te->Start() - tmp_rounded) / 9;

    tmp_rounded = (((int)(te->Stop() / quarter)) * quarter) + quarter;
    // gets the percentage that is used to set margin from bottom
    auto RoundedEnd = (tmp_rounded - te->Stop()) / 9;

    TogglTimeEntryView *view_item = new TogglTimeEntryView();
    poco_check_ptr(view_item);

    view_item->ID = static_cast<unsigned int>(te->ID());
    view_item->DurationInSeconds = static_cast<int>(te->DurationInSeconds());
    view_item->Description = copy_string(te->Description());
    view_item->GUID = copy_string(te->GUID());
    view_item->WID = static_cast<unsigned int>(te->WID());
    view_item->TID = static_cast<unsigned int>(te->TID());
    view_item->PID = static_cast<unsigned int>(te->PID());
    view_item->Duration = copy_string(toggl::Formatter::FormatDuration(te->DurationInSeconds(), Formatter::DurationFormat));

    view_item->Started = static_cast<unsigned int>(te->Start());
    view_item->Ended = static_cast<unsigned int>(te->Stop());
    view_item->StartTimeString = copy_string(toggl::Formatter::FormatTimeForTimeEntryEditor(te->Start()));
    view_item->EndTimeString = copy_string(toggl::Formatter::FormatTimeForTimeEntryEditor(te->Stop()));
    view_item->DateDuration = copy_string(Formatter::FormatDurationForDateHeader(te->DurationInSeconds()));
    if (ws)
        view_item->WorkspaceName = copy_string(ws->Name());
    if (task && project)
        view_item->ProjectAndTaskLabel = copy_string(Formatter::JoinTaskName(task, project));
    if (task)
        view_item->TaskLabel = copy_string(task->Name());
    if (project) {
        view_item->ProjectLabel = copy_string(project->Name());
        view_item->Color = copy_string(project->Color());
    }
    if (client)
        view_item->ClientLabel = copy_string(client->Name());
    if (te->Tags().empty()) {
        view_item->Tags = nullptr;
    } else {
        view_item->Tags = copy_string(te->Tags());
    }
    view_item->Billable = te->Billable();
    view_item->UpdatedAt = static_cast<unsigned int>(te->UpdatedAt());
    view_item->DateHeader = copy_string(toggl::Formatter::FormatDateHeader(te->Start()));
    view_item->DurOnly = te->DurOnly();
    view_item->IsHeader = false;

    if (ws)
        view_item->CanAddProjects = ws->Admin() || !ws->OnlyAdminsMayCreateProjects();
    else
        view_item->CanAddProjects = user->CanAddProjects();
    if (user && ws)
        view_item->CanSeeBillable = user->CanSeeBillable(ws);
    view_item->DefaultWID = te->GetUserData()->User->DefaultWID();

    view_item->Unsynced = te->Unsynced();
    //view_item->Locked = te->Locked;

    /*
    if (te->Error != noError) {
        view_item->Error = copy_string(te->Error);
    } else {
        view_item->Error = nullptr;
    }
    */
/*
    view_item->Group = te->Group;
    view_item->GroupOpen = te->GroupOpen;
    view_item->GroupName = copy_string(te->GroupName);
    view_item->GroupDuration = copy_string(te->GroupDuration);
    view_item->GroupItemCount = te->GroupItemCount;
*/
    view_item->RoundedStart = RoundedStart;
    view_item->RoundedEnd = RoundedEnd;


    view_item->Next = nullptr;

    return view_item;
}

void time_entry_view_item_clear(
    TogglTimeEntryView *item) {
    if (!item) {
        return;
    }
    free(item->Description);
    item->Description = nullptr;

    free(item->WorkspaceName);
    item->WorkspaceName = nullptr;

    free(item->ProjectAndTaskLabel);
    item->ProjectAndTaskLabel = nullptr;

    free(item->TaskLabel);
    item->TaskLabel = nullptr;

    free(item->ProjectLabel);
    item->ProjectLabel = nullptr;

    free(item->ClientLabel);
    item->ClientLabel = nullptr;

    free(item->Duration);
    item->Duration = nullptr;

    free(item->Color);
    item->Color = nullptr;

    free(item->GUID);
    item->GUID = nullptr;

    free(item->Tags);
    item->Tags = nullptr;

    free(item->DateHeader);
    item->DateHeader = nullptr;

    free(item->DateDuration);
    item->DateDuration = nullptr;

    free(item->StartTimeString);
    item->StartTimeString = nullptr;

    free(item->EndTimeString);
    item->EndTimeString = nullptr;

    if (item->Error) {
        free(item->Error);
        item->Error = nullptr;
    }

    free(item->GroupName);
    item->GroupName = nullptr;

    free(item->GroupDuration);
    item->GroupDuration = nullptr;

    delete item;
}

void time_entry_view_list_clear(TogglTimeEntryView *first) {
    while (first) {
        TogglTimeEntryView *next = reinterpret_cast<TogglTimeEntryView *>(first->Next);
        time_entry_view_item_clear(first);
        first = next;
    }
}

TogglSettingsView *settings_view_item_init(const bool_t record_timeline, const SettingsModel &settings, const bool_t use_proxy) {
    //const Proxy &proxy) {
    TogglSettingsView *view = new TogglSettingsView();

    view->RecordTimeline = record_timeline;

    view->DockIcon = settings.dock_icon;
    view->MenubarTimer = settings.menubar_timer;
    view->MenubarProject = settings.menubar_project;
    view->OnTop = settings.on_top;
    view->Reminder = settings.reminder;
    view->UseIdleDetection = settings.use_idle_detection;
    view->IdleMinutes = settings.idle_minutes;
    view->FocusOnShortcut = settings.focus_on_shortcut;
    view->ReminderMinutes = settings.reminder_minutes;
    view->ManualMode = settings.manual_mode;
    view->AutodetectProxy = settings.autodetect_proxy;
    view->Autotrack = settings.autotrack;
    view->OpenEditorOnShortcut = settings.open_editor_on_shortcut;

    view->UseProxy = use_proxy;

    /*
    view->ProxyHost = copy_string(proxy.Host());
    view->ProxyPort = proxy.Port();
    view->ProxyUsername = copy_string(proxy.Username());
    view->ProxyPassword = copy_string(proxy.Password());
    */

    view->RemindMon = settings.remind_mon;
    view->RemindTue = settings.remind_tue;
    view->RemindWed = settings.remind_wed;
    view->RemindThu = settings.remind_thu;
    view->RemindFri = settings.remind_fri;
    view->RemindSat = settings.remind_sat;
    view->RemindSun = settings.remind_sun;

    view->RemindStarts = copy_string(settings.remind_starts);
    view->RemindEnds = copy_string(settings.remind_ends);

    view->Pomodoro = settings.pomodoro;
    view->PomodoroMinutes = settings.pomodoro_minutes;
    view->PomodoroBreak = settings.pomodoro_break;
    view->PomodoroBreakMinutes = settings.pomodoro_break_minutes;
    view->StopEntryOnShutdownSleep = settings.stop_entry_on_shutdown_sleep;
    view->ShowTouchBar = settings.show_touch_bar;
    //view->ActiveTab = settings.active_tab;
    return view;
}

void settings_view_item_clear(TogglSettingsView *view) {
    poco_check_ptr(view);

    free(view->ProxyHost);
    free(view->ProxyUsername);
    free(view->ProxyPassword);

    free(view->RemindStarts);
    free(view->RemindEnds);

    delete view;
}

/*
TogglHelpArticleView *help_article_init(
    const HelpArticle &item) {
    TogglHelpArticleView *result = new TogglHelpArticleView();
    result->Category = copy_string(item.Type);
    result->Name = copy_string(item.Name);
    result->URL = copy_string(item.URL);
    result->Next = nullptr;
    return result;
}

void help_article_item_clear(TogglHelpArticleView *view) {
    if (!view) {
        return;
    }

    free(view->Category);
    view->Category = nullptr;

    free(view->Name);
    view->Name = nullptr;

    free(view->URL);
    view->URL = nullptr;

    delete view;
}

void help_article_list_clear(TogglHelpArticleView *first) {
    while (first) {
        TogglHelpArticleView *next = reinterpret_cast<TogglHelpArticleView *>(first->Next);
        help_article_item_clear(first);
        first = next;
    }
}

TogglHelpArticleView *help_article_list_init(const std::vector<HelpArticleModel> &items) {
    TogglHelpArticleView *first = nullptr;
    for (std::vector<HelpArticle>::const_reverse_iterator it =
        items.rbegin();
            it != items.rend();
            ++it) {
        TogglHelpArticleView *item = help_article_init(*it);
        item->Next = first;
        first = item;
    }
    return first;
}
*/

TogglTimelineChunkView *timeline_chunk_view_init(
    const time_t &start) {
    TogglTimelineChunkView *chunk_view = new TogglTimelineChunkView();
    chunk_view->Started = static_cast<unsigned int>(start);
    chunk_view->StartTimeString = copy_string(
        Formatter::FormatTimeForTimeEntryEditor(start));
    chunk_view->Next = nullptr;
    chunk_view->FirstEvent = nullptr;
    return chunk_view;
}

void timeline_chunk_view_clear(
    TogglTimelineChunkView *chunk_view) {
    if (!chunk_view) {
        return;
    }
    if (chunk_view->StartTimeString) {
        free(chunk_view->StartTimeString);
        chunk_view->StartTimeString = nullptr;
    }
    if (chunk_view->EndTimeString) {
        free(chunk_view->EndTimeString);
        chunk_view->EndTimeString = nullptr;
    }
    if (chunk_view->FirstEvent) {
        TogglTimelineEventView *firstEvent =
            reinterpret_cast<TogglTimelineEventView *>(chunk_view->FirstEvent);
        timeline_event_view_clear(firstEvent);
        chunk_view->FirstEvent = nullptr;
    }
    if (chunk_view->Next) {
        TogglTimelineChunkView *next =
            reinterpret_cast<TogglTimelineChunkView *>(chunk_view->Next);
        timeline_chunk_view_clear(next);
        chunk_view->Next = nullptr;
    }
    delete chunk_view;
}

TogglTimelineEventView *timeline_event_view_init(locked<const TimelineEventModel> &event) {
    TogglTimelineEventView *event_view = new TogglTimelineEventView();
    event_view->Title = copy_string(event->Title());
    event_view->Filename = copy_string(event->Filename());
    event_view->Duration = event->EndTime() - event->Start();
    event_view->DurationString = copy_string(Formatter::FormatDuration(event_view->Duration, Format::ImprovedOnlyMinAndSec));
    event_view->Header = false;
    event_view->Next = nullptr;
    return event_view;
}

void timeline_event_view_update_duration(TogglTimelineEventView *event_view, const int64_t duration) {
    event_view->Duration = duration;
    event_view->DurationString = copy_string(Formatter::FormatDuration(duration, Format::ImprovedOnlyMinAndSec));
}

void timeline_event_view_clear(
    TogglTimelineEventView *event_view) {
    if (!event_view) {
        return;
    }
    if (event_view->Title) {
        free(event_view->Title);
        event_view->Title = nullptr;
    }
    if (event_view->Filename) {
        free(event_view->Filename);
        event_view->Filename = nullptr;
    }
    if (event_view->DurationString) {
        free(event_view->DurationString);
        event_view->DurationString = nullptr;
    }
    if (event_view->Next) {
        TogglTimelineEventView *next =
            reinterpret_cast<TogglTimelineEventView *>(event_view->Next);
        timeline_event_view_clear(next);
        event_view->Next = nullptr;
    }
    delete event_view;
}

Context *app(void *context) {
    poco_check_ptr(context);

    return reinterpret_cast<Context *>(context);
}

Logger logger() {
    return { "toggl_api" };
}

} // namespace toggl
