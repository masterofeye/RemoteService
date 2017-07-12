; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=RemoteService 
AppVersion=2.0.5
DefaultDirName={pf}\Schleissheimer\RemoteSoftware\RemoteService2.0
DefaultGroupName=RemoteService\
UninstallDisplayIcon={app}\RemoteService.exe
Compression=lzma2
SolidCompression=yes
OutputDir=.
OutputBaseFilename=RemoteServiceInstaller
CloseApplications=no

[Files]
Source: "..\build\x64\Release\RemoteService.exe"; DestDir: "{app}"; BeforeInstall: MyBeforeInstall
Source: "..\build\x64\Release\RemoteHiddenHelper.exe"; DestDir: "{app}"
Source: "..\build\x64\Release\DiagUtils.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\FlasherUtils.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\libmysql.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\Qt5Core.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\Qt5Network.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\Qt5Sql.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\Qt5Widgets.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\Qt5Xml.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\Qt5Gui.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\QtService.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\RemoteBoxWrapper.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\RemoteCommunicationLibrary.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\RemoteDataConnectLibrary.dll"; DestDir: "{app}"
Source: "..\build\x64\Release\sqldrivers\qsqlmysql.dll"; DestDir: "{app}\sqldrivers"
Source: "..\build\x64\Release\platforms\qwindows.dll"; DestDir: "{app}\platforms"
Source: "..\build\x64\Release\unregister.bat"; DestDir: "{app}\"
Source: "..\build\x64\Release\register.bat"; DestDir: "{app}\"; BeforeInstall: MyAfterInstall

[Code]
procedure MyBeforeInstall();
var
   ResultCode: integer;
begin
   Exec(ExpandConstant('{app}\unregister.bat'), '', '', SW_HIDE, ewWaitUntilTerminated, ResultCode)
end;



procedure MyAfterInstall();
var
   ResultCode: integer;
begin
   Exec(ExpandConstant('{app}\register.bat'), '', '', SW_HIDE, ewWaitUntilTerminated, ResultCode)
end;