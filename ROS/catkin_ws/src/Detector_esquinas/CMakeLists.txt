cmake_minimum_required(VERSION 3.0.2)
set(PROJECT_NAME Detector_esquinas)
project(${PROJECT_NAME})
add_definitions(-std=c++11)

find_package(catkin REQUIRED COMPONENTS #librerías externas
  roscpp
  sensor_msgs
  cv_bridge
  std_msgs
  geometry_msgs
  image_transport
  aerostack_msgs
  gazebo_ros
)

# add_service_files(  #añadir servicios, pero no lo voy a usar
# FILES
#   servicio.srv
#)

# generate_messages(  #generar mensajes, no se va a usar
#  DEPENDENCIES
#   std_msgs
#)

find_package(OpenCV REQUIRED)
find_package(gazebo REQUIRED)

catkin_package(     #poner de nuevo librerías externas
 INCLUDE_DIRS include
 LIBRARIES ${PROJECT_NAME}
 CATKIN_DEPENDS 
    roscpp
    sensor_msgs
    cv_bridge
    std_msgs
    geometry_msgs
    image_transport
    aerostack_msgs
    gazebo_ros
)

include_directories(   #incluir las anteriores más las de la carpeta include
 ${catkin_INCLUDE_DIRS}
 ${OpenCV_INCLUDE_DIRS}
 include
)

set(CPP_PROJECT_FILES  #añadir todos los .cpp sin main
 
)
 
set(HEADER_PROJECT_FILES #los .hpp de la carpeta include

)
 
link_directories(${GAZEBO_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIR} ${catkin_INCLUDE_DIRS} ${GAZEBO_INCLUDE_DIRS})

# For model_push_plugin plugin
add_library(model_push_plugin SHARED src/model_push.cc)
target_link_libraries(model_push_plugin ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})

# For viento_plugin plugin
add_library(viento_plugin SHARED src/viento.cc)
target_link_libraries(viento_plugin ${catkin_LIBRARIES} ${GAZEBO_LIBRARIES})

# declare library         #descomentar si añado archivos antes (42-49)
# add_library(${PROJECT_NAME}
#  ${CPP_PROJECT_FILES}
#  ${HEADER_PROJECT_FILES}
# )
 
# target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
# add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
 
#ejecutable por cada nodo (cada main)
add_executable(${PROJECT_NAME}_test_mainS src/Detector.cpp)
target_link_libraries(${PROJECT_NAME}_test_mainS ${OpenCV_LIBS} ${catkin_LIBRARIES}) #${PROJECT_NAME}) #descomentar si añado .cpp y .hpp
add_dependencies(${PROJECT_NAME}_test_mainS ${catkin_EXPORTED_TARGETS})

add_executable(${PROJECT_NAME}_test_mainP src/ibvs_luis.cpp)
target_link_libraries(${PROJECT_NAME}_test_mainP ${OpenCV_LIBS} ${catkin_LIBRARIES}) #${PROJECT_NAME}) #descomentar si añado .cpp y .hpp
add_dependencies(${PROJECT_NAME}_test_mainP ${catkin_EXPORTED_TARGETS})
