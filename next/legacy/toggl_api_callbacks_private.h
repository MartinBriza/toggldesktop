#ifndef SRC_TOGGL_API_CALLBACKS_PRIVATE_H_
#define SRC_TOGGL_API_CALLBACKS_PRIVATE_H_

#include "toggl_api_callbacks.h"
#include "context.h"
#include <map>

struct LegacyContext {
    TogglDisplayTimeEntryEditor OnTimeEntryEditor { nullptr };
};

extern std::map<toggl::Context*, LegacyContext> contextMap;

#endif // SRC_TOGGL_API_CALLBACKS_PRIVATE_H_
