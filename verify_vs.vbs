Option Explicit

Sub includeFile(fSpec)
    With CreateObject("Scripting.FileSystemObject")
       executeGlobal .openTextFile(fSpec).readAll()
    End With
End Sub

Function GetScriptDir()
	dim fso ,scriptpath
	Set fso = CreateObject("Scripting.FileSystemObject") 
	GetScriptDir=fso.GetParentFolderName(Wscript.ScriptFullName)
End Function

call includeFile( GetScriptDir() & "\reg_op.vbs")
call includeFile( GetScriptDir() & "\vs_find.vbs")
call includeFile( GetScriptDir() & "\base_func.vbs")


' now we should get the verfiy


dim vsver,isvalid,re,akeys,curkey,result,b,num,rmkey

do While True
	vsver=IsInstallVisualStudio(10.0,"SOFTWARE\Microsoft\VisualStudio")
	If IsEmpty(vsver) Then
		exit do
	End If

	isvalid=VsVerifyCL(vsver)
	if isvalid Then
		exit do
	End If

	' now we enumerate all teh subkeys with 
	akeys=GetRegSubkeys(HKEY_CURRENT_USER,"SOFTWARE\Microsoft\VisualStudio")
	set re = new regexp
	re.IgnoreCase = True
	re.Pattern = "^" & vsver

	for each curkey in akeys
		set result = re.Execute(curkey)
		num = 0
		for each b in result
			num = num + 1
		Next

		if num > 0 Then
			rmkey = "SOFTWARE\Microsoft\VisualStudio\" & curkey
			wscript.stdout.writeline("Find in SOFTWARE\Microsoft\VisualStudio subkey (" &  curkey &")")
			call DeleteRegSubkeys(HKEY_CURRENT_USER,rmkey)
		End If
	Next
Loop
