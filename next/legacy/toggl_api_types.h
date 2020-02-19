#ifndef SRC_TOGGL_API_TYPES_H_
#define SRC_TOGGL_API_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#include <time.h>

#if defined(_WIN32) || defined(WIN32)
#define TOGGL_EXPORT __declspec(dllexport)
#else
#define TOGGL_EXPORT
#endif

#if defined(_WIN32) || defined(WIN32)
#define char_t wchar_t
#define bool_t bool
#else
#define char_t char
#define bool_t int
#endif

// Constants

#define kOnlineStateOnline 0
#define kOnlineStateNoNetwork 1
#define kOnlineStateBackendDown 2

#define kSyncStateIdle 0
#define kSyncStateWork 1

#define kDownloadStatusStarted 0
#define kDownloadStatusDone 1

#define kPromotionJoinBetaChannel 1

// Models

    typedef struct {
        uint64_t ID;
        int64_t DurationInSeconds;
        char_t *Description;
        char_t *ProjectAndTaskLabel;
        char_t *TaskLabel;
        char_t *ProjectLabel;
        char_t *ClientLabel;
        uint64_t WID;
        uint64_t PID;
        uint64_t TID;
        char_t *Duration;
        char_t *Color;
        char_t *GUID;
        bool_t Billable;
        char_t *Tags;
        uint64_t Started;
        uint64_t Ended;
        char_t *StartTimeString;
        char_t *EndTimeString;
        uint64_t UpdatedAt;
        bool_t DurOnly;
        // In case it's a header
        char_t *DateHeader;
        char_t *DateDuration;
        bool_t IsHeader;
        // Additional fields; only when in time entry editor
        bool_t CanAddProjects;
        bool_t CanSeeBillable;
        uint64_t DefaultWID;
        char_t *WorkspaceName;
        // If syncing a time entry ended with an error,
        // the error is attached to the time entry
        char_t *Error;
        bool_t Locked;
        // Indicates if time entry is not synced to server
        bool_t Unsynced;
        // Group attributes
        bool_t Group;
        bool_t GroupOpen;
        char_t *GroupName;
        char_t *GroupDuration;
        uint64_t GroupItemCount;
        // To categorize to 15-minute batches
        uint64_t RoundedStart;
        uint64_t RoundedEnd;
        // Next in list
        void *Next;
    } TogglTimeEntryView;

    typedef struct {
        char_t *Title;
        char_t *Filename;
        int64_t Duration;
        char_t *DurationString;
        bool_t Header;
        // references subevents
        void *Event;
        // Next in list
        void *Next;
    } TogglTimelineEventView;

    typedef struct {
        uint64_t Started;
        uint64_t Ended;
        char_t *StartTimeString;
        char_t *EndTimeString;
        void *Next;
        void *FirstEvent;
        // Reference to Time entries in this Chunk
        void *Entry;
    } TogglTimelineChunkView;

    typedef struct {
        // This is what is displayed to user, includes project and task.
        char_t *Text;
        // This is copied to "time_entry.description" field if item is selected
        char_t *Description;
        // Project label, if has a project
        char_t *ProjectAndTaskLabel;
        char_t *TaskLabel;
        char_t *ProjectLabel;
        char_t *ClientLabel;
        char_t *ProjectColor;
        char_t *ProjectGUID;
        uint64_t TaskID;
        uint64_t ProjectID;
        uint64_t WorkspaceID;
        uint64_t Type;
        // If its a time entry or project, it can be billable
        bool_t Billable;
        // If its a time entry, it has tags
        char_t *Tags;
        char_t *WorkspaceName;
        uint64_t ClientID;
        void *Next;
    } TogglAutocompleteView;

    typedef struct {
        uint64_t ID;
        uint64_t WID;
        char_t *GUID;
        char_t *Name;
        char_t *WorkspaceName;
        bool_t Premium;
        void *Next;
    } TogglGenericView;

    typedef struct {
        char_t *Category;
        char_t *Name;
        char_t *URL;
        void *Next;
    } TogglHelpArticleView;

    typedef struct {
        bool_t UseProxy;
        char_t *ProxyHost;
        uint64_t ProxyPort;
        char_t *ProxyUsername;
        char_t *ProxyPassword;
        bool_t UseIdleDetection;
        bool_t MenubarTimer;
        bool_t MenubarProject;
        bool_t DockIcon;
        bool_t OnTop;
        bool_t Reminder;
        bool_t RecordTimeline;
        int64_t IdleMinutes;
        bool_t FocusOnShortcut;
        int64_t ReminderMinutes;
        bool_t ManualMode;
        bool_t AutodetectProxy;
        bool_t RemindMon;
        bool_t RemindTue;
        bool_t RemindWed;
        bool_t RemindThu;
        bool_t RemindFri;
        bool_t RemindSat;
        bool_t RemindSun;
        char_t *RemindStarts;
        char_t *RemindEnds;
        bool_t Autotrack;
        bool_t OpenEditorOnShortcut;
        bool_t Pomodoro;
        bool_t PomodoroBreak;
        int64_t PomodoroMinutes;
        int64_t PomodoroBreakMinutes;
        bool_t StopEntryOnShutdownSleep;
        bool_t ShowTouchBar;
    } TogglSettingsView;

    typedef struct {
        int64_t ID;
        char_t *Term;
        char_t *ProjectAndTaskLabel;
        void *Next;
    } TogglAutotrackerRuleView;

    typedef struct {
        int64_t ID;
        char_t *Name;
        bool_t VatApplicable;
        char_t *VatRegex;
        char_t *VatPercentage;
        char_t *Code;
        void *Next;
    } TogglCountryView;

    // Callbacks that need to be implemented in UI

    typedef void (*TogglDisplayApp)(
        const bool_t open);

    typedef void (*TogglDisplaySyncState)(
        const int64_t state);

    typedef void (*TogglDisplayUnsyncedItems)(
        const int64_t count);

    typedef void (*TogglDisplayError)(
        const char_t *errmsg,
        const bool_t user_error);

    typedef void (*TogglDisplayOverlay)(
        const int64_t type);

    typedef void (*TogglDisplayOnlineState)(
        const int64_t state);

    typedef void (*TogglDisplayURL)(
        const char_t *url);

    typedef void (*TogglDisplayLogin)(
        const bool_t open,
        const uint64_t user_id);

    typedef void (*TogglDisplayReminder)(
        const char_t *title,
        const char_t *informative_text);

    typedef void (*TogglDisplayPomodoro)(
        const char_t *title,
        const char_t *informative_text);

    typedef void (*TogglDisplayPomodoroBreak)(
        const char_t *title,
        const char_t *informative_text);

    typedef void (*TogglDisplayAutotrackerNotification)(
        const char_t *project_name,
        const uint64_t project_id,
        const uint64_t task_id);

    typedef void (*TogglDisplayPromotion)(
        const int64_t promotion_type);

    typedef void (*TogglDisplayObmExperiment)(
        const uint64_t nr,
        const bool_t included,
        const bool_t seen);

    typedef void (*TogglDisplayTimeEntryList)(
        const bool_t open,
        TogglTimeEntryView *first,
        const bool_t show_load_more_button);

    typedef void (*TogglDisplayTimeline)(
        const bool_t open,
        const char_t *date,
        TogglTimelineChunkView *first,
        TogglTimeEntryView *first_entry,
        long start_day,
        long end_day);

    typedef void (*TogglDisplayAutocomplete)(
        TogglAutocompleteView *first);

    typedef void (*TogglDisplayHelpArticles)(
        TogglHelpArticleView *first);

    typedef void (*TogglDisplayViewItems)(
        TogglGenericView *first);

    typedef void (*TogglDisplayTimeEntryEditor)(
        const bool_t open,
        TogglTimeEntryView *te,
        const char_t *focused_field_name);

    typedef void (*TogglDisplaySettings)(
        const bool_t open,
        TogglSettingsView *settings);

    typedef void (*TogglDisplayTimerState)(
        TogglTimeEntryView *te);

    typedef void (*TogglDisplayIdleNotification)(
        const char_t *guid,
        const char_t *since,
        const char_t *duration,
        const int64_t started,
        const char_t *description);

    typedef void (*TogglDisplayUpdate)(
        const char_t *url);

    typedef void (*TogglDisplayUpdateDownloadState)(
        const char_t *version,
        const int64_t download_state);

    typedef void (*TogglDisplayMessage)(
        const char_t *title,
        const char_t *text,
        const char_t *button,
        const char_t *url);

    typedef char_t * string_list_t[];

    typedef void (*TogglDisplayAutotrackerRules)(
        TogglAutotrackerRuleView *first,
        const uint64_t title_count,
        string_list_t title_list);

    typedef void (*TogglDisplayProjectColors)(
        string_list_t color_list,
        const uint64_t color_count);

    typedef void (*TogglDisplayCountries)(
        TogglCountryView *first);

#ifdef __cplusplus
}
#endif

#endif // SRC_TOGGL_API_TYPES_H_
