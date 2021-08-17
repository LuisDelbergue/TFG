//LIBRERIAS Y NAMESPACES
#include <ros/ros.h>
#include "std_msgs/Float32MultiArray.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PoseStamped.h"
#include "aerostack_msgs/SetControlMode.h"
#include <iostream>
#include "aerostack_msgs/MotionControlMode.h"

//CONSTANTES DEL CUADRADO FINAL IDEAL
const float centroi_x=0.5; //320 EN PIXELES
const float centroi_y=0.5; //240 EN PIXELES
const float i_z=0.4; 

//VARIABLES
float centro_x, centro_y,Cx,Cy,Fx,Fy,tl_x,br_x,tl_y,br_y;
float k1=0.2;  //GANANCIA EN  X e Y
float k2=1;  //GANANCIA EN z, A LO MEJOR IGUAL QUE k1

//CLASE CON LAS VARIABLES Y FUNCIONES SUBSCRIBERS PARA PASAR AL MAIN
class server{
  public:
    void cornersCallback(const std_msgs::Float32MultiArray::ConstPtr &corners_sub);
    void poseCallback(const geometry_msgs::PoseStamped &pose_sub);
    float z;
    float Arr[8];
    bool enable;
};

//OBTENER LOS VALORES DEL VECTOR DE LA IMAGEN (ESQUINAS, CARACTERÍSTICAS IMAGEN)
void server::cornersCallback(const std_msgs::Float32MultiArray::ConstPtr &corners_sub)
{
   if(corners_sub->data.size()>0)
   {
    Arr[0]=corners_sub->data.at(0);  //tl_x
    Arr[1]=corners_sub->data.at(1);  //tl_y
    Arr[2]=corners_sub->data.at(2);  //br_x
    Arr[3]=corners_sub->data.at(3);  //br_y
    Arr[4]=corners_sub->data.at(4);  //Cx
    Arr[5]=corners_sub->data.at(5);  //Cy
    Arr[6]=corners_sub->data.at(6);  //Fx
    Arr[7]=corners_sub->data.at(7);  //Fy
    enable=1;  //BANDERA PARA SOLAMENTE LEER SI SE VE LA PLATAFORMA
   }
   else 
    enable=0;
}

//OBTENER LA ALTURA
 void server::poseCallback(const geometry_msgs::PoseStamped &pose_sub)
 {
  z=pose_sub.pose.position.z;
 }

int main(int argc, char **argv)
{
  server objserver;  //DECLARO LA CLASE
  //INICIO EL NODO
  ros::init(argc, argv, "IBVS");   
  ros::NodeHandle p; 

  //SUBSCRIBERS: COGE EL VECTOR DE ESQUINAS Y CARACTERISTICAS IMAGEN Y LA ALTURA
  ros::Subscriber cornerssub = p.subscribe("Corners_ibvs", 1000, &server::cornersCallback, &objserver);    
  ros::Subscriber pos_sub = p.subscribe("drone1/self_localization/pose", 1000, &server::poseCallback, &objserver);

  //PUBLISHERS: MANDA VELOCIDAD
  ros::Publisher vel_pub = p.advertise<geometry_msgs::TwistStamped>("drone1/actuator_command/speed", 1000);
 
  //PONER MODO VELOCIDAD EL DRON, LLAMNADO SERVICIO
  ros::ServiceClient setControlModeClientSrv = p.serviceClient<aerostack_msgs::SetControlMode>("/drone1/set_control_mode");
  aerostack_msgs::SetControlMode setControlModeSrv;
  setControlModeSrv.request.controlMode.mode = aerostack_msgs::MotionControlMode::GROUND_SPEED;
  setControlModeClientSrv.call(setControlModeSrv);

  ros::Rate loop_rate(10);   

  while (ros::ok())
  {
    std::cout<<objserver.enable<<" - "<<tl_x<<" - "<<objserver.z<<std::endl;
     if(objserver.enable==1) //SI VE LA PLATAFORMA
     {
      //ASGINA VALORES A LAS VARIABLES CON UN NOMBRE MAS LEJIBLE
      tl_x=objserver.Arr[0];
      tl_y=objserver.Arr[1];
      br_x=objserver.Arr[2];
      br_y=objserver.Arr[3];      
      Cx=objserver.Arr[4];
      Cy=objserver.Arr[5];
      Fx=objserver.Arr[6];
      Fy=objserver.Arr[7];

      //CALCULA EL CENTRO Y LOS ERRORES
      centro_x=(br_x-tl_x)/2+tl_x;
      centro_y=(tl_y-br_y)/2+br_y;
      float error_x=centroi_x-centro_x;
      float error_y=centroi_y-centro_y;
      float error_z=i_z-objserver.z; 

      //CALCULA LA VELOCIDAD Y LA PUBLICA
      geometry_msgs::TwistStamped sent;
      sent.header.stamp=ros::Time::now();
      sent.twist.linear.x= -k1*(((error_x)/objserver.z)+(error_z*(centro_x-Cx)/Fx));
      //sent.twist.linear.y= -k1*(((error_y)/objserver.z)+(error_z*(centro_y-Cy)/Fy)); 
      //sent.twist.linear.z=k2*error_z;
      vel_pub.publish(sent);

      std::cout<<error_x<<" - "<<sent.twist.linear.x<<std::endl;
      if(error_x>0){std::cout<<"Moviendo derecha"<<std::endl;}
      if(error_x<0){std::cout<<"Moviendo izquierda"<<std::endl;}
    }
      ros::spinOnce();
      loop_rate.sleep(); 
  }
  ros::spin();
  return 0;
}