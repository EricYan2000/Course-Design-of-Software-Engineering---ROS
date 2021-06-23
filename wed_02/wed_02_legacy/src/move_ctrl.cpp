#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
ros::Publisher vel_pub;

void PubDown(const geometry_msgs::Twist::ConstPtr& msg)
{
    
    geometry_msgs::Twist vel_cmd;
    vel_cmd.linear.x = msg->linear.x;
    vel_cmd.linear.y = msg->linear.y;
    vel_cmd.angular.z = msg->angular.z;
    vel_pub.publish(vel_cmd);
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "move_ctrl");
    ros::NodeHandle n;
    ros::Subscriber vel_sub = n.subscribe("/cmd_move", 10, PubDown);
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    
    ros::spin();

    return 0;
}