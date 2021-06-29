#include "mainWindow.hpp"
#include "appConfig.hpp"

#include "signalso/signal.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QApplication>

#ifdef DEBUGJOUVEN
#include <QDebug>
#ifndef Q_OS_WIN
//this is to get pretty stacktrace when the execution crashes
//instructions:
//1 this only applies to program projects, libs don't need this (libs need debug, -gX flags when compiling)
//2 link to -ldw or the elftutils library
//3 set the DEFINES in the .pro BACKWARD_HAS_UNWIND BACKWARD_HAS_DW (check backward.hpp source for more info about the macros)
//more info https://github.com/bombela/backward-cpp
#include "backward-cpp/backward.hpp"
namespace {
backward::SignalHandling sh;
}
#endif
#endif

int main(int argc, char *argv[])
{
    MACRO_signalHandler
    //qSetMessagePattern(QString("[%{type}] %{appname} (%{file}:%{line}) - %{message} %{backtrace}"));
#ifdef __ANDROID__
    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    //QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    //qputenv("QT_SCALE_FACTOR", "0.6");
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "true");
#endif

    QApplication qtapp(argc, argv);

    mainWindow_c mainWindowTmp;
    mainWindow_ptr_ext = std::addressof(mainWindowTmp);

    appConfig_c appConfigTmp(nullptr);
    appConfig_ptr_ext = std::addressof(appConfigTmp);

    returnValue_ext = qtapp.exec();

    return returnValue_ext;
}
