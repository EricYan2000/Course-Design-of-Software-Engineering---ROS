#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <ctime>
#include <cstdio>

// int _sleep(int);
using namespace std;

class JoyCtrl
{
    public:
        JoyCtrl();
        float left_x, left_y, right_y;
        ros::NodeHandle n;
        ros::Subscriber sub;

        ros::Time current_time;
        ros::Time last_time;
        ros::Publisher velcmd_pub;

    private:
        void callBack(const sensor_msgs::Joy::ConstPtr& joy);
};

JoyCtrl::JoyCtrl()
{
    left_x = 0;
    left_y = 0;
    right_y = 0;
    velcmd_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    sub = n.subscribe<sensor_msgs::Joy>("joy", 10, &JoyCtrl::callBack, this);
    current_time = ros::Time::now();
    last_time = ros::Time::now();

    ROS_INFO("JoyCtrl");
}

static float kl = 0.2;
static float kt = 0.5;

void JoyCtrl::callBack(const sensor_msgs::Joy::ConstPtr& joy)
{
    left_x = joy->axes[1];
    left_y = joy->axes[0];
    right_y = joy->axes[3];

    geometry_msgs::Twist vel_cmd;
    vel_cmd.linear.x = (float)left_x*kl;
    vel_cmd.linear.y = (float)left_y*kl;
    vel_cmd.linear.z = 0;
    vel_cmd.angular.x = 0;
    vel_cmd.angular.y = 0;
    vel_cmd.angular.z = (float)right_y*kt;
    velcmd_pub.publish(vel_cmd);
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "vel_ctrl");

    JoyCtrl myJoyCtrl;

    ros::spin();

    return 0;
    

    /*
    const int sleep_time = 50000;
    ros::init(argc, argv, "vel_ctrl");
    ros::NodeHandle n;
    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    clock_t start, end;
    geometry_msgs::Twist vel_cmd;
    start = clock();
    int cur_time = 0;
    while(n.ok())
    {
        getchar();
        end = clock();
        //int cur_time = (end - start) / (CLOCKS_PER_SEC);
        cur_time ++;

        /*if (cur_time > 2) {
            vel_cmd.linear.x = 0;
            vel_cmd.linear.y = 0;
            vel_cmd.linear.z = 0;
            vel_cmd.angular.x = 0;
            vel_cmd.angular.y = 0;
            vel_cmd.angular.z = 0;
            vel_pub.publish(vel_cmd);
            ros::spinOnce();
        }
        else {
            vel_cmd.linear.x = 0.1;
            vel_cmd.linear.y = 0.1;
            vel_cmd.linear.z = 0;
            vel_cmd.angular.x = 0;
            vel_cmd.angular.y = 0;
            vel_cmd.angular.z = 0;
            vel_pub.publish(vel_cmd);
            ros::spinOnce();
        }*/
        /*
        cur_time %= 4;

        std::cout << cur_time << std::endl;
    
        if (cur_time == 0) {
            vel_cmd.linear.x = 0.1;
            vel_cmd.linear.y = 0.1;
            vel_cmd.linear.z = 0;
            vel_cmd.angular.x = 0;
            vel_cmd.angular.y = 0;
            vel_cmd.angular.z = 0;
            vel_pub.publish(vel_cmd);
            ros::spinOnce();
        }
        else if (cur_time == 1) {
            vel_cmd.linear.x = 0.1;
            vel_cmd.linear.y = -0.1;
            vel_cmd.linear.z = 0;
            vel_cmd.angular.x = 0;
            vel_cmd.angular.y = 0;
            vel_cmd.angular.z = 0;
            vel_pub.publish(vel_cmd);
            ros::spinOnce();
        }
        else if (cur_time == 2) {
            vel_cmd.linear.x = -0.1;
            vel_cmd.linear.y = -0.1;
            vel_cmd.linear.z = 0;
            vel_cmd.angular.x = 0;
            vel_cmd.angular.y = 0;
            vel_cmd.angular.z = 0;
            vel_pub.publish(vel_cmd);
            ros::spinOnce();
        }
        else if (cur_time == 3) {
            vel_cmd.linear.x = -0.1;
            vel_cmd.linear.y = 0.1;
            vel_cmd.linear.z = 0;
            vel_cmd.angular.x = 0;
            vel_cmd.angular.y = 0;
            vel_cmd.angular.z = 0;
            vel_pub.publish(vel_cmd);
            ros::spinOnce();
        }

    }
    return 0;*/
}
/*
int _sleep(int cycle) {
    while(cycle--) {
        std::cout << "sleeping: " << cycle << std::endl;
    }
    return 0;
}*/
