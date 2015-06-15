#######################################################################
# Setting Target_Name as current folder name
get_filename_component(TARGET_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)



set  (SOURCE_FILES		AboutDialog.cpp GLImageWidget.cpp MainWindow.cpp AppMain.cpp )
set  (HEADER_FILES		AboutDialog.h GLImageWidget.h MainWindow.h)
set  (FORM_FILES		MainWindow.ui AboutDialog.ui )

qt5_wrap_ui( HEADER_UI_FILES ${FORM_FILES} )

source_group ("Shader Files" FILES ${SHADER_FILES})
source_group ("Form Files" FILES ${FORM_FILES})
source_group ("Form Header Files" FILES ${HEADER_UI_FILES})

include_directories (${CMAKE_SOURCE_DIR})


add_executable(
  ${TARGET_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  ${HEADER_UI_FILES}
)

target_link_libraries (	
	${TARGET_NAME} 
	Qt5::OpenGL
	Qt5::Widgets
)




#######################################################################
#################### SETUP OUTPUT DIRECTORY ###########################
set( CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/lib_install )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "Single Directory for all executables")
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "Single Directory for all libraries")
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} CACHE INTERNAL "Single Directory for all libraries")
#######################################################################

