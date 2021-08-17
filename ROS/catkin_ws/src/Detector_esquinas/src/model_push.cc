#include <functional>
#include <gazebo/gazebo.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/common/common.hh>
#include <ignition/math/Vector3.hh>
#include <ros/ros.h>

double new_secs=-20;

namespace gazebo
{
  class ModelPush : public ModelPlugin
  {
    public: void Load(physics::ModelPtr _parent, sdf::ElementPtr /*_sdf*/)
    {
      // Store the pointer to the model
      this->model = _parent;

      // Listen to the update event. This event is broadcast every
      // simulation iteration.
      this->updateConnection = event::Events::ConnectWorldUpdateBegin(
          std::bind(&ModelPush::OnUpdate, this));
      
      ROS_WARN("AAALoaded ModelPush Plugin with parent...%s", this->model->GetName().c_str());
    }

    // Called by the world update start event
    public: void OnUpdate()
    { 

      if(new_secs+5>ros::Time::now().toSec())
      {
      	// Apply a small linear velocity to the model.
      	this->model->SetLinearVel(ignition::math::Vector3d(0.3, 0, 0));
      }
      else if(new_secs+10>ros::Time::now().toSec())
      { 	
	      this->model->SetLinearVel(ignition::math::Vector3d(-0.3, 0, 0));
      }
      else
      {
	      new_secs=ros::Time::now().toSec();
      }	
    }

    // Pointer to the model
    private: physics::ModelPtr model;

    // Pointer to the update event connection
    private: event::ConnectionPtr updateConnection;
  };

  // Register this plugin with the simulator
  GZ_REGISTER_MODEL_PLUGIN(ModelPush)
}
