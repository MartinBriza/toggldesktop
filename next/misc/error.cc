#include "error.h"

#include <set>

namespace toggl {

std::string Error::String() const {
    try {
        return values.at(code_);
    }
    catch (std::out_of_range &) {
        return "Unknown error (" + std::to_string(code_) + ")";
    }
}

const std::string &Error::StringRef() const {
    thread_local static std::string lastError;
    try {
        return values.at(code_);
    }
    catch (std::out_of_range &) {
        lastError = "Unknown error (" + std::to_string(code_) + ")";
        return lastError;
    }
}

Error Error::fromString(const std::string &message) {
    return kNoError;
}

bool Error::operator==(const Error::Code &c) const { return code_ == c; }
bool Error::operator!=(const Error::Code &c) const { return code_ != c; }
bool Error::operator==(const Error &o) const { return code_ == o.code_; }
bool Error::operator!=(const Error &o) const { return code_ != o.code_; }

bool Error::operator<(const Error &rhs) const {
    return code_ < rhs.code_;
}

Error &Error::operator=(Error::Code &c) {
    code_ = c;
    return *this;
}

bool Error::IsNoError() const {
    return code_ == NO_ERROR;
}

Error Error::fromHttpStatus(Poco::Net::HTTPResponse::HTTPStatus http_status) {
    return fromHttpStatus((int64_t) http_status);
}

Error Error::fromHttpStatus(int64_t http_status) {
    switch (http_status) {
    case 200:
    case 201:
    case 202:
        return kNoError;
    case 400:
        // data that you sending is not valid/acceptable
        return kBadRequestError;
    case 401:
        // ask user to enter login again, do not obtain new token automatically
        return kUnauthorizedError;
    case 402:
        // requested action allowed only for pro workspace show user upsell
        // page / ask for workspace pro upgrade. do not retry same request
        // unless known that client is pro
        return kPaymentRequiredError;
    case 403:
        // client has no right to perform given request. Server
        return kForbiddenError;
    case 404:
        // request is not possible
        // (or not allowed and server does not tell why)
        return kRequestIsNotPossible;
    case 410:
        return kEndpointGoneError;
    case 418:
        return kUnsupportedAppError;
    case 429:
        return kCannotConnectError;
    case 500:
    case 501:
    case 502:
    case 503:
    case 504:
    case 505:
        return kBackendIsDownError;
    default: {
        Logger("error").error("Unexpected HTTP status code: " + std::to_string(http_status));
        return kCannotConnectError;
    }
    }
}

// FIXME this is pretty likely to be completely wrong
Error Error::fromServerError(const std::string &message) {
    const static std::set<Code> codes {
        kTimeEntryNotFound, kTimeEntryCreatedWithInvalid, kCannotAccessProjectError, kCannotAccessTaskError,
        kOverMaxDurationError, kInvalidStartTimeError, kStartNotBeforeStopError, kBillableIsAPremiumFeature
    };
    for (auto i : codes) {
        if (message.find(Error(i).String()) != std::string::npos)
            return i;
    }
    return kNoError;
}

std::ostream &operator<<(std::ostream &out, const Error &t) {
    out << t.String();
    return out;
}

} // namespace toggl
