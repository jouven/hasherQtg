# hasherQtg
GUI program to create a list of files from the filesystem, generate hash values, save the list in a file and load a saved file to compare the saved values with the current ones

Compilation
-----------
Requires:

Qt library

https://github.com/jouven/criptoQtso

https://github.com/jouven/essentialQtso

https://github.com/jouven/signalso

https://github.com/jouven/sizeConversionso

https://github.com/jouven/threadedFunctionQtso

Run (in hasherQtg source directory or pointing to it):

    qmake

and then:

    make

Images
------
*Linux Xfce4*

![](../master/images/Screenshot_linux_xfce4.png)

*Windows 10*

![](../master/images/Screenshot_windows10.PNG)

*Android 5*

![](../master/images/Screenshot_Android5_QT592_1.png)
![](../master/images/Screenshot_Android5_QT592_2.png)

Save file
------------
```javascript
{
    "files": [
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/.qmake.stash",
            "hash": "1000348246",
            "size": "739"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/Makefile",
            "hash": "551930279",
            "size": "23345"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/appConfig.o",
            "hash": "2140573804",
            "size": "1271632"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/fileData.o",
            "hash": "1087825487",
            "size": "2179736"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/hasherQtg",
            "hash": "64016910",
            "size": "3745440"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/hasherQtg_config.json",
            "hash": "3312363774",
            "size": "255"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/main.o",
            "hash": "473073220",
            "size": "1391488"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/moc_predefs.h",
            "hash": "3726574356",
            "size": "14536"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/moc_window.cpp",
            "hash": "1705998752",
            "size": "12386"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/moc_window.o",
            "hash": "597416576",
            "size": "1231104"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/qrc_icons.cpp",
            "hash": "2318769952",
            "size": "1338"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/qrc_icons.o",
            "hash": "482833924",
            "size": "30824"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/results.json",
            "hash": "3420666192",
            "size": "1323"
        },
        {
            "filePath": "/home/jouven/qtcreatorprojects/hasherQtg/build-Desktop-Debug/window.o",
            "hash": "3347785733",
            "size": "3836096"
        }
    ],
    "hashFormat": "decimal",
    "hashType": "crc32c"
}
```
