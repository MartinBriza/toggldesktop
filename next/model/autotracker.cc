
// Copyright 2015 Toggl Desktop developers

#include "autotracker.h"

#include "Poco/UTF8String.h"

#include "const.h"

namespace toggl {

bool AutotrackerRuleModel::Matches(const TimelineEvent &event) const {
    if (Poco::UTF8::toLower(event.Filename()).find(term_)
            != std::string::npos) {
        return true;
    }
    if (Poco::UTF8::toLower(event.Title()).find(term_)
            != std::string::npos) {
        return true;
    }
    return false;
}

const std::string &AutotrackerRuleModel::Term() const {
    return term_;
}

void AutotrackerRuleModel::SetTerm(const std::string &value) {
    if (term_ != value) {
        term_ = value;
        SetDirty();
    }
}

const Poco::UInt64 &AutotrackerRuleModel::PID() const {
    return pid_;
}

void AutotrackerRuleModel::SetPID(const Poco::UInt64 value) {
    if (pid_ != value) {
        pid_ = value;
        SetDirty();
    }
}

const Poco::UInt64 &AutotrackerRuleModel::TID() const {
    return tid_;
}

void AutotrackerRuleModel::SetTID(const Poco::UInt64 value) {
    if (tid_ != value) {
        tid_ = value;
        SetDirty();
    }
}

std::string AutotrackerRuleModel::String() const {
    std::stringstream ss;
    ss << " local_id=" << LocalID()
       << " term=" << term_
       << " uid=" << UID()
       << " pid=" << pid_
       << " tid=" << tid_;
    return ss.str();
}

std::string AutotrackerRuleModel::ModelName() const {
    return kModelAutotrackerRule;
}

std::string AutotrackerRuleModel::ModelURL() const {
    return "";
}

}  // namespace toggl
