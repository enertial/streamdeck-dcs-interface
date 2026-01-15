# --- JSON ---
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

# --- ASIO ---
add_library(asio INTERFACE)
target_include_directories(asio INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/asio/include")
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)

# --- Websocketpp ---
add_library(websocketpp INTERFACE)
target_include_directories(websocketpp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/websocketpp")
target_link_libraries(websocketpp INTERFACE asio)

# --- Lua ---
add_subdirectory(lua-5.4.8)

# --- Google Test ---
include(FetchContent)
FetchContent_Declare(
  googletest
  # Release 1.17.0 (04/23/2025)
  URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now simply link against gtest or gtest_main as needed. Eg
# add_executable(example example.cpp)
# target_link_libraries(example gtest_main)
# add_test(NAME example_test COMMAND example)
