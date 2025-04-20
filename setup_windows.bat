@echo off
setlocal enabledelayedexpansion

echo ======================================================
echo Jetpack - Script d'installation des dependances Windows
echo ======================================================
echo.
echo Ce script va telecharger et installer :
echo  1. Visual Studio Build Tools (compilateur C++)
echo  2. CMake (systeme de construction)
echo  3. SFML 2.5.1 (bibliotheque graphique)
echo.
echo Les fichiers seront telecharges dans le dossier courant.
echo.
pause

echo.
echo ======================================================
echo Verification des outils existants...
echo ======================================================

REM Vérification de Visual Studio
where cl.exe >nul 2>nul
if %ERRORLEVEL% == 0 (
    echo Visual Studio C++ est deja installe.
    set VS_INSTALLED=1
) else (
    echo Visual Studio C++ n'est pas installe.
    set VS_INSTALLED=0
)

REM Vérification de CMake
where cmake.exe >nul 2>nul
if %ERRORLEVEL% == 0 (
    echo CMake est deja installe.
    set CMAKE_INSTALLED=1
) else (
    echo CMake n'est pas installe.
    set CMAKE_INSTALLED=0
)

echo.
echo ======================================================
echo Telechargement des installers...
echo ======================================================

if %VS_INSTALLED% == 0 (
    echo Telechargement de Visual Studio Build Tools...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://aka.ms/vs/17/release/vs_buildtools.exe' -OutFile 'vs_buildtools.exe'}"
    if %ERRORLEVEL% neq 0 (
        echo Erreur lors du telechargement de VS Build Tools.
        goto error
    )
)

if %CMAKE_INSTALLED% == 0 (
    echo Telechargement de CMake...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/Kitware/CMake/releases/download/v3.28.3/cmake-3.28.3-windows-x86_64.msi' -OutFile 'cmake-3.28.3-windows-x86_64.msi'}"
    if %ERRORLEVEL% neq 0 (
        echo Erreur lors du telechargement de CMake.
        goto error
    )
)

echo Telechargement de SFML 2.5.1...
if not exist sfml (
    mkdir sfml
)
powershell -Command "& {Invoke-WebRequest -Uri 'https://www.sfml-dev.org/files/SFML-2.5.1-windows-vc15-64-bit.zip' -OutFile 'sfml.zip'}"
if %ERRORLEVEL% neq 0 (
    echo Erreur lors du telechargement de SFML.
    goto error
)

echo.
echo ======================================================
echo Installation des outils...
echo ======================================================

if %VS_INSTALLED% == 0 (
    echo Installation de Visual Studio Build Tools...
    start /wait vs_buildtools.exe --quiet --wait --norestart --nocache ^
        --installPath "%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools" ^
        --add Microsoft.VisualStudio.Workload.VCTools ^
        --includeRecommended
    if %ERRORLEVEL% neq 0 (
        echo Erreur lors de l'installation de VS Build Tools.
        goto error
    )
    echo Visual Studio Build Tools installe avec succes.
)

if %CMAKE_INSTALLED% == 0 (
    echo Installation de CMake...
    start /wait msiexec /i cmake-3.28.3-windows-x86_64.msi /quiet /norestart ALLUSERS=1 ADD_CMAKE_TO_PATH=System
    if %ERRORLEVEL% neq 0 (
        echo Erreur lors de l'installation de CMake.
        goto error
    )
    echo CMake installe avec succes.
)

echo Extraction de SFML...
powershell -Command "& {Expand-Archive -Path 'sfml.zip' -DestinationPath 'sfml' -Force}"
if %ERRORLEVEL% neq 0 (
    echo Erreur lors de l'extraction de SFML.
    goto error
)
echo SFML extrait avec succes.

echo.
echo ======================================================
echo Configuration du projet...
echo ======================================================

REM Création du script de compilation
echo @echo off > build_jetpack.bat
echo setlocal >> build_jetpack.bat
echo. >> build_jetpack.bat
echo echo Configuration et compilation du client Jetpack... >> build_jetpack.bat
echo. >> build_jetpack.bat
echo if not exist build mkdir build >> build_jetpack.bat
echo cd build >> build_jetpack.bat
echo. >> build_jetpack.bat
echo set "SFML_DIR=%CD%\..\sfml\SFML-2.5.1\lib\cmake\SFML" >> build_jetpack.bat
echo. >> build_jetpack.bat
echo cmake .. -G "Visual Studio 17 2022" -A x64 -DSFML_DIR="%%SFML_DIR%%" >> build_jetpack.bat
echo if %%ERRORLEVEL%% neq 0 ( >> build_jetpack.bat
echo     echo Erreur lors de la configuration CMake. >> build_jetpack.bat
echo     pause >> build_jetpack.bat
echo     exit /b 1 >> build_jetpack.bat
echo ) >> build_jetpack.bat
echo. >> build_jetpack.bat
echo cmake --build . --config Release >> build_jetpack.bat
echo if %%ERRORLEVEL%% neq 0 ( >> build_jetpack.bat
echo     echo Erreur lors de la compilation. >> build_jetpack.bat
echo     pause >> build_jetpack.bat
echo     exit /b 1 >> build_jetpack.bat
echo ) >> build_jetpack.bat
echo. >> build_jetpack.bat
echo echo. >> build_jetpack.bat
echo echo Copie des DLLs SFML necessaires... >> build_jetpack.bat
echo xcopy /y "..\sfml\SFML-2.5.1\bin\*.dll" "client\Release\" >> build_jetpack.bat
echo. >> build_jetpack.bat
echo echo. >> build_jetpack.bat
echo echo Compilation terminee avec succes! >> build_jetpack.bat
echo echo Le client se trouve dans: %%CD%%\client\Release\jetpack_client.exe >> build_jetpack.bat
echo echo. >> build_jetpack.bat
echo echo Pour lancer le client: >> build_jetpack.bat
echo echo jetpack_client.exe -h IP_SERVEUR -p PORT_SERVEUR [-d] >> build_jetpack.bat
echo echo. >> build_jetpack.bat
echo cd .. >> build_jetpack.bat
echo pause >> build_jetpack.bat

REM Création du script pour renommer le CMakeLists.txt
echo @echo off > prepare_cmake.bat
echo echo Preparation des fichiers CMake pour Windows... >> prepare_cmake.bat
echo. >> prepare_cmake.bat
echo if exist client\CMakeLists.txt (>> prepare_cmake.bat
echo     move /y client\CMakeLists.txt client\CMakeLists_unix.txt >> prepare_cmake.bat
echo ) >> prepare_cmake.bat
echo if exist client\CMakeLists_windows.txt (>> prepare_cmake.bat
echo     copy client\CMakeLists_windows.txt client\CMakeLists.txt >> prepare_cmake.bat
echo     echo Fichier CMakeLists.txt pour Windows mis en place avec succes. >> prepare_cmake.bat
echo ) else (>> prepare_cmake.bat
echo     echo ERREUR: Le fichier CMakeLists_windows.txt n'a pas ete trouve! >> prepare_cmake.bat
echo ) >> prepare_cmake.bat
echo. >> prepare_cmake.bat
echo pause >> prepare_cmake.bat

echo.
echo ======================================================
echo Installation terminee avec succes!
echo ======================================================
echo.
echo Outils installes:
if %VS_INSTALLED% == 0 (
    echo - Visual Studio Build Tools 2022
) else (
    echo - Visual Studio Build Tools (deja installe)
)
if %CMAKE_INSTALLED% == 0 (
    echo - CMake 3.28.3
) else (
    echo - CMake (deja installe)
)
echo - SFML 2.5.1
echo.
echo Scripts crees:
echo - prepare_cmake.bat - Prepare les fichiers CMake pour Windows
echo - build_jetpack.bat - Configure et compile le projet
echo.
echo Etapes suivantes:
echo 1. Executez prepare_cmake.bat pour configurer les fichiers CMake
echo 2. Executez build_jetpack.bat pour compiler le projet
echo.
pause
exit /b 0

:error
echo.
echo ======================================================
echo Une erreur est survenue durant l'installation!
echo ======================================================
echo.
pause
exit /b 1