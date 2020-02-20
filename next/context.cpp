#include "context.h"

#include <json/json.h>

#include "model/user.h"
#include "model/country.h"

namespace toggl {
/*
 * TOOLS
 */
template<typename T>
static void EnsureWID(const UserData* data, T &model) {
    // Do nothing if TE already has WID assigned
    if (model->WID()) {
        return;
    }

    // Try to set default user WID
    if (data->User->DefaultWID()) {
        model->SetWID(data->User->DefaultWID());
        return;
    }

    // Try to set first WID available
    auto it = data->Workspaces.begin();
    if (it != data->Workspaces.end()) {
        auto ws = *it;
        model->SetWID(ws->ID());
    }
}


/*
 * CONTEXT
 */

Context::Context(const std::string &app_name, Context::Callbacks callbacks)
    : callbacks_(callbacks)
    , api(app_name)
    , data(this)
    , eventQueue_(this)
{
    Poco::Data::SQLite::Connector::registerConnector();
    eventQueue_.schedule(std::bind(&Context::init, this));
}

void Context::UiStart() {
    eventQueue_.start();
    callbacks_.OnShowApp(true);
    eventQueue_.schedule(std::bind(&Context::uiStart, this));
}

uuid_t Context::Start(const std::string &description, const std::string &duration, uuid_t task, uuid_t project, const std::string &tags, const time_t &started, const time_t &ended) {
    auto now = time(nullptr);

    auto te = GetData()->TimeEntries.create();
    auto user = *GetData()->User;
    auto p = GetData()->Projects.byGUIDconst(project);
    auto t = GetData()->Tasks.byGUIDconst(task);
    // TODO
    te->SetCreatedWith("HTTPSClient::Config.UserAgent()");
    te->SetDescription(description);
    te->SetUID(user->ID());
    if (p) {
        te->SetPID(p->ID());
        te->SetProjectGUID(p->GUID());
        // Try to set workspace ID from project
        te->SetWID(p->WID());
        te->SetBillable(p->Billable());
    }
    if (t) {
        te->SetTID(t->ID());
        // Try to set workspace ID from task
        if (te->WID() <= 0)
            te->SetWID(t->WID());
    }
    te->SetTags(tags);

    if (started == 0 && ended == 0) {
        if (!duration.empty()) {
            int seconds = Formatter::ParseDurationString(duration);
            te->SetDurationInSeconds(seconds);
            te->SetStop(now);
            te->SetStart(te->Stop() - te->DurationInSeconds());
        } else {
            te->SetDurationInSeconds(-now);
            // dont set Stop, TE is running
            te->SetStart(now);
        }
    } else {
        int seconds = int(ended - started);
        te->SetDurationInSeconds(seconds);
        te->SetStop(ended);
        te->SetStart(started);
    }

    EnsureWID(GetData(), te);

    te->SetUIModified();

    eventQueue_.schedule([this]() { callbacks_.OnTimeEntryList(); });
    eventQueue_.schedule([this]() { callbacks_.OnTimerState(); });

    logger.log("Managed to create a new time entry: ", te->GUID());
    std::vector<ModelChange> changes;
    Error err = db_->SaveUser(user, true, &changes);
    if (err.IsNoError())
        return te->GUID();
    else {
        return {};
    }
}

void Context::Stop() {
    auto te = GetData()->RunningTimeEntry();
    while (te) {
        te->DiscardAt(time(nullptr));
        te = GetData()->RunningTimeEntry();
    }
    callbacks_.OnTimerState();
    callbacks_.OnTimeEntryList();
    auto user = *GetData()->User;
    std::vector<ModelChange> changes;
    db_->SaveUser(user, true, &changes);
}

void Context::init()
{
    db_ = new Database("/home/mbriza/toggldesktop.db");
}

void Context::uiStart() {
    auto user = *data.User;
    logger.log("Before loading, the user email is ", user->Email());
    db_->LoadCurrentUser(user);
    if (user->ID() > 0) {
        logger.log("HERE WE GO! User is: ", user->Email());
        logger.log("We have loaded ", data.TimeEntries.size(), " time entries.");
        data.dumpAll();
        api.setCredentials(user->APIToken(), "api_token");

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();
        eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
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

    auto result = api.v8_me(true, data.LastSince());
    if (result) {
        data.loadAll(*result);
        data.dumpAll();

        callbacks_.OnTimeEntryList();
        callbacks_.OnTimerState();
    }

    auto requests = GetData()->CollectChanges();
    Json::Reader reader;
    std::map<std::string, std::string> responses;
    for (auto i : requests) {
        if (!i.IsEmpty()) {
            auto response = api.Client()->Request(i.method, "https://toggl.space", i.relative_url, i.payload);
            logger.log("=================================");
            logger.log(i.payload);
            logger.log("---------------------------------");
            logger.log(*response);
            logger.log("=================================");
            if (response.error().IsNoError()) {
                if (i.method == Poco::Net::HTTPRequest::HTTP_DELETE) {
                    auto model = GetData()->ByGuid(i.model);
                    model->MarkAsDeletedOnServer();
                }
                else {
                    responses.insert({i.model, *response});
                }
            }
        }
    }

    {
        // first lock everything... this could probably be a tad shorter but it works
        auto tagsLock = GetData()->Tags.lock(false);
        auto clientsLock = GetData()->Clients.lock(false);
        auto projectsLock = GetData()->Projects.lock(false);
        auto timeEntriesLock = GetData()->TimeEntries.lock(false);
        auto countriesLock = GetData()->Countries.lock(false);
        auto workspacesLock = GetData()->Workspaces.lock(false);
        auto tasksLock = GetData()->Tasks.lock(false);
        auto autotrackerRulesLock = GetData()->AutotrackerRules.lock(false);
        auto timelineEventsLock = GetData()->TimelineEvents.lock(false);
        std::lock(tagsLock, clientsLock, projectsLock, timeEntriesLock, countriesLock, workspacesLock, tasksLock, autotrackerRulesLock, timelineEventsLock);

        // then handle the responses
        for (auto i : responses) {
            auto model = GetData()->ByGuid(i.first);
            if (!model) {
                logger.log("Could not find model ", i.first);
                continue;
            }
            Json::Value root;
            reader.parse(i.second, root);
            auto id = root["id"].asUInt64();
            if (!id) {
                // TODO
                continue;
            }
            if (model->ID() <= 0) {
                model->SetID(id);
                continue;
            }
            else if (model->ID() != id) {
                // TODO
            }
            model->LoadFromJSON(root);
        }
    }

    auto user = *GetData()->User;
    std::vector<ModelChange> changes;
    db_->SaveUser(user, true, &changes);

    eventQueue_.schedule(std::chrono::seconds(5), std::bind(&Context::sync, this));
}

} // namespace toggl
