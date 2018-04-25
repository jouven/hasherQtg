#ifndef HASHERQTG_WINDOW_HPP
#define HASHERQTG_WINDOW_HPP

#include <QWidget>
#include <QFileInfo>
#include <QMutexLocker>
#include <QMap>

class QTableWidget;
class QComboBox;
class QPushButton;
class QLabel;
class QVBoxLayout;
class QRadioButton;
class QCheckBox;
#ifdef __ANDROID__
class QScrollArea;
#endif

class mainWindow_c : public QWidget
{
    Q_OBJECT

    QVBoxLayout* mainLayout_pri;
#ifdef __ANDROID__
    QScrollArea* scrollArea_pri;
    QWidget* baseWidget_pri;
#endif
    QLabel* statusBarLabel_pri;
    QTableWidget* fileTable_pri;
    QPushButton* clearFileListButton_pri;
    QPushButton* addFileSelectionToListButton_pri;
    QPushButton* addDirectoryToListButton_pri;
    QPushButton* hashListButton_pri;
    QPushButton* loadSavedListButton_pri;

    QPushButton* clearMatching_pri;
    QPushButton* saveListButton_pri;
    QPushButton* tipsButton_pri;
    QPushButton* aboutButton_pri;

    QRadioButton* hashTypeCrc32cRadio_pri;
    QRadioButton* hashTypeXXHashRadio_pri;
    QRadioButton* hashTypeSHA256Radio_pri;
    QRadioButton* hashTypeWhirlpoolRadio_pri;

    QRadioButton* hashFormatDecimalRadio_pri;
    QRadioButton* hashFormatHexadecimalRadio_pri;
    QRadioButton* hashFormatBase64Radio_pri;

    QCheckBox* useRelativePathOnSaveCheckBox_pri;
#ifdef Q_OS_WIN32
    QCheckBox* useSlashSeparatorCheckbox_pri;
#endif
    //QPushButton* exitButton_pri;

    //not required, it doesn't run on a different thread
    //if the user quits while saving since the GUI thread is the same as
    //the one doing the saving it will have to wait anyway
    //bool saving_pri = false;
    bool hashing_pri = false;
    bool allHashed_pri = false;
    //variables to control animations during hashing
    //. .. ...
    int statusBarHashingAnimationStatus_pri = 0;
    int hashingCurrentRow_pri = 0;
    //the current folder being hashed
    QString hashingCurrentFile_pri;
    //mutex because reading/writing hashingCurrentFolder_pri can happen at the same time
    QMutex hashingCurrentFolderQMutex_pri;

    int finalCounterSeconds_pri = 1;

    //variables and functions to control the directory history
    QMap<QString, uint_fast64_t> directoryPathToDateTime_pri;
    //lower values are first, ascending order (so time wise older are first),
    //delete old using first
    QMap<uint_fast64_t, QString> dateTimeToDirectoryPath_pri;

    void addDirectoryHistory_f(const QString& directory_par_con);
    std::vector<QString> directoryHistory_f() const;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void addDirectoryToList_f(const QFileInfo& dir_par_con);
    bool tryAddFileToList_f(
            const QString& filePath_par_con
            , const QString& savedHash_par_con = QString());
    void closeEvent(QCloseEvent* event) override;
    void clearHashFields_f();
    void changeHashTypeProg_f();
    void changeHashFormatProg_f();
    void hashRows_f(const std::vector<int>& rows_par_con);
    void loadFileList_f(const QStringList& fileList_par_con);
    void hashingStatusThread_f();
    void enableFormatTypeRadios_f(const bool enabled_par_con);
public:
    mainWindow_c();
    ~mainWindow_c();
    void processPositionalArguments_f(const QStringList& positionalArguments_par_con);
Q_SIGNALS:
    //text to set
    void setStatusBarText_signal_f(const QString&);
    //args are row, column and text to set
    void setRowCellField_signal_f(const int, const int, const QString&);
    void setHashRowCellField_signal_f(const int, const int, const QString&);
    //row index
    void scrollToItem_signal_f(const int);
    void saveAfterHash_signal_f();
    void resizeFileTable_signal_f();
private Q_SLOTS:
    void contextMenu(const QPoint &pos);
    //button actions
    void clearFileList_f();
    void addFileSelectionToList_f();
    void browseDirectoryToAdd_f();
    void hashList_f(const bool saveAfter_par_con = false);
    void dialogLoadFileList_f();
    void dialogSaveFileList_f();
    void showAboutMessage_f();
    void mainLoop_f();
    void setRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con);
    void setHashRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con);
    void scrollToItem_f(const int row_par_con);
    void changeHashType_f();
    void changeHashFormat_f();
    void resizeFileTable_f();
    //void exit_f();
    void showTips_f();
    void clearMachingRows_f();
};

#endif //HASHERQTG_WINDOW_HPP
