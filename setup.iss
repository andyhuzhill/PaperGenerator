; 脚本由 Inno Setup 脚本向导 生成！
; 有关创建 Inno Setup 脚本文件的详细资料请查阅帮助文档！

#define MyAppName "长沙理工大学电气与信息工程学院试卷自动生成系统"
#define MyAppVersion "1.0"
#define MyAppPublisher "长沙理工大学电气与信息工程学院"
#define MyAppURL "http://www.csust.edu.cn"
#define MyAppExeName "QtActive.exe"

[Setup]
; 注: AppId的值为单独标识该应用程序。
; 不要为其他安装程序使用相同的AppId值。
; (生成新的GUID，点击 工具|在IDE中生成GUID。)
AppId={{8350400D-4947-47CA-AB34-D5403AA34DCE}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName=电气与信息工程学院试卷自动生成系统
LicenseFile=E:\chouti\licence.txt
InfoBeforeFile=E:\chouti\ReadMe.txt
OutputDir=C:\Documents and Settings\Administrator\桌面\chouti
OutputBaseFilename=setup
Compression=lzma
SolidCompression=yes

[Languages]
Name: "chinesesimp"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\QtWorkspace\QtActive-build-desktop-Qt_4_8_5__4_8_5____\debug\QtActive.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\QtWorkspace\QtActive-build-desktop-Qt_4_8_5__4_8_5____\debug\QtActive.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\QtWorkspace\QtActive-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK____\debug\libgcc_s_dw2-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\QtWorkspace\QtActive-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK____\debug\mingwm10.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\QtWorkspace\QtActive-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK____\debug\QtCored4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\QtWorkspace\QtActive-build-desktop-Qt_4_7_4_for_Desktop_-_MinGW_4_4__Qt_SDK____\debug\QtGuid4.dll"; DestDir: "{app}"; Flags: ignoreversion
; 注意: 不要在任何共享系统文件上使用“Flags: ignoreversion”

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

