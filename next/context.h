#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>

#include "misc/error.h"
#include "togglapi.h"
#include "eventqueue.h"
#include "userdata.h"
#include "settings.h"
#include "db/database.h"

namespace toggl {

class UserModel;
class CountryModel;


inline auto defaultCallback(const std::string &name) {
    return [name](auto...) -> void {
        std::cout << "Callback \"" << name << "\" has not been connected" << std::endl;
    };
};

class Context {
public:
    struct Callbacks {
        // below are callbacks that reflect the legacy ones (but will change, probably)
        std::function<void(const std::string& message, bool user_error)> OnError { defaultCallback("OnError") };
        std::function<void(bool open)> OnShowApp { defaultCallback("OnShowApp") };
        std::function<void(int64_t state)> OnSyncState { defaultCallback("OnSyncState") };
        std::function<void(int64_t count)> OnUnsyncedItems { defaultCallback("OnUnsyncedItems") };
        std::function<void(int64_t type)> OnOverlay { defaultCallback("OnOverlay") };
        std::function<void(const std::string& url)> OnUpdate { defaultCallback("OnUpdate") };
        std::function<void(const std::string& version, int64_t download_state)> OnUpdateDownloadState { defaultCallback("OnUpdateDownloadState") };
        std::function<void(const std::string& title, const std::string& text, const std::string& button, const std::string& url)> OnMessage { defaultCallback("OnMessage") };
        std::function<void(int64_t state)> OnOnlineState { defaultCallback("OnOnlineState") };
        std::function<void(const std::string& url)> OnUrl { defaultCallback("OnUrl") };
        std::function<void(bool open, uint64_t user_id)> OnLogin { defaultCallback("OnLogin") };
        std::function<void(const std::string& title, const std::string& informative_text)> OnReminder { defaultCallback("OnReminder") };
        std::function<void(const std::string& title, const std::string& informative_text)> OnPomodoro { defaultCallback("OnPomodoro") };
        std::function<void(const std::string& title, const std::string& informative_text)> OnPomodoroBreak { defaultCallback("OnPomodoroBreak") };
        std::function<void(const std::string& project_name, uint64_t project_id, uint64_t task_id)> OnAutotrackerNotification { defaultCallback("OnAutotrackerNotification") };
        std::function<void(const std::string& name)> OnToggleEntriesGroup { defaultCallback("OnToggleEntriesGroup") };
        std::function<void(const std::string& guid, const std::string& since, const std::string& duration, int64_t started, const std::string& description)> OnIdleNotification { defaultCallback("OnIdleNotification") };
        std::function<void(const std::list<std::string>& colors)> OnProjectColors { defaultCallback("OnProjectColors") };
        std::function<void(int64_t promotion_type)> OnPromotion { defaultCallback("OnPromotion") };

        // below are callbacks that don't (or won't) reflect the original ones' arguments
        std::function<void()> OnTimeEntryList { defaultCallback("OnTimeEntryList") };
        std::function<void()> OnCountries { defaultCallback("OnCountries") };
        std::function<void()> OnTimerState { defaultCallback("OnTimerState") };
        //std::function<void(bool open, const std::string& date, TogglTimelineChunkView* first, TogglTimeEntryView* first_entry, long start_day, long end_day)> OnTimeline { defaultCallback("OnTimeline") };
        //std::function<void(TogglAutocompleteView* first)> OnMinitimerAutocomplete { defaultCallback("OnMinitimerAutocomplete") };
        //std::function<void(TogglHelpArticleView* first)> OnHelpArticles { defaultCallback("OnHelpArticles") };
        //std::function<void(TogglAutocompleteView* first)> OnTimeEntryAutocomplete { defaultCallback("OnTimeEntryAutocomplete") };
        //std::function<void(TogglAutocompleteView* first)> OnProjectAutocomplete { defaultCallback("OnProjectAutocomplete") };
        //std::function<void(TogglGenericView* first)> OnWorkspaceSelect { defaultCallback("OnWorkspaceSelect") };
        //std::function<void(TogglGenericView* first)> OnClientSelect { defaultCallback("OnClientSelect") };
        //std::function<void(TogglGenericView* first)> OnTags { defaultCallback("OnTags") };
        //std::function<void(bool open, TogglTimeEntryView* time_entry, const std::string& focused_field)> OnTimeEntryEditor { defaultCallback("OnTimeEntryEditor") };
        //std::function<void(bool open, TogglSettingsView* settings)> OnSettings { defaultCallback("OnSettings") };
        //std::function<void(TogglAutotrackerRuleView* first, const std::list<std::string>& title_list)> OnAutotrackerRules { defaultCallback("OnAutotrackerRules") };

        // below are completely new callbacks
    };

    Context(const std::string &app_name, Callbacks callbacks);

    Callbacks *GetCallbacks() {
        return &callbacks_;
    }
    UserData *GetData() {
        return &data;
    }

    void Start() {
        eventQueue_.start();
        callbacks_.OnShowApp(true);
        // TODO
        if (data.User->ID() <= 0) {
            eventQueue_.schedule([this](){ callbacks_.OnLogin(true, 0); });
            eventQueue_.schedule([this](){ callbacks_.OnTimerState(); });
        }
    }

    void Login(const std::string &username, const std::string &password) {
        eventQueue_.schedule(std::bind(&Context::login, this, username, password));
    }

    void GetCountries() {
        eventQueue_.schedule(std::bind(&Context::getCountries, this));
    }


private:
    void login(const std::string &username, const std::string &password);
    void getCountries();

    void sync();

    Callbacks callbacks_;

    Logger logger { "Context" };

    TogglApi api;
    UserData data;
    Database *db_;

    EventQueue eventQueue_;
};

} // namespace toggl

#endif // CONTEXT_H
