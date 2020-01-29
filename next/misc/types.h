// Copyright 2014 Toggl Desktop developers.

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include <string>

#include "misc/error.h"

namespace toggl {

    typedef uint64_t id_t;
    typedef std::string uuid_t;

    // TODO
    typedef Error error;
    const error noError = Error::NO_ERROR;
}

#if defined(_WIN32) || defined(WIN32)
# ifdef TOGGLDESKTOP_DLL_BUILD
#  define TOGGL_INTERNAL_EXPORT __declspec(dllexport)
# else
#  define TOGGL_INTERNAL_EXPORT __declspec(dllimport)
# endif
#else
# define TOGGL_INTERNAL_EXPORT
#endif // _WIN32 || WIN32

#endif  // SRC_TYPES_H_
