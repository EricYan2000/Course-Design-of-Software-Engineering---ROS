# -*- coding: utf-8 -*-
import os
import signal
import PIL.Image
import PIL.ImageTk
import time
import matplotlib.pyplot as plt
import pylab
import subprocess
import numpy as np
from datetime import datetime
dt=datetime.now()

class Map:
    def __init__(self):
        self.process = 0
        self.joystick = 0
        self.saved = 0
        self.built = 0
        self.log = 0


    def link_log(self, log):
        self.log = log


    def buildMap(self):
        self.built = 1
        self.process = subprocess.Popen("roslaunch wed_02_legacy wed_02_gmapping.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.joystick = subprocess.Popen("roslaunch wed_02_legacy wed_02_js_dev.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已开启建图\n'))
        

    def saveMap(self):
        self.saved = 1
        save_process = subprocess.Popen('rosrun map_server map_saver -f /home/robot/catkin_ws/src/wpb_home/wpb_home_tutorials/maps/map', shell=True)
        save_process.wait()
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已保存所建立地图\n'))

        # os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
        # self.process = 0


    def endBuild(self):
        os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
        os.killpg(os.getpgid(self.joystick.pid), signal.SIGINT)

        self.process = 0
        self.joystick = 0
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已关闭建图\n'))




