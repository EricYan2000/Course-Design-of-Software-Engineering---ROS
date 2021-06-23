#include <ros/ros.h>
#include <std_msgs/String.h>
#include <geometry_msgs/Twist.h>
static ros::Publisher spk_pub;
static std_msgs::String strSpeak;

ros::Publisher cmd_vel_pub;

geometry_msgs::Twist vel_cmd;

static float kl = 0.2;
static float kt = 0.5;

void KeywordCB(const std_msgs::String::ConstPtr &msg)
{
    printf("[KeywordCB] - %s \n", msg->data.c_str());
    int nFindIndex = 0;
    nFindIndex = msg->data.find("你叫什么名字");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "我是路西，你呢？";
        spk_pub.publish(strSpeak);
        return;
    }

    nFindIndex = msg->data.find("前");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "好的，向前0.5米";
        spk_pub.publish(strSpeak);

        vel_cmd.linear.x = kl;
        cmd_vel_pub.publish(vel_cmd);

        sleep(3);
        vel_cmd.linear.x = 0;
        cmd_vel_pub.publish(vel_cmd);

        return;
    }

    nFindIndex = msg->data.find("后");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "好的，向后0.5米";
        spk_pub.publish(strSpeak);

        vel_cmd.linear.x = -kl;
        cmd_vel_pub.publish(vel_cmd);

        sleep(3);
        vel_cmd.linear.x = 0;
        cmd_vel_pub.publish(vel_cmd);

        return;
    }

    nFindIndex = msg->data.find("左");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "好的，向左0.5米";
        spk_pub.publish(strSpeak);

        vel_cmd.linear.y = kl;
        cmd_vel_pub.publish(vel_cmd);

        sleep(3);
        vel_cmd.linear.y = 0;
        cmd_vel_pub.publish(vel_cmd);

        return;
    }

    nFindIndex = msg->data.find("右");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "好的，向右0.5米";
        spk_pub.publish(strSpeak);

        vel_cmd.linear.y = -kl;
        cmd_vel_pub.publish(vel_cmd);

        sleep(3);
        vel_cmd.linear.y = 0;
        cmd_vel_pub.publish(vel_cmd);

        return;
    }

    strSpeak.data = "请输入有效指令";
    spk_pub.publish(strSpeak);
    return;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "sr_node");
    ros::NodeHandle n;
    ros::Subscriber sub_sr = n.subscribe("/xfyun/iat", 10, KeywordCB);
    spk_pub = n.advertise<std_msgs::String>("/xfyun/tts", 10);
    cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    

    vel_cmd.linear.x = 0;
    vel_cmd.linear.y = 0;
    vel_cmd.angular.z = 0;

    ros::spin();
    return 0;
}