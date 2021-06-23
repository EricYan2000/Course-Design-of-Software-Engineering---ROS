/*********************************************************************
* Software License Agreement (BSD License)
*
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
*   * Neither the name of the WaterPlus nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  FOOTPRINTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
/*!******************************************************************
 @author     YanXianglong
 ********************************************************************/

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/JointState.h>

using namespace std;

#define MAX_HEIGHT 1.0
#define MIN_HEIGHT 0.5
#define MAX_WIDTH 0.1
#define MIN_WIDTH 0.0
#define HALT_TIME 5000
#define HEIGHT_STEP 0.005
#define WIDTH_STEP 0.002
#define HEIGHT_SPEED 0.5
#define WIDTH_SPEED 5

class TeleopJoy
{
public:
  TeleopJoy();
  bool bStart;
  bool A_button, B_button, X_button, Y_button, LB_button, RB_button;
  bool BACK_button, START_button, XBox_button, LS_button, RS_button;
  float lx, ly, ry;
  float arm_up_down, arm_cling;
  ros::NodeHandle n;
  ros::Subscriber sub;

  ros::Time current_time;
  ros::Time last_time;
  ros::Publisher velcmd_pub;
  ros::Publisher mani_ctrl_pub;

  void callBack(const sensor_msgs::Joy::ConstPtr& joy);
  void SendVelCmd();
  void SendArmCmd();
  void SendUICmd();
  void SendOtherCmd();
  void MsgsInit();
  void EmergencyHalt();
  //ros::Rate TeleopJoy::MySleep(double seconds);

private:
  sensor_msgs::JointState ctrl_msg;
  geometry_msgs::Twist vel_cmd;
};

static float kl = 0.2;
static float kt = 0.5;
static float last_height = 0;
static float last_width = 0;
static bool arm_is_out = false;

float height_protect(float expected_next_height);
float width_protect(float expected_next_width);

TeleopJoy::TeleopJoy()
{
  lx = 0; ly = 0; ry = 0;
  arm_up_down = 0;  arm_cling = 0;
  bStart = false;
  A_button = false; B_button = false; X_button = false; Y_button = false;
  LB_button = false;  RB_button = false;  BACK_button = false;  START_button = false;
  XBox_button = false;  LS_button = false; RS_button = false;

  velcmd_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",100);
  mani_ctrl_pub = n.advertise<sensor_msgs::JointState>("/wpb_home/mani_ctrl", 30);
  sub = n.subscribe<sensor_msgs::Joy>("joy",100,&TeleopJoy::callBack,this);
  current_time = ros::Time::now();
  last_time = ros::Time::now();
 
  MsgsInit();

  ROS_INFO("TeleopJoy");
}

void TeleopJoy::MsgsInit()
{
  ctrl_msg.name.resize(2);
  ctrl_msg.position.resize(2);
  ctrl_msg.velocity.resize(2);
  ctrl_msg.name[0] = "lift";
  ctrl_msg.name[1] = "gripper";
  ctrl_msg.position[0] = 0;
  ctrl_msg.position[1] = 0;

  vel_cmd.linear.x = 0;
  vel_cmd.linear.y = 0;
  vel_cmd.linear.z = 0;
  vel_cmd.angular.x = 0;
  vel_cmd.angular.y = 0;
  vel_cmd.angular.z = 0;
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "my_js_vel");

  TeleopJoy cTeleopJoy;

  ros::Rate r(30);
  while(ros::ok())
  {
    if (!cTeleopJoy.B_button)
      cTeleopJoy.SendVelCmd();
    if (!cTeleopJoy.B_button && arm_is_out == true)
      cTeleopJoy.SendArmCmd();
    cTeleopJoy.SendUICmd();
    cTeleopJoy.SendOtherCmd();
    ros::spinOnce();
    r.sleep();
  }
  return 0;
}

void TeleopJoy::SendVelCmd()
{
  if(bStart == false)
    return;

  /*chassis movement*/
  //geometry_msgs::Twist vel_cmd;
  vel_cmd.linear.x = (float)lx*kl;
  vel_cmd.linear.y = (float)ly*kl;
  vel_cmd.linear.z = 0;
  vel_cmd.angular.x = 0;
  vel_cmd.angular.y = 0;
  vel_cmd.angular.z = (float)ry*kt;
  velcmd_pub.publish(vel_cmd);
}

void TeleopJoy::SendArmCmd()
{
  if(bStart == false)
    return;

  /*arm movement*/
  if (arm_up_down > 0) {
    float next_height = height_protect(last_height + HEIGHT_STEP);
    ctrl_msg.position[0] = next_height;       //升降高度(单位:米)
    ctrl_msg.velocity[0] = HEIGHT_SPEED;     //升降速度(单位:米/秒)
    ctrl_msg.position[1] = last_width;     //手爪指间距(单位:米)
    ctrl_msg.velocity[1] = WIDTH_SPEED;       //手爪开合角速度(单位:度/秒)
    last_height = next_height;
    mani_ctrl_pub.publish(ctrl_msg);
    ROS_INFO("arm going up");
  } else if (arm_up_down < 0) {
    float next_height = height_protect(last_height - HEIGHT_STEP);
    ctrl_msg.position[0] = next_height;       //升降高度(单位:米)
    ctrl_msg.velocity[0] = HEIGHT_SPEED;     //升降速度(单位:米/秒)
    ctrl_msg.position[1] = last_width;     //手爪指间距(单位:米)
    ctrl_msg.velocity[1] = WIDTH_SPEED;       //手爪开合角速度(单位:度/秒)
    last_height = next_height;
    mani_ctrl_pub.publish(ctrl_msg);
    ROS_INFO("arm going up");
  }
  //if (arm_cling > 0) {
  if (X_button) { //cling
    float next_width = width_protect(last_width - WIDTH_STEP);
    ctrl_msg.position[0] = last_height;       //升降高度(单位:米)
    ctrl_msg.velocity[0] = HEIGHT_SPEED;     //升降速度(单位:米/秒)
    ctrl_msg.position[1] = next_width;     //手爪指间距(单位:米)
    ctrl_msg.velocity[1] = WIDTH_SPEED;       //手爪开合角速度(单位:度/秒)
    last_width = next_width;
    mani_ctrl_pub.publish(ctrl_msg);
    ROS_INFO("arm clinging");
  //} else if (arm_cling < 0) {
  } else if (Y_button) { //lose
    float next_width = width_protect(last_width + WIDTH_STEP);
    ctrl_msg.position[0] = last_height;       //升降高度(单位:米)
    ctrl_msg.velocity[0] = HEIGHT_SPEED;     //升降速度(单位:米/秒)
    ctrl_msg.position[1] = next_width;     //手爪指间距(单位:米)
    ctrl_msg.velocity[1] = WIDTH_SPEED;       //手爪开合角速度(单位:度/秒)
    last_width = next_width;
    mani_ctrl_pub.publish(ctrl_msg);
    ROS_INFO("arm losing");
  }
}

float height_protect(float expected_next_height)
{
  if (expected_next_height > MAX_HEIGHT) {
    return MAX_HEIGHT;
  } else if (expected_next_height < MIN_HEIGHT) {
    return MIN_HEIGHT;
  } else {
    return expected_next_height;
  }
}

float width_protect(float expected_next_width)
{
  if (expected_next_width > MAX_WIDTH) {
    return MAX_WIDTH;
  } else if (expected_next_width < MIN_WIDTH) {
    return MIN_WIDTH;
  } else {
    return expected_next_width;
  }
}

void TeleopJoy::SendUICmd()
{
  if (bStart == false)
    return;

  /*UI system control*/
  if (XBox_button) {
    ROS_INFO("[UI system]returning to home page");
  }
  if (BACK_button) {
    ROS_INFO("[UI system]go back");
  }
  if (START_button) {
    ROS_INFO("[UI system]start");
  }
}

// ros::Rate TeleopJoy::MySleep(double seconds)
// {
//   return ros::Rate r(1 / seconds);
// }

void TeleopJoy::SendOtherCmd()
{
  if (bStart == false)
    return;

  /*saved functions*/
  if (A_button) {
    ros::Rate r(1);
    ROS_INFO("A_button is pressed");
    if (arm_is_out) {
      ROS_INFO("deactivating arm");   //Enable arm
      ROS_INFO("[my_js_vel] * -> ZERO ");
      ctrl_msg.position[0] = 0;       //升降高度(单位:米)
      ctrl_msg.velocity[0] = HEIGHT_SPEED;     //升降速度(单位:米/秒)
      ctrl_msg.position[1] = MAX_WIDTH;     //手爪指间距(单位:米)
      ctrl_msg.velocity[1] = WIDTH_SPEED;       //手爪开合角速度(单位:度/秒)
      last_height = 0.0;
      last_width = MAX_WIDTH;
      arm_is_out = false;
    } else {
      ROS_INFO("activating arm");     //Disable arm
      ROS_INFO("[my_js_vel] ZERO -> DOWN");
      ctrl_msg.position[0] = MIN_HEIGHT;     //升降高度(单位:米)
      ctrl_msg.velocity[0] = HEIGHT_SPEED;     //升降速度(单位:米/秒)
      ctrl_msg.position[1] = MAX_WIDTH;     //手爪指间距(单位:米)
      ctrl_msg.velocity[1] = WIDTH_SPEED;       //手爪开合角速度(单位:度/秒)
      last_height = MIN_HEIGHT;
      last_width = MAX_WIDTH;
      arm_is_out = true;
    }
    for (int i = 0; i < 10; i++)
      mani_ctrl_pub.publish(ctrl_msg);
    ROS_INFO("[my_js_vel](SendOtherCmd) sleeping starts");
    r.sleep();
    ROS_INFO("[my_js_vel](SendOtherCmd) sleeping ends");
    //mani_ctrl_pub.publish(ctrl_msg);    //发送指令
  } //此处必须先睡眠，再发送指令，不然机械臂无法接收到展开指令
  if (B_button) { //stop all movements
    ros::Rate r(1);
    ROS_INFO("[Emergency brake] B_button is pressed");
    EmergencyHalt();
  }
  // if (X_button) { //arm cling
  //   ROS_INFO("X_button is pressed");
  // }
  // if (Y_button) { //arm lose
  //   ROS_INFO("Y_button is pressed");
  // }
  if (LS_button) {
    ROS_INFO("LS_button is pressed");
  }
  if (RS_button) {
    ROS_INFO("RS_button is pressed");
  }
}

void TeleopJoy::EmergencyHalt() {
  vel_cmd.linear.x = 0;
  vel_cmd.linear.y = 0;
  vel_cmd.linear.z = 0;
  vel_cmd.angular.x = 0;
  vel_cmd.angular.y = 0;
  vel_cmd.angular.z = 0;
  velcmd_pub.publish(vel_cmd);

  ctrl_msg.position[0] = last_height;     //升降高度(单位:米)
  ctrl_msg.velocity[0] = 0.0;     //升降速度(单位:米/秒)
  ctrl_msg.position[1] = last_width;       //手爪指间距(单位:米)
  ctrl_msg.velocity[1] = 0.0;       //手爪开合角速度(单位:度/秒)
  mani_ctrl_pub.publish(ctrl_msg);    //发送指令

  for (int i = 0; i < 10; i++) {
    velcmd_pub.publish(vel_cmd);
    mani_ctrl_pub.publish(ctrl_msg);
  }
  ROS_INFO("[EmergencyHalt] HALT instruction sent 10 times");
}

void TeleopJoy::callBack(const sensor_msgs::Joy::ConstPtr& joy)
{
  //ROS_INFO("Joy: [%.2f , %.2f]", lx , ry);
  lx = joy->axes[1];  //forward & backward
  ly = joy->axes[0];  //shift
  ry = joy->axes[3];

  arm_up_down = joy->axes[7];
  arm_cling = joy->axes[6];

  A_button = joy->buttons[0];
  B_button = joy->buttons[1];
  X_button = joy->buttons[2];
  Y_button = joy->buttons[3];
  LB_button = joy->buttons[4];
  RB_button = joy->buttons[5];
  BACK_button = joy->buttons[6];
  START_button = joy->buttons[7];
  XBox_button = joy->buttons[8];
  LS_button = joy->buttons[9];
  RS_button = joy->buttons[10];

  bStart = true;
}