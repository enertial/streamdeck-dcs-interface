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

