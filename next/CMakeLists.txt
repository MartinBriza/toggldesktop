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

    misc/formatter.cc
    misc/logger.cc
    misc/error.cpp

    httpclient.cpp
    togglapi.cpp

    main.cc
)

# Set up compilation targets
add_executable(Next ${NEXT_SOURCE_FILES})

# And list the dependencies between them (and system libraries)
target_link_libraries(Next PRIVATE
    ${JSONCPP_LIBRARIES}
    ${LUA_LIBRARIES}
    PocoFoundation PocoCrypto PocoDataSQLite PocoNet PocoNetSSL
)

install(TARGETS Next DESTINATION bin)
