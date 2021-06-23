# -*- coding: utf-8 -*-
import subprocess
import os
import signal
from datetime import datetime

dt=datetime.now()


class InputVoice():
    def __init__(self):
        # self.process1 = 0
        # self.process2 = 0
        self.process = 0
        self.log = 0


    def link_log(self, log):
        self.log = log


    def startVoice(self):
        # self.process1 = subprocess.Popen("rosrun wed_02_sr_pkg wed_02_sr_node_cn",
        #  stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        # self.process2 = subprocess.Popen("roslaunch xfyun_waterplus sr_tts_cn.launch",
        #  stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.process2 = subprocess.Popen("roslaunch wed_02_legacy wed_02_shopping.launch",
         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已开启语音输入\n'))


    def endVoice(self):
        # if self.process1 != 0:
        #     try:
        #         os.killpg(os.getpgid(self.process1.pid), signal.SIGINT)
        #     except:
        #         pass
        # self.process1 = 0
        # if self.process2 != 0:
        #     try:
        #         os.killpg(os.getpgid(self.process2.pid), signal.SIGINT)
        #     except:
        #         pass
        # self.process2 = 0
        if self.process != 0:
            try:
                os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
            except:
                pass
        self.process = 0
        self.log.insert("insert", dt.strftime('%y-%m-%d %I:%M:%S %p 已关闭语音输入\n'))