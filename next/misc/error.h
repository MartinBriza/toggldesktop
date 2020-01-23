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

        MALFORMED_DATA,

        // TODO CHANGE THIS
        kOverMaxDurationError,
        kMaxTagsPerTimeEntryError,
        kInvalidStartTimeError,
        kInvalidStopTimeError,
        kInvalidDateError,
        kStartNotBeforeStopError,
        kMaximumDescriptionLengthError,

        kCheckYourSignupError,
        kEndpointGoneError,
        kForbiddenError,
        kUnsupportedAppError,
        kUnauthorizedError,
        kCannotConnectError,
        kCannotSyncInTestEnv,
        kBackendIsDownError,
        kBadRequestError,
        kRequestIsNotPossible,
        kPaymentRequiredError,
        kCannotAccessWorkspaceError,
        kEmailNotFoundCannotLogInOffline,
        kInvalidPassword,
        kCannotEstablishProxyConnection,
        kCertificateVerifyFailed,
        kCheckYourProxySetup,
        kCheckYourFirewall,
        kProxyAuthenticationRequired,
        kCertificateValidationError,
        kUnacceptableCertificate,
        kCannotUpgradeToWebSocketConnection,
        kSSLException,
        kRateLimit,
        kCannotWriteFile,
        kIsSuspended,
        kRequestToServerFailedWithStatusCode403,
        kMissingWorkspaceID,
        kCannotContinueDeletedTimeEntry,
        kCannotDeleteDeletedTimeEntry,
        kErrorRuleAlreadyExists,
        kPleaseSelectAWorkspace,
        kClientNameMustNotBeEmpty,
        kProjectNameMustNotBeEmpty,
        kProjectNameAlready,
        kProjectNameAlreadyExists,
        kClientNameAlreadyExists,
        kDatabaseDiskMalformed,
        kMissingWS,
        kOutOfDatePleaseUpgrade,
        kThisEntryCantBeSavedPleaseAdd,

        // These were hardcoded at random places over the code:
        BATCH_UPDATE_WITHOUT_GUID,
        CANNOT_DECRYPT_WITHOUT_EMAIL,
        CANNOT_DECRYPT_WITHOUT_PASSWORD,
        CANNOT_DECRYPT_EMPTY_STRING,
        CANNOT_ENCRYPT_WITHOUT_EMAIL,
        CANNOT_ENCRYPT_WITHOUT_PASSWORD,
        CANNOT_ENCRYPT_WITHOUT_API_TOKEN,
        OFFLINE_DATA_BROKEN,
        OFFLINE_ENCRYPTION_FAILED,

        // TODO: These errors need more data, probably only to be printed in the log (users don't really need to see POCO errors)
        POCO_EXCEPTION,
        STD_EXCEPTION,
        STD_STRING_EXCEPTION,

        // I'm tired
        IM_TOO_TIRED_TO_CREATE_CONSTANTS_FOR_THAT_MANY_ERRORS,
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

        { MALFORMED_DATA, "MALFORMED_DATA" },

        // TODO RENAME
        { kOverMaxDurationError, "Max allowed duration per 1 time entry is 999 hours" },
        { kMaxTagsPerTimeEntryError, "Tags are limited to 50 per task" },
        { kInvalidStartTimeError, "Start time year must be between 2006 and 2030" },
        { kInvalidStopTimeError, "Stop time year must be between 2006 and 2030" },
        { kInvalidDateError, "Date year must be between 2006 and 2030" },
        { kStartNotBeforeStopError, "Stop time must be after start time" },
        { kMaximumDescriptionLengthError, "Maximum length for description (3000 chars) exceeded" },

        { kCheckYourSignupError, "Signup failed - please check your details. The e-mail might be already taken." },
        { kEndpointGoneError, "The API endpoint used by this app is gone. Please contact Toggl support!" },
        { kForbiddenError, "Invalid e-mail or password!" },
        { kUnsupportedAppError, "This version of the app is not supported any more. Please visit Toggl website to download a supported app." },
        { kUnauthorizedError, "Unauthorized! Please login again." },
        { kCannotConnectError, "Cannot connect to Toggl" },
        { kCannotSyncInTestEnv, "Cannot sync in test env" },
        { kBackendIsDownError, "Backend is down" },
        { kBadRequestError, "Data that you are sending is not valid/acceptable" },
        { kRequestIsNotPossible, "Request is not possible" },
        { kPaymentRequiredError, "Requested action allowed only for Non-Free workspaces. Please upgrade!" },
        { kCannotAccessWorkspaceError, "cannot access workspace" },
        { kEmailNotFoundCannotLogInOffline, "Login failed. Are you online?" },
        { kInvalidPassword, "Invalid password" },
        { kCannotEstablishProxyConnection, "Cannot establish proxy connection" },
        { kCertificateVerifyFailed, "certificate verify failed" },
        { kCheckYourProxySetup, "Check your proxy setup" },
        { kCheckYourFirewall, "Check your firewall" },
        { kProxyAuthenticationRequired, "Proxy Authentication Required" },
        { kCertificateValidationError, "Certificate validation error" },
        { kUnacceptableCertificate, "Unacceptable certificate from www.toggl.com" },
        { kCannotUpgradeToWebSocketConnection, "Cannot upgrade to WebSocket connection" },
        { kSSLException, "SSL Exception" },
        { kRateLimit, "Too many requests, sync delayed by 1 minute" },
        { kCannotWriteFile, "Cannot write file" },
        { kIsSuspended, "is suspended" },
        { kRequestToServerFailedWithStatusCode403, "Request to server failed with status code: 403" },
        { kMissingWorkspaceID, "Missing workspace ID" },
        { kCannotContinueDeletedTimeEntry, "Cannot continue deleted time entry" },
        { kCannotDeleteDeletedTimeEntry, "Cannot delete deleted time entry" },
        { kErrorRuleAlreadyExists, "rule already exists" },
        { kPleaseSelectAWorkspace, "Please select a workspace" },
        { kClientNameMustNotBeEmpty, "Client name must not be empty" },
        { kProjectNameMustNotBeEmpty, "Project name must not be empty" },
        { kProjectNameAlready, "Project name already" },
        { kProjectNameAlreadyExists, "Project name already exists" },
        { kClientNameAlreadyExists, "Client name already exists" },
        { kDatabaseDiskMalformed, "The database disk image is malformed" },
        { kMissingWS, "You no longer have access to your last workspace" },
        { kOutOfDatePleaseUpgrade, "Your version of Toggl Desktop is out of date, please upgrade!" },
        { kThisEntryCantBeSavedPleaseAdd, "This entry can't be saved - please add" },

        // These were hardcoded at random places over the code:
        { BATCH_UPDATE_WITHOUT_GUID, "Cannot export model to batch update without a GUID" },

        { IM_TOO_TIRED_TO_CREATE_CONSTANTS_FOR_THAT_MANY_ERRORS, "This would be an error message if I had enough time to make them all a proper constant"},
    };

    Error(Code c) : code_(c) {}
    // TODO GET RID OF THIS THING
    Error(const std::string &) : code_(IM_TOO_TIRED_TO_CREATE_CONSTANTS_FOR_THAT_MANY_ERRORS) { }
    std::string thisWillGetRemoved() const { return "something"; }
    bool operator==(const Code &c) const;
    bool operator!=(const Code &c) const;
    bool operator==(const Error &o) const;
    bool operator!=(const Error &o) const;
    bool operator<(const Error &rhs) const;
    Error &operator=(Code &c);

    bool noError() const;

    std::string String() const;

    static Error fromHttpStatus(Poco::Net::HTTPResponse::HTTPStatus http_status);
private:
    Code code_;
};

#endif // ERROR_H
