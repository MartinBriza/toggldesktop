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
#include "misc/memory.h"

#include "httpclient.h"

#include "Poco/Types.h"
#include "Poco/Data/RecordSet.h"

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
    BaseModel(UserData *parent) : userData_(parent) {}
public:
    virtual ~BaseModel() {}

    template<typename T>
    static Error LoadFromDatabase(ProtectedContainer<T> &list, Poco::Data::Session &session, id_t UID) {
        Poco::Data::Statement select(session);
        T::SelectQuery(UID);
        select <<
               "SELECT local_id, id, uid, name, premium, "
               "only_admins_may_create_projects, admin, "
               "projects_billable_by_default, "
               "is_business, locked_time "
               "FROM workspaces "
               "WHERE uid = :uid "
               "ORDER BY name",
               useRef(UID);
        /*
        error err = last_error("loadWorkspaces");
        if (err != noError) {
            return {{}, err};
        }
        */
        Poco::Data::RecordSet rs(select);
        while (!select.done()) {
            select.execute();
            bool more = rs.moveFirst();
            while (more) {
                locked<T> model = list.create(rs);
                /*
                locked<WorkspaceModel> model = list.create();
                model->SetLocalID(rs[0].convert<Poco::Int64>());
                model->SetID(rs[1].convert<Poco::UInt64>());
                model->SetUID(rs[2].convert<Poco::UInt64>());
                model->SetName(rs[3].convert<std::string>());
                model->SetPremium(rs[4].convert<bool>());
                model->SetOnlyAdminsMayCreateProjects(rs[5].convert<bool>());
                model->SetAdmin(rs[6].convert<bool>());
                model->SetProjectsBillableByDefault(rs[7].convert<bool>());
                model->SetBusiness(rs[8].convert<bool>());
                model->SetLockedTime(rs[9].convert<time_t>());
                model->ClearDirty();
                */
                more = rs.moveNext();
            }
        }
        return noError;
    }

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

    HTTPRequest MakeRequest();

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

    toggl::UserData *userData_ { nullptr };

    Poco::Int64 local_id_ { 0 };
    id_t id_ { 0 };
    uuid_t guid_ { "" };
    Poco::Int64 ui_modified_at_ { 0 };
    id_t uid_ { 0 };
    Poco::Int64 deleted_at_ { 0 };
    Poco::Int64 updated_at_ { 0 };
    bool dirty_ { false };
    bool is_marked_as_deleted_on_server_ { false };

    // Flag is set only when sync fails.
    // Its for viewing purposes only. It should not
    // be used to check if a model needs to be
    // pushed to backend. It only means that some
    // attempt to push failed somewhere.
    bool unsynced_ { false };

    // If model push to backend results in an error,
    // the error is attached to the model for later inspection.
    error validation_error_ { noError };
};

}  // namespace toggl

#endif  // SRC_BASE_MODEL_H_
