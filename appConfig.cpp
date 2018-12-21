#include "appConfig.hpp"

//for the config file location
#include "essentialQtso/essentialQt.hpp"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>

void appConfig_c::read_f(const QJsonObject& json)
{
    if (not json["windowGeometry"].isNull())
    {
        QByteArray qByteArrayTmp;
        qByteArrayTmp.append(json["windowGeometry"].toString());
        windowGeometry_pri = qUncompress(QByteArray::fromBase64(qByteArrayTmp));
        windowGeometrySet_pri = true;
    }

    QJsonArray jsonArraySelectedDirectoryHistoryTmp(json["selectedDirectoryHistory"].toArray());
    if (not jsonArraySelectedDirectoryHistoryTmp.isEmpty())
    {
        selectedDirectoryHistory_pri.reserve(jsonArraySelectedDirectoryHistoryTmp.size());
        for (const auto& jsonArrayItem_ite_con : jsonArraySelectedDirectoryHistoryTmp)
        {
            selectedDirectoryHistory_pri.append(jsonArrayItem_ite_con.toString());
        }
        selectedDirectoryHistorySet_pri = true;
    }
}

void appConfig_c::write_f(QJsonObject& json) const
{
    if (not selectedDirectoryHistory_pri.isEmpty())
    {
        QJsonArray jsonArraySelectedDirectoryHistoryTmp;
        for (const QString& directoryItem_ite_con : selectedDirectoryHistory_pri)
        {
            jsonArraySelectedDirectoryHistoryTmp.append(QJsonValue(directoryItem_ite_con));
        }
        json["selectedDirectoryHistory"] = jsonArraySelectedDirectoryHistoryTmp;
    }
    QString qStringTmp;
    qStringTmp.append(qCompress(windowGeometry_pri).toBase64());
    json["windowGeometry"] = qStringTmp;
}

bool appConfig_c::anythingSet_f() const
{
    return selectedDirectoryHistorySet_pri or windowGeometrySet_pri;
}

appConfig_c::appConfig_c()
{
    //no errors here, load if possible else skip
    //load the file
    while (configFilePath_f().second)
    {
        QFile configFileLoad(configFilePath_f().first);
        QByteArray jsonByteArray;
        if (configFileLoad.open(QIODevice::ReadOnly))
        {
            jsonByteArray = configFileLoad.readAll();
        }
        else
        {
            break;
        }

        QJsonDocument jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
        if (jsonDocObj.isNull())
        {
            break;
        }
        else
        {
            read_f(jsonDocObj.object());
            configLoaded_pri = true;
        }
        break;
    }
}


bool appConfig_c::saveConfigFile_f() const
{
    bool configSavedTmp(false);
    if (anythingSet_f())
    {
        QString configFileStr;
        //save file
        if (configFilePath_f().second)
        {
            configFileStr = configFilePath_f().first;
        }
        else
        {
            configFileStr = fileTypePath_f(fileTypes_ec::config);
        }

        QFile configFileSaveTmp(configFileStr);
        if (configFileSaveTmp.open(QIODevice::WriteOnly))
        {
            QJsonObject jsonObjectTmp;
            write_f(jsonObjectTmp);
            QJsonDocument jsonDocumentTmp(jsonObjectTmp);

            configFileSaveTmp.write(jsonDocumentTmp.toJson(QJsonDocument::Indented));
            configSavedTmp = true;
        }
    }
    return configSavedTmp;
}

bool appConfig_c::configLoaded_f() const
{
    return configLoaded_pri;
}

QByteArray appConfig_c::windowGeometry_f() const
{
    return windowGeometry_pri;
}

void appConfig_c::setWindowGeometry_f(const QByteArray& windowGeometry_par_con)
{
    windowGeometry_pri = windowGeometry_par_con;
    windowGeometrySet_pri = true;
}

bool appConfig_c::windowGeometrySet_f() const
{
    return windowGeometrySet_pri;
}

QStringList appConfig_c::selectedDirectoryHistory_f() const
{
    return selectedDirectoryHistory_pri;
}

void appConfig_c::setSelectedDirectoryHistory_f(const QStringList& selectedDirectoryHistory_par_con)
{
    selectedDirectoryHistory_pri = selectedDirectoryHistory_par_con;
    selectedDirectoryHistorySet_pri = true;
}

bool appConfig_c::selectedDirectoryHistorySet_f() const
{
    return selectedDirectoryHistorySet_pri;
}

appConfig_c&appConfig_f()
{
    static appConfig_c appConfig_sta;
    return appConfig_sta;
}
