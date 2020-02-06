// Copyright 2014 Toggl Desktop developers.

#include "base_model.h"

#include <sstream>

#include "./batch_update_result.h"
#include "./formatter.h"

#include "Poco/Timestamp.h"
#include "Poco/DateTime.h"
#include "Poco/LocalDateTime.h"
#include "Poco/Logger.h"
#include "Poco/UUIDGenerator.h"

namespace toggl {

bool BaseModel::NeedsPush() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // Note that if a model has a validation error previously
    // received and attached from the backend, the model won't be
    // pushed again unless the error is somehow fixed by user.
    // We will assume that if user modifies the model, the error
    // will go away. But until then, don't push the errored data.
    return ValidationError().IsNoError() &&
           (NeedsPOST() || NeedsPUT() || NeedsDELETE());
}

bool BaseModel::NeedsPOST() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // No server side ID yet, meaning it's not POSTed yet
    return !id_ && !(deleted_at_ > 0);
}

bool BaseModel::NeedsPUT() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // Model has been updated and is not deleted, needs a PUT
    return id_ && ui_modified_at_ > 0 && !(deleted_at_ > 0);
}

bool BaseModel::NeedsDELETE() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    // Model is deleted, needs a DELETE on server side
    return id_ && (deleted_at_ > 0);
}

bool BaseModel::NeedsToBeSaved() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return !local_id_ || dirty_;
}

void BaseModel::EnsureGUID() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (!guid_.empty()) {
        return;
    }
    SetGUID(BaseModel::GenerateGUID());
}

void BaseModel::ClearValidationError() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetValidationError(toggl::noError);
}

void BaseModel::SetValidationError(const error &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (validation_error_ != value) {
        validation_error_ = value;
        SetDirty();
    }
}

const error &BaseModel::ValidationError() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return validation_error_;
}

bool BaseModel::DuplicateResource(const error &err) const {
    return false;
}

bool BaseModel::ResourceCannotBeCreated(const error &err) const {
    return false;
}

bool BaseModel::ResolveError(const error &err) {
    return false;
}

void BaseModel::SetDeletedAt(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (deleted_at_ != value) {
        deleted_at_ = value;
        SetDirty();
    }
}

const Poco::Int64 &BaseModel::UpdatedAt() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return updated_at_;
}

void BaseModel::SetUpdatedAt(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (updated_at_ != value) {
        updated_at_ = value;
        SetDirty();
    }
}

void BaseModel::SetGUID(const uuid_t &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (guid_ != value) {
        guid_ = value;
        SetDirty();
    }
}

const Poco::UInt64 &BaseModel::UID() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return uid_;
}

void BaseModel::SetUIModifiedAt(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (ui_modified_at_ != value) {
        ui_modified_at_ = value;
        SetDirty();
    }
}

void BaseModel::SetUIModified() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetUIModifiedAt(time(nullptr));
}

const uuid_t &BaseModel::GUID() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return guid_;
}

void BaseModel::SetUID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (uid_ != value) {
        uid_ = value;
        SetDirty();
    }
}

void BaseModel::SetID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (id_ != value) {
        id_ = value;
        SetDirty();
    }
}

const Poco::Int64 &BaseModel::UIModifiedAt() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return ui_modified_at_;
}

void BaseModel::SetUpdatedAtString(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetUpdatedAt(Formatter::Parse8601(value));
}

bool BaseModel::IsMarkedAsDeletedOnServer() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return is_marked_as_deleted_on_server_;
}

void BaseModel::MarkAsDeletedOnServer() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    is_marked_as_deleted_on_server_ = true;
    SetDirty();
}

error BaseModel::LoadFromDataString(const std::string &data_string) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Json::Value root;
    Json::Reader reader;
    if (!reader.parse(data_string, root)) {
        return error::MALFORMED_DATA;
    }
    LoadFromJSON(root["data"]);
    return noError;
}

void BaseModel::Delete() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetDeletedAt(time(nullptr));
    SetUIModified();
}

error BaseModel::ApplyBatchUpdateResult(
    BatchUpdateResult * const update) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    poco_check_ptr(update);

    if (update->ResourceIsGone()) {
        MarkAsDeletedOnServer();
        return noError;
    }

    toggl::error err = update->GetError();
    if (err != error::NO_ERROR) {
        if (DuplicateResource(err) || ResourceCannotBeCreated(err)) {
            MarkAsDeletedOnServer();
            return noError;
        }

        if (ResolveError(err)) {
            return noError;
        }

        SetValidationError(err);
        return err;
    }

    SetValidationError(error::NO_ERROR);

    return LoadFromDataString(update->Body);
}

bool BaseModel::userCannotAccessWorkspace(const error &err) const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return (err == error::kCannotAccessWorkspaceError);
}

std::string BaseModel::batchUpdateRelativeURL() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (NeedsPOST()) {
        return ModelURL();
    }

    std::stringstream url;
    url << ModelURL() << "/" << ID();
    return url.str();
}

std::string BaseModel::batchUpdateMethod() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (NeedsDELETE()) {
        return "DELETE";
    }

    if (NeedsPOST()) {
        return "POST";
    }

    return "PUT";
}

// Convert model JSON into batch update format.
error BaseModel::BatchUpdateJSON(Json::Value *result) const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (GUID().empty()) {
        return error(error::BATCH_UPDATE_WITHOUT_GUID);
    }

    Json::Value body;
    body[ModelName()] = SaveToJSON();

    (*result)["method"] = batchUpdateMethod();
    (*result)["relative_url"] = batchUpdateRelativeURL();
    (*result)["guid"] = GUID();
    (*result)["body"] = body;

    return noError;
}

Poco::Logger &BaseModel::logger() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return Poco::Logger::get(ModelName());
}

void BaseModel::SetDirty() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    dirty_ = true;
}

const bool &BaseModel::Dirty() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return dirty_;
}

void BaseModel::ClearDirty() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    dirty_ = false;
}

const bool &BaseModel::Unsynced() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return unsynced_;
}

void BaseModel::SetUnsynced() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    unsynced_ = true;
}

void BaseModel::ClearUnsynced() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    unsynced_ = false;
}

const Poco::Int64 &BaseModel::DeletedAt() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return deleted_at_;
}

uuid_t BaseModel::GenerateGUID() {
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    Poco::UUID uuid(generator.createRandom());
    return uuid.toString();
}

const Poco::Int64 &BaseModel::LocalID() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return local_id_;
}

void BaseModel::SetLocalID(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    local_id_ = value;
}

const Poco::UInt64 &BaseModel::ID() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return id_;
}

}   // namespace toggl
