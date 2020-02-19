#include "userdata.h"

namespace toggl {

void UserData::dumpAll() {
    std::cout << "==== Workspaces ====" << std::endl << std::flush;
    dump(Workspaces);
    std::cout << "==== Tags ====" << std::endl << std::flush;
    dump(Tags);
    std::cout << "==== Clients ====" << std::endl << std::flush;
    dump(Clients);
    std::cout << "==== Projects ====" << std::endl << std::flush;
    dump(Projects);
    std::cout << "==== Tasks ====" << std::endl << std::flush;
    dump(Tasks);
    std::cout << "==== Time Entries ====" << std::endl << std::flush;
    for (auto i : TimeEntries) {
        std::cout << i->Description() << ", ";
    }
    std::cout << std::endl << std::flush;
    std::cout << "==== Countries ====" << std::endl << std::flush;
    dump(Countries);
}

Error UserData::loadAll(const std::string &json, bool with_user) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(json, root);
    return loadAll(root, with_user);
}

Error UserData::loadAll(const Json::Value &root, bool with_user) {
    Error err;

    if (with_user) {
        if (User) {
            User.clear();
        }
        User.create();
        User->LoadFromJSON(root["data"]);
    }

    err = loadTags(root["data"]["tags"]);
    if (!err.IsNoError())
        return err;
    err = loadClients(root["data"]["clients"]);
    if (!err.IsNoError())
        return err;
    err = loadProjects(root["data"]["projects"]);
    if (!err.IsNoError())
        return err;
    err = loadTimeEntries(root["data"]["time_entries"]);
    if (!err.IsNoError())
        return err;

    return Error::kNoError;
}

std::list<HTTPRequest> UserData::CollectChanges() {
    // first lock everything... this could probably be a tad shorter but it works
    auto tagsLock = Tags.lock(std::defer_lock);
    auto clientsLock = Clients.lock(std::defer_lock);
    auto projectsLock = Projects.lock(std::defer_lock);
    auto timeEntriesLock = TimeEntries.lock(std::defer_lock);
    auto countriesLock = Countries.lock(std::defer_lock);
    auto workspacesLock = Workspaces.lock(std::defer_lock);
    auto tasksLock = Tasks.lock(std::defer_lock);
    auto autotrackerRulesLock = AutotrackerRules.lock(std::defer_lock);
    auto timelineEventsLock = TimelineEvents.lock(std::defer_lock);
    std::lock(tagsLock, clientsLock, projectsLock, timeEntriesLock, countriesLock, workspacesLock, tasksLock, autotrackerRulesLock, timelineEventsLock);

    // define a lambda to collect the data from all containers without caring for their type
    auto collectChanges = [](auto &container) -> std::list<HTTPRequest> {
        std::list<HTTPRequest> innerList;
        for (auto i : container) {
            auto req = i->MakeRequest();
            if (!req.IsEmpty())
                innerList.push_back(req);
        }
        return innerList;
    };

    // and construct the list of all HTTP requests
    std::list<HTTPRequest> list;
    list.splice(list.end(), collectChanges(Tags));
    list.splice(list.end(), collectChanges(Clients));
    list.splice(list.end(), collectChanges(Projects));
    list.splice(list.end(), collectChanges(TimeEntries));
    list.splice(list.end(), collectChanges(Countries));
    list.splice(list.end(), collectChanges(Workspaces));
    list.splice(list.end(), collectChanges(Tasks));
    list.splice(list.end(), collectChanges(AutotrackerRules));
    list.splice(list.end(), collectChanges(TimelineEvents));
    return list;
}

void UserData::DeleteRelatedModelsWithWorkspace(id_t wid) {
    auto deleteByWid = [](auto &list, auto wid) {
        for (auto model : list) {
            if (model->WID() == wid) {
                model->MarkAsDeletedOnServer();
            }
        }
    };
    deleteByWid(Clients, wid);
    deleteByWid(Projects, wid);
    deleteByWid(Tasks, wid);
    deleteByWid(TimeEntries, wid);
    deleteByWid(Tags, wid);
}

void UserData::RemoveClientFromRelatedModels(id_t cid) {
    for (auto model : Projects) {
        if (model->CID() == cid) {
            model->SetCID(0);
        }
    }
}

void UserData::RemoveProjectFromRelatedModels(id_t pid) {
    auto deleteByPid = [](auto &list, auto pid) {
        for (auto model : list) {
            if (model->PID() == pid) {
                model->SetPID(0);
            }
        }
    };
    deleteByPid(Tasks, pid);
    deleteByPid(TimeEntries, pid);
}

void UserData::RemoveTaskFromRelatedModels(id_t tid) {
    for (auto model : TimeEntries) {
        if (model->TID() == tid) {
            model->SetTID(0);
        }
    }
}


} // namespace toggl
