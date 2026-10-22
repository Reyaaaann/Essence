; ESSENCE installer - compile with Inno Setup 6 (build.bat does this for you)
[Setup]
AppId={{8C1E5A52-6E21-4B8B-9A0D-5E55E2C0A7D1}
AppName=Essence
AppVersion=1.0.0
AppPublisher=Reyaan
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=Output
OutputBaseFilename=Essence_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
InfoAfterFile=after_install.txt
UninstallDisplayName=Essence (VST3)

[Files]
Source: "..\build\Essence_artefacts\Release\VST3\Essence.vst3\*"; DestDir: "{commoncf64}\VST3\Essence.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Messages]
WelcomeLabel2=This will install Essence, a dark R&B / trapsoul synthesizer, as a VST3 plugin.%n%nsincerely your's - Reyaan
