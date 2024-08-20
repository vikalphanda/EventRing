# Define source files for test executable
set(TEST_SOURCES
    main.cpp
)

# Create test executable
add_executable(EventHandlerTest ${TEST_SOURCES})

# Link EventHandler library
target_link_libraries(EventHandlerTest EventHandler)
