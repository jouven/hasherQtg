#include "window.hpp"

#include "signalso/signal.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QApplication>

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
    QApplication::setApplicationName("hasherQtg");
    QApplication::setApplicationVersion("1.0");

    QStringList positionalArgs;
    {
        QCommandLineParser commandLineParser;
        commandLineParser.setApplicationDescription("hasherQtg, GUI program to generate hashes, save the results to a file and compare the hashes from file to the current values");
        commandLineParser.addHelpOption();
        commandLineParser.addVersionOption();
        commandLineParser.addPositionalArgument("saved results file", "Optional, path to a result files, it will be loaded at the start");

        commandLineParser.process(*qApp);
        positionalArgs = commandLineParser.positionalArguments();

        locateConfigFilePath_f(commandLineParser, false);
    }

    Window_c window;
    window.show();
    window.processPositionalArguments_f(positionalArgs);
    returnValue_ext = qtapp.exec();

    if (eines::signal::isRunning_f())
    {
        eines::signal::stopRunning_f();
    }
    while (not eines::signal::isTheEnd_f())
    {}

    return returnValue_ext;
}
