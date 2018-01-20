//1 is QT_NO_* (like QT_NO_CLIPBOARD) neccessary anymore?, the define doesn't lead anywhere
//searching in google/qt site doesn't provide any platform that needs it
//2 add icons, right now in *nix it uses the current loaded theme ones, but in windows
//this will not fly
#include "window.hpp"

#include "fileData.hpp"
#include "appConfig.hpp"

#include "threadedFunctionQtso/threadedFunctionQt.hpp"
#include "signalso/signal.hpp"
#include "sizeConversionso/byte.hpp"

#include <QtWidgets>
#include <QSet>

#include <chrono>
#include <unordered_set>

void Window_c::closeEvent(QCloseEvent* event)
{
    if (eines::signal::isRunning_f())
    {
        eines::signal::stopRunning_f();
    }
    event->ignore();
}

void Window_c::clearHashFields_f()
{
    for (auto rowIndex_ite = 0, l = fileTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        if (not fileTable_pri->item(rowIndex_ite, 2)->text().isEmpty())
        {
            fileTable_pri->item(rowIndex_ite, 2)->setText("");
            QString filePathTmp(fileTable_pri->item(rowIndex_ite, 0)->text());
            auto fileDataId(dataHub_f().filePathToFileDataId_f(filePathTmp));
            std::unordered_map<int_fast64_t, fileData_c>::iterator fileDataPairTmp(dataHub_f().fileData(fileDataId));
            fileDataPairTmp->second.clearHash_f();
        }
    }
}

void Window_c::resizeFileTable_f()
{
    fileTable_pri->resizeColumnsToContents();
    fileTable_pri->adjustSize();
    //scrollArea_pri->adjustSize();
    //adjustSize();
//#ifndef __ANDROID__
    QTimer::singleShot(100,[=]
    {
#ifdef __ANDROID__
        baseWidget_pri->adjustSize();
#else
        adjustSize();
#endif

        //fileTable_pri->adjustSize();
        //fileTable_pri->resizeColumnsToContents();
    });
//#endif
}

Window_c::Window_c()
{
    const QRect screenGeometry = QApplication::desktop()->availableGeometry(this);
    statusBarLabel_pri = new QLabel;

    statusBarLabel_pri->setText(tr("Files/folders can be drop inside the window to add them to the list"));

    QGridLayout* centralLayout = new QGridLayout;

    //clear file list
    QIcon newIcon(QIcon::fromTheme("document-new", QIcon(":/images/new.png")));
    clearFileListButton_pri = new QPushButton(newIcon, tr("Clear List"));
    //QFont defaultButtonFont(clearFileListButton_pri->font());
    //defaultButtonFont.setPointSize(16);
    //clearFileListButton_pri->setFont(defaultButtonFont);

    centralLayout->addWidget(clearFileListButton_pri, 0, 2);
    QObject::connect(clearFileListButton_pri, &QPushButton::clicked, this, &Window_c::clearFileList_f);

    //add file selection
    //QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/browse.png"));
    addFileSelectionToListButton_pri = new QPushButton(tr("Add file/s to List"));
    //addFileSelectionToListButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(addFileSelectionToListButton_pri, 0, 0);
    QObject::connect(addFileSelectionToListButton_pri, &QPushButton::clicked, this, &Window_c::addFileSelectionToList_f);

    //add directory
    //QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/browse.png"));
    addDirectoryToListButton_pri = new QPushButton(tr("Add directory contents to List"));
    //addDirectoryToListButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(addDirectoryToListButton_pri, 0, 1);
    QObject::connect(addDirectoryToListButton_pri, &QPushButton::clicked, this, &Window_c::browseDirectoryToAdd_f);

    //hash list
    //QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/hash.png"));
    hashListButton_pri = new QPushButton(tr("Hash List"));
    //hashListButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(hashListButton_pri, 1, 0);
    QObject::connect(hashListButton_pri, &QPushButton::clicked, this, &Window_c::hashList_f);

    //load list from file
    QIcon loadIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    loadSavedListButton_pri = new QPushButton(loadIcon, tr("Load saved List"));
    //loadSavedListButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(loadSavedListButton_pri, 1, 1);
    QObject::connect(loadSavedListButton_pri, &QPushButton::clicked, this, &Window_c::dialogLoadFileList_f);

    //clear matching
    clearMatching_pri = new QPushButton(tr("Clear matching"));
    //clearMatching_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(clearMatching_pri, 1, 2);
    QObject::connect(clearMatching_pri, &QPushButton::clicked, this, &Window_c::clearMachingRows_f);

    //save (and hash if necessary) list into a file
    QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    saveListButton_pri = new QPushButton(saveIcon, tr("Save List"));
    //saveListButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(saveListButton_pri, 2, 0);
    QObject::connect(saveListButton_pri, &QPushButton::clicked, this, &Window_c::dialogSaveFileList_f);

    //check use relative path on save
    //QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/images/about.png"));
    useRelativePathOnSaveCheckBox_pri = new QCheckBox(tr("Save with relative Paths"));
    //useRelativePathOnSave_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(useRelativePathOnSaveCheckBox_pri, 2, 1);

#ifdef Q_OS_WIN32
    useSlashSeparatorCheckbox_pri = new QCheckBox(tr("Use slash as folder separator"));
    centralLayout->addWidget(useSlashSeparatorCheckbox_pri, 2, 2);
#endif

    //tips button
    //QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/images/about.png"));
    tipsButton_pri = new QPushButton(tr("Tips"));
    //aboutButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(tipsButton_pri, 3, 0);
    QObject::connect(tipsButton_pri, &QPushButton::clicked, this, &Window_c::showTips_f);

    //about button
    QIcon aboutIcon = QIcon::fromTheme("help-about", QIcon(":/images/about.png"));
    aboutButton_pri = new QPushButton(aboutIcon, tr("About"));
    //aboutButton_pri->setFont(defaultButtonFont);
    centralLayout->addWidget(aboutButton_pri, 3, 1);
    QObject::connect(aboutButton_pri, &QPushButton::clicked, this, &Window_c::showAboutMessage_f);

    //hash type radio
    hashTypeCrc32cRadio_pri = new QRadioButton(tr("crc32c"));
    hashTypeCrc32cRadio_pri->setChecked(true);
    hashTypeXXHashRadio_pri = new QRadioButton(tr("xxhash"));
    hashTypeSHA256Radio_pri = new QRadioButton(tr("SHA256"));
    hashTypeWhirlpoolRadio_pri = new QRadioButton(tr("Whirlpool"));

    //hash type hbox
    QHBoxLayout *hboxHashType = new QHBoxLayout;
    hboxHashType->setContentsMargins(0,0,0,0);

    //hash type radios
    hboxHashType->addWidget(hashTypeCrc32cRadio_pri);
    hboxHashType->addWidget(hashTypeXXHashRadio_pri);
    hboxHashType->addWidget(hashTypeSHA256Radio_pri);
    hboxHashType->addWidget(hashTypeWhirlpoolRadio_pri);

    //hash type group
    QGroupBox* hashTypeGroupBoxTmp = new QGroupBox(tr("Hash type"));
    hashTypeGroupBoxTmp->setAlignment(Qt::AlignLeft);
    hashTypeGroupBoxTmp->setFlat(true);
    hashTypeGroupBoxTmp->setLayout(hboxHashType);

    centralLayout->addWidget(hashTypeGroupBoxTmp, 4, 0, 1, 3);

    //hash format radios
    hashFormatDecimalRadio_pri = new QRadioButton(tr("decimal"));
    hashFormatDecimalRadio_pri->setChecked(true);
    hashFormatHexadecimalRadio_pri = new QRadioButton(tr("hexadecimal"));
    hashFormatBase64Radio_pri = new QRadioButton(tr("base64"));
    //hash format hbox
    QHBoxLayout *hboxHashFormat = new QHBoxLayout;
    hboxHashFormat->setContentsMargins(0,0,0,0);

    hboxHashFormat->addWidget(hashFormatDecimalRadio_pri);
    hboxHashFormat->addWidget(hashFormatHexadecimalRadio_pri);
    hboxHashFormat->addWidget(hashFormatBase64Radio_pri);

    //hash format group
    QGroupBox* hashFormatGroupBoxTmp = new QGroupBox(tr("Hash format"));
    hashFormatGroupBoxTmp->setAlignment(Qt::AlignLeft);
    hashFormatGroupBoxTmp->setFlat(true);
    hashFormatGroupBoxTmp->setLayout(hboxHashFormat);

    centralLayout->addWidget(hashFormatGroupBoxTmp, 5, 0, 1, 3);

    fileTable_pri = new QTableWidget(0, 5);
    fileTable_pri->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("File Path") << tr("Size") << tr("Current Hash") << tr("Saved Hash") << tr("Match?");
    fileTable_pri->setHorizontalHeaderLabels(labels);
    //fileTable_pri->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    fileTable_pri->setShowGrid(true);
    //warning hangs the program when hashing
    //fileTable_pri->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    fileTable_pri->setContextMenuPolicy(Qt::CustomContextMenu);
    fileTable_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
#ifdef __ANDROID__
    fileTable_pri->setMinimumHeight(screenGeometry.height() / 3);
#endif
    connect(fileTable_pri, &QTableWidget::customContextMenuRequested,
            this, &Window_c::contextMenu);
#ifdef __ANDROID__
    baseWidget_pri = new QWidget(this);
    baseWidget_pri->setAcceptDrops(true);
#else
    this->setAcceptDrops(true);
#endif
    centralLayout->addWidget(fileTable_pri, 6, 0, 1, 3);

    mainLayout_pri = new QVBoxLayout;
    mainLayout_pri->addLayout(centralLayout);
    mainLayout_pri->addWidget(statusBarLabel_pri);
#ifdef __ANDROID__
    baseWidget_pri->setLayout(mainLayout_pri);
#else
    this->setLayout(mainLayout_pri);
#endif

#ifdef __ANDROID__
    scrollArea_pri = new QScrollArea(this);
    scrollArea_pri->setWidget(baseWidget_pri);
    //scrollArea_pri->verticalScrollBar()->setFixedWidth(screenGeometry.width() * 0.05);
    //scrollArea_pri->horizontalScrollBar()->setFixedHeight(screenGeometry.height() * 0.04);
    scrollArea_pri->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    scrollArea_pri->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //scrollArea_pri->setAlignment(Qt::AlignBottom | Qt::AlignRight);
    scrollArea_pri->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea_pri->resize(screenGeometry.width(), screenGeometry.height());
    //scrollArea_pri->setWidgetResizable(true);
#endif


    setWindowTitle(tr("HasherQtg"));

    //TODO android has bugs, scrollto has no effect
#ifdef __ANDROID__
    baseWidget_pri->resize(scrollArea_pri->width() * 0.96, scrollArea_pri->height() * 0.95);
#endif
    //resize(screenGeometry.width(), screenGeometry.height() * 0.99);

    QTimer* mainLoopTimer = new QTimer(qApp);
    QObject::connect(mainLoopTimer, &QTimer::timeout, this, &Window_c::mainLoop_f);
    mainLoopTimer->start(500);

    if (appConfig_f().configLoaded_f())
    {
        //qInfo() << "configFile_f().second" << endl;
        if (appConfig_f().windowGeometrySet_f())
        {
            //qInfo() << "appConfig_f().windowGeometry_f() " << appConfig_f().windowGeometry_f() << endl;
            restoreGeometry(appConfig_f().windowGeometry_f());
        }

        if (appConfig_f().selectedDirectoryHistorySet_f())
        {
            //qInfo() << "appConfig_f().recentlyCataloguedListSet_f() " << appConfig_f().recentlyCataloguedListSet_f() << endl;
            for (const QString& directory_ite_con : appConfig_f().selectedDirectoryHistory_f())
            {
                addDirectoryHistory_f(directory_ite_con);
            }
        }
    }

    QObject::connect(this, &Window_c::setStatusBarText_signal_f, statusBarLabel_pri, &QLabel::setText);
    QObject::connect(this, &Window_c::scrollToItem_signal_f, this, &Window_c::scrollToItem_f);
    QObject::connect(this, &Window_c::setRowCellField_signal_f, this, &Window_c::setRowCellField_f);
    QObject::connect(this, &Window_c::setHashRowCellField_signal_f, this, &Window_c::setHashRowCellField_f);
    QObject::connect(this, &Window_c::saveAfterHash_signal_f, this, &Window_c::dialogSaveFileList_f);
    QObject::connect(this, &Window_c::resizeFileTable_signal_f, this, &Window_c::resizeFileTable_f);

    QObject::connect(hashTypeCrc32cRadio_pri, &QRadioButton::toggled , this, &Window_c::changeHashType_f);
    QObject::connect(hashTypeXXHashRadio_pri, &QRadioButton::toggled , this, &Window_c::changeHashType_f);
    QObject::connect(hashTypeSHA256Radio_pri, &QRadioButton::toggled , this, &Window_c::changeHashType_f);
    QObject::connect(hashTypeWhirlpoolRadio_pri, &QRadioButton::toggled , this, &Window_c::changeHashType_f);

    QObject::connect(hashFormatDecimalRadio_pri, &QRadioButton::toggled , this, &Window_c::changeHashFormat_f);
    QObject::connect(hashFormatHexadecimalRadio_pri, &QRadioButton::toggled , this, &Window_c::changeHashFormat_f);
    QObject::connect(hashFormatBase64Radio_pri, &QRadioButton::toggled , this, &Window_c::changeHashFormat_f);
}

Window_c::~Window_c()
{
    appConfig_f().setWindowGeometry_f(saveGeometry());
    QStringList selectedDirectoryHistory;
    selectedDirectoryHistory.reserve(directoryPathToDateTime_pri.size());
    for (const QString& directory_par_con : directoryHistory_f())
    {
        selectedDirectoryHistory.append(directory_par_con);
    }
    appConfig_f().setSelectedDirectoryHistory_f(selectedDirectoryHistory);
    appConfig_f().saveConfigFile_f();
    if (eines::signal::isRunning_f())
    {
        eines::signal::stopRunning_f();
    }
}

void Window_c::processPositionalArguments_f(const QStringList& positionalArguments_par_con)
{
    QTimer::singleShot(15,[=]
    {
        if (not positionalArguments_par_con.isEmpty())
        {
            loadFileList_f(positionalArguments_par_con);
        }
    });
}

void Window_c::addDirectoryHistory_f(const QString& directory_par_con)
{
    uint_fast64_t nowTmp(std::chrono::steady_clock::now().time_since_epoch().count());
    directoryPathToDateTime_pri.insert(directory_par_con, nowTmp);
    dateTimeToDirectoryPath_pri.insert(nowTmp, directory_par_con);
    if (dateTimeToDirectoryPath_pri.size() > 10)
    {
        uint_fast64_t lastItemKeyTmp(dateTimeToDirectoryPath_pri.firstKey());
        QString lastItemValueTmp(dateTimeToDirectoryPath_pri.first());

        directoryPathToDateTime_pri.remove(lastItemValueTmp);
        dateTimeToDirectoryPath_pri.remove(lastItemKeyTmp);
    }
}

std::vector<QString> Window_c::directoryHistory_f() const
{
    std::vector<QString> historyVector;
    historyVector.reserve(dateTimeToDirectoryPath_pri.size());
    for (const QString& directory_ite_con : dateTimeToDirectoryPath_pri)
    {
        historyVector.emplace_back(directory_ite_con);
    }
    std::reverse(historyVector.begin(), historyVector.end());
    return historyVector;
}

void Window_c::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls() and not hashing_pri)
    {
        event->acceptProposedAction();
    }
}

void Window_c::dropEvent(QDropEvent* event)
{
    for (const QUrl& url_ite_con : event->mimeData()->urls())
    {
        if (url_ite_con.isLocalFile())
        {
            QString someLocalUrl(url_ite_con.toLocalFile());
            QFileInfo localItemInfoTmp(someLocalUrl);
            if (localItemInfoTmp.exists())
            {
                if (localItemInfoTmp.isDir())
                {
                    addDirectoryToList_f(localItemInfoTmp);
                }
                if (localItemInfoTmp.isFile())
                {
                    tryAddFileToList_f(localItemInfoTmp.filePath());
                }
            }
        }
    }
    resizeFileTable_f();
}

void Window_c::mainLoop_f()
{
    if (finalCounterSeconds_pri == 0 and qThreadCount_f() == 0)
    {
        QApplication::exit();
    }
    if (not eines::signal::isRunning_f())
    {
        statusBarLabel_pri->setText(tr("Exiting..."));
        finalCounterSeconds_pri = finalCounterSeconds_pri - 1;
    }
//    QTimer::singleShot(0,[=]
//    {
//        //directoryComboBox_pri->adjustSize();
//        adjustSize();
//    });
}

void Window_c::setRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con)
{
    fileTable_pri->item(row_par_con, column_par_con)->setText(text_par_con);
}

void Window_c::setHashRowCellField_f(const int row_par_con, const int column_par_con, const QString& text_par_con)
{
    fileTable_pri->item(row_par_con, column_par_con)->setText(text_par_con);
    if (not fileTable_pri->item(row_par_con, column_par_con + 1)->text().isEmpty())
    {
        //TODO add icons instead of yes or no
        if (fileTable_pri->item(row_par_con, column_par_con)->text() == fileTable_pri->item(row_par_con, column_par_con + 1)->text())
        {
            fileTable_pri->item(row_par_con, column_par_con+ 2)->setText("yes");
        }
        else
        {
            fileTable_pri->item(row_par_con, column_par_con+ 2)->setText("no");
        }
    }
}

void Window_c::scrollToItem_f(const int row_par_con)
{
    fileTable_pri->scrollToItem(fileTable_pri->item(row_par_con, 0));
}

void Window_c::changeHashType_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No hash type changing while hashing"));
        return;
    }
    QRadioButton* changedRadioTmp = qobject_cast<QRadioButton*>(sender());
    while (true)
    {
        if (changedRadioTmp == hashTypeCrc32cRadio_pri)
        {
            dataHub_f().setHashType_f(eines::hasher_c::hashType_ec::crc32c);
            break;
        }
        if (changedRadioTmp == hashTypeXXHashRadio_pri)
        {
            dataHub_f().setHashType_f(eines::hasher_c::hashType_ec::XXHASH64);
            break;
        }
        if (changedRadioTmp == hashTypeSHA256Radio_pri)
        {
            dataHub_f().setHashType_f(eines::hasher_c::hashType_ec::SHA256);
            break;
        }
        if (changedRadioTmp == hashTypeWhirlpoolRadio_pri)
        {
            dataHub_f().setHashType_f(eines::hasher_c::hashType_ec::whirlpool);
            break;
        }
        break;
    }
    clearHashFields_f();
}

void Window_c::changeHashTypeProg_f()
{
    bool clearHashFieldsTmp(false);
    while (true)
    {
        if (dataHub_f().hashType_f() == eines::hasher_c::hashType_ec::crc32c)
        {
            if (not hashTypeCrc32cRadio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashTypeCrc32cRadio_pri->setChecked(true);
            break;
        }
        if (dataHub_f().hashType_f() == eines::hasher_c::hashType_ec::XXHASH64)
        {
            if (not hashTypeXXHashRadio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashTypeXXHashRadio_pri->setChecked(true);
            break;
        }
        if (dataHub_f().hashType_f() == eines::hasher_c::hashType_ec::SHA256)
        {
            if (not hashTypeSHA256Radio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashTypeSHA256Radio_pri->setChecked(true);
            break;
        }
        if (dataHub_f().hashType_f() == eines::hasher_c::hashType_ec::whirlpool)
        {
            if (not hashTypeWhirlpoolRadio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashTypeWhirlpoolRadio_pri->setChecked(true);
            break;
        }
        break;
    }
    if (clearHashFieldsTmp)
    {
        clearHashFields_f();
    }
}

void Window_c::changeHashFormat_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No hash format changing while hashing"));
        return;
    }
    QRadioButton* changedRadioTmp = qobject_cast<QRadioButton*>(sender());
    while (true)
    {
        if (changedRadioTmp == hashFormatDecimalRadio_pri)
        {
            dataHub_f().setHashFormat_f(eines::hasher_c::outputType_ec::decimalString);
            break;
        }
        if (changedRadioTmp == hashFormatHexadecimalRadio_pri)
        {
            dataHub_f().setHashFormat_f(eines::hasher_c::outputType_ec::hexadecimalString);
            break;
        }
        if (changedRadioTmp == hashFormatBase64Radio_pri)
        {
            dataHub_f().setHashFormat_f(eines::hasher_c::outputType_ec::base64String);
            break;
        }
        break;
    }
    clearHashFields_f();
}

void Window_c::changeHashFormatProg_f()
{
    bool clearHashFieldsTmp(false);
    while (true)
    {
        if (dataHub_f().hashFormat_f() == eines::hasher_c::outputType_ec::decimalString)
        {
            if (not hashFormatDecimalRadio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashFormatDecimalRadio_pri->setChecked(true);
            break;
        }
        if (dataHub_f().hashFormat_f() == eines::hasher_c::outputType_ec::hexadecimalString)
        {
            if (not hashFormatHexadecimalRadio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashFormatHexadecimalRadio_pri->setChecked(true);
            break;
        }
        if (dataHub_f().hashFormat_f() == eines::hasher_c::outputType_ec::base64String)
        {
            if (not hashFormatBase64Radio_pri->isChecked())
            {
                clearHashFieldsTmp = true;
            }
            hashFormatBase64Radio_pri->setChecked(true);
            break;
        }
        break;
    }
    if (clearHashFieldsTmp)
    {
        clearHashFields_f();
    }
}

void Window_c::addDirectoryToList_f(const QFileInfo& dir_par_con)
{
    //get the file contents of the directory and pass the files to tryAddFileToList_f(
#ifdef Q_OS_WIN32
    std::vector<QString> fileStrVector(fileDataController_c::getFilesFromDirectory(dir_par_con, true, useSlashSeparatorCheckbox_pri->isChecked()));
#else
    std::vector<QString> fileStrVector(fileDataController_c::getFilesFromDirectory(dir_par_con));
#endif
    for (const QString& fileStr_ite_con : fileStrVector)
    {
        tryAddFileToList_f(fileStr_ite_con);
        if (not eines::signal::isRunning_f())
        {
            break;
        }
    }
    resizeFileTable_f();
}

void Window_c::browseDirectoryToAdd_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No browsing while hashing"));
        return;
    }

    QFileDialog selectFolderDialogTmp(this);
    selectFolderDialogTmp.setFileMode(QFileDialog::Directory);
    selectFolderDialogTmp.setDirectory(QDir::currentPath());
    selectFolderDialogTmp.setWindowTitle(tr("Select folder"));
    selectFolderDialogTmp.setViewMode(QFileDialog::Detail);
    selectFolderDialogTmp.setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    selectFolderDialogTmp.setOption(QFileDialog::DontUseNativeDialog, true);
    //selectFolderDialogTmp.setOption(QFileDialog::ShowDirsOnly, true);
#ifdef __ANDROID__
    selectFolderDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif
    selectFolderDialogTmp.setOption(QFileDialog::ReadOnly, true);

    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryPathToDateTime_pri.size());
    if (not directoryPathToDateTime_pri.isEmpty())
    {
        for (const QString& directory_par_con : directoryHistory_f())
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    selectFolderDialogTmp.setSidebarUrls(directoriesTmp);

    selectFolderDialogTmp.exec();

    if (selectFolderDialogTmp.result() == QDialog::Accepted)
    {
        QString directoryTmp;
        if (not selectFolderDialogTmp.selectedFiles().isEmpty())
        {
            directoryTmp = selectFolderDialogTmp.selectedFiles().first();
        }

        if (not directoryTmp.isEmpty())
        {
            addDirectoryToList_f(directoryTmp);
            addDirectoryHistory_f(selectFolderDialogTmp.directory().path());
            statusBarLabel_pri->setText(tr("Directory added"));
        }
        else
        {
            statusBarLabel_pri->setText(tr("No directory selected"));
        }
    }
}

bool Window_c::tryAddFileToList_f(
        const QString& filePath_par_con
        , const QString& savedHash_par_con
)
{
    bool successTmp(false);
    int rowTmp(0);
    QFileInfo fileInfoTmp(filePath_par_con);
    if (not fileInfoTmp.exists())
    {
        return successTmp;
    }

    QString absoluteFilePathTmp(fileInfoTmp.absoluteFilePath());
    allHashed_pri = false;

    auto fileDataIdTmp(dataHub_f().filePathToFileDataId_f(absoluteFilePathTmp));
    //it's already in
    if (fileDataIdTmp > -1)
    {
        rowTmp = dataHub_f().fileDataIdToRow_f(fileDataIdTmp);

        //update size
        fileTable_pri->item(rowTmp, 1)->setText(QString::fromStdString(eines::formatByteSizeValue_f(fileInfoTmp.size())));
//        //clear the hash
//        fileTable_pri->item(rowTmp, 2)->setText("");
        fileTable_pri->item(rowTmp, 3)->setText(savedHash_par_con);
        //fileTable_pri->item(rowTmp, 4)->setIcon(QIcon());

        if (not fileTable_pri->item(rowTmp, 2)->text().isEmpty() and not fileTable_pri->item(rowTmp, 3)->text().isEmpty())
        {
            //TODO add icons instead of yes or no
            if (fileTable_pri->item(rowTmp, 2)->text() == fileTable_pri->item(rowTmp, 3)->text())
            {
                fileTable_pri->item(rowTmp, 4)->setText("yes");
            }
            else
            {
                fileTable_pri->item(rowTmp, 4)->setText("no");
            }
        }
    }
    else
    {
        QTableWidgetItem *fileNameCell;
        fileNameCell = new QTableWidgetItem(absoluteFilePathTmp);
        fileNameCell->setFlags(fileNameCell->flags() bitand compl Qt::ItemIsEditable);

        QTableWidgetItem *sizeCell = new QTableWidgetItem(QString::fromStdString(eines::formatByteSizeValue_f(fileInfoTmp.size())));
        sizeCell->setFlags(sizeCell->flags() bitand compl Qt::ItemIsEditable);
        sizeCell->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QTableWidgetItem *hashCell = new QTableWidgetItem;
        hashCell->setFlags(hashCell->flags() bitand compl Qt::ItemIsEditable);
        hashCell->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QTableWidgetItem *savedHashCell = new QTableWidgetItem(savedHash_par_con);
        savedHashCell->setFlags(savedHashCell->flags() bitand compl Qt::ItemIsEditable);
        savedHashCell->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        QTableWidgetItem *matchCell = new QTableWidgetItem;
        matchCell->setFlags(matchCell->flags() bitand compl Qt::ItemIsEditable);
        savedHashCell->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);

        rowTmp = fileTable_pri->rowCount();
        fileTable_pri->insertRow(rowTmp);

        fileTable_pri->setItem(rowTmp, 0, fileNameCell);
        fileTable_pri->setItem(rowTmp, 1, sizeCell);
        fileTable_pri->setItem(rowTmp, 2, hashCell);
        fileTable_pri->setItem(rowTmp, 3, savedHashCell);
        fileTable_pri->setItem(rowTmp, 4, matchCell);
    }

    fileData_c fileDataTmp(absoluteFilePathTmp, fileInfoTmp.size());
    dataHub_f().addFileDataToUMap(fileDataTmp, rowTmp);
    successTmp = true;
    return successTmp;
}

void Window_c::addFileSelectionToList_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No browsing while hashing"));
        return;
    }

    QFileDialog selectFilesDialogTmp(this);
    selectFilesDialogTmp.setFileMode(QFileDialog::ExistingFiles);
    selectFilesDialogTmp.setDirectory(QDir::currentPath());

    selectFilesDialogTmp.setWindowTitle(tr("Select files"));
    selectFilesDialogTmp.setViewMode(QFileDialog::Detail);
    selectFilesDialogTmp.setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    selectFilesDialogTmp.setOption(QFileDialog::DontUseNativeDialog, true);
    //selectFolderDialogTmp.setOption(QFileDialog::ShowDirsOnly, true);
    selectFilesDialogTmp.setOption(QFileDialog::ReadOnly, true);
#ifdef __ANDROID__
    selectFilesDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryPathToDateTime_pri.size());
    if (not directoryPathToDateTime_pri.isEmpty())
    {
        for (const QString& directory_par_con : directoryHistory_f())
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    selectFilesDialogTmp.setSidebarUrls(directoriesTmp);

    selectFilesDialogTmp.exec();

    if (selectFilesDialogTmp.result() == QDialog::Accepted)
    {
        if (not selectFilesDialogTmp.selectedFiles().isEmpty())
        {
            QStringList selectedFiles(selectFilesDialogTmp.selectedFiles());
            for (const QString& fileStr_ite_con : selectedFiles)
            {
                tryAddFileToList_f(fileStr_ite_con);
                if (not eines::signal::isRunning_f())
                {
                    break;
                }
            }
            addDirectoryHistory_f(selectFilesDialogTmp.directory().path());
            resizeFileTable_f();
            statusBarLabel_pri->setText(tr("Selected files added"));
        }
        else
        {
            statusBarLabel_pri->setText(tr("No files selected"));
        }
    }
}

void Window_c::clearFileList_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No clearing while hashing"));
        return;
    }

    if (fileTable_pri->rowCount() == 0)
    {
        return;
    }

    fileTable_pri->setRowCount(0);
    dataHub_f().clearAllFileData_f();
    statusBarLabel_pri->setText(tr("List cleared"));
}

void Window_c::dialogLoadFileList_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No file loading while hashing"));
        return;
    }

    QFileDialog selectFilesDialogTmp(this);
    selectFilesDialogTmp.setFileMode(QFileDialog::ExistingFiles);
    selectFilesDialogTmp.setDirectory(QDir::currentPath());

    selectFilesDialogTmp.setWindowTitle(tr("Select files"));
    selectFilesDialogTmp.setViewMode(QFileDialog::Detail);
    selectFilesDialogTmp.setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    selectFilesDialogTmp.setOption(QFileDialog::DontUseNativeDialog, true);
    //selectFolderDialogTmp.setOption(QFileDialog::ShowDirsOnly, true);
    selectFilesDialogTmp.setOption(QFileDialog::ReadOnly, true);
#ifdef __ANDROID__
    selectFilesDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif


    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryPathToDateTime_pri.size());
    if (not directoryPathToDateTime_pri.isEmpty())
    {
        for (const QString& directory_par_con : directoryHistory_f())
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    selectFilesDialogTmp.setSidebarUrls(directoriesTmp);

    selectFilesDialogTmp.exec();

    if (selectFilesDialogTmp.result() == QDialog::Accepted)
    {
        loadFileList_f(selectFilesDialogTmp.selectedFiles());
    }
}

void Window_c::loadFileList_f(const QStringList& fileList_par_con)
{
    if (not fileList_par_con.isEmpty())
    {
        bool errorTmp(false);
        bool somethingLoaded(false);
        for (const QString& fileStr_ite_con : fileList_par_con)
        {
            QFile savedFile(fileStr_ite_con);
            if (not savedFile.exists())
            {
                statusBarLabel_pri->setText(tr("Save filed does not exist"));
                errorTmp = true;
                break;
            }

            QByteArray jsonByteArray;
            if (savedFile.open(QIODevice::ReadOnly))
            {
                jsonByteArray = savedFile.readAll();
            }
            else
            {
                statusBarLabel_pri->setText(tr("Could not open config file config.json"));
                errorTmp = true;
                break;
            }

            auto jsonDocObj(QJsonDocument::fromJson(jsonByteArray));
            bool thisFileLoadedSomething(false);
            if (jsonDocObj.isNull())
            {
                statusBarLabel_pri->setText(tr("Could not parse json from the save file"));
                errorTmp = true;
                break;
            }
            else
            {
                fileDataRoot_c fileDataRootObj;
                fileDataRootObj.read_f(jsonDocObj.object());

                dataHub_f().setHashFormat_f(fileDataRootObj.hashFormat_f());
                changeHashFormatProg_f();
                dataHub_f().setHashType_f(fileDataRootObj.hashType_f());
                changeHashTypeProg_f();

                for (const fileData_c& fileData_ite_con : fileDataRootObj.fileDataVector_f())
                {
                    thisFileLoadedSomething = tryAddFileToList_f(fileData_ite_con.filePath_f(), QString::fromStdString(fileData_ite_con.hashStr_f())) or thisFileLoadedSomething;
                    if (not eines::signal::isRunning_f())
                    {
                        break;
                    }
                }
            }
            if (thisFileLoadedSomething)
            {
                statusBarLabel_pri->setText(tr("Save file loaded " + fileStr_ite_con.toUtf8()));
                somethingLoaded = true;
            }
            if (not eines::signal::isRunning_f())
            {
                break;
            }
        }
        if (not errorTmp and somethingLoaded)
        {
            resizeFileTable_f();
            statusBarLabel_pri->setText(tr("All save files loaded"));
        }
    }
    else
    {
        statusBarLabel_pri->setText(tr("No save files selected"));
    }
}

void Window_c::dialogSaveFileList_f()
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("No saving while hashing"));
        return;
    }

    if (fileTable_pri->rowCount() == 0)
    {
        statusBarLabel_pri->setText(tr("Nothing to save"));
        return;
    }

    if (not allHashed_pri)
    {
        QMessageBox msgBox(this);
        msgBox.setText("Not all the files are hashed, hash?");
        msgBox.setInformativeText("Results can be saved without hash");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);

        int ret(msgBox.exec());

        bool rehash(ret == QMessageBox::Yes);
        if (rehash)
        {
            hashList_f();
        }
    }

    //save the list in json format
    QFileDialog saveDialogTmp(this);
    saveDialogTmp.setAcceptMode(QFileDialog::AcceptSave);
    saveDialogTmp.setFileMode(QFileDialog::AnyFile);
    saveDialogTmp.setDirectory(QDir::currentPath());
    saveDialogTmp.selectFile("results.json");
    saveDialogTmp.setWindowTitle(tr("Save results file..."));
    saveDialogTmp.setViewMode(QFileDialog::Detail);
    saveDialogTmp.setFilter(QDir::Hidden | QDir::NoDotAndDotDot | QDir::Files | QDir::AllDirs | QDir::Drives);
    saveDialogTmp.setDefaultSuffix("json");
    saveDialogTmp.setOption(QFileDialog::DontUseNativeDialog, true);
#ifdef __ANDROID__
    saveDialogTmp.setGeometry(QApplication::desktop()->availableGeometry(this));
#endif

    QList<QUrl> directoriesTmp;
    directoriesTmp.reserve(directoryPathToDateTime_pri.size());
    if (not directoryPathToDateTime_pri.isEmpty())
    {
        for (const QString& directory_par_con : directoryHistory_f())
        {
            directoriesTmp.append(QUrl::fromLocalFile(directory_par_con));
        }
    }
    saveDialogTmp.setSidebarUrls(directoriesTmp);

    saveDialogTmp.exec();

    while (saveDialogTmp.result() == QDialog::Accepted)
    {
        QString saveFileName;
        if (not saveDialogTmp.selectedFiles().isEmpty())
        {
            saveFileName = saveDialogTmp.selectedFiles().first();
        }

        if (saveFileName.isEmpty())
        {
            statusBarLabel_pri->setText(tr("No Save file selected"));
            break;
        }

        QFile saveFile(saveFileName);
        if (saveFile.open(QIODevice::WriteOnly))
        {
            statusBarLabel_pri->setText(tr("Creating JSON save file..."));
            QJsonObject somethingJson;
            {
                std::vector<fileData_c> fileDataVectorTmp;
                fileDataVectorTmp.reserve(fileTable_pri->rowCount());
                for (auto rowIndex_ite = 0, l = fileTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
                {
                    QString filenameTmp(fileTable_pri->item(rowIndex_ite, 0)->text());
                    auto fileDataId(dataHub_f().filePathToFileDataId_f(filenameTmp));
                    fileDataVectorTmp.emplace_back(dataHub_f().fileData(fileDataId)->second);
                }
                fileDataRoot_c tmp(fileDataVectorTmp, dataHub_f().hashType_f(), dataHub_f().hashFormat_f());
                if (useRelativePathOnSaveCheckBox_pri->isChecked())
                {
                    tmp.relativePathsTo_f(saveDialogTmp.directory());
                }
                tmp.write_f(somethingJson);
            }
            QJsonDocument somethingJsonD(somethingJson);
            saveFile.write(somethingJsonD.toJson(QJsonDocument::Indented));
            statusBarLabel_pri->setText(tr("File saved successfully"));
        }
        else
        {
            statusBarLabel_pri->setText(tr("Couldn't create the save file"));
        }

        break;
    }
}

void Window_c::hashingStatusThread_f()
{
    while (hashing_pri and eines::signal::isRunning_f())
    {
        while (eines::signal::isRunning_f() and hashing_pri)
        {
            {
                QMutexLocker lockerTmp(&hashingCurrentFolderQMutex_pri);
                Q_EMIT scrollToItem_signal_f(hashingCurrentRow_pri);
                if (statusBarHashingAnimationStatus_pri == 0)
                {
                    Q_EMIT setStatusBarText_signal_f(tr("Hashing.") + " "" "" " + hashingCurrentFile_pri);
                    statusBarHashingAnimationStatus_pri = 1;
                    break;
                }
                //if (statusBarLabel_pri->text().startsWith("Hashing. "))
                if (statusBarHashingAnimationStatus_pri == 1)
                {
                    Q_EMIT setStatusBarText_signal_f(tr("Hashing..") + " "" " + hashingCurrentFile_pri);
                    statusBarHashingAnimationStatus_pri = 2;
                    break;
                }
                if (statusBarHashingAnimationStatus_pri == 2)
                    //if (statusBarLabel_pri->text().startsWith("Hashing.. "))
                {
                    Q_EMIT setStatusBarText_signal_f(tr("Hashing...") + " " + hashingCurrentFile_pri);
                    statusBarHashingAnimationStatus_pri = 0;
                    break;
                }
                statusBarHashingAnimationStatus_pri = 0;
            }
            QThread::msleep(1000);
            Q_EMIT setStatusBarText_signal_f(tr("Hashing...") + " " + hashingCurrentFile_pri);

            break;
        }
        //smooth +60fps
        QThread::msleep(15);
    }
    Q_EMIT setStatusBarText_signal_f(tr("All files hashed"));
    hashingCurrentFile_pri.clear();
}

//add another argument in the future if format and type must not be enabled/blocked at the same time
void Window_c::enableFormatTypeRadios_f(const bool enabled_par_con)
{
    hashFormatBase64Radio_pri->setEnabled(enabled_par_con);
    hashFormatDecimalRadio_pri->setEnabled(enabled_par_con);
    hashFormatHexadecimalRadio_pri->setEnabled(enabled_par_con);
//    hashFormatBase64Radio_pri->blockSignals(blockValue_par_con);
//    hashFormatDecimalRadio_pri->blockSignals(blockValue_par_con);
//    hashFormatHexadecimalRadio_pri->blockSignals(blockValue_par_con);

    hashTypeCrc32cRadio_pri->setEnabled(enabled_par_con);
    hashTypeSHA256Radio_pri->setEnabled(enabled_par_con);
    hashTypeWhirlpoolRadio_pri->setEnabled(enabled_par_con);
    hashTypeXXHashRadio_pri->setEnabled(enabled_par_con);
//    hashTypeCrc32cRadio_pri->blockSignals(blockValue_par_con);
//    hashTypeSHA256Radio_pri->blockSignals(blockValue_par_con);
//    hashTypeWhirlpoolRadio_pri->blockSignals(blockValue_par_con);
//    hashTypeXXHashRadio_pri->blockSignals(blockValue_par_con);
}

void Window_c::hashList_f(const bool saveAfter_par_con)
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("Already hashing"));
        return;
    }
    //hash all the files in the list and add the hash value in the fileTable
    //if saved hashes were loaded compare them and fill the match value accordingly
    if (fileTable_pri->rowCount() == 0)
    {
        return;
    }

    //QDialogButtonBox
    QMessageBox msgBox(this);
    msgBox.setText("Do you want to rehash already hashed items?");
    //msgBox.setInformativeText("Do you want to rehash already hashed items?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setLayoutDirection(Qt::LayoutDirectionAuto);


    int ret(msgBox.exec());

    enableFormatTypeRadios_f(false);
    bool rehashTmp(ret == QMessageBox::Yes);

    threadedFunction_c* funcHashList = new threadedFunction_c([=]
    {
        for (auto rowIndex_ite = 0, l = fileTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
        {
            QString filenameTmp(fileTable_pri->item(rowIndex_ite, 0)->text());
            auto fileDataId(dataHub_f().filePathToFileDataId_f(filenameTmp));
            //qInfo() << "rowIndex_ite " << rowIndex_ite << endl;
            //qInfo() << "fileDataId " << fileDataId << endl;
            std::unordered_map<int_fast64_t, fileData_c>::iterator fileDataPairTmp(dataHub_f().fileData(fileDataId));
            //qInfo() << "fileDataPairTmp.second " << fileDataPairTmp.second << endl;
            //qInfo() << "fileDataPairTmp.first.hashStr_f() " << QString::fromStdString(fileDataPairTmp.first.hashStr_f()) << endl;
            if (rehashTmp or fileDataPairTmp->second.hashStr_f().empty())
            {
                {
                    QMutexLocker lockerTmp(&hashingCurrentFolderQMutex_pri);
                    hashingCurrentRow_pri = rowIndex_ite;
                    hashingCurrentFile_pri = filenameTmp;
                }
                auto oldHash(fileDataPairTmp->second.hashStr_f());
                //qInfo() << "rehashing " << rehash << endl;
                fileDataPairTmp->second.generateHash_f(rehashTmp);

                if (oldHash != fileDataPairTmp->second.hashStr_f())
                {
                    Q_EMIT setHashRowCellField_signal_f(rowIndex_ite, 2, QString::fromStdString(fileDataPairTmp->second.hashStr_f()));
                }
            }
            if (not eines::signal::isRunning_f())
            {
                break;
            }
        }
        allHashed_pri = true;
        hashing_pri = false;
        enableFormatTypeRadios_f(true);
        if (saveAfter_par_con)
        {
            Q_EMIT saveAfterHash_signal_f();
        }
        Q_EMIT resizeFileTable_signal_f();
        //qInfo() << "rowIndex_ite final" << endl;
    });

    //threadedFunction_c* funcStatusHash = new threadedFunction_c(std::bind(&Window_c::hashingStatusThread_f, this));

    threadedFunction_c* funcStatusHash = new threadedFunction_c([=]
    {
        hashingStatusThread_f();
    });

    hashing_pri = true;

    QObject::connect(funcStatusHash, &QThread::finished, funcStatusHash, &QThread::deleteLater);
    funcStatusHash->start();

    QObject::connect(funcHashList, &QThread::finished, funcHashList, &QThread::deleteLater);
    funcHashList->start();
}

void Window_c::hashRows_f(const std::vector<int>& rows_par_con)
{
    if (hashing_pri)
    {
        statusBarLabel_pri->setText(tr("Already hashing"));
        return;
    }
    //hash all the files in the list and add the hash value in the fileTable
    //if saved hashes were loaded compare them and fill the match value accordingly
    if (fileTable_pri->rowCount() == 0)
    {
        return;
    }

    //rows_par_con must be by copy because THIS function ends but the thread continues
    threadedFunction_c* funcHashList = new threadedFunction_c([=]//, &rows_par_con]
    {
        for (const int row_ite_con : rows_par_con)
        {
            QString filenameTmp(fileTable_pri->item(row_ite_con, 0)->text());
            auto fileDataId(dataHub_f().filePathToFileDataId_f(filenameTmp));
            std::unordered_map<int_fast64_t, fileData_c>::iterator fileDataPairTmp(dataHub_f().fileData(fileDataId));

            {
                QMutexLocker lockerTmp(&hashingCurrentFolderQMutex_pri);
                hashingCurrentRow_pri = row_ite_con;
                hashingCurrentFile_pri = filenameTmp;
            }
            auto oldHash(fileDataPairTmp->second.hashStr_f());

            fileDataPairTmp->second.generateHash_f(true);

            if (oldHash != fileDataPairTmp->second.hashStr_f())
            {
                Q_EMIT setHashRowCellField_signal_f(row_ite_con, 2, QString::fromStdString(fileDataPairTmp->second.hashStr_f()));
            }
            if (not eines::signal::isRunning_f())
            {
                break;
            }
        }

        hashing_pri = false;
        //Q_EMIT resizeFileTable_signal_f();
    });

    threadedFunction_c* funcStatusHash = new threadedFunction_c([=]
    {
        hashingStatusThread_f();
    });

    hashing_pri = true;

    QObject::connect(funcStatusHash, &QThread::finished, funcStatusHash, &QThread::deleteLater);
    funcStatusHash->start();

    QObject::connect(funcHashList, &QThread::finished, funcHashList, &QThread::deleteLater);
    funcHashList->start();
}

void Window_c::showTips_f()
{
   QMessageBox::about(this, tr("Tips"),
                      tr(
                          "<p>1 Changing hash type/format radios clears hash values</p>"
                          "<p>2 Loading saved files will change the format/type radios to the last loaded file values</p>"
                          "<p>3 Relative paths are relative to the save file directory</p>"
                          "<p>4 There is a context menu on the grid (right click on one or more selected rows)</p>"
                          "<p>5 List can be saved without hashing</p>"
                          "<p>6 A saved file can be dropped on the hasherQtg file and will be loaded at the start</p>"
                          "<p>7 Fastest hash is crc32c (32Bit) followed closely by xxhash (64Bit), but both of them are way faster than any regular SSD so it doesn't matter too much</p>"
                          R"(<p>8 Quitting hasherQtg "normally" during an operation won't instantly exit, during hashing, it will have to finish hashing the current file, during saving, it will have to finish saving</p>)")
                      );
}

void Window_c::clearMachingRows_f()
{
    std::vector<int> rowToRemoveTmp;
    for (auto rowIndex_ite = 0, l = fileTable_pri->rowCount(); rowIndex_ite < l; ++rowIndex_ite)
    {
        if (not fileTable_pri->item(rowIndex_ite, 2)->text().isEmpty() and not fileTable_pri->item(rowIndex_ite, 3)->text().isEmpty())
        {
            if (fileTable_pri->item(rowIndex_ite, 2)->text() == fileTable_pri->item(rowIndex_ite, 3)->text())
            {
                rowToRemoveTmp.emplace_back(rowIndex_ite);
                QTableWidgetItem* filePathCell = fileTable_pri->item(rowIndex_ite, 0);
                QString filename(filePathCell->text());
                dataHub_f().removeFileData_f(filename);
            }
        }
    }

    if (not rowToRemoveTmp.empty())
    {
        //to prevent disaster it's required to remove them in descending order
        //because when one row is removed the ones above decrease by one index
        std::sort(rowToRemoveTmp.begin(), rowToRemoveTmp.end());
        std::reverse(rowToRemoveTmp.begin(), rowToRemoveTmp.end());
        for (const int row_ite_con : rowToRemoveTmp)
        {
            fileTable_pri->removeRow(row_ite_con);
        }
    }
}

void Window_c::showAboutMessage_f()
{
   QMessageBox::about(this, tr("About hasherQtg"),
                      tr(
                          "<p>HasherQtg</b> allows to check the integrity of a list of files using a hash algorithm</p>"
                          "<p>Creaded by: Jouven<br>"
                          R"(Source: <a href="//github.com/jouven/hasherQtg">github.com/jouven/hasherQtg</a><br>)"
                          R"(Homepage: <a href="//avidcalm.com">avidcalm.com</a></p>)"
                         )
   );
}

void Window_c::contextMenu(const QPoint &pos)
{
    if (fileTable_pri->rowCount() == 0 or fileTable_pri->selectedItems().size() == 0)
    {
        return;
    }

    QMenu menu;
    QAction *hashAction = menu.addAction("Hash");
#ifndef QT_NO_CLIPBOARD
    QAction *copyAction = menu.addAction("Copy File Path");
#endif
    QAction *openAction = menu.addAction("Open");
    QAction *removeRowAction = menu.addAction("Remove from the list");


    QAction *action = menu.exec(fileTable_pri->mapToGlobal(pos));
    if (not action)
    {
        return;
    }
    if (action == hashAction)
    {
        std::unordered_set<int> selectedRowsUSet;
        std::vector<int> rowsToHash;
        for (const QTableWidgetItem* selectedCell_ite_con : fileTable_pri->selectedItems())
        {
            //qInfo() << "selectedCell_ite_con->row() " << selectedCell_ite_con->row() << endl;
            selectedRowsUSet.emplace(selectedCell_ite_con->row());
        }
        rowsToHash.reserve(selectedRowsUSet.size());
        for (const int row_ite_con : selectedRowsUSet)
        {
            rowsToHash.emplace_back(row_ite_con);
        }
        hashRows_f(rowsToHash);
    }
    if (action == openAction)
    {
        QSet<QString> filePathsToOpen;
        for (const QTableWidgetItem* selectedCell_ite_con : fileTable_pri->selectedItems())
        {
            QTableWidgetItem* filePathCell = fileTable_pri->item(selectedCell_ite_con->row(), 0);
            QString filePath(filePathCell->text());
            filePathsToOpen.insert(filePath);
        }
        for (const QString& filePath_ite_con : filePathsToOpen)
        {
            QDesktopServices::openUrl(QUrl::fromLocalFile(filePath_ite_con));
        }
    }
#ifndef QT_NO_CLIPBOARD
    if (action == copyAction)
    {
        std::unordered_set<int> selectedRowsUSet;
        for (const QTableWidgetItem* selectedCell_ite_con : fileTable_pri->selectedItems())
        {
            selectedRowsUSet.emplace(selectedCell_ite_con->row());
        }
        QString copyData;
        for (const int row_ite_con : selectedRowsUSet)
        {
            QTableWidgetItem* filePathCell = fileTable_pri->item(row_ite_con, 0);
            QString filename(filePathCell->text());
            copyData.append(QDir::toNativeSeparators(filename)).append('\n');
        }
        copyData.chop(1);
        QGuiApplication::clipboard()->setText(copyData);
    }
#endif
    if (action == removeRowAction)
    {
        std::unordered_set<int> selectedRowsUSet;
        for (const QTableWidgetItem* selectedCell_ite_con : fileTable_pri->selectedItems())
        {
            //qInfo() << "selectedCell_ite_con->row() " << selectedCell_ite_con->row() << endl;
            selectedRowsUSet.emplace(selectedCell_ite_con->row());
        }
        for (const int row_ite_con : selectedRowsUSet)
        {
            QTableWidgetItem* filePathCell = fileTable_pri->item(row_ite_con, 0);
            QString filename(filePathCell->text());
            dataHub_f().removeFileData_f(filename);
        }
        std::vector<int> vectorRows;
        for (const int row_ite_con : selectedRowsUSet)
        {
            vectorRows.emplace_back(row_ite_con);
        }
        //to prevent disaster it's required to remove them in descending order
        //because when one row is removed the ones above decrease by one index
        std::sort(vectorRows.begin(), vectorRows.end());
        std::reverse(vectorRows.begin(), vectorRows.end());
        for (const int row_ite_con : vectorRows)
        {
            fileTable_pri->removeRow(row_ite_con);
        }
    }
}
