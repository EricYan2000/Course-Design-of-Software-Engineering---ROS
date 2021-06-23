#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float64.h>

ros::Publisher vel_pub;

void myCallback(const std_msgs::Float64& msg)
{
    ROS_INFO("The speed is %f\n", msg.data);
    geometry_msgs::Twist vel_cmd;
    vel_cmd.linear.x = msg.data;
    vel_cmd.linear.y = 0;
    vel_cmd.linear.z = 0;
    vel_cmd.angular.x = 0;
    vel_cmd.angular.y = 0;
    vel_cmd.angular.z = 0;
    vel_pub.publish(vel_cmd);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "speed_subscriber");
    ros::NodeHandle n;
    ros::Subscriber spd_sub = n.subscribe("xspeed", 1, myCallback);
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::spin();
    return 0;
}