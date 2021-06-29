#ifndef HASHERQTG_APPCONFIG_HPP
#define HASHERQTG_APPCONFIG_HPP

#include "programConfigQtgso/programConfigGUI.hpp"

class appConfig_c : public programConfigGUI_c
{
    Q_OBJECT

    QStringList positionalArguments_pri;

    void derivedReadJSON_f(const QJsonObject &json_par_con) override
    {}
    void derivedWriteJSONDocumented_f(QJsonObject &json_par) const override
    {}
    void derivedWriteJSON_f(QJsonObject &) const override
    {}
    bool firstPositionalArgumentCanBeConfig_f() const override
    {
        return false;
    }
    bool translationCreatePlacerholders_f() const override
    {
        return true;
    }
    bool loggingSaveLogFileRequired_f() const override
    {
        return true;
    }
    QString programName_f() const override
    {
        return "hasherQtg";
    }
    text_c programDescription_f() const override
    {
        return "hasherQtg, GUI program to generate hashes, save the results to a file and compare the hashes from file to the current values\nCreated by Jouven";
    }
    QString programVersion_f() const override
    {
        return "1.0";
    }

    bool requiresJSONDocumented_f() const override
    {
        return true;
    }
    void derivedConfigureCommandLineParser_f(QCommandLineParser& parser_par) const override;
    void derivedCheckCommandLineParser_f(QCommandLineParser& parser_par) override;
    void derivedStart_f() override;
    void derivedQuit_f() override;
public:
    appConfig_c(QObject* parent_par);

    QStringList commandLinePositionalArguments_f() const;
};

extern appConfig_c* appConfig_ptr_ext;

#endif // HASHERQTG_APPCONFIG_HPP
