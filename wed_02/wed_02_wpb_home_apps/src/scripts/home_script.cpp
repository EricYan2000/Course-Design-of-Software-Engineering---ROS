#include "10_home_script.h"
CHomeScript::CHomeScript()
{
    
}

CHomeScript::~CHomeScript()
{

}

void CHomeScript::Queue()
{
    stAct newAct;

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "科大讯飞语音交互 Demo";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_LISTEN;
    newAct.strTarget = "爱死机";
    newAct.nDuration = 5;
    arAct.push_back(newAct);
    
    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "爱酱时刻跟随在主人身边";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_FOLLOW;
    newAct.fFollowDist = 0.5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_SELECT;
    // newAct.strTarget = "我渴了";
    newAct.strTarget = "";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    /*newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "好的,主人你想来瓶饮料吗";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_LISTEN;
    newAct.strTarget = "是的";
    newAct.nDuration = 5;
    arAct.push_back(newAct);*/

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "好的,这就给您去取";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_ADD_WAYPOINT;
    newAct.strTarget = "follow";
    arAct.push_back(newAct);

    newAct.nAct = ACT_GOTO_DRINK;
    // newAct.strTarget = "kitchen";
    newAct.strTarget = "";
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "我要开始抓取饮料了呢";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_GRAB;
    newAct.strTarget = "";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "我已经取到了呢";
    newAct.nDuration = 2;
    arAct.push_back(newAct);

    newAct.nAct = ACT_GOTO;
    newAct.strTarget = "follow";
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "饮料已取回,现在交给您吗";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_LISTEN;
    newAct.strTarget = "好的";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_PASS;
    newAct.strTarget = "";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_SPEAK;
    newAct.strTarget = "任务完成";
    newAct.nDuration = 5;
    arAct.push_back(newAct);

    newAct.nAct = ACT_FINISH;
    newAct.strTarget = "";
    newAct.nDuration = 5;
    arAct.push_back(newAct);
}