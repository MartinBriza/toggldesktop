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
    eventQueue_.schedule(std::bind(&Context::init, this));
}

void Context::Start() {
    eventQueue_.start();
    callbacks_.OnShowApp(true);
    eventQueue_.schedule(std::bind(&Context::start, this));
}

void Context::init()
{
    db_ = new Database("/home/mbriza/toggldesktop.db");
}

void Context::start() {
    auto user = *data.User;
    logger.log("Before loading, the user email is ", user->Email());
    db_->LoadCurrentUser(user);
    if (user->ID() > 0) {
        logger.log("HERE WE GO! User is: ", user->Email());
        logger.log("We have loaded ", data.TimeEntries.size(), " time entries.");
        data.dumpAll();

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();
    }
    else {
        logger.log("There was no user session in the database");
        callbacks_.OnLogin(true, 0);
        callbacks_.OnTimerState();
    }
}

void Context::login(const std::string &username, const std::string &password) {
    api.setCredentials(username, password);
    auto result = api.v8_me(true, 0);
    if (result) {
        data.loadAll(*result, true);
        data.dumpAll();

        if (data.User->ID() <= 0) {
            data.clear();
            callbacks_.OnError("Login failed: " + result.errorString(), true);
            return;
        }

        auto user = *data.User;
        logger.warning("User is ", user->Email());
        //db_->LoadUserByID(user->ID(), user);
        api.setCredentials(user->APIToken(), "api_token");

        db_->SetCurrentAPIToken(data.User->APIToken(), data.User->ID());
        Error err = user->EnableOfflineLogin(password);
        if (!err.IsNoError())
            GetCallbacks()->OnError(err.String(), true);

        std::vector<ModelChange> changes;
        db_->SaveUser(user, true, &changes);

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();

        eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
    }
    else {
        logger.log(result.errorString());
        callbacks_.OnError("Login failed: " + result.errorString(), true);
    }
}

void Context::getCountries() {
    auto result = api.v9_countries();
    //std::cout << result.first.String() << result.second << std::endl << std::flush;
    data.loadCountries(*result);
    callbacks_.OnCountries();
}

void Context::sync() {
    logger.log("Syncing...");

    auto result = api.v8_me(true, 0);
    if (result) {
        data.loadAll(*result);
        data.dumpAll();

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();
    }

    eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
}

} // namespace toggl
