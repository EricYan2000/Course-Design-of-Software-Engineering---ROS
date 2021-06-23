# -*- coding: utf-8 -*-
import subprocess
import os
import signal
from datetime import datetime

dt = datetime.now()


class Topic():
    def __init__(self):
        self.process = 0
        self.log = 0

    def link_log(self, log):
        self.log = log

    def getTopic(self):
        p = subprocess.Popen("rostopic list", stdin=subprocess.PIPE,
         stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
        p.wait()
        s = p.stdout.readlines()[1:]
        s = [sub.decode('utf-8') for sub in s]
        # s = subprocess.Popen(['ls','-al'], stdout=subprocess.PIPE).communicate()
        # print(type(s))
        # print(s)
        return s