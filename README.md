# VCognitis

[VCognitis](https://barbatum.ru/vcognitis) will help to find interesting people.


## Linux deploy

* Add ```CONFIG += c++11 release``` to .pro file
* From project root folder run ```export QMAKE=[path to qt]/Qt5.12.7/5.12.7/gcc_64/bin/qmake```
* Run ```./delpoy_linux.sh```
* At the end you should get a working .AppImage binary
* For icon setting details and additional info read [HERE](https://github.com/linuxdeploy/QtQuickApp/blob/master/README.md)

## Windows deploy

* Move your release .exe file in clean deploy folder
* Run Qt 5.12.7 (MSVC 2017 64-bit) console from "Start" menu
* ```cd``` to your deploy folder
* run ```windeployqt . --qmldir [your_projects_folder]\vkconfederatefinder\qml```
* Copy [libssl-1_1-x64.dll and libcrypto-1_1-x64.dll](https://slproweb.com/download/Win64OpenSSL_Light-1_1_1d.exe) in your deploy folder
* Copy ```vcruntime140_1.dll``` (43,2kb), ```vcruntime140.dll``` (87,7kb), ```msvcp140.dll```(603kb) in your deploy folder
* Now run your exe file on Win 7 and Win 10  and confirm that it is working properly

## Windows install

* Open ```WindowsInstall.iss``` script in InnoSetup
* Change ```ExecutableFile``` and ```AppFolder``` definitions, and run script. You should get your installer in ```Output``` folder.

## Windows build

* This part of instruction is for those, who can't configure this project to run on Windows 7 64 bit machine, you can skip this part
* Install Qt 5.12.7 with msvc2017_64
* [Download Visual Studio 2017](https://visualstudio.microsoft.com/ru/vs/older-downloads/)
* Install Visual Studio 2017 version 15.8 or later with packages: Инструменты статистического анализа, Базовые компоненты Visual Studio C++, VC++ 2017 version 15.9 v14.16 latest v141 tools, Visual C++ ATL для x86 и x64, Пакет SDK для Windows 10 (10.017134.0)
* If network is not working don't forget about [SSL libs](https://slproweb.com/download/Win64OpenSSL_Light-1_1_1d.exe) ;)


## About version

To change current version you should use git tags:

``` bash 
git tag -a x.x.x -m "Place here message to identify current version(visible only from git)"
git push --tags 
```

Where x.x.x it is version. For example:

```git tag -a 1.2.3 -m "v1.2.3 new way to download users"```


#

Copyright © 2020 [Barbatum](https://barbatum.ru/vcognitis). All rights reserved. Contact: support@barbatum.ru