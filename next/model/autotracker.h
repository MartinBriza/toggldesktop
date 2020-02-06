// Copyright 2015 Toggl Desktop developers.

#ifndef SRC_AUTOTRACKER_H_
#define SRC_AUTOTRACKER_H_

#include <string>
#include <sstream>
#include <vector>

#include "Poco/Types.h"

#include "base_model.h"
#include "timeline_event.h"

namespace toggl {
template<typename T> class ProtectedContainer;

class TOGGL_INTERNAL_EXPORT AutotrackerRuleModel : public BaseModel {
    AutotrackerRuleModel(UserData *parent)
        : BaseModel(parent)
    , term_("")
    , pid_(0)
    , tid_(0) {}
public:
    friend class ProtectedContainer<AutotrackerRuleModel>;

    bool Matches(const TimelineEventModel &event) const;

    const std::string &Term() const;
    void SetTerm(const std::string &value);

    const Poco::UInt64 &PID() const;
    void SetPID(const Poco::UInt64 value);

    const Poco::UInt64 &TID() const;
    void SetTID(const Poco::UInt64 value);

    // Override BaseModel
    std::string String() const override;
    std::string ModelName() const override;
    std::string ModelURL() const override;

    Json::Value SaveToJSON() const override;
    error LoadFromJSON(const Json::Value &root) override;

 private:
    std::string term_;
    Poco::UInt64 pid_;
    Poco::UInt64 tid_;
};

};  // namespace toggl

#endif  // SRC_AUTOTRACKER_H_
