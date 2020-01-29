#include "togglapi.h"

namespace toggl {

TogglApi::TogglApi(const std::string &app_name, const std::string &username, const std::string &password)
    : client_(HttpClient(username, password, true))
    , app_name_(app_name)
{

}

void TogglApi::setCredentials(const std::string &username, const std::string &password) {
    client_.setCredentials(username, password);
}

std::pair<Error, std::string> TogglApi::v8_me(bool with_related_data, int64_t since) {
    std::string path("/api/v8/me");
    path += "?app_name=" + app_name_;
    if (with_related_data)
        path += "&with_related_data=true";
    if (since)
        path += "&since=" + std::to_string(since);
    auto response = client_.Get("https://toggl.space", path);
    return response;
}

std::pair<Error, std::string> TogglApi::v9_status() {
    return client_.Get("https://toggl.space", "/api/v9/status");
}

std::pair<Error, std::string> TogglApi::v9_countries() {
    return client_.Get("https://toggl.space", "/api/v9/countries");
}

} // namespace toggl
