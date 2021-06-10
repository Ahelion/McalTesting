
@echo off

@echo Starting in a different window the renode and the tcp client

start cmd /K "C:\Program Files\Renode\bin\Renode.exe" renode_config.resc
REM wait for 10 secons to assure the machine starts

echo 10 second timeout for Renode to start
TIMEOUT 30

REM start in a diferent window the tcp client
start cmd /K D:\cygwin64\bin\nc.exe localhost 33335 

TIMEOUT 30

