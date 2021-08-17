#!/bin/bash
[ -d "/usr/share/gazebo-7" ] && export GAZEBO_RESOURCE_PATH=/usr/share/gazebo-7
[ -d "/usr/share/gazebo-9" ] && export GAZEBO_RESOURCE_PATH=/usr/share/gazebo-9
[ -d "/usr/share/gazebo-11" ] && export GAZEBO_RESOURCE_PATH=/usr/share/gazebo-11

AEROSTACK_PROJECT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

#SIMULATE WITHOUT CAMERA
	#(cd $(rospack find px4) ; make px4_sitl gazebo)

#SIMULATE WITH CAMERA
	(cd $(rospack find px4) ; DONT_RUN=1 make px4_sitl gazebo)
	export ROS_PACKAGE_PATH=$ROS_PACKAGE_PATH:$(rospack find px4):$(rospack find px4)/Tools/sitl_gazebo
	export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:/home/cvar/Desktop/Universidad/Programación/TFG/ROS/catkin_ws/devel/lib
	export GAZEBO_MODEL_PATH=$GAZEBO_MODEL_PATH:${AEROSTACK_PROJECT}/configs/gazebo/models
	source $(rospack find px4)/Tools/setup_gazebo.bash $(rospack find px4) $(rospack find px4)/build/px4_sitl_default
	source $(rospack find px4)/build/px4_sitl_default/build_gazebo/setup.sh
	roslaunch px4 mavros_posix_sitl.launch world:=${AEROSTACK_PROJECT}/configs/gazebo/worlds/aaa.world vehicle:=iris sdf:=${AEROSTACK_PROJECT}/configs/gazebo/models/iris_cam/iris_cam.sdf respawn_gazebo:=true
	pkill gzserver;pkill gzclient
