cmake_minimum_required(VERSION 3.0.2)
set(PROJECT_NAME Plantillas_nodos)
project(${PROJECT_NAME})
add_definitions(-std=c++11)

find_package(catkin REQUIRED COMPONENTS #librerías externas
  roscpp 
  geometry_msgs
)

# add_service_files(  #añadir servicios, pero no lo voy a usar
# FILES
#   servicio.srv
#)

# generate_messages(  #generar mensajes, no se va a usar
#  DEPENDENCIES
#   std_msgs
#)

catkin_package(     #poner de nuevo librerías externas
 INCLUDE_DIRS include
 LIBRARIES ${PROJECT_NAME}
 CATKIN_DEPENDS 
   roscpp 
   geometry_msgs
)

include_directories(   #incluir las anteriores más las de la carpeta include
 ${catkin_INCLUDE_DIRS}
 include
)

set(CPP_PROJECT_FILES  #añadir todos los .cpp sin main
 
)
 
set(HEADER_PROJECT_FILES #los .hpp de la carpeta include

)
 
# declare library         #descomentar si añado archivos antes (42-49)
# add_library(${PROJECT_NAME}
#  ${CPP_PROJECT_FILES}
#  ${HEADER_PROJECT_FILES}
# )
 
# target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES})
# add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
 
#ejecutable por cada nodo (cada main)
add_executable(${PROJECT_NAME}_test_mainS src/1Subscriber.cpp)
target_link_libraries(${PROJECT_NAME}_test_mainS ${catkin_LIBRARIES}) #${PROJECT_NAME}) #descomentar si añado .cpp y .hpp
add_dependencies(${PROJECT_NAME}_test_mainS ${catkin_EXPORTED_TARGETS})

add_executable(${PROJECT_NAME}_test_mainP src/1Publisher.cpp)
target_link_libraries(${PROJECT_NAME}_test_mainP ${catkin_LIBRARIES}) #${PROJECT_NAME})
add_dependencies(${PROJECT_NAME}_test_mainP ${catkin_EXPORTED_TARGETS})


