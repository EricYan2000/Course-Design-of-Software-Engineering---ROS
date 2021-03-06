/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2017-2020, Waterplus http://www.6-robot.com
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
/* @author Zhang Wanjie                                             */
#include <ros/ros.h>
#include <std_msgs/String.h>
#include "wpb_home_tutorials/Follow.h"
#include <geometry_msgs/Twist.h>
#include "xfyun_waterplus/IATSwitch.h"
#include <sound_play/SoundRequest.h>
#include "wpb_home_tutorials/Follow.h"
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <waterplus_map_tools/Waypoint.h>
#include <waterplus_map_tools/GetWaypointByName.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <geometry_msgs/Twist.h>

using namespace std;

#define STATE_READY     0
#define STATE_FOLLOW    1
#define STATE_ASK       2
#define STATE_GOTO      3
#define STATE_GRAB      4
#define STATE_COMEBACK  5
#define STATE_PASS      6

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
static string strGoto;
static sound_play::SoundRequest spk_msg;
static ros::Publisher spk_pub;
static ros::Publisher vel_pub;
static string strToSpeak = "";
static string strKeyWord = "";
static ros::ServiceClient clientIAT;
static xfyun_waterplus::IATSwitch srvIAT;
static ros::ServiceClient cliGetWPName;
static waterplus_map_tools::GetWaypointByName srvName;
static ros::Publisher add_waypoint_pub;
static ros::ServiceClient follow_start;
static ros::ServiceClient follow_stop;
static ros::ServiceClient follow_resume;
static wpb_home_tutorials::Follow srvFlw;
static ros::Publisher behaviors_pub;
static std_msgs::String behavior_msg;

static std_msgs::String strSpeak;

static ros::Subscriber grab_result_sub;
static ros::Subscriber pass_result_sub;
static bool bGrabDone;
static bool bPassDone;

geometry_msgs::Twist vel_cmd;

static int nState = STATE_READY;
static int nDelay = 0;

static bool canRelease = false;

static vector<string> arKeyword;
static vector<string> arObjectWant;
static int nIndexWant = 0;

static float kl = 0.2;
static float kt = 0.5;

// ?????????????????????
void InitKeyword()
{
    arKeyword.push_back("start");   //??????????????????????????????,???????????????
    arKeyword.push_back("???");
    arKeyword.push_back("???");
    arKeyword.push_back("??????");
}

// ???????????????arKeyword?????????????????????
static string FindKeyword(string inSentence)
{
    string res = "";
    int nSize = arKeyword.size();
    for(int i=0;i<nSize;i++)
    {
        int nFindIndex = inSentence.find(arKeyword[i]);
        if(nFindIndex >= 0)
        {
            res = arKeyword[i];
            break;
        }
    }
    return res;
}

// ?????????????????????arKeyword?????????
static string FindMultiKeyword(string inSentence)
{
    string res = "";
    int nSize = arKeyword.size();
    for(int i=0;i<nSize;i++)
    {
        int nFindIndex = inSentence.find(arKeyword[i]);
        if(nFindIndex >= 0)
        {
            arObjectWant.push_back(arKeyword[i]);
        }
    }
    int nNumObject = arObjectWant.size();
    for(int i=0;i<nNumObject;i++)
    {
        res += arObjectWant[i];
        res += " , ";
    }
    return res;
}

// ??????????????????????????????????????????
void AddNewWaypoint(string inStr)
{
    tf::TransformListener listener;
    tf::StampedTransform transform;
    try
    {
        listener.waitForTransform("/map","/base_footprint",  ros::Time(0), ros::Duration(10.0) );
        listener.lookupTransform("/map","/base_footprint", ros::Time(0), transform);
    }
    catch (tf::TransformException &ex) 
    {
        ROS_ERROR("[lookupTransform] %s",ex.what());
        return;
    }

    float tx = transform.getOrigin().x();
    float ty = transform.getOrigin().y();
    tf::Stamped<tf::Pose> p = tf::Stamped<tf::Pose>(tf::Pose(transform.getRotation() , tf::Point(tx, ty, 0.0)), ros::Time::now(), "map");
    geometry_msgs::PoseStamped new_pos;
    tf::poseStampedTFToMsg(p, new_pos);

    waterplus_map_tools::Waypoint new_waypoint;
    new_waypoint.name = inStr;
    new_waypoint.pose = new_pos.pose;
    add_waypoint_pub.publish(new_waypoint);

    ROS_WARN("[New Waypoint] %s ( %.2f , %.2f )" , new_waypoint.name.c_str(), tx, ty);
}

// ????????????
// void Speak(string inStr)
// {
//     spk_msg.arg = inStr;
//     spk_msg.volume = 1.0f;  //indigo(Ubuntu 14.04)????????????????????????????????????
//     spk_pub.publish(spk_msg);
//     ros::spinOnce();
// }

void Speak(string inStr)
{
    strSpeak.data = inStr;
    spk_pub.publish(strSpeak);
}

// ??????????????????
static void FollowSwitch(bool inActive, float inDist)
{
    if(inActive == true)
    {
        srvFlw.request.thredhold = inDist;
        if (!follow_start.call(srvFlw))
        {
            ROS_WARN("[CActionManager] - follow start failed...");
        }
    }
    else
    {
        if (!follow_stop.call(srvFlw))
        {
            ROS_WARN("[CActionManager] - failed to stop following...");
        }
    }
}

// ????????????????????????
static void GrabSwitch(bool inActive)
{
    if(inActive == true)
    {
        behavior_msg.data = "grab start";
        behaviors_pub.publish(behavior_msg);
    }
    else
    {
        behavior_msg.data = "grab stop";
        behaviors_pub.publish(behavior_msg);
    }
}

// ??????????????????
static void PassSwitch(bool inActive)
{
    if(inActive == true)
    {
        behavior_msg.data = "pass start";
        behaviors_pub.publish(behavior_msg);
    }
    else
    {
        behavior_msg.data = "pass stop";
        behaviors_pub.publish(behavior_msg);
    }
}

// ??????????????????????????????
void KeywordCB(const std_msgs::String::ConstPtr & msg)
{
    ROS_WARN("------ Keyword = %s ------",msg->data.c_str());

    int forwardIndex = msg->data.find("???");
    int backIndex = msg->data.find("???");
    int leftIndex = msg->data.find("???");
    int rightIndex = msg->data.find("???");
    if(forwardIndex >= 0 || backIndex >= 0 || leftIndex >= 0 || rightIndex >= 0)
    {
        if (forwardIndex >= 0)
        {
            vel_cmd.linear.x = kl;
        }
        if (backIndex >= 0)
        {
            vel_cmd.linear.x = -kl;
        }
        if (leftIndex >= 0)
        {
            vel_cmd.linear.y = kl;
        }
        if (rightIndex >= 0)
        {
            vel_cmd.linear.y = -kl;
        }
        vel_pub.publish(vel_cmd);
        sleep(3);
        vel_cmd.linear.x = 0;
        vel_cmd.linear.y = 0;
        vel_pub.publish(vel_cmd);
    }

    if(nState == STATE_FOLLOW)
    {
        // ?????????????????????????????????????????????????????????
        string strKeyword = FindKeyword(msg->data);
        int nLenOfKW = strlen(strKeyword.c_str());
        if(nLenOfKW > 0)
        {
            // ?????????????????????????????????
            AddNewWaypoint(strKeyword);
            string strSpeak = strKeyword + "?????????????????????????????????????????????"; 
            Speak(strSpeak);
        }

        // ?????????????????????
        int nFindIndex = msg->data.find("????????????");
        if(nFindIndex >= 0)
        {
            FollowSwitch(false, 0);
            AddNewWaypoint("master");
            nState = STATE_ASK;
            nDelay = 0;
            Speak("?????????????????????????????????????????????");
        }
    }

    if(nState == STATE_ASK)
    {
        // ???????????????????????????????????????????????????????????????
        string strKeyword = FindMultiKeyword(msg->data);
        int nLenOfKW = strlen(strKeyword.c_str());
        if(nLenOfKW > 0)
        {
            // ?????????????????????????????????
            string strSpeak = "????????????????????? " + strKeyword; 
            Speak(strSpeak);
            sleep(5);   //????????????????????????????????????????????????
            nIndexWant = 0;
            nState = STATE_GOTO;
        }
    }

    if (nState == STATE_PASS && canRelease == false)
    {
        string strSpeak = "??????????????????????????????????????????";
        Speak(strSpeak);
        sleep(2);
        int nFindIndex = msg->data.find("??????");
        if(nFindIndex >= 0)
        {
            nDelay = 0;
            canRelease = true;
            Speak("??????????????????????????????");
        }
    }
}

// ??????????????????
void GrabResultCallback(const std_msgs::String::ConstPtr& res)
{
    int nFindIndex = 0;
    nFindIndex = res->data.find("done");
    if( nFindIndex >= 0 )
    {
        bGrabDone = true;
    }
}

// ??????????????????
void PassResultCallback(const std_msgs::String::ConstPtr& res)
{
    int nFindIndex = 0;
    nFindIndex = res->data.find("done");
    if( nFindIndex >= 0 )
    {
        bPassDone = true;
    }
}


int main(int argc, char** argv)
{
    ros::init(argc, argv, "wpb_home_shopping");

    ros::NodeHandle n;
    ros::Subscriber sub_sr = n.subscribe("/xfyun/iat", 10, KeywordCB);
    follow_start = n.serviceClient<wpb_home_tutorials::Follow>("wpb_home_follow/start");
    follow_stop = n.serviceClient<wpb_home_tutorials::Follow>("wpb_home_follow/stop");
    follow_resume = n.serviceClient<wpb_home_tutorials::Follow>("wpb_home_follow/resume");
    cliGetWPName = n.serviceClient<waterplus_map_tools::GetWaypointByName>("/waterplus/get_waypoint_name");
    add_waypoint_pub = n.advertise<waterplus_map_tools::Waypoint>( "/waterplus/add_waypoint", 1);
    //spk_pub = n.advertise<sound_play::SoundRequest>("/robotsound", 20);
    spk_pub = n.advertise<std_msgs::String>("/xfyun/tts", 10);
    //spk_msg.sound = sound_play::SoundRequest::SAY;
    //spk_msg.command = sound_play::SoundRequest::PLAY_ONCE;
    vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    clientIAT = n.serviceClient<xfyun_waterplus::IATSwitch>("xfyun_waterplus/IATSwitch");
    behaviors_pub = n.advertise<std_msgs::String>("/wpb_home/behaviors", 30);
    grab_result_sub = n.subscribe<std_msgs::String>("/wpb_home/grab_result",30,&GrabResultCallback);
    pass_result_sub = n.subscribe<std_msgs::String>("/wpb_home/pass_result",30,&PassResultCallback);

    InitKeyword();

    ROS_WARN("[main] wed_02_shopping");
    ros::Rate r(30);
	
    while(ros::ok())
    {
        // 1?????????????????????
        if(nState == STATE_READY)
        {
            // ?????????????????????????????????????????????
            nDelay ++;
            // ROS_WARN("[STATE_READY] - nDelay = %d", nDelay);
            if(nDelay > 100)
            {
                AddNewWaypoint("start");
                nDelay = 0;
                nState = STATE_FOLLOW;
                string hello_string = "???????????????????????????????????????";
	            Speak(hello_string);
            }
        }

        // 2???????????????
        if(nState == STATE_FOLLOW)
        {
            if(nDelay == 0)
            {
               FollowSwitch(true, 0.7);
            }
            nDelay ++;
        }

        // 3???????????????????????????
        if(nState == STATE_ASK)
        {
            
        }

        // 4????????????????????????
        if(nState == STATE_GOTO)
        {
            int nNumWant = arObjectWant.size();
            if(nIndexWant < nNumWant)
            {
                strGoto = arObjectWant[nIndexWant];
            }
            srvName.request.name = strGoto;
            if (cliGetWPName.call(srvName))
            {
                std::string name = srvName.response.name;
                float x = srvName.response.pose.position.x;
                float y = srvName.response.pose.position.y;
                ROS_INFO("[STATE_GOTO] Get_wp_name = %s (%.2f,%.2f)", strGoto.c_str(),x,y);

                MoveBaseClient ac("move_base", true);
                if(!ac.waitForServer(ros::Duration(5.0)))
                {
                    ROS_INFO("The move_base action server is no running. action abort...");
                }
                else
                {
                    move_base_msgs::MoveBaseGoal goal;
                    goal.target_pose.header.frame_id = "map";
                    goal.target_pose.header.stamp = ros::Time::now();
                    goal.target_pose.pose = srvName.response.pose;
                    ac.sendGoal(goal);
                    ac.waitForResult();
                    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
                    {
                        ROS_INFO("Arrived at %s!",strGoto.c_str());
                        Speak("????????????????????????");
                        nState = STATE_GRAB;
                        nDelay = 0;
                    }
                    else
                    {
                        ROS_INFO("Failed to get to %s ...",strGoto.c_str() );
                        Speak("?????????????????????????????????");
                        nState = STATE_ASK;
                    }
                }
                
            }
            else
            {
                ROS_ERROR("Failed to call service GetWaypointByName");
                Speak("There is no this waypoint.");
                nState = STATE_ASK;
            }
        }

        // 5???????????????
        if(nState == STATE_GRAB)
        {
            if(nDelay == 0)
            {
                bGrabDone = false;
                GrabSwitch(true);
            }
            nDelay ++;
            if(bGrabDone == true)
            {
                GrabSwitch(false);
                Speak("??????????????????????????????");
                nState = STATE_COMEBACK;
            }
        }

        // 6?????????????????????
        if(nState == STATE_COMEBACK)
        {
            srvName.request.name = "master";
            if (cliGetWPName.call(srvName))
            {
                std::string name = srvName.response.name;
                float x = srvName.response.pose.position.x;
                float y = srvName.response.pose.position.y;
                ROS_INFO("[STATE_COMEBACK] Get_wp_name = %s (%.2f,%.2f)", strGoto.c_str(),x,y);

                MoveBaseClient ac("move_base", true);
                if(!ac.waitForServer(ros::Duration(5.0)))
                {
                    ROS_INFO("The move_base action server is no running. action abort...");
                }
                else
                {
                    move_base_msgs::MoveBaseGoal goal;
                    goal.target_pose.header.frame_id = "map";
                    goal.target_pose.header.stamp = ros::Time::now();
                    goal.target_pose.pose = srvName.response.pose;
                    ac.sendGoal(goal);
                    ac.waitForResult();
                    if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
                    {
                        ROS_INFO("Arrived at %s!",strGoto.c_str());
                        Speak("???????????????????????????????????????");
                        nState = STATE_PASS;
                        nDelay = 0;
                    }
                    else
                    {
                        ROS_INFO("Failed to get to %s ...",strGoto.c_str() );
                        Speak("?????????????????????????????????");
                        nState = STATE_ASK;
                    }
                }
                
            }
            else
            {
                ROS_ERROR("Failed to call service GetWaypointByName");
                Speak("????????? master ?????????");
                nState = STATE_ASK;
            }
        }

        // 7?????????????????????
        if(nState == STATE_PASS && canRelease)
        {
            if(nDelay == 0)
            {
                bPassDone = false;
                PassSwitch(true);
            }
            nDelay ++;
            if(bPassDone == true)
            {
                PassSwitch(false);
                nIndexWant ++;
                int nNumWant = arObjectWant.size();
                if(nIndexWant < nNumWant)
                {
                    string strSpeak = "???????????????????????????"+arObjectWant[nIndexWant];
                    Speak(strSpeak);
                    sleep(3);   //???????????????????????????????????????
                    nState = STATE_GOTO;
                }
                else
                {
                    Speak("????????????????????????????????????????????????");
                    nState = STATE_ASK;
                }
                canRelease = false;
            }
        }
        
        ros::spinOnce();
        r.sleep();
    }

    return 0;
}