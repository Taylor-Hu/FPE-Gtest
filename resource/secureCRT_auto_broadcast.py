# $language = "python"
# $interface = "1.0"

import sys;
reload(sys);
sys.setdefaultencoding('gbk')

#secureCRT 示例脚本
#根据从串口捕获的字符串来调用ts播放器服务端

import os

#cur_path = os.getcwd()

#编辑ts_ctl.exe所在的文件路径
cur_path = r"C:\Users\zou\Desktop\win7_driver\socket_server_client"

#码流搜索路径
ts_path = "g:\\ts0312\\"

#改成客户端的路径和远程端口
cmd_head = r" " + cur_path + r"\ts_ctl.exe -tcp=127.0.0.1:6800 "

#objFso = CreateObject("Scripting.FileSystemObject")

#if 0 == objFso.FileExists(cur_path + "\ts_ctl.exe"):
#	crt.Dialog.MessageBox("error : file " +  cur_path + r"\ts_ctl.exe is not exist!!" )
#	cur_path = inputbox("please input the path of ts_ctl.exe ","config params")
	
#if 0 == objFso.FileExists(cur_path + "\fpe_capture.exe"):
#	crt.Dialog.MessageBox("error : file " +  cur_path + r"\fpe_capture.exe is not exist!!" )



#修改成caputure的路径
capture_path = r" "+ cur_path + r"\fpe_capture.exe "

os.system(cmd_head + " set_ts_search_path([[" + ts_path + "]])")

crt.Screen.Send ("script:Load auto broadcast ok")

cmd = capture_path + "停止码流播放"  + cmd_head
os.system(cmd)

def Main():
	# Using GetScriptTab() will make this script 'tab safe' in that all of the
	# script's functionality will be carried out on the correct tab. From here
	# on out we'll use the objTab object instead of the crt object.
	objTab = crt.GetScriptTab()
	objTab.Screen.Synchronous = True
	objTab.Screen.IgnoreEscape = True

	while True:
		idx = objTab.Screen.WaitForStrings("PROMPT")

		tail="[/P]"
		#crt.Dialog.MessageBox("xx")

		str = objTab.Screen.ReadString(tail)
		#crt.Dialog.MessageBox(str)
		
		if idx==1:
			cmd = capture_path + "\"" + str + "\""  + cmd_head
		#print cmd
		#crt.Dialog.MessageBox(cmd)
		ret = os.system(cmd)
		if ret==0:
			#crt.Dialog.MessageBox("ok")
			crt.Screen.Send ("script: Auto broadcast ok")
			crt.Sleep(1000)
			crt.Screen.Send ("\n")
			
		if ret > 0:
			#crt.Dialog.MessageBox("fail")
			crt.Screen.Send("script: Auto broadcast fail, please manual play...")

Main()


