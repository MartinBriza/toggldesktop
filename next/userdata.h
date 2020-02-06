#ifndef USERDATA_H
#define USERDATA_H

#include <iostream>
#include <string>
#include <list>
#include <mutex>

#include <json/json.h>

#include "misc/error.h"
#include "misc/memory.h"

#include "model/base_model.h"
#include "model/client.h"
#include "model/project.h"
#include "model/tag.h"
#include "model/time_entry.h"
#include "model/user.h"
#include "model/country.h"
#include "model/workspace.h"
#include "model/timeline_event.h"
#include "model/autotracker.h"
#include "model/settings.h"

namespace toggl {

class UserData {
public:
    UserData(Context *context)
        : context_(context)
    {
        User.create(context);
        Settings.create();
    }

    ProtectedModel<UserModel> User;
    ProtectedModel<SettingsModel> Settings;
    ProtectedContainer<TagModel> Tags;
    ProtectedContainer<ClientModel> Clients;
    ProtectedContainer<ProjectModel> Projects;
    ProtectedContainer<TimeEntryModel> TimeEntries;
    ProtectedContainer<CountryModel> Countries;
    ProtectedContainer<WorkspaceModel> Workspaces;
    ProtectedContainer<TaskModel> Tasks;
    ProtectedContainer<AutotrackerRuleModel> AutotrackerRules;
    ProtectedContainer<TimelineEventModel> TimelineEvents;

    // TODO move this somewhere else?
    locked<TimeEntryModel> RunningTimeEntry() {
        for (auto i : TimeEntries) {
            if (i->IsTracking())
                return i;
        }
        return locked<TimeEntryModel>();
    }

    void clear() {
        // TODO clearing User is probably a mistake
        User.clear();
        Settings.clear();
        Tags.clear();
        Clients.clear();
        Projects.clear();
        TimeEntries.clear();
        Countries.clear();
        Workspaces.clear();
        Tasks.clear();
        AutotrackerRules.clear();
        TimelineEvents.clear();
    }

    void dumpAll();

    Error loadAll(const std::string &json, bool with_user = false);
    Error loadAll(const Json::Value &root, bool with_user = false);

    // TODO boilerplate
    Error loadTags(const Json::Value &root) {
        return load<TagModel>(Tags, root);
    }
    Error loadClients(const Json::Value &root) {
        return load<ClientModel>(Clients, root);
    }
    Error loadProjects(const Json::Value &root) {
        return load<ProjectModel>(Projects, root);
    }
    Error loadTimeEntries(const Json::Value &root) {
        return load<TimeEntryModel>(TimeEntries, root);
    }
    Error loadCountries(const Json::Value &root) {
        return load<CountryModel>(Countries, root);
    }

private:
    Context *context_;

    template <typename T>
    void dump(ProtectedContainer<T> &list) {
        for (auto i : list) {
            std::cout << i->Name() << ", ";
        }
        std::cout << std::endl << std::flush;
    }
    template <typename T>
    void clear(locked<std::vector<T*>> &list) {
        for (auto i : *list)
            delete i;
        list->clear();
    }
    template <typename T>
    Error load(ProtectedContainer<T> &list, const Json::Value &root) {
        list.clear();
        if (!root.isArray())
            return Error::MALFORMED_DATA;
        for (auto i : root) {
            auto item = list.create();
            item->LoadFromJSON(i);
        }
        return Error::NO_ERROR;
    }
};

} // namespace toggl

#endif // USERDATA_H
