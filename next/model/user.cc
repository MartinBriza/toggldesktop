// Copyright 2014 Toggl Desktop developers.

#include "user.h"

#include <time.h>

#include <sstream>

#include "./client.h"
#include "./const.h"
#include "./formatter.h"
#include "./project.h"
#include "./tag.h"
#include "./task.h"
#include "./time_entry.h"
#include "./timeline_event.h"

#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/Crypto/Cipher.h"
#include "Poco/Crypto/CipherFactory.h"
#include "Poco/Crypto/CipherKey.h"
#include "Poco/Crypto/CryptoStream.h"
#include "Poco/DigestStream.h"
#include "Poco/Logger.h"
#include "Poco/Random.h"
#include "Poco/RandomStream.h"
#include "Poco/SHA1Engine.h"
#include "Poco/Stopwatch.h"
#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "Poco/UTF8String.h"

namespace toggl {

UserModel::~UserModel() {
}

UserModel *UserModel::constructFromJSON(Context *ctx, const Json::Value &root) {
    UserModel *user = new UserModel(ctx);
    user->loadUserAndRelatedDataFromJSON(root, false);
    if (user->ID() == 0) {
        delete user;
        return nullptr;
    }
    return user;
}

void UserModel::SetFullname(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (fullname_ != value) {
        fullname_ = value;
        SetDirty();
    }
}

void UserModel::SetTimeOfDayFormat(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Formatter::TimeOfDayFormat = value;
    if (timeofday_format_ != value) {
        timeofday_format_ = value;
        SetDirty();
    }
}

void UserModel::SetDurationFormat(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Formatter::DurationFormat = value;
    if (duration_format_ != value) {
        duration_format_ = value;
        SetDirty();
    }
}

void UserModel::SetOfflineData(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (offline_data_ != value) {
        offline_data_ = value;
        SetDirty();
    }
}

void UserModel::SetStoreStartAndStopTime(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (store_start_and_stop_time_ != value) {
        store_start_and_stop_time_ = value;
        SetDirty();
    }
}

void UserModel::SetRecordTimeline(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (record_timeline_ != value) {
        record_timeline_ = value;
        SetDirty();
    }
}

void UserModel::SetEmail(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (email_ != value) {
        email_ = value;
        SetDirty();
    }
}

void UserModel::SetAPIToken(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // API token is not saved into DB, so no
    // no dirty checking needed for it.
    api_token_ = value;
}

void UserModel::SetSince(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (since_ != value) {
        since_ = value;
        SetDirty();
    }
}

void UserModel::SetDefaultWID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (default_wid_ != value) {
        default_wid_ = value;
        SetDirty();
    }
}

void UserModel::SetDefaultPID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (default_pid_ != value) {
        default_pid_ = value;
        SetDirty();
    }
}

void UserModel::SetDefaultTID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (default_tid_ != value) {
        default_tid_ = value;
        SetDirty();
    }
}

void UserModel::SetCollapseEntries(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (collapse_entries_ != value) {
        collapse_entries_ = value;
        SetDirty();
    }
}

bool UserModel::HasValidSinceDate() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // has no value
    if (!Since()) {
        return false;
    }

    // too old
    Poco::Timestamp ts = Poco::Timestamp::fromEpochTime(time(nullptr))
                         - (60 * Poco::Timespan::DAYS);
    Poco::Int64 min_allowed = ts.epochTime();
    if (Since() < min_allowed) {
        return false;
    }

    return true;
}

std::string UserModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " default_wid=" << default_wid_
        << " api_token=" << api_token_
        << " since=" << since_
        << " record_timeline=" << record_timeline_;
    return ss.str();
}

error UserModel::LoadUserAndRelatedDataFromJSONString(
    const std::string &json,
    const bool &including_related_data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);

    if (json.empty()) {
        Poco::Logger &logger = Poco::Logger::get("json");
        logger.warning("cannot load empty JSON");
        return noError;
    }

    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(json, root)) {
        return error("Failed to LoadUserAndRelatedDataFromJSONString");
    }

    SetSince(root["since"].asInt64());

    Poco::Logger &logger = Poco::Logger::get("json");
    std::stringstream s;
    s << "User data as of: " << Since();
    logger.debug(s.str());

    loadUserAndRelatedDataFromJSON(root["data"], including_related_data);

    return noError;
}

void UserModel::loadUserAndRelatedDataFromJSON(
    Json::Value data,
    const bool &including_related_data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);

    if (!data["id"].asUInt64()) {
        logger().error("Backend is sending invalid data: ignoring update without an ID");  // NOLINT
        return;
    }

    SetID(data["id"].asUInt64());
    SetDefaultWID(data["default_wid"].asUInt64());
    SetAPIToken(data["api_token"].asString());
    SetEmail(data["email"].asString());
    SetFullname(data["fullname"].asString());
    SetRecordTimeline(data["record_timeline"].asBool());
    SetStoreStartAndStopTime(data["store_start_and_stop_time"].asBool());
    SetTimeOfDayFormat(data["timeofday_format"].asString());
    SetDurationFormat(data["duration_format"].asString());
}

bool UserModel::LoadUserPreferencesFromJSON(
    Json::Value data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (data.isMember("CollapseTimeEntries")
            && data["CollapseTimeEntries"].asBool() != CollapseEntries()) {
        SetCollapseEntries(data["CollapseTimeEntries"].asBool());
        return true;
    }
    return false;
}


error UserModel::UserID(
    const std::string &json_data_string,
    Poco::UInt64 *result) {
    *result = 0;
    Json::Value root;
    Json::Reader reader;
    bool ok = reader.parse(json_data_string, root);
    if (!ok) {
        return error("error parsing UserID JSON");
    }
    *result = root["data"]["id"].asUInt64();
    return noError;
}

error UserModel::LoginToken(
    const std::string &json_data_string,
    std::string *result) {
    *result = "";
    Json::Value root;
    Json::Reader reader;
    bool ok = reader.parse(json_data_string, root);
    if (!ok) {
        return error("error parsing UserID JSON");
    }
    *result = root["login_token"].asString();
    return noError;
}

error UserModel::UpdateJSON(
    std::vector<TimeEntryModel *> * const time_entries,
    std::string *result) const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);

    poco_check_ptr(time_entries);

    Json::Value c;

    // Time entries go last
    for (std::vector<TimeEntryModel *>::const_iterator it =
        time_entries->begin();
            it != time_entries->end(); ++it) {
        Json::Value update;
        error err = (*it)->BatchUpdateJSON(&update);
        if (err != noError) {
            return err;
        }
        c.append(update);
    }

    Json::StyledWriter writer;
    *result = writer.write(c);

    return noError;
}

std::string UserModel::generateKey(const std::string &password) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Poco::SHA1Engine sha1;
    Poco::DigestOutputStream outstr(sha1);
    outstr << Email();
    outstr << password;
    outstr.flush();
    const Poco::DigestEngine::Digest &digest = sha1.digest();
    return Poco::DigestEngine::digestToHex(digest);
}

error UserModel::SetAPITokenFromOfflineData(const std::string &password) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (Email().empty()) {
        return Error::CANNOT_DECRYPT_WITHOUT_EMAIL;
    }
    if (password.empty()) {
        return Error::CANNOT_DECRYPT_WITHOUT_PASSWORD;
    }
    if (OfflineData().empty()) {
        return Error::CANNOT_DECRYPT_EMPTY_STRING;
    }
    try {
        Poco::Crypto::CipherFactory& factory =
            Poco::Crypto::CipherFactory::defaultFactory();

        std::string key = generateKey(password);

        Json::Value data;
        Json::Reader reader;
        if (!reader.parse(OfflineData(), data)) {
            return Error::OFFLINE_DATA_BROKEN;
        }

        std::istringstream istr(data["salt"].asString());
        Poco::Base64Decoder decoder(istr);
        std::string salt("");
        decoder >> salt;

        Poco::Crypto::CipherKey ckey("aes-256-cbc", key, salt);
        Poco::Crypto::Cipher* pCipher = factory.createCipher(ckey);

        std::string decrypted = pCipher->decryptString(
            data["encrypted"].asString(),
            Poco::Crypto::Cipher::ENC_BASE64);

        delete pCipher;
        pCipher = nullptr;

        SetAPIToken(decrypted);
    } catch(const Poco::Exception& exc) {
        return Error::POCO_EXCEPTION;
    } catch(const std::exception& ex) {
        return Error::STD_EXCEPTION;
    } catch(const std::string & ex) {
        return Error::STD_STRING_EXCEPTION;
    }
    return noError;
}

error UserModel::EnableOfflineLogin(
    const std::string &password) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (Email().empty()) {
        return Error::CANNOT_ENCRYPT_WITHOUT_EMAIL;
    }
    if (password.empty()) {
        return Error::CANNOT_ENCRYPT_WITHOUT_PASSWORD;
    }
    if (APIToken().empty()) {
        return Error::CANNOT_ENCRYPT_WITHOUT_API_TOKEN;
    }
    try {
        Poco::Crypto::CipherFactory& factory =
            Poco::Crypto::CipherFactory::defaultFactory();

        std::string key = generateKey(password);

        std::string salt("");
        Poco::RandomInputStream ri;
        ri >> salt;

        Poco::Crypto::CipherKey ckey("aes-256-cbc", key, salt);

        Poco::Crypto::Cipher* pCipher = factory.createCipher(ckey);

        std::ostringstream str;
        Poco::Base64Encoder enc(str);
        enc << salt;
        enc.close();

        Json::Value data;
        data["salt"] = str.str();
        data["encrypted"] = pCipher->encryptString(
            APIToken(),
            Poco::Crypto::Cipher::ENC_BASE64);
        std::string json = Json::FastWriter().write(data);

        delete pCipher;
        pCipher = nullptr;

        SetOfflineData(json);

        std::string token = APIToken();
        error err = SetAPITokenFromOfflineData(password);
        if (err != noError) {
            return err;
        }
        if (token != APIToken()) {
            return Error::OFFLINE_ENCRYPTION_FAILED;
        }
    } catch(const Poco::Exception& exc) {
        return Error::POCO_EXCEPTION;
    } catch(const std::exception& ex) {
        return Error::STD_EXCEPTION;
    } catch(const std::string & ex) {
        return Error::STD_STRING_EXCEPTION;
    }
    return noError;
}

std::string UserModel::ModelName() const {
    return kModelUser;
}

std::string UserModel::ModelURL() const {
    return "/api/v9/me";
}

}  // namespace toggl
