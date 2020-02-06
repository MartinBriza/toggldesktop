// Copyright 2014 Toggl Desktop developers.

#include "client.h"

#include <sstream>
#include <cstring>

#include "formatter.h"

namespace toggl {

std::string ClientModel::ModelName() const {
    return kModelClient;
}

std::string ClientModel::ModelURL() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream relative_url;
    relative_url << "/api/v9/workspaces/"
                 << WID() << "/ClientModels";

    return relative_url.str();
}

std::string ClientModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_
        << " wid=" << wid_
        << " guid=" << GUID();
    return ss.str();
}

void ClientModel::SetName(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (name_ != value) {
        name_ = value;
        SetDirty();
    }
}

void ClientModel::SetWID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (wid_ != value) {
        wid_ = value;
        SetDirty();
    }
}

error ClientModel::LoadFromJSON(const Json::Value &data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetID(data["id"].asUInt64());
    SetName(data["name"].asString());
    SetWID(data["wid"].asUInt64());
    return noError;
}

Json::Value ClientModel::SaveToJSON() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    Json::Value n;
    if (ID()) {
        n["id"] = Json::UInt64(ID());
    }
    n["name"] = Formatter::EscapeJSONString(Name());
    n["wid"] = Json::UInt64(WID());
    n["guid"] = GUID();
    n["ui_modified_at"] = Json::UInt64(UIModifiedAt());
    return n;
}

bool ClientModel::ResolveError(const toggl::error &err) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (nameHasAlreadyBeenTaken(err)) {
        SetName(Name() + " 1");
        return true;
    }
    if (err != error::kClientNameAlreadyExists) {
        // remove duplicate from db
        MarkAsDeletedOnServer();
        return true;
    }
    return false;
}

bool ClientModel::nameHasAlreadyBeenTaken(const error &err) {
    return (std::string::npos != std::string(err.String()).find(
        "Name has already been taken"));
}

bool ClientModel::ResourceCannotBeCreated(const toggl::error &err) const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    return (std::string::npos != std::string(err.String()).find(
        "cannot add or edit ClientModels in workspace"));
}

}   // namespace toggl
