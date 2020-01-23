// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_BASE_MODEL_H_
#define SRC_BASE_MODEL_H_

#include <string>
#include <vector>
#include <cstring>
#include <ctime>
#include <mutex>

#include <json/json.h>  // NOLINT

#include "./types.h"
#include "./const.h"

#include "Poco/Types.h"

namespace Poco {
class Logger;
}

namespace toggl {

class BatchUpdateResult;

class TOGGL_INTERNAL_EXPORT BaseModel {
 public:
    BaseModel()
        : local_id_(0)
    , id_(0)
    , guid_("")
    , ui_modified_at_(0)
    , uid_(0)
    , dirty_(false)
    , deleted_at_(0)
    , is_marked_as_deleted_on_server_(false)
    , updated_at_(0)
    , validation_error_(noError)
    , unsynced_(false) {}

    virtual ~BaseModel() {}

    static std::string GenerateGUID();

    const Poco::Int64 &LocalID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return local_id_;
    }
    void SetLocalID(const Poco::Int64 value) {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        local_id_ = value;
    }

    const Poco::UInt64 &ID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return id_;
    }
    void SetID(const Poco::UInt64 value);

    const Poco::Int64 &UIModifiedAt() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return ui_modified_at_;
    }
    void SetUIModifiedAt(const Poco::Int64 value);
    void SetUIModified() {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        SetUIModifiedAt(time(nullptr));
    }

    const std::string &GUID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return guid_;
    }
    void SetGUID(const std::string &value);

    const Poco::UInt64 &UID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return uid_;
    }
    void SetUID(const Poco::UInt64 value);

    void SetDirty();
    const bool &Dirty() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return dirty_;
    }
    void ClearDirty() {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        dirty_ = false;
    }

    const bool &Unsynced() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return unsynced_;
    }
    void SetUnsynced();
    void ClearUnsynced() {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        unsynced_ = false;
    }

    // Deleting a time entry hides it from
    // UI and flags it for removal from server:
    const Poco::Int64 &DeletedAt() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return deleted_at_;
    }
    void SetDeletedAt(const Poco::Int64 value);

    const Poco::Int64 &UpdatedAt() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return updated_at_;
    }
    void SetUpdatedAt(const Poco::Int64 value);

    std::string UpdatedAtString() const;
    void SetUpdatedAtString(const std::string &value);

    // When a model is deleted
    // on server, it will be removed from local
    // DB using this flag:
    bool IsMarkedAsDeletedOnServer() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return is_marked_as_deleted_on_server_;
    }
    void MarkAsDeletedOnServer() {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        is_marked_as_deleted_on_server_ = true;
        SetDirty();
    }

    bool NeedsPush() const;
    bool NeedsPOST() const;
    bool NeedsPUT() const;
    bool NeedsDELETE() const;

    bool NeedsToBeSaved() const;

    void EnsureGUID();

    void ClearValidationError();
    void SetValidationError(const error &value);
    const error &ValidationError() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return validation_error_;
    }

    virtual std::string String() const = 0;
    virtual std::string ModelName() const = 0;
    virtual std::string ModelURL() const = 0;

    virtual void LoadFromJSON(Json::Value value) {}
    virtual Json::Value SaveToJSON() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return 0;
    }

    virtual bool DuplicateResource(const toggl::error &err) const {
        return false;
    }
    virtual bool ResourceCannotBeCreated(const toggl::error &err) const {
        return false;
    }
    virtual bool ResolveError(const toggl::error &err) {
        return false;
    }

    error LoadFromDataString(const std::string &);

    void Delete();

    error ApplyBatchUpdateResult(BatchUpdateResult * const);

    // Convert model JSON into batch update format.
    error BatchUpdateJSON(Json::Value *result) const;

 protected:
    Poco::Logger &logger() const;

    bool userCannotAccessWorkspace(const toggl::error &err) const;

    mutable std::recursive_mutex mutex_;

 private:
    std::string batchUpdateRelativeURL() const;
    std::string batchUpdateMethod() const;

    id_t local_id_;
    id_t id_;
    guid_t guid_;
    Poco::Int64 ui_modified_at_;
    id_t uid_;
    Poco::Int64 deleted_at_;
    Poco::Int64 updated_at_;
    bool dirty_;
    bool is_marked_as_deleted_on_server_;

    // Flag is set only when sync fails.
    // Its for viewing purposes only. It should not
    // be used to check if a model needs to be
    // pushed to backend. It only means that some
    // attempt to push failed somewhere.
    bool unsynced_;

    // If model push to backend results in an error,
    // the error is attached to the model for later inspection.
    error validation_error_;
};

}  // namespace toggl

#endif  // SRC_BASE_MODEL_H_
