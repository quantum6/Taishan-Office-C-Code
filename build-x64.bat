:: prepare vs enviroment
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" amd64

DEL /s/q Debug
DEL /s/q Release
DEL /s/q x64

SET CONFIG="Debug|x64"

for %%i in ( product-Office ^
             product-Reade ^
             RapidMenu ^
             Downloader ^
             Unpackager ^
             Upgrader ) ^
do (
    devenv ts-framework3.3.sln /rebuild %CONFIG% /project %%i /projectconfig %CONFIG%
)
