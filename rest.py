# Usage：定时提醒休息
#     添加计划任务，在window用户登录时执行 "pythonw.exe rest.py"
#     按需要修改rest_min来调整休息间隔，默认45分钟

import tkinter, tkinter.messagebox
import time

root = tkinter.Tk()
root.withdraw() # 使message只有一个弹窗
root.wm_attributes('-topmost', 1) # 窗口置顶（强制关注）

# 每rest_min分钟提醒休息
rest_min = 45

print("休息提醒启动：将会每%d分钟提醒一次" % (rest_min))

while True:
    time.sleep(rest_min * 10)
    ret = tkinter.messagebox.showinfo("提醒", "休息时间到！")