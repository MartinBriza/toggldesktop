#ifndef TOGGLAPI_H
#define TOGGLAPI_H

#include "httpclient.h"

class TogglApi {
public:
    TogglApi(const std::string &app_name, const std::string &username = {}, const std::string &password = {});

    std::pair<Error, std::string> v8_me(bool with_related_data);

    std::pair<Error, std::string> v9_status();

    // POST timeline_upload + /api/v8/timeline_settings + payload(json)
    // POST api + /api/v8/feedback/web + form()
    // POST api + /api/v8/desktop_login_tokens + payload(empty json)

    // GET api + /api/v8/desktop_login + login_token=
    // GET api + /api/v9/me/time_entries + since=
    // GET api + /api/v9/me/workspaces
    // GET api + /api/v9/workspaces/ + ID + /preferences
    // GET api + /api/v9/me/preferences/desktop
    // POST api + /api/v9/signup + app_name=  + payload(json)
    // POST api + /api/v9/signup + payload(json)
    //  ^- google signup sends app name but regular doesn't for some reason
    // POST api + /api/v9/me/accept_tos
    // GET api + /api/v9/countries

private:
    HttpClient client_;
    std::string app_name_;
};
#endif // TOGGLAPI_H
