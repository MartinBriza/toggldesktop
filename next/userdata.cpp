#include "userdata.h"

void UserData::dumpAll() {
    {
        std::unique_lock<std::recursive_mutex> lock(tagsMutex_);
        std::cout << "==== Tags ====" << std::endl << std::flush;
        dump(tags_);
        lock.unlock();
    }{
        std::unique_lock<std::recursive_mutex> lock(clientsMutex_);
        std::cout << "==== Clients ====" << std::endl << std::flush;
        dump(clients_);
        lock.unlock();
    }{
        std::unique_lock<std::recursive_mutex> lock(projectsMutex_);
        std::cout << "==== Projects ====" << std::endl << std::flush;
        dump(projects_);
        lock.unlock();
    }{
        std::unique_lock<std::recursive_mutex> lock(timeEntriesMutex_);
        std::cout << "==== Time Entries ====" << std::endl << std::flush;
        for (auto i : timeEntries_) {
            std::cout << i->Description() << ", ";
        }
        lock.unlock();
    }
    std::cout << std::endl << std::flush;
}

const std::list<toggl::TimeEntryModel *> &UserData::timeEntries() {
    return timeEntries_;
}
