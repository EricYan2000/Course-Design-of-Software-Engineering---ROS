# -*- coding: utf-8 -*-
from tkinter.constants import FALSE
from map import Map
import os
import subprocess
import signal
from datetime import datetime

dt = datetime.now()


class MapPoint:
    def __init__(self):
        self.process = 0
        self.log = 0

    def openrviz(self):
        self.process = subprocess.Popen("roslaunch waterplus_map_tools wpb_home_nav_test.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已开启地图标注\n'))
        pass

    def save_point(self):
        #create new map points
        save_process = subprocess.Popen("rosrun waterplus_map_tools wp_saver",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        save_process.wait()
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已保存当前航点\n'))
        pass

    def delete_point(self):
        #create new map points
        if os.path.exists("/home/robot/waypoints.xml"):
            os.remove("/home/robot/waypoints.xml")
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已删除当前航点\n'))
        pass

    def end(self):
        if self.process != 0:
            try:
                os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
            except Exception as e:
                print(e)
                pass
        pass

    def link_log(self, log):
        self.log = log

    
