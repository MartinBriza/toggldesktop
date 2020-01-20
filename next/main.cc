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
#include "context.h"

#include <json/json.h>

using namespace toggl;

void OnError(const std::string &error) {
    std::cerr << "An error occurred: " << error << std::endl << std::flush;
}

static Context::Callbacks callbacks {
    OnError
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
