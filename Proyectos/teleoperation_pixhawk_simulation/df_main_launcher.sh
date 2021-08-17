#!/bin/bash

NUMID_DRONE=1
NETWORK_ROSCORE=$1
SESSION=$USER
UAV_MASS=1.5

MAV_NAME=iris

export AEROSTACK_PROJECT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Kill any previous session (-t -> target session, -a -> all other sessions )
tmux kill-session -t $SESSION
tmux kill-session -a

# Create new session  (-2 allows 256 colors in the terminal, -s -> session name, -d -> not attach to the new session)
tmux -2 new-session -d -s $SESSION

# Create roscore 
# send-keys writes the string into the sesssion (-t -> target session , C-m -> press Enter Button)
tmux rename-window -t $SESSION:0 'roscore'
tmux send-keys -t $SESSION:0 "roscore" C-m
sleep 1

# Create one window per roslaunch (-t -> target session , -n -> window name) 
tmux new-window -t $SESSION:1 -n 'viewer + teleop'
tmux send-keys "sleep 1;roslaunch alphanumeric_viewer alphanumeric_viewer.launch --wait \
    drone_id_namespace:=drone$NUMID_DRONE \
    my_stack_directory:=${AEROSTACK_PROJECT}" C-m

tmux split-window -t $SESSION:1
tmux send-keys  "sleep 1;roslaunch keyboard_teleoperation_with_pid_control keyboard_teleoperation_with_pid_control.launch --wait \
  drone_id_namespace:=drone$NUMID_DRONE" C-m
            
tmux new-window -t $SESSION:2 -n 'pixhawk interface'
tmux send-keys "roslaunch pixhawk_interface pixhawk_interface.launch \
--wait drone_id_namespace:=drone$NUMID_DRONE acro_mode:=true simulation_mode:=true " C-m

tmux new-window -t $SESSION:3 -n 'Basic Quadrotor Behaviors'
tmux send-keys "roslaunch basic_quadrotor_behaviors basic_quadrotor_behaviors.launch --wait \
  namespace:=drone$NUMID_DRONE" C-m

tmux new-window -t $SESSION:4 -n 'Quadrotor Motion With DF Control'
tmux send-keys "roslaunch quadrotor_motion_with_df_control quadrotor_motion_with_df_control.launch --wait \
    namespace:=drone$NUMID_DRONE \
    uav_mass:=$UAV_MASS" C-m

tmux new-window -t $SESSION:5 -n 'GroundTruth Gazebo'
tmux send-keys "rosrun topic_tools relay /drone${NUMID_DRONE}/mavros/local_position/pose /drone${NUMID_DRONE}/self_localization/pose & \
rosrun topic_tools relay drone${NUMID_DRONE}/mavros/local_position/velocity_local /drone${NUMID_DRONE}/self_localization/speed" C-m

tmux new-window -t $SESSION:6 -n 'Throttle_Controller'
tmux send-keys "sleep 2; roslaunch thrust2throttle_controller thrust2throttle_controller.launch --wait \
  namespace:=drone$NUMID_DRONE \
  uav_mass:=$UAV_MASS" C-m

tmux new-window -t $SESSION:7 -n 'Python interpreter'
tmux send-keys "roslaunch python_based_mission_interpreter_process python_based_mission_interpreter_process.launch --wait \
  drone_id_namespace:=drone$NUMID_DRONE \
  drone_id_int:=$NUMID_DRONE \
  my_stack_directory:=${AEROSTACK_PROJECT} \
  mission_configuration_folder:=${AEROSTACK_PROJECT}/configs/mission_df" C-m

tmux new-window -t $SESSION:8 -n 'Behavior Coordinator'
tmux send-keys "roslaunch behavior_coordinator behavior_coordinator.launch --wait \
  robot_namespace:=drone$NUMID_DRONE \
  catalog_path:=${AEROSTACK_PROJECT}/configs/mission_df/behavior_catalog.yaml" C-m

tmux attach-session -t $SESSION:1

