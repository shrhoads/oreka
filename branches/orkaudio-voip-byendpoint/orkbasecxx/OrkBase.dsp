# Microsoft Developer Studio Project File - Name="OrkBase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=OrkBase - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OrkBase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OrkBase.mak" CFG="OrkBase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OrkBase - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "OrkBase - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OrkBase - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /Ob0 /I "." /I "C:\devExt\boost\boost_1_32_0" /I "C:\devExt\ACE_wrappers" /I "C:\devExt\xerces++\xerces-c_2_6_0-windows_nt-msvc_60\include" /I "C:\devExt\log4cxx\log4cxx-0.9.7\include" /D "BUILD_ORKBASE" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 xerces-c_2.lib log4cxx.lib ace.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /libpath:"C:\devExt\ACE_wrappers\lib" /libpath:"C:\devExt\log4cxx\log4cxx-0.9.7\msvc\Lib\Release" /libpath:"C:\devExt\xerces++\xerces-c_2_6_0-windows_nt-msvc_60\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Release\OrkBase.dll ..\OrkAudio\OrkBase.dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "C:\devExt\boost\boost_1_32_0" /I "C:\devExt\ACE_wrappers" /I "C:\devExt\xerces++\xerces-c_2_6_0-windows_nt-msvc_60\include" /I "C:\devExt\log4cxx\log4cxx-0.9.7\include" /D "BUILD_ORKBASE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 xerces-c_2D.lib log4cxx.lib aced.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"Debug/OrkBaseD.dll" /pdbtype:sept /libpath:"C:\devExt\ACE_wrappers\lib" /libpath:"C:\devExt\log4cxx\log4cxx-0.9.7\msvc\Lib\Debug" /libpath:"C:\devExt\xerces++\xerces-c_2_6_0-windows_nt-msvc_60\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy Debug\OrkBaseD.dll ..\OrkAudio\OrkBaseD.dll
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "OrkBase - Win32 Release"
# Name "OrkBase - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Serializers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Serializers\DomSerializer.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Serializers\DomSerializer.h
# End Source File
# Begin Source File

SOURCE=.\Serializers\Serializer.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Serializers\Serializer.h
# End Source File
# Begin Source File

SOURCE=.\Serializers\SingleLineSerializer.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Serializers\SingleLineSerializer.h
# End Source File
# Begin Source File

SOURCE=.\Serializers\UrlSerializer.cpp
# End Source File
# Begin Source File

SOURCE=.\Serializers\UrlSerializer.h
# End Source File
# Begin Source File

SOURCE=.\Serializers\XmlRpcSerializer.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Serializers\XmlRpcSerializer.h
# End Source File
# End Group
# Begin Group "Messages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Messages\AsyncMessage.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Messages\AsyncMessage.h
# End Source File
# Begin Source File

SOURCE=.\Messages\Message.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Messages\Message.h
# End Source File
# Begin Source File

SOURCE=.\Messages\SyncMessage.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Messages\SyncMessage.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AudioCapture.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioCapture.h
# End Source File
# Begin Source File

SOURCE=.\Filter.cpp
# End Source File
# Begin Source File

SOURCE=.\Filter.h
# End Source File
# Begin Source File

SOURCE=.\g711.c
# End Source File
# Begin Source File

SOURCE=.\g711.h
# End Source File
# Begin Source File

SOURCE=.\Object.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Object.h
# End Source File
# Begin Source File

SOURCE=.\ObjectFactory.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\ObjectFactory.h
# End Source File
# Begin Source File

SOURCE=.\OrkBase.cpp

!IF  "$(CFG)" == "OrkBase - Win32 Release"

!ELSEIF  "$(CFG)" == "OrkBase - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\OrkBase.h
# End Source File
# Begin Source File

SOURCE=.\OrkClient.cpp
# End Source File
# Begin Source File

SOURCE=.\OrkClient.h
# End Source File
# Begin Source File

SOURCE=.\StdString.h
# End Source File
# Begin Source File

SOURCE=.\Utils.cpp
# End Source File
# Begin Source File

SOURCE=.\Utils.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project