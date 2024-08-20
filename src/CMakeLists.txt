# define source files
set(SOURCES 
    EventHandler.cpp
)

# define header files
set(HEADERS 
    EventHandler.h
    Event.h
    EventTypes.h
)

# Find nlohmann_json library
find_package(nlohmann_json 3.11.3 REQUIRED)

# Add include directories
include_directories(${nlohmann_json_INCLUDE_DIRS})

# create a library
add_library(EventHandler STATIC ${SOURCES} ${HEADERS})

# link nholmann_json
target_link_libraries(EventHandler nlohmann_json::nlohmann_json)

# set include directories
target_include_directories(EventHandler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
