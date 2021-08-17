#include <ros/ros.h>
#include "std_msgs/Float32MultiArray.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PoseStamped.h"
#include <iostream>

const float centroi_x=0.5; //320
const float centroi_y=0.5; //240
float centro_x, centro_y;
float k=1;  //ganancia


class server{
  public:
    void cornersCallback(const std_msgs::Float32MultiArray::ConstPtr &corners_sub);
    //void poseCallback(const geometry_msgs::PoseStamped &pose_sub);
    //float z;
    float Arr[6];
};

void server::cornersCallback(const std_msgs::Float32MultiArray::ConstPtr &corners_sub)
{
  //    int i = 0;
	//  for(std::vector<float>::const_iterator it = corners_sub->data.begin(); it != corners_sub->data.end(); ++it)
	//  {
	//  	Arr[i] = *it;
	//  	i++;
	//  }
  Arr[0]=corners_sub->data.at(0);
  Arr[5]=corners_sub->data.at(5);
}

// void server::poseCallback(const geometry_msgs::PoseStamped &pose_sub)
// {
//  z=pose_sub.pose.position.z;
// }

int main(int argc, char **argv)
{
  server objserver;
  ros::init(argc, argv, "IBVS");   
  ros::NodeHandle p;   
  ros::Subscriber cornerssub = p.subscribe("Corners_ibvs", 1000, &server::cornersCallback, &objserver); 
  
  //ros::Subscriber pos_sub = p.subscribe("self_localization/pose", 1000, &server::poseCallback, &objserver);

  //ros::Publisher vel_pub = p.advertise<geometry_msgs::TwistStamped>("actuator_command/speed", 1000);
  

  ros::Rate loop_rate(10);   

  while (ros::ok())
  {
      std::cout<<objserver.Arr[0]<<std::endl; //20
      std::cout<<objserver.Arr[5]<<std::endl; //20
  //  centro_x=(objserver.corners_received.data[2]-objserver.corners_received.data[0])/2+objserver.corners_received.data[0];
  //  centro_y=(objserver.corners_received.data[1]-objserver.corners_received.data[3])/2+objserver.corners_received.data[3];
  //  int error_x=centroi_x-centro_x;
  //  int error_y=centroi_y-centro_y;
   //std::cout<<objserver.corners_received.data[4]<<std::endl; //480
  //  if(error_x>0){std::cout<<"Moviendo izquierda"<<std::endl;}
  //  if(error_x<0){std::cout<<"Moviendo derecha"<<std::endl;}

   geometry_msgs::TwistStamped sent;
   sent.header.stamp=ros::Time::now();

  //  sent.twist.linear.x=-(k*error_x); // cambiar
  //  sent.twist.linear.y=-(k*error_y); // cambiar

   //vel_pub.publish(sent);
   ros::spinOnce();
   loop_rate.sleep();
  }

  ros::spin();
  return 0;
}