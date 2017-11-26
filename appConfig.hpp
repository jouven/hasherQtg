#ifndef HASHERQTG_APPCONFIG_HPP
#define HASHERQTG_APPCONFIG_HPP

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QJsonObject>

class appConfig_c
{
    bool configLoaded_pri = false;

    QByteArray windowGeometry_pri;
    bool windowGeometrySet_pri = false;

    QStringList selectedDirectoryHistory_pri;
    bool selectedDirectoryHistorySet_pri = false;

    void loadConfig_f();

    void read_f(const QJsonObject &json);
    void write_f(QJsonObject &json) const;
    bool anythingSet_f() const;
public:
    appConfig_c();

    bool saveConfigFile_f() const;

    bool configLoaded_f() const;

    QByteArray windowGeometry_f() const;
    void setWindowGeometry_f(const QByteArray& windowGeometry_par_con);
    bool windowGeometrySet_f() const;

    QStringList selectedDirectoryHistory_f() const;
    void setSelectedDirectoryHistory_f(const QStringList& selectedDirectoryHistory_par_con);
    bool selectedDirectoryHistorySet_f() const;

};

extern appConfig_c& appConfig_f();

#endif // HASHERQTG_APPCONFIG_HPP
