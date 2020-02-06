#include "context.h"

#include <json/json.h>

#include "model/user.h"
#include "model/country.h"

namespace toggl {

Context::Context(const std::string &app_name, Context::Callbacks callbacks)
    : callbacks_(callbacks)
    , api(app_name)
    , data(this)
    , eventQueue_(this)
{
    Poco::Data::SQLite::Connector::registerConnector();
    db_ = new Database("/home/mbriza/code/build-mb-toggldesktop-Desktop-Debug/toggldesktop.db");
    auto user = *data.User;
    logger.log("Before loading, the user email is ", user->Email());
    db_->LoadCurrentUser(user);
    logger.log("HERE WE GO! User is: ", user->Email());
}

void Context::login(const std::string &username, const std::string &password) {
    api.setCredentials(username, password);
    auto result = api.v8_me(true, 0);
    if (result.first == Error::NO_ERROR) {
        data.loadAll(result.second, true);
        data.dumpAll();

        if (data.User) {
            api.setCredentials(data.User->APIToken(), "api_token");
        }
        else {
            data.clear();
            callbacks_.OnError("Login failed: " + result.first.String(), true);
            return;
        }

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
    data.loadCountries(result.second);
    callbacks_.OnCountries();
}

void Context::sync() {
    logger.log("Syncing...");

    auto result = api.v8_me(true, 0);
    if (result.first == Error::NO_ERROR) {
        data.loadAll(result.second);
        data.dumpAll();

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();
    }

    eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
}

} // namespace toggl
