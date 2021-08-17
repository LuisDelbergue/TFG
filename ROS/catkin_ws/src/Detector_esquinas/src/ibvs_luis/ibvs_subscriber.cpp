#include <ros/ros.h>
#include "geometry_msgs/Quaternion.h"
#include <iostream>

int centro_x, centro_y;
const int centroi_x=320;
const int centroi_y=480;

void chatterCallback(const geometry_msgs::Quaternion msg)
{
  centro_x=(msg.z-msg.x)/2+msg.x;
  centro_y=(msg.y-msg.w)/2+msg.w;
}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "IBVS");   
  ros::NodeHandle p;   
  ros::Subscriber sub = p.subscribe("Corners_ibvs", 1000, chatterCallback);
  // ros::Publisher chatter_pub = n.advertise<std_msgs::String>("different_chatter", 1000);

  // ros::Rate loop_rate(10);   

  // int count = 0;
  // while (ros::ok())
  // {

  //   std_msgs::String msg;

  //   std::stringstream ss;
  //   ss << "hello world " << count;
  //   msg.data = ss.str();

  //   ROS_INFO("%s", msg.data.c_str());


  //   chatter_pub.publish(msg);

  //   ros::spinOnce();
  //   loop_rate.sleep();
  // }

  ros::spin();
  return 0;
}