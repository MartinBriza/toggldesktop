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

} // namespace toggl
