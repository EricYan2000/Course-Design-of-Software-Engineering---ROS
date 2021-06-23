# -*- coding: utf-8 -*-
from tkinter.constants import FALSE
from tkinter.constants import TRUE

from numpy.core.numeric import True_
from map import Map
import os
import subprocess
import signal
from datetime import datetime
import shutil

dt=datetime.now()

class CruAgent:
    def __init__(self):
        self.process_rviz = 0
        self.process_curise = 0
        self.log = 0
        self.moved = False
        pass

    # USELESS
    def checkpointexist(self):
        # open the map
        # check if waypoints.xml exist
        if os.path.exists("/home/robot/waypoints.xml"):
            return TRUE
        return FALSE


    def openrviz(self):
        if os.path.exists("/home/robot/waypoints.xml"):
            shutil.move("/home/robot/waypoints.xml", "/home/robot/waypoints.bak")
            self.moved = True

        self.process_curise = subprocess.Popen("roslaunch waterplus_map_tools wpb_home_nav_test.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 正在进行巡航\n'))
        pass

    # USELESS
    def createwaypoint(self):
        #create new way points
        save_process = subprocess.Popen("rosrun waterplus_map_tools wp_saver",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        save_process.wait()
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已保存当前航点\n'))
        pass


    def startcur(self):
        #rosrun waterplus_map_tools wp_nav_test 开始导航
        #start the curise process
        self.process_curise = subprocess.Popen("rosrun waterplus_map_tools wp_nav_test",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 正在进行巡航\n'))
        pass

    def end(self):
        if self.process_curise != 0:
            try:
                os.killpg(os.getpgid(self.process_curise.pid), signal.SIGINT)
            except Exception as e:
                pass
        
        if self.process_rviz != 0:
            try:
                os.killpg(os.getpgid(self.process_rviz.pid), signal.SIGINT)
            except Exception as e:
                pass

        self.process_rviz = 0
        self.process_curise = 0

        if self.moved:
            if os.path.exists("/home/robot/waypoints.bak"):
                shutil.move("/home/robot/waypoints.bak", "/home/robot/waypoints.xml")
            self.moved = True

        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已关闭巡航\n'))
        pass

    def link_log(self, log):
        self.log = log

