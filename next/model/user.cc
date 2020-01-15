// Copyright 2014 Toggl Desktop developers.

#include "../src/user.h"

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

User::~User() {
}

Project *User::CreateProject(
    const Poco::UInt64 workspace_id,
    const Poco::UInt64 client_id,
    const std::string &client_guid,
    const std::string &client_name,
    const std::string &project_name,
    const bool is_private,
    const std::string &project_color,
    const bool billable) {

    Project *p = new Project();
    p->SetWID(workspace_id);
    p->SetName(project_name);
    p->SetCID(client_id);
    p->SetClientGUID(client_guid);
    p->SetUID(ID());
    p->SetActive(true);
    p->SetPrivate(is_private);
    p->SetBillable(billable);
    p->SetClientName(client_name);
    if (!project_color.empty()) {
        p->SetColorCode(project_color);
    }

    AddProjectToList(p);

    return p;
}

void User::AddProjectToList(Project *p) {
    bool WIDMatch = false;
    bool CIDMatch = false;

    // We should push the project to correct alphabetical position
    // (since we try to avoid sorting the large list)
    for (std::vector<Project *>::iterator it =
        related.Projects.begin();
            it != related.Projects.end(); ++it) {
        Project *pr = *it;
        if (p->WID() == pr->WID()) {
            WIDMatch = true;
            if ((p->CID() == 0 && p->ClientGUID().empty()) && pr->CID() == 0) {
                // Handle adding project without client
                CIDMatch = true;
                if (Poco::UTF8::icompare(p->Name(), pr->Name()) < 0) {
                    related.Projects.insert(it, p);
                    return;
                }
            } else if (Poco::UTF8::icompare(p->ClientName(), pr->ClientName()) == 0) {
                // Handle adding project with client
                CIDMatch = true;
                if (Poco::UTF8::icompare(p->FullName(), pr->FullName()) < 0) {
                    related.Projects.insert(it,p);
                    return;
                }
            } else if (CIDMatch) {
                // in case new project is last in client list
                related.Projects.insert(it,p);
                return;
            } else if ((p->CID() != 0 || !p->ClientGUID().empty()) && pr->CID() != 0) {
                if (Poco::UTF8::icompare(p->FullName(), pr->FullName()) < 0) {
                    related.Projects.insert(it,p);
                    return;
                }
            }
        } else if (WIDMatch) {
            //In case new project is last in workspace list
            related.Projects.insert(it,p);
            return;
        }
    }

    // if projects vector is empty or project should be added to the end
    related.Projects.push_back(p);
}

std::string User::DateDuration(TimeEntry * const te) const {
    Poco::Int64 date_duration(0);
    std::string date_header = Formatter::FormatDateHeader(te->Start());
    for (std::vector<TimeEntry *>::const_iterator it =
        related.TimeEntries.begin();
            it != related.TimeEntries.end();
            ++it) {
        TimeEntry *n = *it;
        if (Formatter::FormatDateHeader(n->Start()) == date_header) {
            Poco::Int64 duration = n->DurationInSeconds();
            if (duration > 0) {
                date_duration += duration;
            }
        }
    }
    return Formatter::FormatDurationForDateHeader(date_duration);
}

bool User::HasPremiumWorkspaces() const {
    for (std::vector<Workspace *>::const_iterator it =
        related.Workspaces.begin();
            it != related.Workspaces.end();
            ++it) {
        Workspace *model = *it;
        if (model->Premium()) {
            return true;
        }
    }
    return false;
}

bool User::CanAddProjects() const {
    for (std::vector<Workspace *>::const_iterator it =
        related.Workspaces.begin();
            it != related.Workspaces.end();
            ++it) {
        Workspace *model = *it;
        if (model->OnlyAdminsMayCreateProjects()) {
            return false;
        }
    }
    return true;
}

void User::SetFullname(const std::string &value) {
    if (fullname_ != value) {
        fullname_ = value;
        SetDirty();
    }
}

void User::SetTimeOfDayFormat(const std::string &value) {
    Formatter::TimeOfDayFormat = value;
    if (timeofday_format_ != value) {
        timeofday_format_ = value;
        SetDirty();
    }
}

void User::SetDurationFormat(const std::string &value) {
    Formatter::DurationFormat = value;
    if (duration_format_ != value) {
        duration_format_ = value;
        SetDirty();
    }
}

void User::SetOfflineData(const std::string &value) {
    if (offline_data_ != value) {
        offline_data_ = value;
        SetDirty();
    }
}

void User::SetStoreStartAndStopTime(const bool value) {
    if (store_start_and_stop_time_ != value) {
        store_start_and_stop_time_ = value;
        SetDirty();
    }
}

void User::SetRecordTimeline(const bool value) {
    if (record_timeline_ != value) {
        record_timeline_ = value;
        SetDirty();
    }
}

void User::SetEmail(const std::string &value) {
    if (email_ != value) {
        email_ = value;
        SetDirty();
    }
}

void User::SetAPIToken(const std::string &value) {
    // API token is not saved into DB, so no
    // no dirty checking needed for it.
    api_token_ = value;
}

void User::SetSince(const Poco::Int64 value) {
    if (since_ != value) {
        since_ = value;
        SetDirty();
    }
}

void User::SetDefaultWID(const Poco::UInt64 value) {
    if (default_wid_ != value) {
        default_wid_ = value;
        SetDirty();
    }
}

void User::SetDefaultPID(const Poco::UInt64 value) {
    if (default_pid_ != value) {
        default_pid_ = value;
        SetDirty();
    }
}

void User::SetDefaultTID(const Poco::UInt64 value) {
    if (default_tid_ != value) {
        default_tid_ = value;
        SetDirty();
    }
}

void User::SetCollapseEntries(const bool value) {
    if (collapse_entries_ != value) {
        collapse_entries_ = value;
        SetDirty();
    }
}

// Stop a time entry, mark it as dirty.
// Note that there may be multiple TE-s running. If there are,
// all of them are stopped (multi-tracking is not supported by Toggl).
void User::Stop(std::vector<TimeEntry *> *stopped) {
    TimeEntry *te = RunningTimeEntry();
    while (te) {
        if (stopped) {
            stopped->push_back(te);
        }
        te->StopTracking();
        te = RunningTimeEntry();
    }
}

TimeEntry *User::RunningTimeEntry() const {
    for (std::vector<TimeEntry *>::const_iterator it =
        related.TimeEntries.begin();
            it != related.TimeEntries.end();
            ++it) {
        if ((*it)->DurationInSeconds() < 0) {
            return *it;
        }
    }
    return nullptr;
}

bool User::HasValidSinceDate() const {
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

std::string User::String() const {
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " default_wid=" << default_wid_
        << " api_token=" << api_token_
        << " since=" << since_
        << " record_timeline=" << record_timeline_;
    return ss.str();
}

void User::DeleteRelatedModelsWithWorkspace(const Poco::UInt64 wid) {
    deleteRelatedModelsWithWorkspace(wid, &related.Clients);
    deleteRelatedModelsWithWorkspace(wid, &related.Projects);
    deleteRelatedModelsWithWorkspace(wid, &related.Tasks);
    deleteRelatedModelsWithWorkspace(wid, &related.TimeEntries);
    deleteRelatedModelsWithWorkspace(wid, &related.Tags);
}

void User::RemoveClientFromRelatedModels(const Poco::UInt64 cid) {
    for (std::vector<Project *>::iterator it = related.Projects.begin();
            it != related.Projects.end(); ++it) {
        Project *model = *it;
        if (model->CID() == cid) {
            model->SetCID(0);
        }
    }
}

void User::RemoveProjectFromRelatedModels(const Poco::UInt64 pid) {
    removeProjectFromRelatedModels(pid, &related.Tasks);
    removeProjectFromRelatedModels(pid, &related.TimeEntries);
}

error User::LoadUserAndRelatedDataFromJSONString(
    const std::string &json,
    const bool &including_related_data) {

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

void User::loadUserAndRelatedDataFromJSON(
    Json::Value data,
    const bool &including_related_data) {

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

bool User::LoadUserPreferencesFromJSON(
    Json::Value data) {
    if (data.isMember("CollapseTimeEntries")
            && data["CollapseTimeEntries"].asBool() != CollapseEntries()) {
        SetCollapseEntries(data["CollapseTimeEntries"].asBool());
        return true;
    }
    return false;
}


error User::UserID(
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

error User::LoginToken(
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

error User::UpdateJSON(
    std::vector<TimeEntry *> * const time_entries,
    std::string *result) const {

    poco_check_ptr(time_entries);

    Json::Value c;

    // Time entries go last
    for (std::vector<TimeEntry *>::const_iterator it =
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

std::string User::generateKey(const std::string &password) {
    Poco::SHA1Engine sha1;
    Poco::DigestOutputStream outstr(sha1);
    outstr << Email();
    outstr << password;
    outstr.flush();
    const Poco::DigestEngine::Digest &digest = sha1.digest();
    return Poco::DigestEngine::digestToHex(digest);
}

error User::SetAPITokenFromOfflineData(const std::string &password) {
    if (Email().empty()) {
        return error("cannot decrypt offline data without an e-mail");
    }
    if (password.empty()) {
        return error("cannot decrypt offline data without a password");
    }
    if (OfflineData().empty()) {
        return error("cannot decrypt empty string");
    }
    try {
        Poco::Crypto::CipherFactory& factory =
            Poco::Crypto::CipherFactory::defaultFactory();

        std::string key = generateKey(password);

        Json::Value data;
        Json::Reader reader;
        if (!reader.parse(OfflineData(), data)) {
            return error("failed to parse offline data");
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
        return exc.displayText();
    } catch(const std::exception& ex) {
        return ex.what();
    } catch(const std::string & ex) {
        return ex;
    }
    return noError;
}

error User::EnableOfflineLogin(
    const std::string &password) {
    if (Email().empty()) {
        return error("cannot enable offline login without an e-mail");
    }
    if (password.empty()) {
        return error("cannot enable offline login without a password");
    }
    if (APIToken().empty()) {
        return error("cannot enable offline login without an API token");
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
            return error("offline login encryption failed");
        }
    } catch(const Poco::Exception& exc) {
        return exc.displayText();
    } catch(const std::exception& ex) {
        return ex.what();
    } catch(const std::string & ex) {
        return ex;
    }
    return noError;
}

bool User::CanSeeBillable(
    const Workspace *ws) const {
    if (!HasPremiumWorkspaces()) {
        return false;
    }
    if (ws && !ws->Premium()) {
        return false;
    }
    return true;
}

std::string User::ModelName() const {
    return kModelUser;
}

std::string User::ModelURL() const {
    return "/api/v9/me";
}

template<class T>
void deleteZombies(
    const std::vector<T> &list,
    const std::set<Poco::UInt64> &alive) {
    for (size_t i = 0; i < list.size(); ++i) {
        BaseModel *model = list[i];
        if (!model->ID()) {
            // If model has no server-assigned ID, it's not even
            // pushed to server. So actually we don't know if it's
            // a zombie or not. Ignore:
            continue;
        }
        if (alive.end() == alive.find(model->ID())) {
            model->MarkAsDeletedOnServer();
        }
    }
}

template <typename T>
void deleteRelatedModelsWithWorkspace(const Poco::UInt64 wid,
                                      std::vector<T *> *list) {
    typedef typename std::vector<T *>::iterator iterator;
    for (iterator it = list->begin(); it != list->end(); ++it) {
        T *model = *it;
        if (model->WID() == wid) {
            model->MarkAsDeletedOnServer();
        }
    }
}

template <typename T>
void removeProjectFromRelatedModels(const Poco::UInt64 pid,
                                    std::vector<T *> *list) {
    typedef typename std::vector<T *>::iterator iterator;
    for (iterator it = list->begin(); it != list->end(); ++it) {
        T *model = *it;
        if (model->PID() == pid) {
            model->SetPID(0);
        }
    }
}

}  // namespace toggl
