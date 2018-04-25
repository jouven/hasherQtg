//helper class/s to deal with the shortcomings of QTableWidget,
//because it only deals with rows and cells, it doesn't detect duplicates,
//it doesn't "map" where is anything, uses QVariant as underlying value,
//so... lets skip that, the Data and setData, and just use some extra mappings and have more functionality

#ifndef HASHERQTG_FILEDATA_HPP
#define HASHERQTG_FILEDATA_HPP

#include "criptoQtso/hashQt.hpp"

#include <QHash>
#include <QString>
#include <QFileInfo>
#include <QJsonObject>

#include <string>
#include <unordered_map>

//what goes in a row
class fileData_c
{
    //the id is a, fast, means to map the row position with the filePath
    int_fast64_t id_pri = 0;

    QString filePath_pri;
    int_fast64_t fileSize_pri = 0;
//    uint_fast64_t hashNumeric_pri = 0;
//    bool hashNumericSet_pri = false;
    std::string hashStr_pri;
    //bool hashStrSet_pri = false;
public:
    fileData_c();
    //id is automatically assigned each a time an fileData_c object is created,
    //first id is 0, afterwards is previous + 1
    fileData_c(
            const QString& filename_par_con
            , const int_fast64_t fileSize_par_con);

    void generateHash_f(const bool force_par_con = false);
    void clearHash_f();

    int_fast64_t id_f() const;
    QString filePath_f() const;
    int_fast64_t fileSize_f() const;
    //uint_fast64_t hashNumeric_f() const;
    //bool hashNumericSet_f() const;
    std::string hashStr_f() const;
    //bool hashStrSet_f() const;

    void write_f(QJsonObject &json) const;
    void read_f(const QJsonObject &json);

    void relativePathsTo_f(const QDir& saveFileParent_par_con);
};

//to de/serialize
class fileDataRoot_c
{
    std::vector<fileData_c> fileDataVector_pri;
    eines::hasher_c::hashType_ec hashType_pri = eines::hasher_c::hashType_ec::crc32c;
    eines::hasher_c::outputType_ec hashFormat_pri = eines::hasher_c::outputType_ec::decimalString;
public:
    fileDataRoot_c() = default;
    fileDataRoot_c(
            const std::vector<fileData_c>& fileDataVector_par_con
            , const eines::hasher_c::hashType_ec hashType_par_con
            , const eines::hasher_c::outputType_ec hashFormat_par_con);

    void write_f(QJsonObject &json) const;
    void read_f(const QJsonObject &json);
    std::vector<fileData_c> fileDataVector_f() const;
    eines::hasher_c::hashType_ec hashType_f() const;
    eines::hasher_c::outputType_ec hashFormat_f() const;

    void relativePathsTo_f(const QDir& saveFileParent_par_con);
};

//main hub of data
class fileDataController_c
{
    //key = fileData id, value = row number
    std::unordered_map<int_fast64_t, int> fileDataIdToRow_pri;
    //key = filePath, value = fileData id
    QHash<QString, int_fast64_t> filePathTofileDataIdUMap_pri;
    //key = fileData Id, value = fileData obj
    std::unordered_map<int_fast64_t, fileData_c> fileDataIdToFileDataUMap_pri;

    eines::hasher_c::hashType_ec hashType_pri = eines::hasher_c::hashType_ec::crc32c;
    eines::hasher_c::outputType_ec hashFormat_pri = eines::hasher_c::outputType_ec::decimalString;

public:
    fileDataController_c() = default;
    //returns true on insert, false if it just updates
    void addFileDataToUMap(const fileData_c& obj_par_con, const int row_par_con);

    //-1 if not found, >-1 otherwise
    int fileDataIdToRow_f(const int_fast64_t fileDataId_par_con) const;
    int_fast64_t filePathToFileDataId_f(const QString& filePath_con) const;

    std::unordered_map<int_fast64_t, fileData_c>::iterator fileData(const int_fast64_t fileDataId_par_con);

    //not necessary, given a row, the filename field can be checked so...
    //int fileDataIdToRow(const int_fast64_t fileDataId_par_con) const;
    //QString fileDataIdToFilePath(const int_fast64_t fileDataId_par_con) const;

    void clearAllFileData_f();
    bool removeFileData_f(const QString& filePath_par_con);
    //bool removeFileData_f(const int row_par_con);

    static std::vector<QString> getFilesFromDirectory(
            const QFileInfo& directory_par_con
            , const bool useAbsolutePaths_par_con = true
            , const bool useSlashSeparator_par_con = false
            , const bool includeSubdirectories_par_con = true
            , const QStringList& filenameFilters_par_con = QStringList()
            , const QString& includeDirectoriesWithFileX_par_con = QString());

    eines::hasher_c::hashType_ec hashType_f() const;
    void setHashType_f(const eines::hasher_c::hashType_ec hashType_par_con);
    eines::hasher_c::outputType_ec hashFormat_f() const;
    void setHashFormat_f(const eines::hasher_c::outputType_ec hashFormat_par_con);

    //OBSOLETED by std::unordered_map<int_fast64_t, fileData_c>::iterator fileData(const int_fast64_t fileDataId_par_con);
    //otherwise the hashing process was all or nothing, now is file by file
    //rehash means hash files which have hash values already
    //void hashAll_f(const bool rehash_par_con = true);
    //void hash_f(const QString& filePath_par_con);
    //QString currentHashedFile_f();
};

extern fileDataController_c& dataHub_f();

#endif // HASHERQTG_FILEDATA_HPP
