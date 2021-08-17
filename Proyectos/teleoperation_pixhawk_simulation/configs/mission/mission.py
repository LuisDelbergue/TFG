#!/usr/bin/env python3

import mission_execution_control as mxc
import rospy
import time
'''
This is a simple mission, the drone takes off, follows a path and lands
1.startTask. Starts the task and continues.
2.executeTask. Starts the task and waits until it stops.
'''
def mission():
  print("Starting mission...")

  print("Taking off...")
  mxc.executeTask('TAKE_OFF')

  print("FOLLOW_PATH...")
  mxc.executeTask('FOLLOW_PATH', path = [ [2, 0, 3] ])

  

