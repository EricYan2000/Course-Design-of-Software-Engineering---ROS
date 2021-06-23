# -*- coding: utf-8 -*-
import subprocess
import os
import signal
from datetime import datetime

dt=datetime.now()


class Nav():
    def __init__(self):
        self.process = 0
        self.log = 0


    def link_log(self, log):
        self.log = log


    def startnav(self):
        self.process = subprocess.Popen("roslaunch wed_02_legacy wed_02_nav.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已开启导航\n'))


    def endnav(self):
        if self.process != 0:
            try:
                os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
            except Exception as e:
                print(e)
                print("NAV KILL FAILED")
                self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 关闭导航失败！\n'))
                return

        self.process = 0
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已关闭导航\n'))
