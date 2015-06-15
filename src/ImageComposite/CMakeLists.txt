#######################################################################
# Setting Target_Name as current folder name
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

set  (SOURCE_FILES		ImageCompositeAppMain.cpp )

include_directories (${CMAKE_SOURCE_DIR})


add_executable(
  ${TARGET_NAME}
  ${SOURCE_FILES}
)

target_link_libraries (	
	${TARGET_NAME} 
	Qt5::Core
	Qt5::Gui
)


#######################################################################
#################### SETUP OUTPUT DIRECTORY ###########################
set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/lib_install )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "Single Directory for all executables")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "Single Directory for all libraries")
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "Single Directory for all libraries")
#######################################################################

