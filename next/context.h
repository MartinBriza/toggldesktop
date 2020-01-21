#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>

#include "misc/error.h"
#include "togglapi.h"
#include "eventqueue.h"

class Context {
public:
    struct Callbacks {
        std::function<void(const std::string&, bool)> OnError;
        std::function<void(bool)> OnShowApp;
        std::function<void(int64_t)> OnSyncState;
        std::function<void(int64_t)> OnUnsyncedItems;
        std::function<void(int64_t)> OnOverlay;
        std::function<void(const std::string&)> OnUpdate;
        std::function<void(const std::string&, int64_t)> OnUpdateDownloadState;
        std::function<void(const std::string&, const std::string&, const std::string&, const std::string&)> OnMessage;
        std::function<void(int64_t)> OnOnlineState;
        std::function<void(const std::string&)> OnUrl;
        std::function<void(bool, uint64_t)> OnLogin;
        std::function<void(const std::string&, const std::string&)> OnReminder;
        std::function<void(const std::string&, const std::string&)> OnPomodoro;
        std::function<void(const std::string&, const std::string&)> OnPomodoroBreak;
        std::function<void(const std::string&, uint64_t, uint64_t)> OnAutotrackerNotification;
        //std::function<void(bool, TogglTimeEntryView, bool)> OnTimeEntryList;
        std::function<void(const std::string&)> OnToggleEntriesGroup;
        //std::function<void(bool, const std::string&, TogglTimelineChunkView*, TogglTimeEntryView*, OnTimeline;
        //std::function<void(TogglAutocompleteView*)> OnMinitimerAutocomplete;
        //std::function<void(TogglHelpArticleView*)> OnHelpArticles;
        //std::function<void(TogglAutocompleteView*)> OnTimeEntryAutocomplete;
        //std::function<void(TogglAutocompleteView*)> OnProjectAutocomplete;
        //std::function<void(TogglGenericView*)> OnWorkspaceSelect;
        //std::function<void(TogglGenericView*)> OnClientSelect;
        //std::function<void(TogglGenericView*)> OnTags;
        //std::function<void(bool, TogglTimeEntryView*, const std::string&)> OnTimeEntryEditor;
        //std::function<void(bool, TogglSettingsView*)> OnSettings;
        //std::function<void(TogglTimeEntryView*)> OnTimerState;
        std::function<void(const std::string&, const std::string&, const std::string&, int64_t, const std::string&)> OnIdleNotification;
        //std::function<void(TogglAutotrackerRuleView*, uint64_t, const std::list<std::string>&)> OnAutotrackerRules;
        std::function<void(const std::list<std::string>&)> OnProjectColors;
        //std::function<void(TogglCountryView*)> OnCountries;
        std::function<void(int64_t)> OnPromotion;
    };

    Context(const std::string &app_name, Callbacks callbacks)
        : callbacks_(callbacks)
        , api(app_name)
        , eventQueue_(this)
    {

    }

    Callbacks *GetCallbacks() {
        return &callbacks_;
    }

    void Start() {
        eventQueue_.start();
        callbacks_.OnShowApp(true);
        // TODO
        eventQueue_.schedule([=](){callbacks_.OnLogin(true, 0); });
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

    Callbacks callbacks_;

    TogglApi api;

    EventQueue eventQueue_;
};

#endif // CONTEXT_H
