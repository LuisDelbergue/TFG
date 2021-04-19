cmake_minimum_required(VERSION 3.0.2)
set(PROJECT_NAME Plantillas_nodos)
project(${PROJECT_NAME})
add_definitions(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp 
  geometry_msgs
)

add_service_files(
 FILES
#   servicio.srv
)

generate_messages(
 DEPENDENCIES
#   std_msgs
)

catkin_package(
 INCLUDE_DIRS include
 LIBRARIES ${PROJECT_NAME}
 CATKIN_DEPENDS 
   roscpp 
   geometry_msgs
)

include_directories(
 ${catkin_INCLUDE_DIRS}
 include
)

set(CPP_PROJECT_FILES 
 Nodos/1Subscriber.cpp
)
 
set(HEADER_PROJECT_FILES
# include/ros_env.hpp
)
 
# declare library
add_library(${PROJECT_NAME}
 ${CPP_PROJECT_FILES}
 ${HEADER_PROJECT_FILES}
)
 
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
 
#Test CPP
#add_executable(${PROJECT_NAME}_test_main src/main_test.cpp)
#target_link_libraries(${PROJECT_NAME}_test_main ${catkin_LIBRARIES} ${PROJECT_NAME})
#add_dependencies(${PROJECT_NAME}_test_main ${catkin_EXPORTED_TARGETS})


