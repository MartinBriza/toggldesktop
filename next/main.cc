#include "credentials.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>

#include "model/tag.h"
#include "model/time_entry.h"
#include "model/project.h"
#include "model/client.h"
#include "model/user.h"

#include "misc/error.h"
#include "misc/logger.h"
#include "misc/const.h"

#include "httpclient.h"
#include "togglapi.h"

#include <json/json.h>

using namespace toggl;

void OnError(const std::string &error) {
    std::cerr << "An error occurred: " << error << std::endl << std::flush;
}


class UserData {
public:
    UserData() {

    }

    void dumpAll() {
        std::cout << "==== Tags ====" << std::endl << std::flush;
        dump(tags_);
        std::cout << "==== Clients ====" << std::endl << std::flush;
        dump(clients_);
        std::cout << "==== Projects ====" << std::endl << std::flush;
        dump(projects_);
        std::cout << "==== Time Entries ====" << std::endl << std::flush;
        for (auto i : timeEntries_) {
            std::cout << i->Description() << ", ";
        }
        std::cout << std::endl << std::flush;
    }

    Error loadTags(const Json::Value &root) {
        return load<Tag>(tags_, root);
    }
    Error loadClients(const Json::Value &root) {
        return load<Client>(clients_, root);
    }
    Error loadProjects(const Json::Value &root) {
        return load<Project>(projects_, root);
    }
    Error loadTimeEntries(const Json::Value &root) {
        return load<TimeEntry>(timeEntries_, root);
    }

private:
    template <typename T>
    void dump(std::list<T*> &list) {
        for (auto i : list) {
            std::cout << i->Name() << ", ";
        }
        std::cout << std::endl << std::flush;
    }
    template <typename T>
    void clear(std::list<T*> &list) {
        for (auto i : list)
            delete i;
        list.clear();
    }
    template <typename T>
    Error load(std::list<T*> &list, const Json::Value &root) {
        clear(list);
        if (!root.isArray())
            return Error::MALFORMED_DATA;
        for (auto i : root) {
            T *item = new T();
            item->LoadFromJSON(i);
            list.push_back(item);
        }
        return Error::NO_ERROR;
    }

    std::list<Tag*> tags_;
    std::list<Client*> clients_;
    std::list<Project*> projects_;
    std::list<TimeEntry*> timeEntries_;
};

static struct Callbacks {
    void (*OnError)(const std::string &error);
} callbacks {
    OnError
};

class Context {
public:
    Context(const std::string &app_name, Callbacks callbacks)
        : callbacks_(callbacks)
        , api(app_name)
        , workThread(&Context::threadLoop, this)
    {

    }

    void Login(const std::string &username, const std::string &password) {
        std::unique_lock<std::recursive_mutex> lock(queueLock);
        queue.push(std::bind(&Context::login, this, username, password));
    }
private:
    void login(const std::string &username, const std::string &password) {
        api.setCredentials(username, password);
        auto result = api.v8_me(true, 0);
        if (result.first == Error::NO_ERROR) {
            Json::Value root;
            Json::Reader reader;
            reader.parse(result.second, root);


            std::cout << root.toStyledString() << std::endl << std::flush;

            User *user = User::constructFromJSON(this, root["data"]);

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

    Callbacks callbacks_;

    std::queue<std::function<void(void)>> queue;
    std::recursive_mutex queueLock;

    void threadLoop() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::unique_lock<std::recursive_mutex> lock(queueLock);
            if (queue.size()) {
                queue.front()();
                queue.pop();
            }
        }
    }

    TogglApi api;
    // needs to be always last, here and in the constructor initializer list too
    std::thread workThread;
};

int main(void) {
    Context *ctx = new Context("linux_native_app", callbacks);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    ctx->Login(TEST_USERNAME, TEST_PASSWORD);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    return EXIT_SUCCESS;


    TogglApi api("linux_native_app", TEST_USERNAME, TEST_PASSWORD);

    auto result = api.v8_me(true);

    std::cout << result.second << std::endl;
    std::cout << result.first.String() << std::endl;

    return EXIT_SUCCESS;
}
