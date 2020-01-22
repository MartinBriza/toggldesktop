#include "userdata.h"

void UserData::dumpAll() {
    {
        auto tags = Tags();
        std::cout << "==== Tags ====" << std::endl << std::flush;
        dump(tags);
    }{
        auto clients = Clients();
        std::cout << "==== Clients ====" << std::endl << std::flush;
        dump(clients);
    }{
        auto projects = Projects();
        std::cout << "==== Projects ====" << std::endl << std::flush;
        dump(projects);
    }{
        auto TEs = TimeEntries();
        std::cout << "==== Time Entries ====" << std::endl << std::flush;
        for (auto i : *TEs) {
            std::cout << i->Description() << ", ";
        }
    }
    std::cout << std::endl << std::flush;
}
