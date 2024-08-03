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

# create a library
add_library(EventRing STATIC ${SOURCES} ${HEADERS})

# link nholmann_json
target_link_libraries(EventRing nlohmann_json::nlohmann_json)

# set include directories
target_include_directories(EventRing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})