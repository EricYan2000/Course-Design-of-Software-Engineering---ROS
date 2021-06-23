#include <ros/ros.h>
#include <std_msgs/String.h>
#include <sensor_msgs/LaserScan.h>

void lidar_Callback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
    int nNum = scan->ranges.size();
    //for (int i = 0; i < nNum; i++) {
    //    ROS_INFO("Point[%d], %f", i, scan->ranges[i]);
    //}
    ROS_INFO("Point[%d], %f", 180, scan->ranges[180]);

}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "obstacle_detection_node");
    ros::NodeHandle nh;
    ros::Subscriber sub = nh.subscribe("/scan", 10, &lidar_Callback);

    ros::spin();
}