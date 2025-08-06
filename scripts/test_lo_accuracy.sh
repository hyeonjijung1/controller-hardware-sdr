@echo off
REM test_lo.bat - Sends a CAT command to your SDR device and reads the response

set COMPORT=COM11

REM Send CAT command (Change this as needed)
echo FA; > %COMPORT%

REM Pause for device to respond
timeout /t 1 > nul

REM Display response
type %COMPORT%
