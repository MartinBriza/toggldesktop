set(CMAKE_CXX_FLAGS
    -Wall
)

# And include dirs
include_directories(
    ${OAUTH_INCLUDE_DIR}
    ${BUGSNAG_INCLUDE_DIR}
    ${LUA_INCLUDE_DIRS}
    ${JSONCPP_INCLUDE_DIRS}
    ${POCO_INCLUDE_DIRS}

    .
    model
    misc

)
set(NEXTLIBRARY_INCLUDE_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)

# TogglDesktopLibrary sources
set(NEXT_SOURCE_FILES
    model/base_model.cc
    model/batch_update_result.cc
    model/client.cc
    model/autotracker.cc
    model/project.cc
    model/settings.cc
    model/tag.cc
    model/task.cc
    model/time_entry.cc
    model/user.cc
    model/workspace.cc
    model/country.cc

    misc/formatter.cc
    misc/logger.cc
    misc/error.cpp
    misc/memory.cpp

    entity/user.cpp

    httpclient.cpp
    togglapi.cpp
    context.cpp
    eventqueue.cpp
    userdata.cpp
    settings.cpp

    legacy_api.cc
)

set(NEXT_APP_SOURCE_FILES
    main.cc
)

add_library(NextLibrary ${NEXT_SOURCE_FILES})
target_link_libraries(NextLibrary PRIVATE
    ${JSONCPP_LIBRARIES}
    ${LUA_LIBRARIES}
    PocoFoundation PocoCrypto PocoDataSQLite PocoNet PocoNetSSL
    pthread
)

add_executable(Next ${NEXT_APP_SOURCE_FILES})
target_link_libraries(Next PRIVATE
    NextLibrary
    ${JSONCPP_LIBRARIES}
    ${LUA_LIBRARIES}
    PocoFoundation PocoCrypto PocoDataSQLite PocoNet PocoNetSSL
    pthread
)


install(TARGETS Next DESTINATION bin)
install(TARGETS NextLibrary DESTINATION lib)
