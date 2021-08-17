#include <ros/ros.h>
#include "geometry_msgs/Quaternion.h"
#include "geometry_msgs/TwistStamped.h"
#include <iostream>

const int centroi_x=320;
const int centroi_y=240;
int centro_x, centro_y;
float k=1;  //ganancia

class server{
  public:
    geometry_msgs::Quaternion corners_received;
    void chatterCallback(const geometry_msgs::Quaternion &corners_sub);
};

void server::chatterCallback(const geometry_msgs::Quaternion &corners_sub)
{
  corners_received=corners_sub;
}

int main(int argc, char **argv)
{
  server objserver;
  ros::init(argc, argv, "IBVS");   
  ros::NodeHandle p;   
  ros::Subscriber sub = p.subscribe("Corners_ibvs", 1000, &server::chatterCallback, &objserver); 

  ros::Publisher chatter_pub = p.advertise<geometry_msgs::TwistStamped>("mavros/local_position/velocity_body", 1000);
  ros::Rate loop_rate(10);   

  while (ros::ok())
  {
   centro_x=(objserver.corners_received.z-objserver.corners_received.x)/2+objserver.corners_received.x;
   centro_y=(objserver.corners_received.y-objserver.corners_received.w)/2+objserver.corners_received.w;
   int error_x=centroi_x-centro_x;
   int error_y=centroi_y-centro_y;

   geometry_msgs::TwistStamped sent;
   sent.header.stamp=ros::Time::now();

   sent.twist.linear.x=-(k*error_x); // cambiar
   sent.twist.linear.y=-(k*error_y); // cambiar

   chatter_pub.publish(sent);
   ros::spinOnce();
   loop_rate.sleep();
  }

  ros::spin();
  return 0;
}