#include "fileData.hpp"

#include "cryptoQtso/hashQt.hpp"
#include "signalso/signal.hpp"

#include <QJsonArray>
#include <QDir>
#ifdef DEBUGJOUVEN
#include <QDebug>
#endif

int_fast64_t nextFileDataId_f()
{
    static int_fast64_t rootId(0);
    rootId = rootId + 1;
    return rootId;
}

//////////////////
//fileData_c
//////////////////
QString fileData_c::filePath_f() const
{
    return filePath_pri;
}

int_fast64_t fileData_c::fileSize_f() const
{
    return fileSize_pri;
}

//uint_fast64_t fileData_c::hashNumeric_f() const
//{
//    return hashNumeric_pri;
//}

//bool fileData_c::hashNumericSet_f() const
//{
//    return hashNumericSet_pri;
//}

std::string fileData_c::hashStr_f() const
{
    return hashStr_pri;
}

//bool fileData_c::hashStrSet_f() const
//{
//    return hashStrSet_pri;
//}

void fileData_c::write_f(QJsonObject& json) const
{
    json["filePath"] = filePath_pri;
    //json/javascript can't fit 64bit numbers, IEEE 754 shit, which is floating point, not integer, so it's less than a 64bit integer
    //so... use string notation
    json["size"] = QString::number(fileSize_pri);
//    if (hashNumericSet_f())
//    {
//        json["hash"] = QString::number(hashNumeric_pri);
//    }
//    if (hashStrSet_f())
//    {
    if (not hashStr_pri.empty())
    {
        json["hash"] = QString::fromStdString(hashStr_pri);
    }
    //}
}

void fileData_c::read_f(const QJsonObject& json)
{
    filePath_pri = json["filePath"].toString();
    fileSize_pri = json["size"].toInt();
//    if (outputType_par_con == eines::hasher_c::outputType_ec::unsigned64bitInteger)
//    {
//        hashNumeric_pri = json["hash"].toInt();
//    }
//    else
//    {
    if (not json["hash"].isUndefined())
    {
        hashStr_pri = json["hash"].toString().toStdString();
    }
        //    }
}

void fileData_c::relativePathsTo_f(const QDir& saveFileParent_par_con)
{
    filePath_pri = saveFileParent_par_con.relativeFilePath(filePath_pri);
}

fileData_c::fileData_c()
    : id_pri(nextFileDataId_f())
{}

fileData_c::fileData_c(
        const QString& filename_par_con
        , const int_fast64_t fileSize_par_con)
    : id_pri(nextFileDataId_f())
    , filePath_pri(filename_par_con)
    , fileSize_pri(fileSize_par_con)
{}


void fileData_c::generateHash_f(
        const bool force_par_con)
{
    //if forced or none of the hashes are set
    if (force_par_con or hashStr_f().empty())
    {
        eines::hasher_c hasher
        (
            eines::hasher_c::inputType_ec::file
            , filePath_pri
            , dataHub_f().hashFormat_f()
            , dataHub_f().hashType_f()
        );
        hasher.generateHash_f();
        //qInfo() << "hasher.getError_f() " <<  hasher.getError_f() << endl;
//        if (hasher.hashNumberResultSet_f())
//        {
//            hashNumeric_pri = hasher.hashNumberResult_f();
//            hashNumericSet_pri = true;
//            hashStrSet_pri = false;
//            hashStr_pri.clear();
//        }
        //qInfo() << "hasher.hashStringResult_f() " <<  QString::fromStdString(hasher.hashStringResult_f()) << endl;
        //qInfo() << "hasher.hashStringResultSet_f() " <<  hasher.hashStringResultSet_f() << endl;
        if (hasher.hashStringResultSet_f())
        {
            hashStr_pri = hasher.hashStringResult_f();
            //hashStrSet_pri = true;
            //hashNumericSet_pri = false;
        }
    }
}

void fileData_c::clearHash_f()
{
    hashStr_pri.clear();
}

int_fast64_t fileData_c::id_f() const
{
    return id_pri;
}

//////////////////
//fileDataController_c
//////////////////
eines::hasher_c::hashType_ec fileDataController_c::hashType_f() const
{
    return hashType_pri;
}

std::vector<QString> fileDataController_c::getFilesFromDirectory(
        const QFileInfo& directory_par_con
        , const bool useAbsolutePaths_par_con
        , const bool useSlashSeparator_par_con
        , const bool includeSubdirectories_par_con
        , const QStringList& filenameFilters_par_con
        , const QString& includeDirectoriesWithFileX_par_con
)
{
    std::vector<QString> result;
    if (not directory_par_con.exists() or not directory_par_con.isDir())
    {
        return result;
    }

    QDir sourceDir(directory_par_con.absoluteFilePath());

    //if the setting is empty
    //account it as it is found
    bool xFileFoundRoot(includeDirectoriesWithFileX_par_con.isEmpty());
    if (not xFileFoundRoot)
    {
        //source dir, get the root files
        QStringList rootFileListUnfiltered(sourceDir.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot));
        for (const QString& filename_ite_con : rootFileListUnfiltered)
        {
            if (filename_ite_con == includeDirectoriesWithFileX_par_con)
            {
                xFileFoundRoot = true;
                break;
            }
        }
    }
    if (xFileFoundRoot)
    {
        //source dir, get the root files
        QStringList rootFileList(sourceDir.entryList(filenameFilters_par_con, QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot, QDir::Name));
        for (const QString& filename_ite_con : rootFileList)
        {
#ifdef DEBUGJOUVEN
            //QOUT_TS("sourceDir.path() + '/' + filename_ite_con " << sourceDir.path() + '/' + filename_ite_con << endl);
#endif
            QFileInfo sourceFileTmp(sourceDir.path() + '/' + filename_ite_con);
            if (sourceFileTmp.exists() and sourceFileTmp.isFile())
            {
                QString filePathTmp;
                if (useAbsolutePaths_par_con)
                {
                    filePathTmp = useSlashSeparator_par_con ? sourceFileTmp.filePath() : QDir::toNativeSeparators(sourceFileTmp.filePath());
                }
                else
                {
                    filePathTmp = useSlashSeparator_par_con ? sourceDir.relativeFilePath(sourceFileTmp.filePath()) : QDir::toNativeSeparators(sourceDir.relativeFilePath(sourceFileTmp.filePath()));
                }
                result.emplace_back(filePathTmp);
            }
        }
    }

    if (includeSubdirectories_par_con)
    {
        //source dir, get the all the subfolders (names) of the base folder
        QStringList subfolders(sourceDir.entryList(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks));

        //while subfolders keep being found
        while (not subfolders.isEmpty())
        {
            QStringList newSubfoldersTmp;
            //for every subfolder gathered
            for (const auto& subfolder_ite_con : subfolders)
            {
                //do a QDir of the subfolder, using the initial folder + all the subfolder "depth" that has been traveled/iterated
                QDir currentSubfolderDirTmp(directory_par_con.absoluteFilePath() + '/' + subfolder_ite_con);

                //get the subfolders of the one it's iterating
                QStringList subFoldersTmp(currentSubfolderDirTmp.entryList(QDir::Dirs | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks));
                //for the found subfolder, prepend the previous subfolder path string
                for (auto& subfolderTmp_ite : subFoldersTmp)
                {
                    //qDebug() << "subfolder_ite_con + QDir::separator() + subfolderTmp_ite " << subfolder_ite_con + QDir::separator() + subfolderTmp_ite << endl;
                    //prepend the parent subfolder
                    subfolderTmp_ite.prepend(subfolder_ite_con + "/");
                }
                newSubfoldersTmp.append(subFoldersTmp);

                //if the setting is empty
                //account it as it is found
                bool xFileFoundSubdirectory(includeDirectoriesWithFileX_par_con.isEmpty());
                if (not xFileFoundSubdirectory)
                {
                    //get the files of the subfolder
                    QStringList subDirectoryFileListUnfiltered(currentSubfolderDirTmp.entryList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot));
                    for (const QString& filename_ite_con : subDirectoryFileListUnfiltered)
                    {
                        if (filename_ite_con == includeDirectoriesWithFileX_par_con)
                        {
                            xFileFoundSubdirectory = true;
                            break;
                        }
                    }
                }

                if (xFileFoundSubdirectory)
                {
                    //get the files of the subfolder
                    QStringList subDirectoryFileList(currentSubfolderDirTmp.entryList(filenameFilters_par_con, QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot, QDir::Name));
                    for (const auto& filename_ite_con : subDirectoryFileList)
                    {
                        QFileInfo sourceFileTmp(sourceDir.path() + '/' + subfolder_ite_con + '/' + filename_ite_con);
                        if (sourceFileTmp.exists() and sourceFileTmp.isFile())
                        {
                            QString filePathTmp;
                            if (useAbsolutePaths_par_con)
                            {
                                filePathTmp = useSlashSeparator_par_con ? sourceFileTmp.filePath() : QDir::toNativeSeparators(sourceFileTmp.filePath());
                            }
                            else
                            {
                                filePathTmp = useSlashSeparator_par_con ? sourceDir.relativeFilePath(sourceFileTmp.filePath()) : QDir::toNativeSeparators(sourceDir.relativeFilePath(sourceFileTmp.filePath()));
                            }
                            result.emplace_back(filePathTmp);
                        }
                    }
                }
            }
            subfolders = newSubfoldersTmp;
        }
    }

    return result;
}

void fileDataController_c::setHashType_f(const eines::hasher_c::hashType_ec hashType_par_con)
{
    hashType_pri = hashType_par_con;
}

eines::hasher_c::outputType_ec fileDataController_c::hashFormat_f() const
{
    return hashFormat_pri;
}

void fileDataController_c::setHashFormat_f(const eines::hasher_c::outputType_ec hashFormat_par_con)
{
    hashFormat_pri = hashFormat_par_con;
}

//void fileDataController_c::hashAll_f(const bool rehash_par_con)
//{
//    for (std::pair<const int_fast64_t, fileData_c>& fileData_ite : fileDataIdToFileDataUMap_pri)
//    {
//        if (not signalso::isRunning_f())
//        {
//            break;
//        }
//        {
//            QMutexLocker tmp(&currentHashedFileQMutex_pri);
//            currentHashedFile_pri = fileData_ite.second.filePath_f();
//        }
//        //it's already checked in generateHash_f
////        if ((fileData_ite.second.hashNumericSet_f() or fileData_ite.second.hashStrSet_f()) and not rehash_par_con)
////        {
////            //if already set and not rehash
////            //nothing
////        }
////        else
////        {
//            fileData_ite.second.generateHash_f(rehash_par_con);
////        }
//    }
//    {
//        QMutexLocker tmp(&currentHashedFileQMutex_pri);
//        currentHashedFile_pri.clear();
//    }
//}

//void fileDataController_c::hash_f(const QString& filePath_par_con)
//{

//}

//QString fileDataController_c::currentHashedFile_f()
//{
//    QMutexLocker tmp(&currentHashedFileQMutex_pri);
//    return currentHashedFile_pri;
//}

void fileDataController_c::addFileDataToUMap(
        const fileData_c& obj_par_con
        , const int row_par_con)
{
    fileDataIdToRow_pri[obj_par_con.id_f()] = row_par_con;
    filePathTofileDataIdUMap_pri.insert(obj_par_con.filePath_f(), obj_par_con.id_f());
    fileDataIdToFileDataUMap_pri[obj_par_con.id_f()] = obj_par_con;
}

int fileDataController_c::fileDataIdToRow_f(const int_fast64_t fileDataId_par_con) const
{
    int result(-1);
    auto findResult(fileDataIdToRow_pri.find(fileDataId_par_con));
    if (findResult != fileDataIdToRow_pri.end())
    {
        result = findResult->second;
    }
    return result;
}

int_fast64_t fileDataController_c::filePathToFileDataId_f(const QString& filePath_con) const
{
    int_fast64_t result(-1);
    auto findResult(filePathTofileDataIdUMap_pri.find(filePath_con));
    if (findResult != filePathTofileDataIdUMap_pri.end())
    {
        result = findResult.value();
    }
    return result;
}

std::unordered_map<int_fast64_t, fileData_c>::iterator fileDataController_c::fileData(const int_fast64_t fileDataId_par_con)
{
    return fileDataIdToFileDataUMap_pri.find(fileDataId_par_con);
//    bool found(false);
//    fileData_c result;
//    auto findResult(fileDataIdToFileDataUMap_pri.find(fileDataId_par_con));
//    if (findResult != fileDataIdToFileDataUMap_pri.end())
//    {
//        result = findResult->second;
//        found = true;
//    }
    //    return {result, found};
}

void fileDataController_c::clearAllFileData_f()
{
    fileDataIdToRow_pri.clear();
    filePathTofileDataIdUMap_pri.clear();
    fileDataIdToFileDataUMap_pri.clear();
}

bool fileDataController_c::removeFileData_f(const QString& filePath_par_con)
{
    auto fileDataId(filePathTofileDataIdUMap_pri.value(filePath_par_con));
    fileDataIdToRow_pri.erase(fileDataId);
    auto result(fileDataIdToFileDataUMap_pri.erase(fileDataId) > 0);
    filePathTofileDataIdUMap_pri.remove(filePath_par_con);
    return result;
}

//int fileDataController_c::fileDataIdToRow(const int_fast64_t fileDataId_par_con) const
//{

//}

//QString fileDataController_c::fileDataIdToFilePath(const int_fast64_t fileDataId_par_con) const
//{

//}

fileDataController_c& dataHub_f()
{
    static fileDataController_c catalog_sta;
    return catalog_sta;
}

std::vector<fileData_c> fileDataRoot_c::fileDataVector_f() const
{
    return fileDataVector_pri;
}

eines::hasher_c::hashType_ec fileDataRoot_c::hashType_f() const
{
    return hashType_pri;
}

eines::hasher_c::outputType_ec fileDataRoot_c::hashFormat_f() const
{
    return hashFormat_pri;
}

void fileDataRoot_c::relativePathsTo_f(const QDir& saveFileParent_par_con)
{
    for (fileData_c& fileData_ite : fileDataVector_pri)
    {
        fileData_ite.relativePathsTo_f(saveFileParent_par_con);
    }
}

fileDataRoot_c::fileDataRoot_c(const std::vector<fileData_c>& fileDataVector_par_con
                               , const eines::hasher_c::hashType_ec hashType_par_con
            , const eines::hasher_c::outputType_ec hashFormat_par_con)
        : fileDataVector_pri(fileDataVector_par_con)
        , hashType_pri(hashType_par_con)
        , hashFormat_pri(hashFormat_par_con)
{}

void fileDataRoot_c::write_f(QJsonObject& json) const
{
    if (hashType_pri == eines::hasher_c::hashType_ec::crc32c)
    {
        json["hashType"] = "crc32c";
    }
    if (hashType_pri == eines::hasher_c::hashType_ec::SHA256)
    {
        json["hashType"] = "SHA256";
    }
    if (hashType_pri == eines::hasher_c::hashType_ec::whirlpool)
    {
        json["hashType"] = "whirlpool";
    }
    if (hashType_pri == eines::hasher_c::hashType_ec::XXHASH64)
    {
        json["hashType"] = "XXHASH64";
    }

    if (hashFormat_pri == eines::hasher_c::outputType_ec::decimalString)
    {
        json["hashFormat"] = "decimal";
    }
    if (hashFormat_pri == eines::hasher_c::outputType_ec::hexadecimalString)
    {
        json["hashFormat"] = "hex";
    }
    if (hashFormat_pri == eines::hasher_c::outputType_ec::base64String)
    {
        json["hashFormat"] = "base64";
    }
    QJsonArray filesArrayTmp;
    for (const fileData_c& fileData_ite_con : fileDataVector_pri)
    {
        QJsonObject jsonObjectTmp;
        fileData_ite_con.write_f(jsonObjectTmp);
        filesArrayTmp.append(jsonObjectTmp);
    }
    json["files"] = filesArrayTmp;
}

void fileDataRoot_c::read_f(const QJsonObject& json)
{
    if (json["hashFormat"].toString() == "decimal")
    {
        hashFormat_pri = eines::hasher_c::outputType_ec::decimalString;
    }
    if (json["hashFormat"].toString() == "hex")
    {
        hashFormat_pri = eines::hasher_c::outputType_ec::hexadecimalString;
    }
    if (json["hashFormat"].toString() == "base64")
    {
        hashFormat_pri = eines::hasher_c::outputType_ec::base64String;
    }

    if (json["hashType"].toString() == "crc32c")
    {
        hashType_pri = eines::hasher_c::hashType_ec::crc32c;
    }
    if (json["hashType"].toString() == "SHA256")
    {
        hashType_pri = eines::hasher_c::hashType_ec::SHA256;
    }
    if (json["hashType"].toString() == "whirlpool")
    {
        hashType_pri = eines::hasher_c::hashType_ec::whirlpool;
    }
    if (json["hashType"].toString() == "XXHASH64")
    {
        hashType_pri = eines::hasher_c::hashType_ec::XXHASH64;
    }

    QJsonArray arrayTmp(json["files"].toArray());
    fileDataVector_pri.reserve(arrayTmp.size());
    for (const auto& item_ite_con : arrayTmp)
    {
        QJsonObject fileDataJsonObject = item_ite_con.toObject();
        fileData_c fileDataTmp;
        fileDataTmp.read_f(fileDataJsonObject);
        fileDataVector_pri.emplace_back(fileDataTmp);
    }
}
