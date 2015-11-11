

Function RunCommand(cmd)
	dim objsh,res
	set objsh = wscript.CreateObject("WScript.Shell")
	res = objsh.Run(cmd,1,true)
	if res <> 0 Then
		WScript.Stderr.WriteLine("run command ("& cmd &") error ("& res &")")
		WScript.Quit(res)
	End if
End Function

Function GetCwd()
	dim fso
	set fso = WScript.CreateObject("Scripting.FileSystemObject")
	GetCwd = fso.GetAbsolutePathName(".")
	set fso = Nothing
End Function

Function GetAbsPath(path)
	dim fso
	set fso = WScript.CreateObject("Scripting.FileSystemObject")
	GetAbsPath= fso.GetAbsolutePathName(path)
	set fso = Nothing
End Function

Function GetWholePath(fname)
	dim fso
	set fso = WScript.CreateObject("Scripting.FileSystemObject")
	GetWholePath = fso.GetAbsolutePathName(fname)
	set fso = Nothing
End Function

Function CheckVariable(varname)
	dim wsh,val,key
	set wsh = WScript.CreateObject("WScript.Shell")
	key = "%" & varname & "%"
	val = wsh.ExpandEnvironmentStrings(key)
	if val  = key Then
		wscript.stderr.write("variable (" + varname + ") not defined" & chr(13) & chr(10)) 
		wscript.quit(4)
	end if
End Function

Function GetEnv(varname)
	dim wsh,val,key
	set wsh = WScript.CreateObject("WScript.Shell")
	key = "%" & varname & "%"
	val = wsh.ExpandEnvironmentStrings(key)
	if val  = key Then
		GetEnv=null
	else
		GetEnv=val
	end if
End Function

Function SetEnv(key,value)
	dim objShell,colprocenvars
	Set objShell = WScript.CreateObject("WScript.Shell")
	Set colprocenvars = objShell.Environment("Process")	
	colprocenvars(key) = value
End Function


Function Chdir(path)
	Dim oShell : Set oShell = CreateObject("WScript.Shell")
	oShell.CurrentDirectory = path
	set oShell=Nothing
End Function



Function FileExists(pathf)
	dim fso
	set fso = CreateObject("Scripting.FileSystemObject")
	if (fso.FileExists(pathf)) Then
		FileExists=1
	Else
		FileExists=0
	End If	
End Function

