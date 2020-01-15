
#include <cstdlib>
#include <iostream>
#include <memory>

#include "credentials.h"

#include "misc/error.h"
#include "misc/logger.h"
#include "misc/const.h"

#include <json/json.h>

#include <string>
#include <sstream>

#include "Poco/DeflatingStream.h"
#include "Poco/Environment.h"
#include "Poco/Exception.h"
#include "Poco/FileStream.h"
#include "Poco/InflatingStream.h"
#include "Poco/Logger.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/Context.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/Net/HTTPCredentials.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/InvalidCertificateHandler.h"
#include "Poco/Net/NameValueCollection.h"
#include "Poco/Net/PrivateKeyPassphraseHandler.h"
#include "Poco/Net/SecureStreamSocket.h"
#include "Poco/Net/Session.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/NumberParser.h"
#include "Poco/StreamCopier.h"
#include "Poco/TextEncoding.h"
#include "Poco/URI.h"
#include "Poco/UTF8Encoding.h"

static class Settings {
public:
    std::string CACertPath { "" };
    std::string UserAgent { "AHOJ" };
    bool IgnoreCert { true };
} settings;

bool isRedirect(const Poco::Int64 status_code) {
    return (status_code >= 300 && status_code < 400);
}


class HttpClient {
public:
    HttpClient(const std::string &username, const std::string &password, bool ignoreCert = false)
        : username_(username)
        , password_(password)
    {
        auto acceptCertHandler = new Poco::Net::AcceptCertificateHandler(true);

        Poco::Net::Context::VerificationMode verification_mode = Poco::Net::Context::VERIFY_RELAXED;
        if (ignoreCert) {
            verification_mode = Poco::Net::Context::VERIFY_NONE;
        }
        poco_context_ = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", settings.CACertPath, verification_mode, 9, true, "ALL");
        Poco::Net::SSLManager::instance().initializeClient(nullptr, acceptCertHandler, poco_context_);
    }

    std::pair<Error, std::string> Request(const std::string &method, const std::string &host, const std::string &path, const std::string &payload = {}) {
        //HTTPSRequest req) const {
        auto response = makeHttpRequest(method, path, payload);

        if (isRedirect(response.first)) {
            // Reattempt request to the given location.
            Poco::URI uri(response.second);

            //req.host = uri.getScheme() + "://" + uri.getHost();
            //req.relative_url = uri.getPathEtc();
            std::string newHost =  uri.getScheme() + "://" + uri.getHost() + ":" + std::to_string(uri.getPort());
            std::string newPath = uri.getPathEtc();

            response = makeHttpRequest(method, newHost, path, payload);
        }
        return { Error::fromHttpStatus(response.first), response.second };
    }
    std::pair<Poco::Net::HTTPResponse::HTTPStatus, std::string> makeHttpRequest(const std::string &method, const std::string &host, const std::string &path, const std::string &payload = {}) {
        std::string encoded_url("");
        Poco::URI::encode(path, "", encoded_url);
        Poco::URI uri(host);

        auto session = createSession(uri);

        Poco::Net::HTTPRequest poco_req(method, encoded_url, Poco::Net::HTTPMessage::HTTP_1_1);
        setupRequest(poco_req);
        // FIXME: should get content type as parameter instead
        if (payload.size()) {
            poco_req.setContentType(kContentTypeApplicationJSON);
        }

        // TODO LEVEL 1
        // if (!req.form) {
            std::istringstream requestStream(payload);

            Poco::DeflatingInputStream gzipRequest(
                requestStream,
                Poco::DeflatingStreamBuf::STREAM_GZIP);
            Poco::DeflatingStreamBuf *pBuff = gzipRequest.rdbuf();

            Poco::Int64 size = pBuff->pubseekoff(0, std::ios::end, std::ios::in);
            pBuff->pubseekpos(0, std::ios::in);

            /* ANOTHER TODO
            if (req.method != Poco::Net::HTTPRequest::HTTP_GET) {
                poco_req.setContentLength(size);
                poco_req.set("Content-Encoding", "gzip");
                poco_req.setChunkedTransferEncoding(true);
            }
            */

            session->sendRequest(poco_req) << pBuff << std::flush;
        /* TODO LEVEL 1 CONTINUED
        } else {
            req.form->prepareSubmit(poco_req);
            std::ostream& send = session.sendRequest(poco_req);
            req.form->write(send);
        }
        */

        // Receive response
        Poco::Net::HTTPResponse response;
        return receiveResponse(response, session);
    }

    std::pair<Error, std::string> Get(const std::string &host, const std::string &path, const std::string &payload = {}) {
        return Request("GET", host, path, payload);
    }
private:
    std::shared_ptr<Poco::Net::HTTPClientSession> createSession(const Poco::URI &uri) {
        std::shared_ptr<Poco::Net::HTTPClientSession> session;
        if (uri.getScheme() == "http") {
            session = std::make_shared<Poco::Net::HTTPClientSession>(uri.getHost(), uri.getPort());
        }
        else {
            session = std::make_shared<Poco::Net::HTTPSClientSession>(uri.getHost(), uri.getPort(), poco_context_);
        }
        session->setKeepAlive(false);
        session->setTimeout(Poco::Timespan(timeout_seconds_ * Poco::Timespan::SECONDS));

        /* TODO
        error err = Netconf::ConfigureProxy(req.host + encoded_url, &session);
        if (err != noError) {
            resp.err = error("Error while configuring proxy: " + err);
            logger().error(resp.err);
            return resp;
        }
        */
        return session;
    }

    void setupRequest(Poco::Net::HTTPRequest &poco_req) {
        poco_req.setKeepAlive(false);
        poco_req.set("User-Agent", settings.UserAgent);
        // Request gzip unless downloading files
        poco_req.set("Accept-Encoding", "gzip");

        if (!username_.empty() && !password_.empty()) {
            Poco::Net::HTTPBasicCredentials cred(username_, password_);
            cred.authenticate(poco_req);
        }
    }

    std::pair<Poco::Net::HTTPResponse::HTTPStatus, std::string> receiveResponse(Poco::Net::HTTPResponse &response, std::shared_ptr<Poco::Net::HTTPClientSession> session) {
        std::string responseBody("");
        std::istream& is = session->receiveResponse(response);

        // When we get redirect, set the Location as response body
        if (isRedirect(response.getStatus()) && response.has("Location")) {
            std::string decoded_url("");
            Poco::URI::decode(response.get("Location"), decoded_url);
            responseBody = decoded_url;

        // Inflate, if gzip was sent
        } else if (response.has("Content-Encoding") &&
                   "gzip" == response.get("Content-Encoding")) {
            Poco::InflatingInputStream inflater(
                is,
                Poco::InflatingStreamBuf::STREAM_GZIP);
            {
                std::stringstream ss;
                ss << inflater.rdbuf();
                responseBody = ss.str();
            }

        // Write the response to string
        } else {
            std::streamsize n = Poco::StreamCopier::copyToString(is, responseBody);
        }

        if (429 == response.getStatus()) {
            /* TODO
            Poco::Timestamp ts = Poco::Timestamp() + (60 * kOneSecondInMicros);
            banned_until_[req.host] = ts;
            */
        }

        Error err = Error::fromHttpStatus(response.getStatus());

        // Parse human-readable error message from response if Content Type JSON
        if (err != Error::NO_ERROR && response.getContentType().find(kContentTypeApplicationJSON) != std::string::npos) {
            Json::Value root;
            Json::Reader reader;
            if (reader.parse(responseBody, root)) {
                responseBody = root["error_message"].asString();
            }
        }
    }

    std::string username_;
    std::string password_;
    Poco::Net::Context::Ptr poco_context_;

    Poco::Int64 timeout_seconds_ { 64 };
};

class TogglApi {
public:
    TogglApi(const std::string &app_name, const std::string &username = {}, const std::string &password = {})
        : client_(HttpClient(username, password, true))
        , app_name_(app_name)
    {

    }
    std::pair<Error, std::string> v8_me(const std::string &username, const std::string &password, bool with_related_data) {
        std::string path("/api/v8/me");
        path += "?app_name=" + app_name_;
        if (with_related_data)
            path += "&with_related_data=true";
        auto response = client_.Get(path, username, password);
        return response;
    }
private:
    HttpClient client_;
    std::string app_name_;
};

int main(void) {
    TogglApi api("linux_native_app");

    auto result = api.v8_me(TEST_USERNAME, TEST_PASSWORD, false);

    std::cout << result.second << std::endl;
    std::cout << result.first.String() << std::endl;

    return EXIT_SUCCESS;
}
