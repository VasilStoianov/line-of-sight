cmake_minimum_required(VERSION 3.12)
project(myapp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add your source files
set(SOURCES
   main.cpp
    # Add more source files as needed
)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link SFML libraries
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
