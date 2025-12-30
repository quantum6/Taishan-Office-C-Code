:: prepare vs enviroment
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" amd64

DEL /s/q Debug
DEL /s/q Release
DEL /s/q Win32

SET CONFIG="Debug|Win32"

for %%i in ( product-Office ^
             product-Reade ^
             RapidMenu ^
             Downloader ^
             Unpackager ^
             Upgrader ^
             Win32-nptsoffice ^
             Win32-nptsreader ^
             Win32-proxy-Office ^
             Win32-proxy-Reader ) ^
do (
    devenv ts-framework3.3.sln /rebuild %CONFIG% /project %%i /projectconfig %CONFIG%
)
