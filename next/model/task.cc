// Copyright 2014 Toggl Desktop developers.

#include "task.h"

#include <sstream>

namespace toggl {

std::string TaskModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_
        << " wid=" << wid_
        << " pid=" << pid_;
    return ss.str();
}

void TaskModel::SetPID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (pid_ != value) {
        pid_ = value;
        SetDirty();
    }
}

void TaskModel::SetWID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (wid_ != value) {
        wid_ = value;
        SetDirty();
    }
}

void TaskModel::SetName(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (name_ != value) {
        name_ = value;
        SetDirty();
    }
}

void TaskModel::SetActive(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (active_ != value) {
        active_ = value;
        SetDirty();
    }
}

void TaskModel::LoadFromJSON(Json::Value data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetID(data["id"].asUInt64());
    SetName(data["name"].asString());
    SetPID(data["pid"].asUInt64());
    SetWID(data["wid"].asUInt64());
    SetActive(data["active"].asBool());
}

std::string TaskModel::ModelName() const {
    return kModelTask;
}

std::string TaskModel::ModelURL() const {
    return "/api/v9/tasks";
}

}   // namespace toggl
