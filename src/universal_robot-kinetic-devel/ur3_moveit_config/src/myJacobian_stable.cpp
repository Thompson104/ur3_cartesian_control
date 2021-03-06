/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2012, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Sachin Chitta */

#include <ros/ros.h>
#include <sensor_msgs/JointState.h>
#include <sensor_msgs/Joy.h>


// MoveIt
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/robot_model/robot_model.h>
#include <moveit/robot_state/robot_state.h>

class Joy2Joint
{
public:
 Joy2Joint()
 {
  pub_counter_ = 1;
  joint_state_pub_ = n_.advertise<sensor_msgs::JointState>("joint_states", 10);
  joy_sub_ = n_.subscribe("joy_continuous",1, &Joy2Joint::callback, this);
  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  kinematic_model_ = robot_model_loader.getModel();
  joint_model_group_ = kinematic_model_->getJointModelGroup("manipulator");
  // Get Joint Values
  // ^^^^^^^^^^^^^^^^
  joint_values_={0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
 }

 void callback(const sensor_msgs::Joy& msg)
 {
  // Start
  // ^^^^^
  //ROS_INFO("Model frame: %s", kinematic_model_->getModelFrame().c_str());

  robot_state::RobotStatePtr kinematic_state(new robot_state::RobotState(kinematic_model_));
  kinematic_state->setJointGroupPositions(joint_model_group_, joint_values_);  
  const std::vector<std::string> &joint_names_ = joint_model_group_->getVariableNames();

  // Get the Jacobian
  // ^^^^^^^^^^^^^^^^
  // We can also get the Jacobian from the :moveit_core:`RobotState`.
  Eigen::Vector3d reference_point_position(0.0, 0.0, 0.0);
  Eigen::MatrixXd jacobian;

  kinematic_state->getJacobian(joint_model_group_,
                               kinematic_state->getLinkModel(joint_model_group_->getLinkModelNames().back()),
                               reference_point_position, jacobian);
  ROS_INFO_STREAM("Jacobian: " << jacobian);
  sensor_msgs::JointState pub_msg;

  pub_msg.header.seq = pub_counter_;
  pub_counter_++;
  ros::Time now = ros::Time::now();
  pub_msg.header.stamp.sec = now.sec;
  pub_msg.header.stamp.nsec = now.nsec;
 
  pub_msg.name = joint_names_;
  pub_msg.position = joint_values_;

  joint_values_[0] += 0.01;

  /**
   * The publish() function is how you send messages. The parameter
   * is the message object. The type of this object must agree with the type
   * given as a template parameter to the advertise<>() call, as was done
   * in the constructor above.
   */
  joint_state_pub_.publish(pub_msg);
  
 }
private:
 int pub_counter_;
 ros::NodeHandle n_;
 ros::Publisher joint_state_pub_;
 ros::Subscriber joy_sub_;
 std::vector<std::string> joint_names_;
 const robot_state::JointModelGroup *joint_model_group_;
 robot_model::RobotModelPtr kinematic_model_;
 std::vector<double> joint_values_;
};

//MAIN
//^^^^
int main(int argc, char **argv)
{
  ros::init(argc, argv, "Joy2Joint");

  Joy2Joint joyful_object;

  ros::spin();  
  return 0;
}
/*
  // Start
  // ^^^^^
  robot_model_loader::RobotModelLoader robot_model_loader("robot_description");
  robot_model::RobotModelPtr kinematic_model = robot_model_loader.getModel();
  ROS_INFO("Model frame: %s", kinematic_model->getModelFrame().c_str());

  robot_state::RobotStatePtr kinematic_state(new robot_state::RobotState(kinematic_model));
  kinematic_state->setToDefaultValues();
  const robot_state::JointModelGroup *joint_model_group = kinematic_model->getJointModelGroup("manipulator");
  // get joint_names
  const std::vector<std::string> &joint_names = joint_model_group->getVariableNames();

  // Get Joint Values
  // ^^^^^^^^^^^^^^^^
  std::vector<double> joint_values;
  kinematic_state->copyJointGroupPositions(joint_model_group, joint_values);
  for (std::size_t i = 0; i < joint_names.size(); ++i)
  {
    ROS_INFO("Joint %s: %f", joint_names[i].c_str(), joint_values[i]);
  }


  // Get the Jacobian
  // ^^^^^^^^^^^^^^^^
  // We can also get the Jacobian from the :moveit_core:`RobotState`.
  Eigen::Vector3d reference_point_position(0.0, 0.0, 0.0);
  Eigen::MatrixXd jacobian;
  
  kinematic_state->getJacobian(joint_model_group,
                               kinematic_state->getLinkModel(joint_model_group->getLinkModelNames().back()),
                               reference_point_position, jacobian);
  ROS_INFO_STREAM("Jacobian: " << jacobian);
  // END_TUTORIAL

  int seq = 1;
  while (ros::ok())
  {

      kinematic_state->getJacobian(joint_model_group,
                               kinematic_state->getLinkModel(joint_model_group->getLinkModelNames().back()),
                               reference_point_position, jacobian);
   */ /**
     * This is a message object. You stuff it with data, and then publish it.
     *//*
    sensor_msgs::JointState msg;

    msg.header.seq = seq;
    seq++;
    ros::Time now = ros::Time::now();
    msg.header.stamp.sec = now.sec;
    msg.header.stamp.nsec = now.nsec;

    msg.name = joint_names; 
    msg.position = joint_values;

    joint_values[0] += 0.01;

   */ /**
     * The publish() function is how you send messages. The parameter
     * is the message object. The type of this object must agree with the type
     * given as a template parameter to the advertise<>() call, as was done
     * in the constructor above.
     */
   /* joint_state_pub.publish(msg);

    ros::spinOnce();

    loop_rate.sleep();
  }

  ros::shutdown();
  return 0; 
*/
