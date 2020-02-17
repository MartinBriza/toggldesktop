#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "misc/error.h"

#include <string>
#include <memory>

#include <Poco/URI.h>
#include <Poco/Net/Context.h>
#include <Poco/Net/HTTPRequest.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPClientSession.h>
#include <Poco/Net/HTMLForm.h>

using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPClientSession;

namespace toggl {

class HttpClient {
public:
    HttpClient(const std::string &username, const std::string &password, bool ignoreCert = false);

    static bool isRedirect(const Poco::Int64 status_code);

    void setCredentials(const std::string &username, const std::string &password);

    inline static const std::string CONTENTTYPE_FORM_DATA { "multipart/form-data" };
    inline static const std::string CONTENTTYPE_JSON { "application/json" } ;

    Result<std::string> Get(const std::string &host, const std::string &path, const std::string &payload = {});

    Result<std::string> Request(const std::string &method, const std::string &host, const std::string &path, const std::string &payload = {});
private:
    std::pair<HTTPResponse::HTTPStatus, std::string> internalRequest(const std::string &method, const std::string &host, const std::string &path, const std::string &payload);
    std::pair<HTTPResponse::HTTPStatus, std::string> internalRequest(const std::string &method, const std::string &host, const std::string &path, Poco::Net::HTMLForm *form);

    std::shared_ptr<HTTPClientSession> createSession(const Poco::URI &uri);
    void setupRequest(HTTPRequest &poco_req);
    std::pair<HTTPResponse::HTTPStatus, std::string> receiveResponse(Poco::Net::HTTPResponse &response, std::shared_ptr<Poco::Net::HTTPClientSession> session);

    std::string username_;
    std::string password_;

    std::string CACertPath_;
    std::string userAgent_ { "linux_native_app" };

    Poco::Net::Context::Ptr poco_context_;

    Poco::Int64 timeout_seconds_ { 64 };
};

} // namespace toggl

#endif // HTTPCLIENT_H
