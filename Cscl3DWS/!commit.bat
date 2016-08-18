@echo off

l:\csclCvsTools\CVSNT\cvs -N -q -d :local:L:\csclCvs commit -R
if errorlevel 1 goto update

l:
cd L:\collaborative-nir\program\Cscl3DWS
L:\csclCvsTools\CVSNT\cvs -N -d :local:L:\csclCvs update
"C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\msdev.exe" Cscl3DWS.dsp /MAKE "Cscl3DWS - Win32 Debug" /REBUILD
copy /y L:\collaborative-nir\program\Cscl3DWS\debug\Cscl3DWS.dll L:\collaborative-nir\program\Dll\Cscl3DWS.dll
exit

:update
l:\csclCvsTools\CVSNT\cvs -N -d :local:L:\csclCvs update
echo 
echo В проект были внесены изменения. Убедитесь в его работоспособности