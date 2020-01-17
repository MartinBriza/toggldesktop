// Copyright 2014 Toggl Desktop developers.

#include "tag.h"

#include <sstream>

#include "./const.h"

namespace toggl {

std::string Tag::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_
        << " wid=" << wid_
        << " guid=" << GUID();
    return ss.str();
}

void Tag::SetWID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (wid_ != value) {
        wid_ = value;
        SetDirty();
    }
}

void Tag::SetName(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (name_ != value) {
        name_ = value;
        SetDirty();
    }
}

void Tag::LoadFromJSON(Json::Value data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetID(data["id"].asUInt64());
    SetName(data["name"].asString());
    SetWID(data["wid"].asUInt64());
}

std::string Tag::ModelName() const {
    return kModelTag;
}

std::string Tag::ModelURL() const {
    return "/api/v9/tags";
}

}   // namespace toggl
