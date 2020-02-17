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
    namespace Data {
        class Session;
    }
}

namespace toggl {

class UserData;
class BatchUpdateResult;

class TOGGL_INTERNAL_EXPORT BaseModel {
protected:
    BaseModel(UserData *parent)
    : userData_(parent)
    , local_id_(0)
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
public:
    virtual ~BaseModel() {}

    toggl::UserData *Parent();
    const toggl::UserData *Parent() const;

    void EnsureGUID();

    static uuid_t GenerateGUID();

    const Poco::Int64 &LocalID() const;
    void SetLocalID(const Poco::Int64 value);

    const Poco::UInt64 &ID() const;
    void SetID(const Poco::UInt64 value);

    const Poco::Int64 &UIModifiedAt() const;
    void SetUIModifiedAt(const Poco::Int64 value);
    void SetUIModified();

    const uuid_t &GUID() const;
    void SetGUID(const uuid_t &value);

    const Poco::UInt64 &UID() const;
    void SetUID(const Poco::UInt64 value);

    void SetDirty();
    const bool &Dirty() const;
    void ClearDirty();

    const bool &Unsynced() const;
    void SetUnsynced();
    void ClearUnsynced();

    // Deleting a time entry hides it from
    // UI and flags it for removal from server:
    const Poco::Int64 &DeletedAt() const;
    void SetDeletedAt(const Poco::Int64 value);

    const Poco::Int64 &UpdatedAt() const;
    void SetUpdatedAt(const Poco::Int64 value);

    std::string UpdatedAtString() const;
    void SetUpdatedAtString(const std::string &value);

    // When a model is deleted
    // on server, it will be removed from local
    // DB using this flag:
    bool IsMarkedAsDeletedOnServer() const;
    void MarkAsDeletedOnServer();

    bool NeedsPush() const;
    bool NeedsPOST() const;
    bool NeedsPUT() const;
    bool NeedsDELETE() const;
    bool NeedsToBeSaved() const;

    void ClearValidationError();
    void SetValidationError(const error &value);
    const error &ValidationError() const;

    virtual std::string String() const = 0;
    virtual std::string ModelName() const = 0;
    virtual std::string ModelURL() const = 0;

    virtual error LoadFromJSON(const Json::Value &value) = 0;
    virtual Json::Value SaveToJSON() const { return {}; }

    //virtual void LoadFromDatabase() = 0;
    //virtual void SaveToDatabase(Poco::Data::Session *session) = 0;

    virtual bool DuplicateResource(const toggl::error &err) const;
    virtual bool ResourceCannotBeCreated(const toggl::error &err) const;
    virtual bool ResolveError(const toggl::error &err);

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

    toggl::UserData *userData_;

    Poco::Int64 local_id_;
    id_t id_;
    uuid_t guid_;
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
