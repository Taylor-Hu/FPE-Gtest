# $language = "python"
# $interface = "1.0"

#secureCRT ʾ���ű�
#���ݴӴ��ڲ�����ַ���������ts�����������

import os
import time

#�ĳɿͻ��˵�·����Զ�̶˿�
cmd_head = r"e:\ts_ctl.exe -tcp=127.0.0.1:6800 "

os.system(cmd_head + " -d")

def Main():
	# Using GetScriptTab() will make this script 'tab safe' in that all of the
	# script's functionality will be carried out on the correct tab. From here
	# on out we'll use the objTab object instead of the crt object.
	objTab = crt.GetScriptTab()
	objTab.Screen.Synchronous = True
	objTab.Screen.IgnoreEscape = True
	
	
	
	
	while True:
		objTab.Screen.WaitForStrings(["get_PcRealTime()"])
		#objTab.Screen.Send()
		m = time.strftime('%H:%M:%S\n',time.localtime(time.time()))
		objTab.Screen.Send(m)
		
		

Main()


