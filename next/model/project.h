// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_PROJECT_H_
#define SRC_PROJECT_H_

#include <string>
#include <vector>

#include "./base_model.h"
#include "./types.h"
#include "misc/memory.h"

#include "Poco/Types.h"

namespace toggl {
template<typename T> class ProtectedContainer;
class ClientModel;

class TOGGL_INTERNAL_EXPORT ProjectModel : public BaseModel {
    ProjectModel(UserData *parent) : BaseModel(parent) {}
    ProjectModel(UserData *parent, const Json::Value &root);
public:
    friend class ProtectedContainer<ProjectModel>;
    static std::vector<std::string> ColorCodes;

    // Override BaseModel
    std::string String() const override;
    std::string ModelName() const override;
    std::string ModelURL() const override;
    error LoadFromJSON(const Json::Value &value) override;
    Json::Value SaveToJSON() const override;
    bool DuplicateResource(const toggl::error &err) const override;
    bool ResourceCannotBeCreated(const toggl::error &err) const override;
    bool ResolveError(const toggl::error &err) override;

    /******************************************************************
     * SETTERS AND GETTERS
     */
    const Poco::UInt64 &WID() const {
        return wid_;
    }
    void SetWID(const Poco::UInt64 value);

    const Poco::UInt64 &CID() const {
        return cid_;
    }
    void SetCID(const Poco::UInt64 value);

    const uuid_t &ClientGUID() const {
        return client_guid_;
    }
    void SetClientGUID(const uuid_t &);

    const std::string &Name() const {
        return name_;
    }
    void SetName(const std::string &value);

    const std::string &Color() const {
        return color_;
    }
    void SetColor(const std::string &value);

    std::string ColorCode() const;
    error SetColorCode(const std::string &color_code);

    const bool &Active() const {
        return active_;
    }
    void SetActive(const bool value);

    const bool &IsPrivate() const {
        return private_;
    }
    void SetPrivate(const bool value);

    const bool &Billable() const {
        return billable_;
    }
    void SetBillable(const bool value);

    const std::string &ClientName() const {
        return client_name_;
    }
    void SetClientName(const std::string &value);

    /******************************************************************
     * SIMPLE DERIVED DATA
     */
    std::string FullName() const;


    /******************************************************************
     * DEPENDENCY TREE DERIVED DATA
     */
    locked<ClientModel> Client();
    locked<const ClientModel> Client() const;

 private:
    bool clientIsInAnotherWorkspace(const toggl::error &err) const;
    bool onlyAdminsCanChangeProjectVisibility(const toggl::error &err) const;

    Poco::UInt64 wid_ { 0 };
    Poco::UInt64 cid_ { 0 };
    std::string name_ { "" };
    std::string color_ { "" };
    bool active_ { false };
    bool private_ { false };
    bool billable_ { false };
    uuid_t client_guid_ { "" };
    std::string client_name_ { "" };
};

template<typename T, size_t N> T *end(T (&ra)[N]);

}  // namespace toggl

#endif  // SRC_PROJECT_H_
