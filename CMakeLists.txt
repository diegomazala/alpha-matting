cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

# Create folder to allocate cmake owner projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


project      (ImgProc)

set ( PROJECT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set ( PROJECT_SOURCE_DIR "${PROJECT_ROOT_DIR}/src")
set ( PROJECT_BINARY_DIR "${PROJECT_ROOT_DIR}/bin")
set ( PROJECT_SHADER_DIR "${PROJECT_ROOT_DIR}/shaders")

include_directories	(${PROJECT_ROOT_DIR}/include)

#######################################################################
############################# SETUP QT5 ###############################
find_package (Qt5OpenGL REQUIRED)
find_package (Qt5Widgets REQUIRED)
if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)	# Find includes in corresponding build directories
include_directories	(${Qt5Widgets_INCLUDE_DIRS} ${Qt5OpenGL_INCLUDE_DIRS})
#######################################################################



add_subdirectory (${PROJECT_SOURCE_DIR}/ChromaKey)
add_subdirectory (${PROJECT_SOURCE_DIR}/ImageDiff)
add_subdirectory (${PROJECT_SOURCE_DIR}/ImageComposite)
add_subdirectory (${PROJECT_SOURCE_DIR}/AlphaMatting)







