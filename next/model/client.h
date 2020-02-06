    // Copyright 2014 Toggl Desktop developers.

#ifndef SRC_CLIENT_H_
#define SRC_CLIENT_H_

#include <string>

#include "./types.h"

#include <json/json.h>  // NOLINT

#include "Poco/Types.h"

#include "./base_model.h"

namespace toggl {
template<typename T> class ProtectedContainer;

class TOGGL_INTERNAL_EXPORT ClientModel : public BaseModel {
    ClientModel()
        : BaseModel()
    , wid_(0)
    , name_("") {}
public:
    friend class ProtectedContainer<ClientModel>;

    const Poco::UInt64 &WID() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return wid_;
    }
    void SetWID(const Poco::UInt64 value);

    const std::string &Name() const {
        std::scoped_lock<std::recursive_mutex> lock(mutex_);
        return name_;
    }
    void SetName(const std::string &value);

    // Override BaseModel
    std::string String() const override;
    std::string ModelName() const override;
    std::string ModelURL() const override;
    error LoadFromJSON(const Json::Value &value) override;
    Json::Value SaveToJSON() const override;
    bool ResolveError(const toggl::error &err) override;
    bool ResourceCannotBeCreated(const toggl::error &err) const override;

 private:
    Poco::UInt64 wid_;
    std::string name_;

    static bool nameHasAlreadyBeenTaken(const error &err);
};

}  // namespace toggl

#endif  // SRC_CLIENT_H_
