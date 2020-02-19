// Copyright 2014 Toggl Desktop developers.

// NB! Setters should not directly calculate
// anything besides setting the field asked.
// This is because same setters are used when
// loading from database, JSON etc. If time entry
// needs to be recalculated after some user
// action, the recalculation should be started
// from context, using specific functions, not
// setters.

#include "time_entry.h"

#include <sstream>
#include <algorithm>

#include <json/json.h>  // NOLINT

#include "formatter.h"
#include "string_tools.h"
#include "userdata.h"

#include "Poco/DateTime.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Logger.h"
#include "Poco/NumberParser.h"
#include "Poco/Timestamp.h"

namespace toggl {

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::useRef;
using Poco::Data::Keywords::limit;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::now;

TimeEntryModel::TimeEntryModel(UserData *parent, const Json::Value &root)
    : BaseModel(parent)
{
    LoadFromJSON(root);
}

bool TimeEntryModel::ResolveError(const error &err) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (durationTooLarge(err) && Stop() && Start()) {
        Poco::Int64 seconds =
            (std::min)(Stop() - Start(),
                       Poco::Int64(kMaxTimeEntryDurationSeconds));
        SetDurationInSeconds(seconds);
        return true;
    }
    if (startTimeWrongYear(err) && Stop() && Start()) {
        Poco::Int64 seconds =
            (std::min)(Stop() - Start(),
                       Poco::Int64(kMaxTimeEntryDurationSeconds));
        SetDurationInSeconds(seconds);
        SetStart(Stop() - Duration());
        return true;
    }
    if (stopTimeMustBeAfterStartTime(err) && Stop() && Start()) {
        SetStop(Start() + DurationInSeconds());
        return true;
    }
    if (userCannotAccessWorkspace(err)) {
        SetWID(0);
        SetPID(0);
        SetTID(0);
        return true;
    }
    if (userCannotAccessTheSelectedProject(err)) {
        SetPID(0);
        SetTID(0);
        return true;
    }
    if (userCannotAccessSelectedTask(err)) {
        SetTID(0);
        return true;
    }
    if (billableIsAPremiumFeature(err)) {
        SetBillable(false);
        return true;
    }
    if (isMissingCreatedWith(err)) {
        // SetCreatedWith(HTTPSClient::Config.UserAgent()); TODO
        return true;
    }
    return false;
}

bool TimeEntryModel::isNotFound(const error &err) const {
    return err == error::TIME_ENTRY_NOT_FOUMD;
}

locked<WorkspaceModel> TimeEntryModel::Workspace() {
    if (WID() > 0)
        return Parent()->Workspaces.byId(WID());
    return {};
}

locked<const WorkspaceModel> TimeEntryModel::Workspace() const {
    if (WID() > 0)
        return Parent()->Workspaces.byId(WID());
    return {};
}

locked<ProjectModel> TimeEntryModel::Project() {
    if (PID() > 0)
        return Parent()->Projects.byId(WID());
    return {};
}

locked<const ProjectModel> TimeEntryModel::Project() const {
    if (PID() > 0)
        return Parent()->Projects.byId(WID());
    return {};
}

locked<TaskModel> TimeEntryModel::Task() {
    if (TID() > 0)
        return Parent()->Tasks.byId(WID());
    return {};
}

locked<const TaskModel> TimeEntryModel::Task() const {
    if (TID() > 0)
        return Parent()->Tasks.byId(WID());
    return {};
}

locked<UserModel> TimeEntryModel::User() {
    return *Parent()->User;
}

locked<const UserModel> TimeEntryModel::User() const {
    return *Parent()->User;
}

bool TimeEntryModel::isMissingCreatedWith(const error &err) const {
    return err == error::INVALID_INPUT;
}

bool TimeEntryModel::userCannotAccessTheSelectedProject(const error &err) const {
    return err == error::ACCESS_PROHIBITED;
}

bool TimeEntryModel::userCannotAccessSelectedTask(const error &err) const {
    return err == error::ACCESS_PROHIBITED;
}

bool TimeEntryModel::durationTooLarge(const error &err) const {
    return err == error::DURATION_TOO_LONG;
}

bool TimeEntryModel::startTimeWrongYear(const error &err) const {
    return err == error::START_TIME_OUT_OF_RANGE;
}

bool TimeEntryModel::stopTimeMustBeAfterStartTime(const error &err) const {
    return err == error::END_TIME_BEFORE_START_TIME;
}

bool TimeEntryModel::billableIsAPremiumFeature(const error &err) const {
    return err == error::PREMIUM_FEATURE;
}

void TimeEntryModel::DiscardAt(const Poco::Int64 at) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (!IsTracking()) {
        logger().error("Cannot discard time entry that is not tracking");
        return;
    }

    if (!at) {
        logger().error("Cannot discard time entry without a timestamp");
        return;
    }

    if (at < Start()) {
        logger().error("Cannot discard time entry with start time bigger than current moment");
        return;
    }

    Poco::Int64 duration = at - Start();

    if (duration < 0) {
        logger().error("Discarding with this time entry would result in negative duration");  // NOLINT
        return;
    }

    SetDurationInSeconds(duration);
    SetStop(at);
    SetUIModified();
}

void TimeEntryModel::StopTracking() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    DiscardAt(time(nullptr));
}

std::string TimeEntryModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "TimeEntry"
        << " ID=" << ID()
        << " local_id=" << LocalID()
        << " description=" << description_
        << " wid=" << wid_
        << " guid=" << GUID()
        << " pid=" << pid_
        << " tid=" << tid_
        << " start=" << start_
        << " stop=" << stop_
        << " duration=" << duration_in_seconds_
        << " billable=" << billable_
        << " unsynced=" << unsynced_
        << " duronly=" << duronly_
        << " tags=" << Tags()
        << " created_with=" << CreatedWith()
        << " ui_modified_at=" << UIModifiedAt()
        << " deleted_at=" << DeletedAt()
        << " updated_at=" << UpdatedAt();
    return ss.str();
}

void TimeEntryModel::SetLastStartAt(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (last_start_at_ != value) {
        last_start_at_ = value;
    }
}

void TimeEntryModel::SetDurOnly(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (duronly_ != value) {
        duronly_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetStart(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (start_ != value) {
        start_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetStop(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (stop_ != value) {
        stop_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetDescription(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    const std::string &trimValue = trim_whitespace(value);
    if (description_ != trimValue) {
        description_ = trimValue;
        SetDirty();
    }
}

void TimeEntryModel::SetStopString(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetStop(Formatter::Parse8601(value));
}

void TimeEntryModel::SetCreatedWith(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (created_with_ != value) {
        created_with_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetBillable(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (billable_ != value) {
        billable_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetWID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (wid_ != value) {
        wid_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetStopUserInput(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetStopString(value);

    if (Stop() < Start()) {
        // Stop time cannot be before start time,
        // it'll get an error from backend.
        Poco::Timestamp ts =
            Poco::Timestamp::fromEpochTime(Stop()) + 1*Poco::Timespan::DAYS;
        SetStop(ts.epochTime());
    }

    if (Stop() < Start()) {
        logger().error("Stop time must be after start time!");
        return;
    }

    if (!IsTracking()) {
        SetDurationInSeconds(Stop() - Start());
    }

    if (Dirty()) {
        ClearValidationError();
        SetUIModified();
    }
}

void TimeEntryModel::SetTID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (tid_ != value) {
        tid_ = value;
        SetDirty();
    }
}

static const char kTagSeparator = '\t';

void TimeEntryModel::SetTags(const std::string &tags) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (Tags() != tags) {
        TagNames.clear();
        if (!tags.empty()) {
            std::stringstream ss(tags);
            while (ss.good()) {
                std::string tag;
                getline(ss, tag, kTagSeparator);
                TagNames.push_back(tag);
            }
        }
        SetDirty();
    }
}

void TimeEntryModel::SetPID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (pid_ != value) {
        pid_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetDurationInSeconds(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (duration_in_seconds_ != value) {
        duration_in_seconds_ = value;
        SetDirty();
    }
}

void TimeEntryModel::SetStartUserInput(const std::string &value,
                                  const bool keepEndTimeFixed) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Poco::Int64 start = Formatter::Parse8601(value);
    if (IsTracking()) {
        SetDurationInSeconds(-start);
    } else {
        auto stop = Stop();
        if (keepEndTimeFixed && stop > start) {
            SetDurationInSeconds(stop - start);
        } else {
            SetStop(start + DurationInSeconds());
        }
    }
    SetStart(start);

    if (Dirty()) {
        ClearValidationError();
        SetUIModified();
    }
}

void TimeEntryModel::SetStartString(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetStart(Formatter::Parse8601(value));
}

void TimeEntryModel::SetDurationUserInput(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    int seconds = Formatter::ParseDurationString(value);
    if (IsTracking()) {
        time_t now = time(nullptr);
        time_t start = now - seconds;
        SetStart(start);
        SetDurationInSeconds(-start);
    } else {
        SetDurationInSeconds(seconds);
    }
    SetStop(Start() + seconds);

    if (Dirty()) {
        ClearValidationError();
        SetUIModified();
    }
}

void TimeEntryModel::SetProjectGUID(const uuid_t &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (project_guid_ != value) {
        project_guid_ = value;
        SetDirty();
    }
}

const std::string TimeEntryModel::Tags() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    for (auto it = TagNames.begin(); it != TagNames.end(); ++it) {
        if (it != TagNames.begin()) {
            ss << kTagSeparator;
        }
        ss << *it;
    }
    return ss.str();
}

const std::string TimeEntryModel::TagsHash() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::vector<std::string> sortedTagNames(TagNames);
    sort(sortedTagNames.begin(), sortedTagNames.end());
    std::stringstream ss;
    for (auto it = sortedTagNames.begin(); it != sortedTagNames.end(); ++it) {
        if (it != sortedTagNames.begin()) {
            ss << kTagSeparator;
        }
        ss << *it;
    }
    return ss.str();
}

std::string TimeEntryModel::StopString() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return Formatter::Format8601(stop_);
}

std::string TimeEntryModel::StartString() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return Formatter::Format8601(start_);
}

bool TimeEntryModel::IsToday() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Poco::Timestamp ts = Poco::Timestamp::fromEpochTime(Start());
    Poco::LocalDateTime datetime(ts);
    Poco::LocalDateTime today;
    return today.year() == datetime.year() &&
           today.month() == datetime.month() &&
           today.day() == datetime.day();
}

error TimeEntryModel::LoadFromJSON(const Json::Value &data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // No ui_modified_at in server responses.
    // Compare updated_at with ui_modified_at to see if ui has been changed
    Json::Value at = data["at"];
    Poco::Int64 updated_at(0);
    if (at.isString()) {
        updated_at = Formatter::Parse8601(at.asString());
    } else {
        updated_at = at.asInt64();
    }

    if (data.isMember("id")) {
        SetID(data["id"].asUInt64());
    }

    if (updated_at != 0 &&
            (UIModifiedAt() >= updated_at ||
             UpdatedAt() >= updated_at)) {
        std::stringstream ss;
        ss  << "Will not overwrite time entry "
            << "[" << String() << "]"
            << " with server data because we have a newer or same updated_at"
            << " [Server updated_at: " << updated_at << "]";
        logger().debug(ss.str());
        // TODO maybe probably actually an error
        return noError;
    }

    if (data.isMember("tags")) {
        loadTagsFromJSON(data["tags"]);
    }

    if (data.isMember("created_with")) {
        SetCreatedWith(data["created_with"].asString());
    }

    SetDescription(data["description"].asString());

    if (data.isMember("wid")) {
        SetWID(data["wid"].asUInt64());
    } else {
        SetWID(0);
    }
    if (data.isMember("pid")) {
        SetPID(data["pid"].asUInt64());
    } else {
        SetPID(0);
    }
    if (data.isMember("tid")) {
        SetTID(data["tid"].asUInt64());
    } else {
        SetTID(0);
    }
    SetStartString(data["start"].asString());
    SetStopString(data["stop"].asString());
    SetDurationInSeconds(data["duration"].asInt64());
    SetBillable(data["billable"].asBool());
    SetDurOnly(data["duronly"].asBool());
    SetUpdatedAtString(data["at"].asString());

    SetUIModifiedAt(0);
    ClearUnsynced();
    return noError;
}

Json::Value TimeEntryModel::SaveToJSON() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Json::Value n;
    if (ID()) {
        n["id"] = Json::UInt64(ID());
    }
    n["description"] = Formatter::EscapeJSONString(Description());
    // Workspace ID can't be 0 on server side. So don't
    // send 0 if we have no default workspace ID, because
    // NULL is not 0
    if (WID()) {
        n["wid"] = Json::UInt64(WID());
    }
    n["guid"] = GUID();
    if (!PID() && !ProjectGUID().empty()) {
        n["pid"] = ProjectGUID();
    } else {
        n["pid"] = Json::UInt64(PID());
    }

    if (PID()) {
        n["pid"] = Json::UInt64(PID());
    } else {
        n["pid"] = Json::nullValue;
    }

    if (TID()) {
        n["tid"] = Json::UInt64(TID());
    } else {
        n["tid"] = Json::nullValue;
    }

    n["start"] = StartString();
    if (Stop()) {
        n["stop"] = StopString();
    }
    n["duration"] = Json::Int64(DurationInSeconds());
    n["billable"] = Billable();
    n["duronly"] = DurOnly();
    n["ui_modified_at"] = Json::UInt64(UIModifiedAt());
    n["created_with"] = Formatter::EscapeJSONString(CreatedWith());

    Json::Value tag_nodes;
    if (TagNames.size() > 0) {
        for (std::vector<std::string>::const_iterator it = TagNames.begin();
                it != TagNames.end();
                ++it) {
            std::string tag_name = Formatter::EscapeJSONString(*it);
            tag_nodes.append(Json::Value(tag_name));
        }
    } else {
        Json::Reader reader;
        reader.parse("[]", tag_nodes);
    }
    n["tags"] = tag_nodes;

    return n;
}

Poco::Int64 TimeEntryModel::RealDurationInSeconds() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    auto now = time(nullptr);

    return now + DurationInSeconds();
}

void TimeEntryModel::loadTagsFromJSON(Json::Value list) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    TagNames.clear();

    for (unsigned int i = 0; i < list.size(); i++) {
        std::string tag = list[i].asString();
        if (!tag.empty()) {
            TagNames.push_back(tag);
        }
    }
}

std::string TimeEntryModel::ModelName() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return kModelTimeEntry;
}

std::string TimeEntryModel::ModelURL() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream relative_url;
    relative_url << "/api/v9/workspaces/"
                 << WID() << "/time_entries";

    if (ID()) {
        relative_url << "/" << ID();
    }

    return relative_url.str();
}

}   // namespace toggl
