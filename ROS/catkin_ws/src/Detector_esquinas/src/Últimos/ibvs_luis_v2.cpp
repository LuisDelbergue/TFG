#include <ros/ros.h>
#include "std_msgs/Float32MultiArray.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PoseStamped.h"
#include "aerostack_msgs/SetControlMode.h"
#include <iostream>

//#include "aerostack_msgs/MotionControlMode.h"

const float centroi_x=0.5; //320
const float centroi_y=0.5; //240
//const float i_z=40; 

float centro_x, centro_y,Cx,Cy,Fx,Fy,tl_x,br_x,tl_y,br_y;
float k1=1;  //ganancia x e y
float k2=1;  //ganancia z, a lo mejor igual que x e y

// bool setControlMode(int new_control_mode){
//   // Prepare service message
//   aerostack_msgs::SetControlMode setControlModeSrv;
//   setControlModeSrv.request.controlMode.mode = new_control_mode;
//   // use service
//   if (setControlModeClientSrv.call(setControlModeSrv)){
//     return setControlModeSrv.response.ack;
//   }else{
//     return false;
//   }
// }

class server{
  public:
    void cornersCallback(const std_msgs::Float32MultiArray::ConstPtr &corners_sub);
    //void poseCallback(const geometry_msgs::PoseStamped &pose_sub);
    //float z;
    float Arr[8];
};

void server::cornersCallback(const std_msgs::Float32MultiArray::ConstPtr &corners_sub)
{
  Arr[0]=corners_sub->data.at(0);
  Arr[1]=corners_sub->data.at(1);
  Arr[2]=corners_sub->data.at(2);
  Arr[3]=corners_sub->data.at(3);
  Arr[4]=corners_sub->data.at(4);
  Arr[5]=corners_sub->data.at(5);
  Arr[6]=corners_sub->data.at(6);
  Arr[7]=corners_sub->data.at(7);
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
 
  ros::ServiceClient setControlModeClientSrv = p.serviceClient<aerostack_msgs::SetControlMode>("/drone1/GROUND_SPEED");
  aerostack_msgs::SetControlMode setControlModeSrv;
  setControlModeSrv.request.controlMode.mode = aerostack_msgs::MotionControlMode::GROUND_SPEED;
  setControlModeClientSrv.call(setControlModeSrv);

  ros::Rate loop_rate(10);   

  while (ros::ok())
  {
    //setControlMode(aerostack_msgs::MotionControlMode::GROUND_SPEED);
    tl_x=objserver.Arr[0];
    tl_y=objserver.Arr[1];
    br_x=objserver.Arr[2];
    br_y=objserver.Arr[3];      
    Cx=objserver.Arr[4];
    Cy=objserver.Arr[5];
    Fx=objserver.Arr[6];
    Fy=objserver.Arr[7];

    //std::cout<<objserver.Arr[0]<<std::endl; 

    centro_x=(br_x-tl_x)/2+tl_x;
    centro_y=(tl_y-br_y)/2+br_y;
    float error_x=centroi_x-centro_x;
    float error_y=centroi_y-centro_y;
  //  float error_z=i_z-z;

  //  if(error_x>0){std::cout<<"Moviendo izquierda"<<std::endl;}
  //  if(error_x<0){std::cout<<"Moviendo derecha"<<std::endl;}

    geometry_msgs::TwistStamped sent;
    sent.header.stamp=ros::Time::now();

  //  sent.twist.linear.x=-(k1*error_x)/z)+(error_z*(centro_x-Cx)/Fx);//coger Cx y fx de la cámara
  //  sent.twist.linear.y=-(k1*error_y)/z+(error_z*(centro_y-Cy)/Fy); 
  //  sent.twist.linear.y=-(k2*error_z);

   //vel_pub.publish(sent);
    ros::spinOnce();
    loop_rate.sleep();
  }

  ros::spin();
  return 0;
}