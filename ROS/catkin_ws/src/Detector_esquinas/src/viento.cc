#include <functional>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <ros/ros.h>
#include<time.h>
#include<stdlib.h>

namespace gazebo
{
  class viento : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
      // Store the pointer to the model
      this->model = _parent;

      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          std::bind(&viento::OnUpdate, this));
      
      ROS_WARN("BBBLoaded viento Plugin with parent...%s", this->model->GetName().c_str());
    }

    // Called by the world update start event
    public: void OnUpdate()
    { 
        srand(time(NULL));
if(ros::Time::now().toSec()>40){
	ROS_INFO("El tiempo es %f",ros::Time::now().toSec());}
      if(ros::Time::now().toSec()>60){
	ROS_INFO("El tiempo es %f",ros::Time::now().toSec());

	this->model->GetLink("base_link")->SetForce(ignition::math::Vector3d(0, (((rand()%8)+2)*0.6) , 0));
	//this->model->SetForce(ignition::math::Vector3d(0, ((rand()%8)*0.1) , 0));
	
      	// Apply a small linear velocity to the model.	
      	//this->model->SetLinearVel(ignition::math::Vector3d(0, (((rand()%4)+1)*0.001) , 0)); //entre 0-0.5 m/s
      }
    }

    // Pointer to the model
    private: physics::ModelPtr model;

    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(viento)
}
