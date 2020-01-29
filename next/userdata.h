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
#include "model/country.h"

namespace toggl {

class UserData {
public:
    UserData() {

    }

    ProtectedModel<TagModel> Tags;
    ProtectedModel<ClientModel> Clients;
    ProtectedModel<ProjectModel> Projects;
    ProtectedModel<TimeEntryModel> TimeEntries;
    ProtectedModel<CountryModel> Countries;

    // TODO move this somewhere else?
    locked<TimeEntryModel> RunningTimeEntry() {
        auto TEs = TimeEntries();
        for (auto i : *TEs) {
            if (i->IsTracking())
                return TimeEntries.make_locked(i);
        }
        return locked<TimeEntryModel>();
    }

    void dumpAll();

    // TODO boilerplate
    Error loadTags(const Json::Value &root) {
        auto tags = Tags();
        return load<TagModel>(tags, root);
    }
    Error loadClients(const Json::Value &root) {
        auto clients = Clients();
        return load<ClientModel>(clients, root);
    }
    Error loadProjects(const Json::Value &root) {
        auto projects = Projects();
        return load<ProjectModel>(projects, root);
    }
    Error loadTimeEntries(const Json::Value &root) {
        auto TEs = TimeEntries();
        return load<TimeEntryModel>(TEs, root);
    }
    Error loadCountries(const Json::Value &root) {
        auto countries = Countries();
        return load<CountryModel>(countries, root);
    }

private:
    template <typename T>
    void dump(locked<std::vector<T*>> &list) {
        for (auto i : *list) {
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
    Error load(locked<std::vector<T*>> &list, const Json::Value &root) {
        clear(list);
        if (!root.isArray())
            return Error::MALFORMED_DATA;
        for (auto i : root) {
            T *item = new T();
            item->LoadFromJSON(i);
            list->push_back(item);
        }
        return Error::NO_ERROR;
    }
};

} // namespace toggl

#endif // USERDATA_H
