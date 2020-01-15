#include "error.h"

bool Error::operator==(const Error::Code &c) { return code_ == c; }
bool Error::operator!=(const Error::Code &c) { return code_ != c; }

Error &Error::operator=(Error::Code &c) {
    code_ = c;
    return *this;
}

Error Error::fromHttpStatus(Poco::Net::HTTPResponse::HTTPStatus http_status) {
    switch (http_status) {
        case 200:
        case 201:
        case 202:
            return NO_ERROR;
        case 400:
            // data that you sending is not valid/acceptable
            return BAD_REQUEST;
        case 401:
            // ask user to enter login again, do not obtain new token automatically
            return UNAUTHORIZED;
        case 402:
            // requested action allowed only for pro workspace show user upsell
            // page / ask for workspace pro upgrade. do not retry same request
            // unless known that client is pro
            return PAYMENT_REQUIRED;
        case 403:
            // client has no right to perform given request. Server
            return FORBIDDEN;
        case 404:
            // request is not possible
            // (or not allowed and server does not tell why)
            return REQUEST_IMPOSSIBLE;
        case 410:
            return ENDPOINT_GONE;
        case 418:
            return UNSUPPORTED_APP;
        case 429:
            return CANNOT_CONNECT;
        case 500:
        case 501:
        case 502:
        case 503:
        case 504:
        case 505:
            return BACKEND_OFFLINE;
        default: {
            LOGGER.error("Unexpected HTTP status code: " + std::to_string(http_status));
            return CANNOT_CONNECT;
        }
    }
}
