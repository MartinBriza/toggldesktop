#include "memory.h"

#include "model/time_entry.h"

#include <iostream>

namespace toggl {

UserData *ProtectedContainerBase::GetUserData() {
    return userData_;
}

const UserData *ProtectedContainerBase::GetUserData() const {
    return userData_;
}

}
