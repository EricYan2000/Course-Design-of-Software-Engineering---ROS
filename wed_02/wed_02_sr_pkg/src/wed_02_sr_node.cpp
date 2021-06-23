#include <ros/ros.h>
#include <std_msgs/String.h>
static ros::Publisher spk_pub;
static std_msgs::String strSpeak;
void KeywordCB(const std_msgs::String::ConstPtr &msg)
{
    ROS_WARN("[KeywordCB] - %s ", msg->data.c_str());
    int nFindIndex = 0;
    nFindIndex = msg->data.find("Hello");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "Hi";
        spk_pub.publish(strSpeak);
        return;
    }
    nFindIndex = msg->data.find("Goodbye") + msg->data.find("Good bye");
    if (nFindIndex >= 0)
    {
        strSpeak.data = "Bye Bye";
        spk_pub.publish(strSpeak);
        return;
    }

    strSpeak.data = "Sorry, I can't understand you!";
    spk_pub.publish(strSpeak);
    return;

}
int main(int argc, char **argv)
   
{
    ros::init(argc, argv, "sr_node");
    ros::NodeHandle n;
    ros::Subscriber sub_sr = n.subscribe("/xfyun/iat", 10, KeywordCB);
    spk_pub = n.advertise<std_msgs::String>("/xfyun/tts", 10);
    ros::spin();
    return 0;
}