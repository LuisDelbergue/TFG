//librería de ROS
#include "ros/ros.h" 
//librería para usar el tipo de mesaje                   
#include "geometry_msgs/Point.h"        

//función cuando se llama cuando se recibe un dato
void chatterCallback(const geometry_msgs::Point msg){  
  //se almacena el dato 
  ROS_INFO("[%f] [%f] [%f] \n", msg.x, msg.y, msg.z ); 
}

//inicia el main
int main(int argc, char **argv){
  
  //inicializa el nodo como subscriber
  ros::init(argc, argv, "nombre_subscriber"); 

  //constructor
  ros::NodeHandle n; 

  //Se subscribe al tópico, va almacenando 1000 datos como máximo hasta tirar los viejos, llama a la función
  ros::Subscriber sub = n.subscribe("drone111/sensor_measurement/altitude", 1000, chatterCallback);

  //Espera constantemente a que llegue algo
  ros::spin();

  return 0;
}