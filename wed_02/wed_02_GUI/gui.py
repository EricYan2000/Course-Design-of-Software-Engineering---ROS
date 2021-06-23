# -*- coding: utf-8 -*-
import tkinter
from os import name
import tkinter as tk
import tkinter.messagebox
from tkinter import *
import time
from typing import NoReturn
from tkinter import ttk
import numpy as np
import PIL.Image
import threading
from datetime import datetime
from topic import *
from map import *
from cruAgent import *
from inputVoice import *
from grab import *
from joyStick import *
from core import *
from mapPoint import *
from nav import *
# import tkinter.messagebox
dt=datetime.now() #创建一个datetime类对象
log = 0
vlog=0
root = 0


# 控制界面大小的常量
xlength = 960
ylength = 540
fontbig = 15
fontsmall = 9

joybtn = 0
navbtn = 0
voicebtn = 0
grabbtn = 0
mapbtn=0
crubtn=0
mappointbtn=0
m = Map()           # 建图
cu = CruAgent()     # 巡航
i = InputVoice()    # 语音输入
nav = Nav()   # 导航
g = Grab()          # 抓取
j = JoyStick()      # 手柄控制
c = Core()
t = Topic()
point = MapPoint()  # 地图标点
#多线程记录时间,一定时间杀死程序

def killWindow(root):
    root.destroy()
    c.coreDown()

def build_map_gui(root):
    build_map()
    global mapbtn
    mapbtn['text'] = '建图中'
    mapbtn.config(state='disabled')
    subroot = Toplevel()
    subroot.title('建图操作提示')
    subroot.geometry('270x'+str(ylength))
    subroot.resizable(False, False)
    lb1 = Label(subroot, font=('Noto Sans CJK SC', fontbig),
                fg='#000000', text='-----您可以操纵手柄控制机器人移动：-----')
    lb1.pack()
  
    def close():
        # 关闭导航
        stop_build_map()
        subroot.destroy()
        mapbtn['text'] = '建图'
        mapbtn.config(state='normal')

    btn = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                               fontbig), fg='#000000', text='关闭', command=close)
    btn['background'] = '#9fa4c3'
    btn2 = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                                fontbig), fg='#000000', text='保存', command=save_map)
    btn2['background'] = '#9fa4c3'
    btn.place(relx=0.60, rely=0.110, relwidth=0.18, relheight=0.08)
    btn2.place(relx=0.20, rely=0.110, relwidth=0.18, relheight=0.08)
    # lb.place(relx=0.36, rely=0.425, relwidth=0.18, relheight=0.1)
    # im_root = get_image('signin.png', 450, 200)
    # canvas_root.create_image(220,100, image=im_root)
    subroot.protocol('WM_DELETE_WINDOW', close)
    subroot.mainloop()
    
# 建图模块
def build_map():
    m.buildMap()

def save_map():
    m.saveMap()

def stop_build_map():
    m.endBuild()
    pass

def createPoint():
    cu.createwaypoint()

# 巡航模块
def cruise_gui():
    global crubtn
    cu.openrviz()
    crubtn['text'] = '巡航中'
    crubtn.config(state='disabled')
    subroot = Toplevel()
    subroot.title('巡航操作提示')
    subroot.geometry('270x'+str(ylength))
    subroot.resizable(False, False)
    lb1 = Label(subroot, font=('Noto Sans CJK SC', fontbig),
                fg='#000000', text='-----您可以按照提示定点巡航：-----')
    lb1.pack()

    def close():
        # 关闭导航
        cu.end()
        subroot.destroy()
        crubtn['text'] = '定点巡航'
        crubtn.config(state='normal')
    
    # tt = ''
    # if cu.checkpointexist():
    #     tt = '更改巡航点'
    # else:
    #     tt = '新建巡航点'
    # btn1 = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
    #                                                 fontbig), fg='#000000', text=tt, command=createPoint)
    # btn1['background'] = '#9fa4c3'
    # btn1.place(relx=0.20, rely=0.110, relwidth=0.58, relheight=0.08)

    def cruise():
        #if cu.checkpointexist():
        #    #新建一个页面让用户选择是否更改巡航点
        cu.startcur()
        #else:
        #    subsubroot = Toplevel()
        #    subsubroot.title('巡航操作提示')
        #    subsubroot.geometry('270x150')
        #    subsubroot.resizable(False, False)
        #    sublb1 = Label(subsubroot, font=('Noto Sans CJK SC', fontbig),
        #        fg='#000000', text='-----请新建巡航点：-----')
        #    sublb1.pack()
        #    # subsubroot.protocol('WM_DELETE_WINDOW', close)
        #    subsubroot.mainloop()

    btn2 = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                                fontbig), fg='#000000', text='开始巡航', command=cruise)
    btn2['background'] = '#9fa4c3'
    btn2.place(relx=0.20, rely=0.110, relwidth=0.58, relheight=0.08)

    btn = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                               fontbig), fg='#000000', text='关闭', command=close)
    btn['background'] = '#9fa4c3'
    btn.place(relx=0.20, rely=0.310, relwidth=0.58, relheight=0.08)
    subroot.protocol('WM_DELETE_WINDOW', close)
    subroot.mainloop()
    
def finish_cruise():
    #need delete
    pass
def delete_point():
    point.delete_point()

def save_point():
    point.save_point()

def map_point_gui():
    # joy()
    point.openrviz()
    global mappointbtn
    mappointbtn['text'] = '地图标注中'
    mappointbtn.config(state='disabled')
    subroot = Toplevel()
    subroot.title('地图标注操作提示')
    subroot.geometry('270x'+str(ylength))
    subroot.resizable(False, False)
    lb1 = Label(subroot, font=('Noto Sans CJK SC', fontbig),
                fg='#000000', text='-----您可以按照提示进行地图标注-----')
    lb1.pack()
    # lb2 = Label(subroot, font=('Noto Sans CJK SC', fontbig),
    #             fg='#000000', text='-----手柄开启中-----')
    # lb2.pack()
    # lb3 = Label(subroot, font=('Noto Sans CJK SC', fontbig),
    #             fg='#000000', text='1.您可以操纵手柄在场地中运动\n2.在到达某一地点后\n3.语音告知机器人该地点位置\n即可完成标注')
    # lb3.pack()

    def close():
        # 关闭导航
        # stop_joy()
        point.end()
        subroot.destroy()
        mappointbtn['text'] = '地图标注'
        mappointbtn.config(state='normal')

    btn = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                                fontbig), fg='#000000', text='保存标点', command=save_point)
    btn['background'] = '#9fa4c3'
    btn.place(relx=0.200, rely=0.110, relwidth=0.58, relheight=0.08)
    btn2 = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                                fontbig), fg='#000000', text='删除标点', command=delete_point)
    btn2['background'] = '#9fa4c3'
    btn2.place(relx=0.20, rely=0.310, relwidth=0.58, relheight=0.08)

    btn = Button(subroot, relief='flat', font=('Noto Sans CJK SC',
                                               fontbig), fg='#000000', text='关闭', command=close)
    btn['background'] = '#9fa4c3'
    btn.place(relx=0.20, rely=0.510, relwidth=0.58, relheight=0.08)
    subroot.protocol('WM_DELETE_WINDOW', close)
    
    subroot.protocol('WM_DELETE_WINDOW', close)
    subroot.mainloop()

def map_point():
    pass

def resource_control():
    usr_control()

def getTopic():
    return t.getTopic()

def process_gui():
    win_process = Tk()
    win_process.title("进程管理")
    win_process.geometry('300x400')

    text = Text(win_process, width=300, height=400, font=('Noto Sans CJK', fontbig))
    scroll = Scrollbar(win_process)
    # 放到窗口的右侧, 填充Y竖直方向
    scroll.pack(side=RIGHT, fill=Y)
    # 两个控件关联
    scroll.config(command=text.yview)
    text.config(yscrollcommand=scroll.set)
    text.pack()
    str1 = getTopic()
    text.insert(INSERT, str1)

    win_process.mainloop()

def usr_control():
    arr = np.load("./database.npy")  # 读取数据库
    for i in arr:
        print(i[0], i[1], i[2])
        
    #     np.save("./database.npy", arr)  # 保存数据库
    win = Tk()
    win.title("用户管理")
    win.geometry('600x400')

    nameLabel = Label(win, text='用户名：').grid(row=0, column=0)
    nameStringvar = StringVar()
    nameEntry = Entry(
        win, width=20, textvariable=nameStringvar)
    nameEntry.grid(row=0, column=1)

    passLabel = Label(win, text='密码：').grid(row=1, column=0)
    passStringvar = StringVar()
    passEntry = Entry(
        win, width=20, textvariable=passStringvar)
    passEntry.grid(row=1, column=1)

    roleLabel = Label(win, text='身份：').grid(row=2, column=0)
    roleStringvar = StringVar()
    roleEntry = Entry(
        win, width=20, textvariable=roleStringvar)
    roleEntry.grid(row=2, column=1)

    def select():
        arr = np.load("./database.npy")
        xh = nameEntry.get()
        print("xh is"+xh)
        list_re=[]
        if len(xh) == 0:
            list_re=arr
        else:
            for i in arr:
                print(i[0], i[1], i[2])
                if i[0]==xh:
                    list_re.append(i)
        x = tree.get_children()
        for item in x:
            tree.delete(item)
        for i in range(len(list_re)):
            tree.insert('', i, text=i, values=(list_re[i][0], list_re[i][1], list_re[i][2]))
    selectButton = Button(win, text='查询', width=10,
                            height=1, command=select).grid(row=3, column=0, pady=5)

    def insert():

        temp = [nameEntry.get(), passEntry.get(), roleEntry.get()]
        tree.insert('', 'end', value=temp)
        tk.messagebox.showinfo('提示', nameEntry.get() + '信息插入成功！')
        arr = np.load("./database.npy")
        arr = np.append(arr, [temp], axis=0)  # 加入数据库,注意不能随意添加管理员用户,因此新注册用户只能是普通用户
        np.save("./database.npy",arr)#保存数据库
    insertButton = Button(win, text='插入', width=10,
                            height=1, command=insert).grid(row=3, column=1, pady=5)

    def delete():
        arr = np.load("./database.npy")
        xh = nameEntry.get()
        cnt = 0
        for i in arr:
            print(i[0], i[1], i[2])
            if i[0] == xh:
                break
            cnt = cnt + 1
        
        arr = np.delete(arr,cnt,0)
        np.save("./database.npy", arr)  # 保存数据库
        tk.messagebox.showinfo('提示', xh + '信息删除成功！')
        
    deleteButton = Button(win, text='删除', width=10,
                            height=1, command=delete).grid(row=3, column=2, pady=5)

    def clear():
        nameEntry.delete(0,END)
        passEntry.delete(0, END)
        roleEntry.delete(0, END)

    clearButton = Button(win, text='清空', width=10, height=1,
                            command=clear).grid(row=3, column=3, pady=5)

    tree = ttk.Treeview(win)
    tree['column'] = ('姓名', '密码', '角色')
    tree.column('姓名', width=100)
    tree.column('密码', width=100)
    tree.column('角色', width=100)
    tree.heading('姓名', text='姓名')
    tree.heading('密码', text='密码')
    tree.heading('角色', text='角色')
    tree.grid(row=4, column=0, columnspan=10)
      
    btn_s = Button(win, 
                 width=10,text='进程管理',command=process_gui)
    btn_s.place(relx=0.36, rely=0.90)

    win.mainloop()

# 语音输入模块
def voice():
    i.startVoice()

def stop_voice():
    i.endVoice()

def voiceGui():
    voice()
    global voicebtn
    voicebtn['text'] = '语音开启中'
    voicebtn.config(state = 'disabled')
    root = Toplevel()
    root.title('语音指令')
    root.geometry('270x'+str(ylength))
    root.resizable(False, False)
    # canvas_root = tkinter.Canvas(root, width=450, height=450)
    lb1 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='-----您可以说：-----')
    lb1.pack()
    lb2 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='· 向左/右/前/后走')
    lb2.pack()
    lb3 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='· 停止跟随')
    lb3.pack()
    lb4 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='   ')
    lb4.pack()

    def close():
        # 关闭语音控制
        stop_voice()
        root.destroy()
        voicebtn['text']='输入语音指令'
        voicebtn.config(state = 'normal')

    btn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='关闭', command=close)
    btn['background']='#9fa4c3'
    btn.pack()
    # lb.place(relx=0.36, rely=0.425, relwidth=0.18, relheight=0.1)
    # im_root = get_image('signin.png', 450, 200)
    # canvas_root.create_image(220,100, image=im_root)
    root.protocol('WM_DELETE_WINDOW', close)
    root.mainloop()

# 导航模块
def navigation():
    nav.startnav()
    pass
    
def stop_navigation():
    nav.endnav()
    pass

def waypoints(objecttype):
    # nav.goto(objecttype) # nav类里要加这个方法
    print(objecttype)

def navigationGui():
    navigation()
    global navbtn
    navbtn['text']='导航中'
    navbtn.config(state = 'disabled') 
    root = Toplevel()
    root.title('导航操作提示')
    root.geometry('360x'+str(ylength))
    root.resizable(False, False)
    lb1 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='----------您可以：----------')
    lb1.pack()
    '''lb2 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='选择已有的目标点')
    lb2.pack()

    com = ttk.Combobox(root, width=15, font=('Noto Sans CJK SC', fontbig))     # #创建下拉菜单
    com.pack()
    com["value"] = ("水", "文具盒", "充电头")    # #给下拉菜单设定值
    # com.current(0)    # #设定下拉菜单的默认值为第3个，即山东
    global objectType

    def boxfunc(event):
        global objectType
        objectType = com.get()
    
    com.bind("<<ComboboxSelected>>", boxfunc)     # #给下拉菜单绑定事件

    b1 = Button(root, text='确定', relief='flat', font=('Noto Sans CJK SC', fontbig), fg='#ffffff', command=lambda :waypoints(objectType))
    # b1.place(relx=0.54, rely=0.66, relwidth=0.3, relheight=0.09)
    b1['background']='#9fa4c3'
    b1.pack()

    lb3 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='----------您也可以：----------')
    lb3.pack()'''
    lb4 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='手动选择目的位置，请依次操作：')
    lb4.pack()
    lb5 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='点击"2D Pos Estimate"')
    lb5.pack()
    lb6 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='点击"2D Nav Goal"')
    lb6.pack()
    def close():
        # 关闭导航
        stop_navigation()
        root.destroy()
        navbtn['text']='开始导航'
        navbtn.config(state = 'normal')

    btn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='关闭', command=close)
    btn['background']='#9fa4c3'
    btn.pack()
    # lb.place(relx=0.36, rely=0.425, relwidth=0.18, relheight=0.1)
    # im_root = get_image('signin.png', 450, 200)
    # canvas_root.create_image(220,100, image=im_root)
    root.protocol('WM_DELETE_WINDOW', close)
    root.mainloop()

# 抓取模块
def grab():
    # 模块功能为导航+抓取
    # nav.startnav()
    g.startgrab()

def stop_grab():
    g.endgrab()

def grabGui():
    grab()
    global grabbtn
    grabbtn['text']='导航+抓取中'
    grabbtn.config(state = 'disabled')
    root=Toplevel()
    root.title('导航操作提示')
    root.geometry('360x'+str(ylength))
    root.resizable(False, False)
    lb1 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='----------您可以：----------')
    lb1.pack()
    lb2 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='选择已有的目标点')
    lb2.pack()
    lb3 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='机器人会自动去为您取回目标物体')
    lb3.pack()
    lb4 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='----------您也可以：----------')
    lb4.pack()
    lb5 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='手动选择目的位置，请依次操作：')
    lb5.pack()
    lb6 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='点击"2D Pos Estimate"')
    lb6.pack()
    lb7 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='点击"2D Nav Goal"')
    lb7.pack()
    def close():
        stop_grab()
        root.destroy()
        grabbtn['text']='开始抓取'
        grabbtn.config(state = 'normal')
    
    btn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='关闭', command=close)
    btn['background']='#9fa4c3'
    btn.pack()
    # lb.place(relx=0.36, rely=0.425, relwidth=0.18, relheight=0.1)
    # im_root = get_image('signin.png', 450, 200)
    # canvas_root.create_image(220,100, image=im_root)
    root.protocol('WM_DELETE_WINDOW', close)
    root.mainloop()

# 手柄控制模块
def joy():
    j.startjoy()
    pass

def stop_joy():
    j.endjoy()
    pass

def joyGui():
    joy()
    global joybtn
    joybtn['text']='手柄开启中'
    joybtn.config(state = 'disabled')
    root = Toplevel()
    root.title('手柄操作提示')
    root.geometry('590x450')
    root.resizable(False, False)
    canvas_root = tkinter.Canvas(root, width=590, height=450)
    im_root = get_image('pic/joystick.png', 590, 450)
    canvas_root.create_image(295, 225, image=im_root)
    canvas_root.pack()

    lb1 = Label(root, font=('Noto Sans CJK SC', fontbig), fg='#000000', text='-----手柄键位图：-----')
    lb1.pack()
    
    def close():
        # 关闭手柄控制
        stop_joy()
        root.destroy()
        joybtn['text']='开启手柄'
        joybtn.config(state = 'normal')

    btn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='关闭', command=close)
    btn.place(relx=0.45, rely=0.8)
    btn['background']='#9fa4c3'
    
    root.protocol('WM_DELETE_WINDOW', close)
    root.mainloop()

'''
以下是GUI
'''
def get_image(filename, width, height):
    im = PIL.Image.open(filename).resize((width,height))
    return PIL.ImageTk.PhotoImage(im)

def administrator(): #开发人员用户界面：建图 巡航 用户管理 任务管理
    finishBM = False
    global root
    root= Tk()
    root.title('ROS开发人员控制程序')
    root.geometry(str(xlength)+'x'+str(ylength)) # 这里的乘号不是 * ，而是小写英文字母 x
    root.resizable(False, False)
    
    canvas_root = tkinter.Canvas(root, width=xlength, height=ylength)
    im_root = get_image('pic/admin.png', xlength, ylength)
    canvas_root.create_image(xlength/2, ylength/2, image=im_root)
    canvas_root.pack()
    global mapbtn
    mapbtn = Button(root, relief='flat', font=('Noto Sans CJK SC', fontbig),
                  fg='#ffffff', text='建立地图', command=lambda: build_map_gui(root))
    mapbtn.place(relx=0.36, rely=0.200, relwidth=0.18, relheight=0.1)
    mapbtn['background'] = '#9fa4c3'  # 按钮颜色

    global mappointbtn
    mappointbtn = Button(root, relief='flat', font=('Noto Sans CJK SC',
                                             fontbig), fg='#ffffff', text='地图标注', command=map_point_gui)
    mappointbtn.place(relx=0.36, rely=0.425, relwidth=0.18, relheight=0.1)
    mappointbtn['background'] = '#9fa4c3'

    global crubtn
    crubtn = Button(root, relief='flat', font=('Noto Sans CJK SC',
                                             fontbig), fg='#ffffff', text='定点巡航', command=cruise_gui)
    crubtn.place(relx=0.36, rely=0.650, relwidth=0.18, relheight=0.1)
    crubtn['background'] = '#9fa4c3'

    btn4 = Button(root, relief='flat', font=('Noto Sans CJK SC',
                                             fontbig), fg='#ffffff', text='资源管理', command=resource_control)
    btn4.place(relx=0.08, rely=0.200, relwidth=0.18, relheight=0.1)
    btn4['background'] = '#9fa4c3'

    retbtn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='<', command=lambda :ret(root))
    retbtn.place(relx=0.02, rely=0.02, relwidth=0.05, relheight=0.08)
    retbtn['background']='#9fa4c3'

    # global mo
    # mo = move(0,0,0,0)

    # 流动 log
    global alog
    alog = Text(root, relief='flat', font=('Noto Sans CJK SC',
                                          fontbig), fg='#000000', background='#ffffff')
    alog.place(relx=0.07, rely=0.400, relwidth=0.20, relheight=0.4)
    
    cu.link_log(alog)
    point.link_log(alog)
    m.link_log(alog)
    j.link_log(alog)

    root.protocol('WM_DELETE_WINDOW', lambda :killWindow(root))
    root.mainloop()

def user(): #普通用户界面：语音输入 导航 抓取 手柄控制
    global root
    root= Tk()
    root.title('ROS用户控制程序')
    root.geometry(str(xlength)+'x'+str(ylength))
    root.resizable(False, False)
    
    canvas_root = tkinter.Canvas(root, width=xlength, height=ylength)
    im_root = get_image('pic/user.png', xlength, ylength)
    canvas_root.create_image(xlength/2, ylength/2, image=im_root)
    canvas_root.pack()
    
    global voicebtn
    voicebtn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='输入语音指令', command=voiceGui)
    voicebtn.place(relx=0.36, rely=0.200, relwidth=0.18, relheight=0.1)
    voicebtn['background']='#9fa4c3'

    global navbtn
    navbtn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='开始导航', command=navigationGui)
    navbtn.place(relx=0.36, rely=0.425, relwidth=0.18, relheight=0.1)
    navbtn['background']='#9fa4c3'
    
    global grabbtn
    grabbtn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='物体抓取', command=grabGui)
    grabbtn.place(relx=0.36, rely=0.650, relwidth=0.18, relheight=0.1)
    grabbtn['background']='#9fa4c3'

    global joybtn
    joybtn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='开启手柄', command=joyGui)
    joybtn.place(relx=0.08, rely=0.200, relwidth=0.18, relheight=0.1)
    joybtn['background']='#9fa4c3'

    retbtn = Button(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', text='<', command=lambda :ret(root))
    retbtn.place(relx=0.02, rely=0.02, relwidth=0.05, relheight=0.08)
    retbtn['background']='#9fa4c3'

    # 流动 log
    global log
    log = Text(root, relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#000000', background='#ffffff')
    log.place(relx=0.07, rely=0.400, relwidth=0.20, relheight=0.5)

    nav.link_log(log)
    j.link_log(log)
    g.link_log(log)
    i.link_log(log)

    root.protocol('WM_DELETE_WINDOW', lambda :killWindow(root))
    root.mainloop()

def ret(top):
    top.destroy()
    c.coreDown()
    loginGui()

def login(E1, E2, top):
    username = E1.get()
    password = E2.get()
    arr = np.load("./database.npy")#读取数据库
    matched  = False
    for i in arr:
        print(i[0],i[1],i[2])
        if i[0] == username:#匹配用户名
            if i[1] == password:#匹配密码
                matched = True
                if i[2] == '1':  # 是管理员
                    top.destroy()
                    c.upCore()
                    administrator()
                    break
                else: #普通用户
                    top.destroy()
                    c.upCore()
                    user()
                    break
    if matched == False: #未匹配到用户名或密码
        root = Toplevel()
        root.title('登录错误')
        root.geometry('450x200')
        root.resizable(False, False)
        canvas_root = tkinter.Canvas(root, width=450, height=200)
        im_root = get_image('login_error.png', 450, 200)
        canvas_root.create_image(220,100, image=im_root)
        canvas_root.pack()
        root.mainloop()

def signin(E1, E2, top):
    username = E1.get()
    password = E2.get()
	
    if len(username)==0 or len(password)==0:	#用户名或密码都不能为空
        root = Toplevel()
        root.title('输入错误')
        root.geometry('450x200')
        root.resizable(False, False)
        canvas_root = tkinter.Canvas(root, width=450, height=200)
        im_root = get_image('empty_error.png', 450, 200)
        canvas_root.create_image(220,100, image=im_root)
        canvas_root.pack()
        root.mainloop()
	
    arr = np.load("./database.npy")#读取数据库
    arr = np.append(arr,[[username,password,'2']],axis=0)#加入数据库,注意不能随意添加管理员用户,因此新注册用户只能是普通用户
    np.save("./database.npy",arr)#保存数据库

    top.destroy()
    loginGui()

def modifyPW(E1, E2, E3, top):
    username = E1.get()
    oldpassword = E2.get()
    newpassword = E3.get()
    arr = np.load("./database.npy")#读取数据库
    matched  = False
    for i in arr:
        print(i[0],i[1],i[2])
        if i[0] == username:#匹配用户名
            if i[1] == oldpassword:#匹配密码
                matched = True
                i[1] = newpassword
                np.save("./database.npy",arr)#保存数据库
    if matched == False: #未匹配到用户名或密码
        root = Toplevel()
        root.title('用户名不存在或密码错误')
        root.geometry('450x200')
        root.resizable(False, False)
        canvas_root = tkinter.Canvas(root, width=450, height=200)
        im_root = get_image('login_error.png', 450, 200)
        canvas_root.create_image(220,100, image=im_root)
        canvas_root.pack()
        root.mainloop()
    top.destroy()
    loginGui()

def signinGui(top): #注册界面
    top.destroy()
    top = Tk()
    top.geometry(str(xlength)+'x'+str(ylength)) 
    top.title('ROS注册')
    top.resizable(False, False)

    canvas_top = tkinter.Canvas(top, width=xlength, height=ylength)
    im_top = get_image('pic/login.png', xlength, ylength)
    canvas_top.create_image(xlength/2, ylength/2, image=im_top)
    canvas_top.pack()

    E1 = Entry(top, bd=5, font=('Noto Sans CJK SC', fontbig)) #读入用户名
    E1.place(relx=0.54, rely=0.40,  relwidth=0.3, relheight=0.09)

    password = StringVar()  #读入密码并且不显示密码
    E2 = Entry(top, bd=5,textvariable=password,show='*',font=(fontbig))
    E2.place(relx=0.54, rely=0.52, relwidth=0.3, relheight=0.09)

    #注册
    b1 = Button(top, text='注   册',relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', command=lambda :signin(E1, E2, top))
    b1.place(relx=0.54, rely=0.66, relwidth=0.3, relheight=0.09)
    b1['background']='#6c7788'
    b1['activebackground']='#7f8da3'
    top.mainloop()

def modifyPWGui(top):
    top.destroy()
    top = Tk()
    top.geometry(str(xlength)+'x'+str(ylength)) 
    top.title('ROS修改密码')
    top.resizable(False, False)

    canvas_top = tkinter.Canvas(top, width=xlength, height=ylength)
    im_top = get_image('pic/login.png', xlength, ylength)
    canvas_top.create_image(xlength/2, ylength/2, image=im_top)
    canvas_top.pack()

    L1 = Label(top, text='用户名：', font=('Noto Sans CJK SC', fontbig), fg='#ffffff', background='#a8a8b4')
    L1.place(relx=0.54, rely=0.40,  relwidth=0.1, relheight=0.07)

    E1 = Entry(top, bd=5, font=('Noto Sans CJK SC', fontbig)) #读入用户名
    E1.place(relx=0.64, rely=0.40,  relwidth=0.2, relheight=0.07)

    L2 = Label(top, text='原密码：', font=('Noto Sans CJK SC', fontbig), fg='#ffffff', background='#a8a8b4')
    L2.place(relx=0.54, rely=0.48,  relwidth=0.1, relheight=0.07)

    oldpassword = StringVar()  #读入密码并且不显示密码
    E2 = Entry(top, bd=5,textvariable=oldpassword,show='*',font=(fontbig))
    E2.place(relx=0.64, rely=0.48, relwidth=0.2, relheight=0.07)

    L3 = Label(top, text='新密码：', font=('Noto Sans CJK SC', fontbig), fg='#ffffff', background='#a8a8b4')
    L3.place(relx=0.54, rely=0.56,  relwidth=0.1, relheight=0.07)

    newpassword = StringVar()  #读入密码并且不显示密码
    E3 = Entry(top, bd=5,textvariable=newpassword,show='*',font=(fontbig))
    E3.place(relx=0.64, rely=0.56, relwidth=0.2, relheight=0.07)

    #注册
    b1 = Button(top, text='确认修改',relief='flat',font=('Noto Sans CJK SC', fontbig), fg='#ffffff', command=lambda :modifyPW(E1, E2, E3, top))
    b1.place(relx=0.54, rely=0.66, relwidth=0.3, relheight=0.09)
    b1['background']='#6c7788'
    b1['activebackground']='#7f8da3'
    top.mainloop()

def loginGui(): #登录界面
    top = Tk()
    top.geometry(str(xlength)+'x'+str(ylength)) 
    top.title('ROS登录')
    top.resizable(False, False)

    canvas_top = tkinter.Canvas(top, width=xlength, height=ylength)
    im_top = get_image('pic/login.png', xlength, ylength)
    canvas_top.create_image(xlength/2, ylength/2, image=im_top)
    canvas_top.pack()

    E1 = Entry(top, bd=5, font=('Noto Sans CJK SC', fontbig)) #读入用户名
    E1.place(relx=0.54, rely=0.40, relwidth=0.3, relheight=0.09)

    password = StringVar()  #读入密码并且不显示密码
    E2 = Entry(top, bd=5, show='*', font=(fontbig), textvariable=password)
    E2.place(relx=0.54, rely=0.52, relwidth=0.3, relheight=0.09)

    #登录
    b1 = Button(top, text='登   录', relief='flat', font=('Noto Sans CJK SC', fontbig), fg='#ffffff', command=lambda :login(E1, E2, top))
    b1.place(relx=0.54, rely=0.66, relwidth=0.3, relheight=0.09)
    b1['background']='#6c7788'
    b1['activebackground']='#7f8da3'

    #去注册
    b2 = Button(top, text='去注册', relief='flat', font=('Noto Sans CJK SC', fontsmall, "underline"), fg='#040044', command=lambda :signinGui(top))
    b2.place(relx=0.78, rely=0.76, relwidth=0.06, relheight=0.05)
    b2['background']='#a8a8b4'

    #修改密码
    b3 = Button(top, text='修改密码', relief='flat', font=('Noto Sans CJK SC', fontsmall, "underline"), fg='#040044', command=lambda :modifyPWGui(top))
    b3.place(relx=0.54, rely=0.76, relwidth=0.07, relheight=0.05)
    b3['background']='#a8a8b4'
    top.mainloop()

loginGui()
# user()
# administrator()
