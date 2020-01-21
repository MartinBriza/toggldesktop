#include "context.h"

#include <json/json.h>

#include "model/user.h"
#include "model/country.h"

void Context::login(const std::string &username, const std::string &password) {
    api.setCredentials(username, password);
    auto result = api.v8_me(true, 0);
    if (result.first == Error::NO_ERROR) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(result.second, root);

        std::cout << root.toStyledString() << std::endl << std::flush;

        if (user) {
            delete user;
        }
        user = toggl::UserModel::constructFromJSON(this, root["data"]);

        if (user) {
            std::cout << "Logged in as user " << user->ID() << std::endl << std::flush;
            api.setCredentials(user->APIToken(), "api_token");
            /*
            if (callbacks_.OnLogin)
                callbacks_.OnLogin(true, user->ID());
            */
        }
        else {
            if (callbacks_.OnError) {
                std::cerr << "CALLING ONERROR\n";
                callbacks_.OnError("Login failed: " + result.first.String(), true);
            }
            else {
                std::cerr << "ONERROR IS MIA\n";
            }
            return;
        }

        data.loadTags(root["data"]["tags"]);
        data.loadClients(root["data"]["clients"]);
        data.loadProjects(root["data"]["projects"]);
        data.loadTimeEntries(root["data"]["time_entries"]);
        data.dumpAll();

        if (callbacks_.OnTimeEntryList)
            callbacks_.OnTimeEntryList(true, data.timeEntries(), true);
    }
    else {
        std::cout << result.first.String() << std::endl << std::flush;
        if (callbacks_.OnError) {
            std::cerr << "CALLING ONERROR\n";
            callbacks_.OnError("Login failed: " + result.first.String(), true);
        }
    }
}

void Context::getCountries() {
    auto result = api.v9_countries();
    //std::cout << result.first.String() << result.second << std::endl << std::flush;
    Json::Value root;
    Json::Reader reader;
    reader.parse(result.second, root);
    std::list<toggl::CountryModel*> countries;
    for (auto i : root) {
        toggl::CountryModel *c = new toggl::CountryModel();
        c->LoadFromJSON(i);
        countries.push_back(c);
    }
    if (callbacks_.OnCountries)
        callbacks_.OnCountries(countries);
    for (auto i : countries) {
        delete i;
    }
}
