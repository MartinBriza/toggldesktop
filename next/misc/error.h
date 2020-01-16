#ifndef ERROR_H
#define ERROR_H

#include "logger.h"

#include <string>
#include <map>

#include <Poco/Net/HTTPResponse.h>

class Error {
public:
    enum Code {
        NO_ERROR,
        UNAUTHORIZED,
        BAD_REQUEST,
        PAYMENT_REQUIRED,
        FORBIDDEN,
        REQUEST_IMPOSSIBLE,
        ENDPOINT_GONE,
        UNSUPPORTED_APP,
        CANNOT_CONNECT,
        BACKEND_OFFLINE,

        MALFORMED_DATA
    };
    inline static const std::map<Code, std::string> values {
        { NO_ERROR, "" },
        { UNAUTHORIZED, "Please log in again" },
        { BAD_REQUEST, "BAD_REQUEST" },
        { PAYMENT_REQUIRED, "PAYMENT_REQUIRED" },
        { FORBIDDEN, "FORBIDDEN" },
        { REQUEST_IMPOSSIBLE, "REQUEST_IMPOSSIBLE" },
        { ENDPOINT_GONE, "ENDPOINT_GONE" },
        { UNSUPPORTED_APP, "UNSUPPORTED_APP" },
        { CANNOT_CONNECT, "CANNOT_CONNECT" },
        { BACKEND_OFFLINE, "BACKEND_OFFLINE" },

        { MALFORMED_DATA, "MALFORMED_DATA" }
    };

    Error(Code c) : code_(c) {}
    bool operator==(const Code &c);
    bool operator!=(const Code &c);
    Error &operator=(Code &c);

    std::string String() const;

    static Error fromHttpStatus(Poco::Net::HTTPResponse::HTTPStatus http_status);
private:
    Code code_;
};

#endif // ERROR_H
