
Option Explicit



Function IsInstallVisualStudio(version,pathkey)
	dim re,result,num,akeys,getkey,a,dval,b,gval
	akeys=GetRegSubkeys(HKEY_CURRENT_USER, pathkey)
	if IsEmpty(akeys) Then
		IsInstallVisualStudio=Empty
	else
		getkey=0
		gval=0
		set re = new regexp
		re.Pattern = "^[0-9]+(.[0-9]*)?$"
		for each a in akeys
			set result = re.Execute(a)
			num = 0
			for each b in result
				num = num + 1
			Next

			if num > 0 Then
				dval = CDbl(a)
				' if find the version is great than the version we search so we get it
				if dval > version or abs(dval - version) < 0.0001 Then
					if gval < dval Then
						gval=dval
						getkey=a
					End If
				End If
			End If
		Next

		if getkey > 0 Then			
			IsInstallVisualStudio=getkey
		else
			IsInstallVisualStudio=Empty
		End If
	End If
End Function


Function FindoutInstallBasedir(directory,vsver)
	dim narr,i,j,num,sp,mstr

	narr = Split(directory,"\")
	if IsArray(narr) Then
		num=Ubound(narr)
		i=num
		mstr="Microsoft Visual Studio " & vsver
		do While i >= 0
			if narr(i) = mstr Then
				exit do
			End If
			i = i - 1
		Loop

		j=1
		sp=narr(0)
		While j < (i+1)
			sp = sp & "\" & narr(j)
			j = j + 1
		Wend
		FindoutInstallBasedir=sp
	else
		FindoutInstallBasedir=""
	End If

End Function



Function VsVerifyCL(vsver)
	dim basedir,instdir,ret,clcmd

	instdir = ReadReg("HKEY_CURRENT_USER\SOFTWARE\Microsoft\VisualStudio\"& vsver &"_Config\InstallDir")
	if IsEmpty(instdir) Then
		VsVerifyCL=0
		exit function
	End If

	basedir=FindoutInstallBasedir(instdir,vsver)
	clcmd = basedir & "\VC\bin\cl.exe" 
	ret=FileExists(clcmd)
	if ret Then
		VsVerifyCL=1
	Else
		VsVerifyCL=0
	End If	
End Function

