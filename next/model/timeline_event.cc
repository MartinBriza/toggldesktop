// Copyright 2015 Toggl Desktop developers.

#include "timeline_event.h"

#include <sstream>
#include <cstring>

namespace toggl {

std::string TimelineEventModel::String() const {
    std::stringstream ss;
    ss << "TimelineEventModel"
       << " guid=" << GUID()
       << " local_id=" << LocalID()
       << " start_time=" << Start()
       << " end_time=" << EndTime()
       << " filename=" << Filename()
       << " title=" << Title()
       << " duration=" << Duration();
    return ss.str();
}

std::string TimelineEventModel::ModelName() const {
    return kModelTimelineEvent;
}

std::string TimelineEventModel::ModelURL() const {
    return "";
}

void TimelineEventModel::SetTitle(const std::string &value) {
    if (title_ != value) {
        title_ = value;
        SetDirty();
    }
}

void TimelineEventModel::SetStart(const Poco::Int64 value) {
    if (start_time_ != value) {
        start_time_ = value;
        updateDuration();
        SetDirty();
    }
}

void TimelineEventModel::SetEndTime(const Poco::Int64 value) {
    if (end_time_ != value) {
        end_time_ = value;
        updateDuration();
        SetDirty();
    }
}

void TimelineEventModel::SetIdle(const bool value) {
    if (idle_ != value) {
        idle_ = value;
        SetDirty();
    }
}

void TimelineEventModel::SetFilename(const std::string &value) {
    if (filename_ != value) {
        filename_ = value;
        SetDirty();
    }
}

void TimelineEventModel::SetChunked(const bool value) {
    if (chunked_ != value) {
        chunked_ = value;
        SetDirty();
    }
}

void TimelineEventModel::SetUploaded(const bool value) {
    if (uploaded_ != value) {
        uploaded_ = value;
        SetDirty();
    }
}

Json::Value TimelineEventModel::SaveToJSON() const {
    Json::Value n;
    n["guid"] = GUID();
    n["filename"] = Filename();
    n["title"] = Title();
    n["start_time"] = Json::Int64(Start());
    n["end_time"] = Json::Int64(EndTime());
    n["created_with"] = "timeline";
    return n;
}

void TimelineEventModel::updateDuration() {
    Poco::Int64 value = end_time_ - start_time_;
    duration_ = value < 0 ? 0 : value;
}

toggl::error toggl::TimelineEventModel::LoadFromJSON(const Json::Value &) {
    return noError;
}

}   // namespace toggl
