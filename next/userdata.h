#ifndef USERDATA_H
#define USERDATA_H

#include <iostream>
#include <string>
#include <list>
#include <mutex>

#include <json/json.h>

#include "misc/error.h"

#include "model/base_model.h"
#include "model/client.h"
#include "model/project.h"
#include "model/tag.h"
#include "model/time_entry.h"

using toggl::Tag;
using toggl::Client;
using toggl::Project;
using toggl::TimeEntry;

class UserData {
public:
    UserData() {

    }

    void dumpAll();

    Error loadTags(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(tagsMutex_);
        return load<Tag>(tags_, root);
    }
    Error loadClients(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(clientsMutex_);
        return load<Client>(clients_, root);
    }
    Error loadProjects(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(projectsMutex_);
        return load<Project>(projects_, root);
    }
    Error loadTimeEntries(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(timeEntriesMutex_);
        return load<TimeEntry>(timeEntries_, root);
    }

private:
    template <typename T>
    void dump(std::list<T*> &list) {
        for (auto i : list) {
            std::cout << i->Name() << ", ";
        }
        std::cout << std::endl << std::flush;
    }
    template <typename T>
    void clear(std::list<T*> &list) {
        for (auto i : list)
            delete i;
        list.clear();
    }
    template <typename T>
    Error load(std::list<T*> &list, const Json::Value &root) {
        clear(list);
        if (!root.isArray())
            return Error::MALFORMED_DATA;
        for (auto i : root) {
            T *item = new T();
            item->LoadFromJSON(i);
            list.push_back(item);
        }
        return Error::NO_ERROR;
    }

    std::list<Tag*> tags_;
    std::recursive_mutex tagsMutex_;
    std::list<Client*> clients_;
    std::recursive_mutex clientsMutex_;
    std::list<Project*> projects_;
    std::recursive_mutex projectsMutex_;
    std::list<TimeEntry*> timeEntries_;
    std::recursive_mutex timeEntriesMutex_;
};

#endif // USERDATA_H
