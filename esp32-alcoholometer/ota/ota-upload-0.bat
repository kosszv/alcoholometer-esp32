@echo off

set prj_name=esp32-alcoholometer
set prj_dir=c:\PlatformIO\%prj_name%
set prj_elf=%prj_dir%\ota\firmware
set prj_obj=esp32dev
set prj_ota=63e5a6fd-0cc5-49d8-a642-e0860f075292

echo --------------------------------------------------------------------------------------------------------------------
echo Backup ELF-file for debug...
echo --------------------------------------------------------------------------------------------------------------------

rem del /f /q %prj_elf%\firmware_3.elf
rem rename %prj_elf%\firmware_2.elf firmware_3.elf
rem rename %prj_elf%\firmware_1.elf firmware_2.elf
rem rename %prj_elf%\firmware.elf firmware_1.elf
xcopy "%prj_dir%\.pio\build\%prj_obj%\firmware.elf" %prj_elf%\ /I /H /R /K /Y 

echo --------------------------------------------------------------------------------------------------------------------
echo Upload BIN-file to FTP
echo --------------------------------------------------------------------------------------------------------------------

"C:\Program Files (x86)\WinSCP\WinSCP.com" /command "open ftp://**********:***********************************@kotyara12.ru/" "cd /www/kotyara12.ru/%prj_ota%" "put "%prj_dir%\.pio\build\%prj_obj%\firmware.bin" -transfer=binary -preservetime" "exit" /log="ota_upload.log" /ini=nul
