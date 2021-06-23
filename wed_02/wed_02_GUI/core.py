import subprocess
import os
import signal

class Core():
    def __init__(self):
        self.process = 0

    def upCore(self):
        self.process = subprocess.Popen("roslaunch wed_02_legacy wed_02_core.launch",
            stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True, preexec_fn=os.setsid)

    def coreDown(self):
        if self.process != 0:
            try:
                os.killpg(os.getpgid(self.process.pid), signal.SIGINT)
            except Exception as e:
                print(e)
                print("CORE KILL FAILED")

        self.process = 0