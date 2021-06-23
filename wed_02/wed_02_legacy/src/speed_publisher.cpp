#include <ros/ros.h>
#include <std_msgs/Float64.h>

int main(int argc, char **argv)
{
    ros::init(argc, argv, "speed_publisher");
    ros::NodeHandle n;
    ros::Publisher spd_pub = n.advertise<std_msgs::Float64>("xspeed", 1);
    std_msgs::Float64 input_float;
    input_float.data = 0.1;
    while (ros::ok()) {
        spd_pub.publish(input_float);
        ros::spinOnce();
    }
}