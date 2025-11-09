include(GenerateExportHeader)

add_library(ca2co INTERFACE continuation.hpp)
add_library(ca2co::ca2co ALIAS ca2co)

target_link_libraries(ca2co INTERFACE ca2co_options ca2co_warnings)

target_include_directories(ca2co ${WARNING_GUARD} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>)

target_compile_features(ca2co INTERFACE cxx_std_23)

set_target_properties(
  ca2co
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

# generate_export_header(ca2co EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/ca2co/ca2co_export.hpp)

# if(NOT BUILD_SHARED_LIBS)
#  target_compile_definitions(ca2co PUBLIC CO_GO_STATIC_DEFINE)
# endif()
