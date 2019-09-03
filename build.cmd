@echo off
:: AxoMem Nucleus - for developing Persistent Applications
:: Copyright (C) 2018-2019 AxoMem Pte Ltd.

:: This program is free software; you can redistribute it and/or modify
:: it under the terms of the GNU General Public License Version 2 (only)
:: as published by the Free Software Foundation.

:: This program is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:: GNU General Public License Version 2 for more details.

:: You should have received a copy of the GNU General Public License
:: along with this program; if not, see http://www.gnu.org/licenses/

:: This is an example cmake-based build script for Windows. Current assumed environment:
::
::     Cmake - https://cmake.org/download/
::     Visual Studio 2017   or    Visual Studio Build Tools 2017
::
:: Copy and customise to your environment. Any cmd files starting with build- are ignored by git
:: Please contribute back any substantial changes, bugs, issues or success with new versions of Visual Studio

IF %0 EQU "%~dpnx0" GOTO DOUBLECLICK_ERR
IF "%VCINSTALLDIR%!"==! GOTO TOOLS_ERR

IF %1!==! GOTO BUILD
IF /I %1==CONFIGURE GOTO CONFIGURE
GOTO UNKNOWN_PARM_ERR

:CONFIGURE
ECHO.
ECHO *** Starting CMAKE Configuration
ECHO.
IF %VCPKG_ROOT%!==! GOTO VCPKG_ROOT_ERR
SET VCPKG_CMAKE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
IF NOT EXIST %VCPKG_CMAKE% GOTO VCPKG_CMAKE_ERR
ECHO Using VCPKG CMAKE file %VCPKG_CMAKE%

IF EXIST build (
    ECHO.
    ECHO Note - reconfiguring build directory. There may be issues if CMAKE files or dependencies have changed.
    ECHO Tip - use this to clear your build directory: RMDIR /S /Q build
    TIMEOUT 10
)
ECHO.
cmake . -Bbuild -G "Visual Studio 15 2017 Win64" -DCMAKE_TOOLCHAIN_FILE=%VCPKG_CMAKE% -DVCPKG_TARGET_TRIPLET=x64-windows
ECHO.
ECHO CMAKE Configuration complete. To build, run BUILD with no parameters
ECHO.
GOTO END

:BUILD
ECHO.
ECHO *** Starting Build...
ECHO.
IF EXIST build (
    ECHO Note - There may be issues if CMAKE files or dependencies have changed.
    ECHO        Rerun BUILD CONFIGURE to rebuild the cmake build configuration
    TIMEOUT 10
) ELSE (
    ECHO ERROR - BUILD directory not found. Run BUILD CONFIGURE first to set up cmake environment.
    GOTO END
)
ECHO.
msbuild build\ALL_BUILD.vcxproj

GOTO END
REM =======================================================

:DOUBLECLICK_ERR
:TOOLS_ERR
ECHO This script needs to be run from within the x64 Native Tools Command Prompt for VS 2017  command window
GOTO END

:UNKNOWN_PARM_ERR
ECHO Unknown parameter. Leave blank to build or CONFIGURE to configure cmake.
GOTO END

:VCPKG_ROOT_ERR
ECHO Set VCPKG_ROOT environment variable to point to the root of your VCPKG directory, using forward slashes and no trailing slash
ECHO EG SET VCPKG_ROOT=c:/users/john/vcpkg
ECHO See https://docs.microsoft.com/en-us/cpp/build/vcpkg?view=vs-2019  for more information
GOTO END

:VCPKG_CMAKE_ERR
ECHO Unable to find VCPKG CMAKE file at %VCPKG_CMAKE%
ECHO Please check your VCPKG_ROOT setting. Currently its %VCPKG_ROOT%
GOTO END

:END
REM detects if run from explorer and pauses if so
IF %0 EQU "%~dpnx0" TIMEOUT 10
