include(GenerateExportHeader)

add_library(decision_tree INTERFACE ./ml/decision_tree.hpp ./ml/nn.hpp)
add_library(decision_tree::decision_tree ALIAS decision_tree)
target_include_directories(decision_tree ${WARNING_GUARD} INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>)
target_compile_features(decision_tree INTERFACE cxx_std_23)

set_target_properties(
  decision_tree
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

# generate_export_header(decision_tree EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/decision_tree/decision_tree_export.hpp)

# if(NOT BUILD_SHARED_LIBS)
#  target_compile_definitions(decision_tree PUBLIC CO_GO_STATIC_DEFINE)
# endif()
