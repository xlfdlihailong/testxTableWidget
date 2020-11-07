@echo off
echo %1 %2 %3 %4
plink.exe -batch -pw %1 root@%2 "%3" >%4