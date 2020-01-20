#include "context.h"

#include <json/json.h>

#include "model/user.h"
#include "userdata.h"

void Context::login(const std::string &username, const std::string &password) {
    api.setCredentials(username, password);
    auto result = api.v8_me(true, 0);
    if (result.first == Error::NO_ERROR) {
        Json::Value root;
        Json::Reader reader;
        reader.parse(result.second, root);

        std::cout << root.toStyledString() << std::endl << std::flush;

        toggl::User *user = toggl::User::constructFromJSON(this, root["data"]);

        if (user) {
            std::cout << "Logged in as user " << user->ID() << std::endl << std::flush;
            api.setCredentials(user->APIToken(), "api_token");
        }
        else {
            if (callbacks_.OnError)
                callbacks_.OnError("Login failed: " + result.first.String());
            return;
        }

        UserData *data = new UserData();
        data->loadTags(root["data"]["tags"]);
        data->loadClients(root["data"]["clients"]);
        data->loadProjects(root["data"]["projects"]);
        data->loadTimeEntries(root["data"]["time_entries"]);
        data->dumpAll();

    }
    else {
        std::cout << result.first.String() << std::endl << std::flush;
    }
}
