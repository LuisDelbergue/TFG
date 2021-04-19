//librería de ROS
#include "ros/ros.h" 
//librería para usar el tipo de mesaje                   
#include "geometry_msgs/Point.h"
//librería para texto pantalla <<
#include <sstream>

//inicia el main
int main(int argc, char **argv){

  //inicializa el nodo como publisher
  ros::init(argc, argv, "talker");

  //constructor
  ros::NodeHandle n;

  //Para publicar a través del tópico y con un buffer de 1000
  ros::Publisher pub = n.advertise<geometry_msgs::TwistStamped::Vector3>("drone111/quadrotor_pid_controller_process", 1000);

  //Variable Rate que llamamos loop_rate para un bucle de 10Hz
  ros::Rate loop_rate(10);

  //inicializa el contador a cero
  int count = 0;

  //hasta que no se pulse cntrl+C
  while (ros::ok()){

    //creamos el mensaje a enviar
    geometry_msgs::TwistStamped::Vector3 msg;
    msg.x=1;
    msg.y=1;
    msg.z=0;

    //imprimir por pantalla
    ROS_INFO("%f %f %f", msg.data.c_str());

    //publicamos el dato
    pub.publish(msg);

    //los topics se ponen a la cola hasta que hay un spinOnce que se envían
    ros::spinOnce();

    //Es como un delay que cuenta el tiempo que ha pasado desde la última vez
    loop_rate.sleep();

    //
    ++count;}

  return 0;
}