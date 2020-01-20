// Copyright 2014 Toggl Desktop developers.

#include "workspace.h"

#include <sstream>
#include "./formatter.h"

namespace toggl {

std::string WorkspaceModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_;
    return ss.str();
}

void WorkspaceModel::SetName(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (name_ != value) {
        name_ = value;
        SetDirty();
    }
}

void WorkspaceModel::SetPremium(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (premium_ != value) {
        premium_ = value;
        SetDirty();
    }
}

void WorkspaceModel::SetOnlyAdminsMayCreateProjects(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (only_admins_may_create_projects_ != value) {
        only_admins_may_create_projects_ = value;
        SetDirty();
    }
}

void WorkspaceModel::SetAdmin(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (admin_ != value) {
        admin_ = value;
        SetDirty();
    }
}

void WorkspaceModel::SetProjectsBillableByDefault(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (projects_billable_by_default_ != value) {
        projects_billable_by_default_ = value;
        SetDirty();
    }
}

void WorkspaceModel::SetBusiness(const bool value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (business_ != value) {
        business_ = value;
        SetDirty();
    }
}

void WorkspaceModel::SetLockedTime(const time_t value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (locked_time_ != value) {
        locked_time_ = value;
        SetDirty();
    }
}

void WorkspaceModel::LoadFromJSON(Json::Value n) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetID(n["id"].asUInt64());
    SetName(n["name"].asString());
    SetPremium(n["premium"].asBool());
    SetOnlyAdminsMayCreateProjects(
        n["only_admins_may_create_projects"].asBool());
    SetAdmin(n["admin"].asBool());
    SetProjectsBillableByDefault(n["projects_billable_by_default"].asBool());

    auto profile = n["profile"].asUInt64();
    SetBusiness(profile > 13);
}

void WorkspaceModel::LoadSettingsFromJson(Json::Value n) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    auto lockDateString = n["report_locked_at"].asString();
    if (!lockDateString.empty()) {
        auto time = Formatter::Parse8601(lockDateString);
        SetLockedTime(time);
    }
}

std::string WorkspaceModel::ModelName() const {
    return kModelWorkspace;
}

std::string WorkspaceModel::ModelURL() const {
    return "/api/v9/workspaces";
}


}   // namespace toggl
