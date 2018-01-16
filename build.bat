cd "C:\Users\Andyhk\Documents\visual studio 2017\Projects\AndyOS\"

@SET Rebase="Rebase\bin\debug\Rebase.exe"
@SET FileAdder="FileAdder\bin\debug\FileAdder.exe"
@SET OS="Debug\andyos.exe"
@SET BootBinary="Root\boot\andyos.bin"
@SET ISOName="AndyOS.iso"

@SET MapPath="Kernel\map.map"
@SET FILE1="IMG.BMP"
@SET FILE2="MODEL.3D"

%Rebase% 0x100000 %OS%
REM %FileAdder% %MapPath% %Kernel% %FILE1% "FILE1"
REM %FileAdder% %MapPath% %Kernel% %FILE2% "FILE2"
copy %OS% %BootBinary% /y


IF exist %windir%\sysnative\bash.exe (%windir%\sysnative\bash.exe -c "grub-mkrescue -o %ISOName% Root/") ELSE (bash.exe -c "grub-mkrescue -o %ISOName% Kernel/" & pause)