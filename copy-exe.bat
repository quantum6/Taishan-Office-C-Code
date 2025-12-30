SET TSOFFICE="C:\Program Files (x86)\Taishan\Office"
SET BUILD=Release

del %TSOFFICE%\System\*.log

SET TSO=Office.exe
taskkill /im %TSO% /f
COPY /Y  windows-Office\Debug\%TSO%                %TSOFFICE%\bin

SET RM=RapidMenu.exe
taskkill /im %RM% /f
COPY /Y   windows-RapidMenu\%BUILD%\%RM%           %TSOFFICE%\bin

COPY /Y  windows-nptsoffice\%BUILD%\nptsoffice.dll %TSOFFICE%\Plugins

PAUSE