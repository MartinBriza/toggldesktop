// Copyright 2014 Toggl Desktop developers.

#include "workspace.h"

#include <sstream>
#include "./formatter.h"

namespace toggl {

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::useRef;
using Poco::Data::Keywords::limit;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::now;

std::string WorkspaceModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_;
    return ss.str();
}

Error WorkspaceModel::LoadFromDatabase(ProtectedContainer<WorkspaceModel> &list, Poco::Data::Session &session, id_t UID) {
    Poco::Data::Statement select(session);
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
            more = rs.moveNext();
        }
    }
    return noError;
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

error WorkspaceModel::LoadFromJSON(const Json::Value &n) {
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
    return noError;
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
