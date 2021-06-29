#include "appConfig.hpp"
#include "mainWindow.hpp"

#include "signalProxyQtso/signalProxyQtso.hpp"

#include <QCommandLineParser>

void appConfig_c::derivedConfigureCommandLineParser_f(QCommandLineParser& parser_par) const
{
    parser_par.addPositionalArgument("saved results file", "Optional, path to a result files, it will be loaded at the start");
}

void appConfig_c::derivedCheckCommandLineParser_f(QCommandLineParser& parser_par)
{
    positionalArguments_pri = parser_par.positionalArguments();
}

QStringList appConfig_c::commandLinePositionalArguments_f() const
{
    return positionalArguments_pri;
}


void appConfig_c::derivedStart_f()
{
    signalso::signalProxy_ptr_ext = new signalso::signalProxy_c(qApp);

    QObject::connect(this, &appConfig_c::started_signal, mainWindow_ptr_ext, &mainWindow_c::start_f);
    QObject::connect(mainWindow_ptr_ext, &mainWindow_c::closeWindow_signal, this, &appConfig_c::quit_signal);

    loadConfigFile_f();
    tryLoadTranslations_f();
    loadLogging_f();

    //this class doesn't even read anything extra from the json configuration file, so don't quit when there is no json config
//    if (configLoaded_f())
//    {

//    }
//    else
//    {
//        //MACRO_ADDLOG("Config not loaded quitting", QString(), messageType_ec::error);
//        //messageUser_f({"Couldn't load {0} config file, pass \"-g\" argument to generate a config file documentation file", configFilePath_f()}, messageType_ec::informationrmation);
//        Q_EMIT quit_signal();
//    }
}

void appConfig_c::derivedQuit_f()
{
    MACRO_ADDLOG("Config file/s saved", QString(), messageType_ec::debug);
    saveConfigFile_f();
    saveTranslationFile_f();
}

appConfig_c::appConfig_c(QObject* parent_par)
    : programConfigGUI_c(parent_par)
{}

appConfig_c* appConfig_ptr_ext = nullptr;
