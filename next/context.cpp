#include "context.h"

#include <json/json.h>

#include "model/user.h"
#include "model/country.h"

namespace toggl {

void Context::login(const std::string &username, const std::string &password) {
    api.setCredentials(username, password);
    auto result = api.v8_me(true, 0);
    if (result.first == Error::NO_ERROR) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(result.second, root);

        if (user) {
            delete user;
        }
        user = UserModel::constructFromJSON(this, root["data"]);

        if (user) {
            logger.log("Logged in as user", user->ID());
            api.setCredentials(user->APIToken(), "api_token");
        }
        else {
            callbacks_.OnError("Login failed: " + result.first.String(), true);
            return;
        }

        data.loadTags(root["data"]["tags"]);
        data.loadClients(root["data"]["clients"]);
        data.loadProjects(root["data"]["projects"]);
        data.loadTimeEntries(root["data"]["time_entries"]);
        data.dumpAll();

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();

        eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
    }
    else {
        logger.log(result.first.String());
        callbacks_.OnError("Login failed: " + result.first.String(), true);
    }

    logger.warning("SIZEOF:", sizeof(TimeEntryModel));
}

void Context::getCountries() {
    auto result = api.v9_countries();
    //std::cout << result.first.String() << result.second << std::endl << std::flush;
    Json::Value root;
    Json::Reader reader;
    reader.parse(result.second, root);
    data.loadCountries(root);
    callbacks_.OnCountries();
}

void Context::sync() {
    logger.log("Syncing...");

    auto result = api.v8_me(true, 0);
    if (result.first == Error::NO_ERROR) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(result.second, root);

        data.loadTags(root["data"]["tags"]);
        data.loadClients(root["data"]["clients"]);
        data.loadProjects(root["data"]["projects"]);
        data.loadTimeEntries(root["data"]["time_entries"]);
        data.dumpAll();

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();
    }

    eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
}

} // namespace toggl
