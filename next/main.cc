#include "credentials.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <atomic>
#include <condition_variable>

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

    Error loadTags(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(tagsMutex_);
        return load<Tag>(tags_, root);
    }
    Error loadClients(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(clientsMutex_);
        return load<Client>(clients_, root);
    }
    Error loadProjects(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(projectsMutex_);
        return load<Project>(projects_, root);
    }
    Error loadTimeEntries(const Json::Value &root) {
        std::scoped_lock<std::recursive_mutex> lock(timeEntriesMutex_);
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
    std::recursive_mutex tagsMutex_;
    std::list<Client*> clients_;
    std::recursive_mutex clientsMutex_;
    std::list<Project*> projects_;
    std::recursive_mutex projectsMutex_;
    std::list<TimeEntry*> timeEntries_;
    std::recursive_mutex timeEntriesMutex_;
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
    {

    }

    void Start() {
        workThread = std::thread(&Context::threadLoop, this);
    }

    void QueueTestPrint() {
        std::unique_lock<std::recursive_mutex> lock(queueLock);
        queue.emplace(std::chrono::system_clock::now() + std::chrono::seconds(5), std::bind(&Context::testPrint, this));
        lock.unlock();
        cv.notify_all();
    }

    void Login(const std::string &username, const std::string &password) {
        std::unique_lock<std::recursive_mutex> lock(queueLock);
        queue.emplace(std::chrono::system_clock::now(), std::bind(&Context::login, this, username, password));
        lock.unlock();
        cv.notify_all();
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
    void testPrint() {
        std::cout << "Testing" << std::endl << std::flush;
    }

    Callbacks callbacks_;

    std::multimap<std::chrono::time_point<std::chrono::system_clock>, std::function<void(void)>> queue;
    std::recursive_mutex queueLock;
    std::condition_variable cv;
    std::mutex cvMutex;

    [[noreturn]] void threadLoop() {
        while (true) {
            std::unique_lock<std::recursive_mutex> lock(queueLock);
            auto time = std::chrono::system_clock::now();
            while (queue.begin() != queue.end() && queue.begin()->first <= time) {
                std::cerr << "The time is now" << std::to_string(time.time_since_epoch().count()) << std::endl << std::flush;
                std::cerr << "The event time is" << std::to_string(queue.begin()->first.time_since_epoch().count()) << std::endl << std::flush;
                queue.begin()->second();
                queue.erase(queue.begin());
            }
            if (queue.begin() != queue.end()) {
                std::cerr << "Waiting for an event" << std::endl << std::flush;
                auto nextTime = queue.begin()->first;
                lock.unlock();
                std::unique_lock<std::mutex> cvLock(cvMutex);
                cv.wait_until(cvLock, nextTime);
            }
            else {
                std::cerr << "Waiting before somebody wakes me up" << std::endl << std::flush;
                lock.unlock();
                std::unique_lock<std::mutex> cvLock(cvMutex);
                cv.wait(cvLock);
            }
        }
    }

    TogglApi api;
    // needs to be always last, here and in the constructor initializer list too
    std::thread workThread;
};

int main(void) {
    Context *ctx = new Context("linux_native_app", callbacks);
    ctx->Start();

    std::this_thread::sleep_for(std::chrono::seconds(5));
    ctx->QueueTestPrint();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ctx->Login(TEST_USERNAME, TEST_PASSWORD);

    std::this_thread::sleep_for(std::chrono::seconds(15));

    return EXIT_SUCCESS;


    TogglApi api("linux_native_app", TEST_USERNAME, TEST_PASSWORD);

    auto result = api.v8_me(true);

    std::cout << result.second << std::endl;
    std::cout << result.first.String() << std::endl;

    return EXIT_SUCCESS;
}
