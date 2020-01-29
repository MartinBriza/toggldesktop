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

void BaseModel::SetDeletedAt(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (deleted_at_ != value) {
        deleted_at_ = value;
        SetDirty();
    }
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

void BaseModel::SetUIModifiedAt(const Poco::Int64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (ui_modified_at_ != value) {
        ui_modified_at_ = value;
        SetDirty();
    }
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

void BaseModel::SetUpdatedAtString(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetUpdatedAt(Formatter::Parse8601(value));
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

void BaseModel::SetUnsynced() {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    unsynced_ = true;
}

uuid_t BaseModel::GenerateGUID() {
    Poco::UUIDGenerator& generator = Poco::UUIDGenerator::defaultGenerator();
    Poco::UUID uuid(generator.createRandom());
    return uuid.toString();
}

}   // namespace toggl
