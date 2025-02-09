cmake_minimum_required(VERSION 3.14)

# Project-level configuration.
set(PROJECT_NAME "flutter_ar_vr")
project(${PROJECT_NAME} LANGUAGES CXX)

# Explicitly opt in to modern CMake behaviors to avoid warnings with recent
# versions of CMake.
cmake_policy(VERSION 3.14...3.25)

# This value is used when generating builds using this plugin, so it must
# not be changed
set(PLUGIN_NAME "flutter_ar_vr_plugin")

# Set OpenXR SDK directories
set(OpenXR_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openxr_sdk/include)
set(OpenXR_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openxr_sdk/native/x64/release/lib)
set(OpenXR_DLL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openxr_sdk/native/x64/release/bin)

# Any new source files that you add to the plugin should be added here.
list(APPEND PLUGIN_SOURCES
  "flutter_ar_vr_plugin.cpp"
  "flutter_ar_vr_plugin.h"
)

# Add OpenXR include directory
include_directories(${OpenXR_INCLUDE_DIR})

# Define the plugin library target. Its name must not be changed (see comment
# on PLUGIN_NAME above).
add_library(${PLUGIN_NAME} SHARED
  "include/flutter_ar_vr/flutter_ar_vr_plugin_c_api.h"
  "flutter_ar_vr_plugin_c_api.cpp"
  ${PLUGIN_SOURCES}
)

# Apply a standard set of build settings that are configured in the
# application-level CMakeLists.txt. This can be removed for plugins that want
# full control over build settings.
apply_standard_settings(${PLUGIN_NAME})

# Symbols are hidden by default to reduce the chance of accidental conflicts
# between plugins. This should not be removed; any symbols that should be
# exported should be explicitly exported with the FLUTTER_PLUGIN_EXPORT macro.
set_target_properties(${PLUGIN_NAME} PROPERTIES
  CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)

# Source include directories and library dependencies. Add any plugin-specific
# dependencies here.
target_include_directories(${PLUGIN_NAME} INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link OpenXR loader library
target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin)

# Link OpenXR loader library from the bundled SDK
target_link_libraries(${PLUGIN_NAME} PRIVATE ${OpenXR_LIB_DIR}/openxr_loader.lib)

# List of absolute paths to libraries that should be bundled with the plugin.
# This list could contain prebuilt libraries, or libraries created by an
# external build triggered from this build file.
set(flutter_ar_vr_bundled_libraries
  "${OpenXR_DLL_DIR}/openxr_loader.dll"
  PARENT_SCOPE
)

# Copy the OpenXR loader DLL to the output directory after build
add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${OpenXR_DLL_DIR}/openxr_loader.dll
  $<TARGET_FILE_DIR:${PLUGIN_NAME}>
)
