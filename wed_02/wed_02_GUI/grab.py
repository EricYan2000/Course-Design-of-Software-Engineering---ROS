# -*- coding: utf-8 -*-
import subprocess
import os
import signal
from datetime import datetime

dt=datetime.now()


class Grab():
    def __init__(self):
        self.process = 0
        self.log = 0


    def link_log(self, log):
        self.log = log


    def startgrab(self):
        # self.process = subprocess.Popen("roslaunch wed_02_legacy wed_02_shopping.launch",
        self.process = subprocess.Popen("roslaunch wed_02_wpb_home_apps home.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已开启物体抓取\n'))


    def endgrab(self):
        if self.process != 0:
            try:
                os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
            except Exception as e:
                print(e)
                print("GRAB KILL FAILED")
                self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 关闭物体抓取失败！\n'))
                return

        self.process = 0
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已关闭物体抓取\n'))
