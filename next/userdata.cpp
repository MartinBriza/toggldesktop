#include "userdata.h"

namespace toggl {

void UserData::dumpAll() {
    {
        std::cout << "==== Tags ====" << std::endl << std::flush;
        dump(Tags);
    }{
        std::cout << "==== Clients ====" << std::endl << std::flush;
        dump(Clients);
    }{
        std::cout << "==== Projects ====" << std::endl << std::flush;
        dump(Projects);
    }{
        std::cout << "==== Time Entries ====" << std::endl << std::flush;
        for (auto i : TimeEntries) {
            std::cout << i->Description() << ", ";
        }
    }
    std::cout << std::endl << std::flush;
}

Error UserData::loadAll(const std::string &json) {
    Json::Value root;
    Json::Reader reader;
    reader.parse(json, root);
    return loadAll(root);
}

Error UserData::loadAll(const Json::Value &root) {
    Error err;
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

} // namespace toggl
