// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_TIME_ENTRY_H_
#define SRC_TIME_ENTRY_H_

#include <string>
#include <vector>

#include "./base_model.h"
#include "./formatter.h"
#include "./types.h"
#include "misc/memory.h"

#include "Poco/Types.h"

namespace toggl {
template<typename T> class ProtectedContainer;
class UserModel;

class TOGGL_INTERNAL_EXPORT TimeEntryModel : public BaseModel, public TimedEvent {
    TimeEntryModel(UserData *parent)
        : BaseModel(parent)
    , wid_(0)
    , pid_(0)
    , tid_(0)
    , billable_(false)
    , start_(0)
    , stop_(0)
    , duration_in_seconds_(0)
    , description_("")
    , duronly_(false)
    , created_with_("")
    , project_guid_("")
    , unsynced_(false)
    , last_start_at_(0) {}
public:
    friend class ProtectedContainer<TimeEntryModel>;

    virtual ~TimeEntryModel() {}

    // Override BaseModel

    std::string ModelName() const override;
    std::string ModelURL() const override;
    std::string String() const override;
    virtual bool ResolveError(const error &err) override;
    error LoadFromJSON(const Json::Value &value) override;
    Json::Value SaveToJSON() const override;

    /******************************************************************
     * SETTERS AND GETTERS
     */
    const Poco::Int64 &LastStartAt() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return last_start_at_;
    }
    void SetLastStartAt(const Poco::Int64 value);

    std::vector<std::string> TagNames;

    const std::string Tags() const;
    void SetTags(const std::string &tags);

    const std::string TagsHash() const;

    const Poco::UInt64 &WID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return wid_;
    }
    void SetWID(const Poco::UInt64 value);

    const Poco::UInt64 &PID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return pid_;
    }
    void SetPID(const Poco::UInt64 value);

    const Poco::UInt64 &TID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return tid_;
    }
    void SetTID(const Poco::UInt64 value);

    const bool &Billable() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return billable_;
    }
    void SetBillable(const bool value);

    const Poco::Int64 &DurationInSeconds() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return duration_in_seconds_;
    }
    void SetDurationInSeconds(const Poco::Int64 value);

    const bool &DurOnly() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return duronly_;
    }
    void SetDurOnly(const bool value);

    const std::string &Description() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return description_;
    }
    void SetDescription(const std::string &value);

    std::string StartString() const;
    void SetStartString(const std::string &value);

    std::string StopString() const;
    void SetStopString(const std::string &value);

    const Poco::Int64 &Stop() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return stop_;
    }
    void SetStop(const Poco::Int64 value);

    const std::string &CreatedWith() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return created_with_;
    }
    void SetCreatedWith(const std::string &value);

    const uuid_t &ProjectGUID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return project_guid_;
    }
    void SetProjectGUID(const uuid_t &);

    // Implement TimedEvent
    const Poco::Int64 &Start() const override {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return start_;
    }
    void SetStart(const Poco::Int64 value);
    virtual const Poco::Int64 &Duration() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return DurationInSeconds();
    }

    /******************************************************************
     * SIMPLE DERIVED DATA
     */
    bool IsToday() const;

    bool IsTracking() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return duration_in_seconds_ < 0;
    }

    Poco::Int64 RealDurationInSeconds() const;

    bool isNotFound(const error &err) const;

    const std::string GroupHash() const;

    /******************************************************************
     * DEPENDENCY TREE DERIVED DATA
     */
    locked<WorkspaceModel> Workspace();
    locked<const WorkspaceModel> Workspace() const;
    locked<ProjectModel> Project();
    locked<const ProjectModel> Project() const;
    locked<TaskModel> Task();
    locked<const TaskModel> Task() const;
    locked<UserModel> User();
    locked<const UserModel> User() const;

    /******************************************************************
     * COMPLEX DATA MODIFICATION
     * (beyond the complexity of just basic setters)
     */
    void DiscardAt(const Poco::Int64);

    // User-triggered changes to timer:
    void SetDurationUserInput(const std::string &);
    void SetStopUserInput(const std::string &);
    void SetStartUserInput(const std::string &, const bool);

    void StopTracking();

 private:
    Poco::UInt64 wid_;
    Poco::UInt64 pid_;
    Poco::UInt64 tid_;
    bool billable_;
    Poco::Int64 start_;
    Poco::Int64 stop_;
    Poco::Int64 duration_in_seconds_;
    std::string description_;
    bool duronly_;
    std::string created_with_;
    uuid_t project_guid_;
    bool unsynced_;
    Poco::Int64 last_start_at_;

    bool setDurationStringHHMMSS(const std::string &value);
    bool setDurationStringHHMM(const std::string &value);
    bool setDurationStringMMSS(const std::string &value);

    void loadTagsFromJSON(Json::Value value);

    bool durationTooLarge(const error &err) const;
    bool startTimeWrongYear(const error &err) const;
    bool stopTimeMustBeAfterStartTime(const error &err) const;
    bool userCannotAccessTheSelectedProject(const error &err) const;
    bool userCannotAccessSelectedTask(const error &err) const;
    bool billableIsAPremiumFeature(const error &err) const;
    bool isMissingCreatedWith(const error &err) const;
};

}  // namespace toggl

#endif  // SRC_TIME_ENTRY_H_
