// Copyright 2014 Toggl Desktop developers.

#include "tag.h"

#include <sstream>

#include "./const.h"
#include "misc/memory.h"

namespace toggl {

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::useRef;
using Poco::Data::Keywords::limit;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::now;

TagModel::TagModel(UserData *parent, const Json::Value &root)
    : BaseModel(parent)
{
    LoadFromJSON(root);
}

TagModel::TagModel(UserData *parent, Poco::Data::RecordSet &rs)
    : BaseModel(parent)
{
    SetLocalID(rs[0].convert<Poco::Int64>());
    if (rs[1].isEmpty()) {
        SetID(0);
    } else {
        SetID(rs[1].convert<Poco::UInt64>());
    }
    SetUID(rs[2].convert<Poco::UInt64>());
    SetName(rs[3].convert<std::string>());
    SetWID(rs[4].convert<Poco::UInt64>());
    if (rs[5].isEmpty()) {
        SetGUID({});
    } else {
        SetGUID(rs[5].convert<uuid_t>());
    }
    ClearDirty();
}

Error TagModel::LoadFromDatabase(ProtectedContainer<TagModel> &list, Poco::Data::Session &session, id_t UID) {
    Poco::Data::Statement select(session);
    select <<
           "SELECT local_id, id, uid, name, wid, guid "
           "FROM tags "
           "WHERE uid = :uid "
           "ORDER BY name",
           useRef(UID);
    /* TODO
    error err = last_error("loadTags");
    if (err != noError) {
        return err;
    }
    */
    Poco::Data::RecordSet rs(select);
    while (!select.done()) {
        select.execute();
        bool more = rs.moveFirst();
        while (more) {
            locked<TagModel> model = list.create(rs);
            more = rs.moveNext();
        }
    }
    return noError;
}

std::string TagModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_
        << " wid=" << wid_
        << " guid=" << GUID();
    return ss.str();
}

void TagModel::SetWID(const Poco::UInt64 value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (wid_ != value) {
        wid_ = value;
        SetDirty();
    }
}

void TagModel::SetName(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (name_ != value) {
        name_ = value;
        SetDirty();
    }
}

error TagModel::LoadFromJSON(const Json::Value &data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetID(data["id"].asUInt64());
    SetName(data["name"].asString());
    SetWID(data["wid"].asUInt64());
    return noError;
}

std::string TagModel::ModelName() const {
    return kModelTag;
}

std::string TagModel::ModelURL() const {
    return "/api/v9/tags";
}

}   // namespace toggl
