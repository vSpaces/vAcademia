@echo off

l:\csclCvsTools\CVSNT\cvs -N -q -d :local:L:\csclCvs commit -R
if errorlevel 1 goto update

l:
cd L:\collaborative-nir\program\ComMan
L:\csclCvsTools\CVSNT\cvs -N -d :local:L:\csclCvs update
"C:\Program Files\Microsoft Visual Studio\Common\MSDev98\Bin\msdev.exe" ComMan.dsp /MAKE "ComMan - Win32 Debug" /REBUILD
copy /y L:\collaborative-nir\program\ComMan\debug\ComMan.dll L:\collaborative-nir\program\Dll\ComMan.dll
exit

:update
l:\csclCvsTools\CVSNT\cvs -N -d :local:L:\csclCvs update
echo 
echo В проект были внесены изменения. Убедитесь в его работоспособности