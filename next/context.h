#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>

#include "misc/error.h"
#include "togglapi.h"
#include "eventqueue.h"

class Context {
public:
    struct Callbacks {
        std::function<void(const std::string&)> OnError;
    };

    Context(const std::string &app_name, Callbacks callbacks)
        : callbacks_(callbacks)
        , api(app_name)
        , eventQueue_(this)
    {

    }

    Callbacks *GetCallbacks() {
        return &callbacks_;
    }

    void Start() {
        eventQueue_.start();
    }

    void QueueTestPrint() {
        eventQueue_.schedule(std::chrono::system_clock::now() + std::chrono::seconds(5), std::bind(&Context::testPrint, this));
    }

    void Login(const std::string &username, const std::string &password) {
        eventQueue_.schedule(std::chrono::system_clock::now(), std::bind(&Context::login, this, username, password));
    }
private:
    void login(const std::string &username, const std::string &password);
    void testPrint() {
        std::cout << "Testing" << std::endl << std::flush;
    }

    Callbacks callbacks_;

    TogglApi api;

    EventQueue eventQueue_;
};

#endif // CONTEXT_H
