#include "country.h"

#include <sstream>

namespace toggl {

using Poco::Data::Keywords::use;
using Poco::Data::Keywords::useRef;
using Poco::Data::Keywords::limit;
using Poco::Data::Keywords::into;
using Poco::Data::Keywords::now;

CountryModel::CountryModel(UserData *parent, const Json::Value &root)
    : BaseModel(parent)
{
    LoadFromJSON(root);
}

std::string CountryModel::String() const {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    std::stringstream ss;
    ss  << "ID=" << ID()
        << " local_id=" << LocalID()
        << " name=" << name_
        << " country_code=" << country_code_
        << " guid=" << GUID();
    return ss.str();
}

void CountryModel::SetName(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (name_ != value) {
        name_ = value;
        SetDirty();
    }
}

void CountryModel::SetCountryCode(const std::string &value) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    if (country_code_ != value) {
        country_code_ = value;
        SetDirty();
    }
}

error CountryModel::LoadFromJSON(const Json::Value &data) {
    std::scoped_lock<std::recursive_mutex> lock(mutex_);
    SetID(data["id"].asUInt64());
    SetName(data["name"].asString());
    SetCountryCode(data["country_code"].asString());
    return noError;
}

std::string CountryModel::ModelName() const {
    return kModelCountry;
}

std::string CountryModel::ModelURL() const {
    return "/api/v9/countries";
}

}   // namespace toggl
