# Microsoft Developer Studio Project File - Name="CAF" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CAF - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CAF.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CAF.mak" CFG="CAF - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CAF - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CAF - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "CAF"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "opt"
# PROP Intermediate_Dir "opt"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /compile_only /dll /nologo /warn:nofileopt
# ADD F90 /compile_only /dll /nologo /warn:nofileopt
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAF_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "..\..\include" /I "$(QTDIR)\include" /I "$(OCCINC)" /D "NDEBUG" /D "_USRDLL" /D "CAF_EXPORTS" /D "WNT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib qt-mt333.lib tkernel.lib tkcaf.lib /nologo /dll /machine:I386 /out:"../../win32/bin/caf.dll" /implib:"../../win32/lib/caf.lib" /libpath:"$(OCCLIB)" /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "deb"
# PROP Intermediate_Dir "deb"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
F90=df.exe
# ADD BASE F90 /check:bounds /compile_only /dbglibs /debug:full /dll /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD F90 /check:bounds /compile_only /dbglibs /debug:full /dll /nologo /traceback /warn:argument_checking /warn:nofileopt
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "CAF_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "..\..\include" /I "$(QTDIR)\include" /I "$(OCCINC)" /D "_DEBUG" /D "_USRDLL" /D "CAF_EXPORTS" /D "WNT" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib qt-mt333.lib tkernel.lib tkcaf.lib /nologo /dll /debug /machine:I386 /out:"../../win32/bind/caf.dll" /implib:"../../win32/libd/caf.lib" /pdbtype:sept /libpath:"$(OCCLIBD)" /libpath:"$(QTDIR)\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "CAF - Win32 Release"
# Name "CAF - Win32 Debug"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\CAF\CAF.h

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Update $(InputName).h...
InputPath=..\CAF\CAF.h
InputName=CAF

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\include\$(InputName).h

# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Update $(InputName).h...
InputPath=..\CAF\CAF.h
InputName=CAF

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\include\$(InputName).h

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\CAF_Application.h

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate moc_$(InputName).cxx...
InputPath=.\CAF_Application.h
InputName=CAF_Application

BuildCmds= \
	type $(InputPath) > ..\..\include\$(InputName).h \
	$(QTDIR)\bin\moc.exe $(InputPath) -o moc\moc_$(InputName).cxx \
	

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate moc_$(InputName).cxx...
InputPath=.\CAF_Application.h
InputName=CAF_Application

BuildCmds= \
	type $(InputPath) > ..\..\include\$(InputName).h \
	$(QTDIR)\bin\moc.exe $(InputPath) -o moc\moc_$(InputName).cxx \
	

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CAF\CAF_Operation.h

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate moc_$(InputName).cxx...
InputPath=..\CAF\CAF_Operation.h
InputName=CAF_Operation

BuildCmds= \
	type $(InputPath) > ..\..\include\$(InputName).h \
	$(QTDIR)\bin\moc.exe $(InputPath) -o moc\moc_$(InputName).cxx \
	

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate moc_$(InputName).cxx...
InputPath=..\CAF\CAF_Operation.h
InputName=CAF_Operation

BuildCmds= \
	type $(InputPath) > ..\..\include\$(InputName).h \
	$(QTDIR)\bin\moc.exe $(InputPath) -o moc\moc_$(InputName).cxx \
	

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CAF\CAF_Study.h

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate moc_$(InputName).cxx...
InputPath=..\CAF\CAF_Study.h
InputName=CAF_Study

BuildCmds= \
	type $(InputPath) > ..\..\include\$(InputName).h \
	$(QTDIR)\bin\moc.exe $(InputPath) -o moc\moc_$(InputName).cxx \
	

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate moc_$(InputName).cxx...
InputPath=..\CAF\CAF_Study.h
InputName=CAF_Study

BuildCmds= \
	type $(InputPath) > ..\..\include\$(InputName).h \
	$(QTDIR)\bin\moc.exe $(InputPath) -o moc\moc_$(InputName).cxx \
	

"moc\moc_$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\CAF\CAF_Tools.h

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Update $(InputName).h...
InputPath=..\CAF\CAF_Tools.h
InputName=CAF_Tools

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\include\$(InputName).h

# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Update $(InputName).h...
InputPath=..\CAF\CAF_Tools.h
InputName=CAF_Tools

"..\..\include\$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\include\$(InputName).h

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;f90;for;f;fpp"
# Begin Source File

SOURCE=.\CAF_Application.cxx
# End Source File
# Begin Source File

SOURCE=.\CAF_Operation.cxx
# End Source File
# Begin Source File

SOURCE=.\CAF_Study.cxx
# End Source File
# Begin Source File

SOURCE=.\CAF_Tools.cxx
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resources\CAF_images.po

!IF  "$(CFG)" == "CAF - Win32 Release"

# Begin Custom Build - Generate $(InputName).qm...
InputPath=.\resources\CAF_images.po
InputName=CAF_images

"..\..\resources\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\msg2qm.exe $(InputPath) ..\..\resources\$(InputName).qm

# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# Begin Custom Build - Generate $(InputName).qm...
InputPath=.\resources\CAF_images.po
InputName=CAF_images

"..\..\resources\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\msg2qm.exe $(InputPath) ..\..\resources\$(InputName).qm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resources\CAF_msg_en.po

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate $(InputName).qm...
InputPath=.\resources\CAF_msg_en.po
InputName=CAF_msg_en

"..\..\resources\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\msg2qm.exe $(InputPath) ..\..\resources\$(InputName).qm

# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Generate $(InputName).qm...
InputPath=.\resources\CAF_msg_en.po
InputName=CAF_msg_en

"..\..\resources\$(InputName).qm" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(QTDIR)\bin\msg2qm.exe $(InputPath) ..\..\resources\$(InputName).qm

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resources\redo.png

!IF  "$(CFG)" == "CAF - Win32 Release"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Update $(InputName).png...
InputPath=.\resources\redo.png
InputName=redo

"..\..\resources\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\resources\$(InputName).png

# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# PROP Ignore_Default_Tool 1
# Begin Custom Build - Update $(InputName).png...
InputPath=.\resources\redo.png
InputName=redo

"..\..\resources\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\resources\$(InputName).png

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\resources\undo.png

!IF  "$(CFG)" == "CAF - Win32 Release"

# Begin Custom Build - Update $(InputName).png...
InputPath=.\resources\undo.png
InputName=undo

"..\..\resources\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\resources\$(InputName).png

# End Custom Build

!ELSEIF  "$(CFG)" == "CAF - Win32 Debug"

# Begin Custom Build - Update $(InputName).png...
InputPath=.\resources\undo.png
InputName=undo

"..\..\resources\$(InputName).png" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	type $(InputPath) > ..\..\resources\$(InputName).png

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Meta Object Files"

# PROP Default_Filter "cpp;c;cxx"
# Begin Source File

SOURCE=.\moc\moc_CAF_Application.cxx
# End Source File
# Begin Source File

SOURCE=.\moc\moc_CAF_Operation.cxx
# End Source File
# Begin Source File

SOURCE=.\moc\moc_CAF_Study.cxx
# End Source File
# End Group
# End Target
# End Project
