/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Help;
    QAction *action_About;
    QAction *action_Quit;
    QWidget *centralWidget;
    QRadioButton *radioButtonFileIOtestmode;
    QRadioButton *radioButtonMemoryOnlyTestMode;
    QTextEdit *textEditCmdLineBuf;
    QFrame *line_sep_main;
    QPushButton *buttonRunTest;
    QLabel *label_cmd_line;
    QGroupBox *groupBoxBuffers;
    QCheckBox *checkBoxRandomBufferFill;
    QLabel *labelBufferSize;
    QLineEdit *lineEditBufferSize;
    QGroupBox *groupBoxFiles;
    QTextEdit *textEditFileDirName;
    QLineEdit *lineEditFileSize;
    QLineEdit *lineEditFilesNumber;
    QLabel *labelFileDirName;
    QLabel *labelFileSize;
    QLabel *labelFilesNumber;
    QGroupBox *groupBoxMemoryMode;
    QCheckBox *checkBoxMemoryOnlyTest;
    QCheckBox *checkBoxAutoMemoryTest;
    QLineEdit *lineEditAutoMemBufSize;
    QLabel *labelAutoMemBufSize;
    QGroupBox *groupBoxGeneralOptions;
    QCheckBox *checkBoxLoopForever;
    QCheckBox *checkBoxTimeProfiling;
    QLineEdit *lineEditPassesNumber;
    QLineEdit *lineEditThreadsNumber;
    QLabel *labelPassesNumber;
    QLabel *labelThreadsNumber;
    QCheckBox *checkBoxVerbose;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menu_File;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(841, 477);
        action_Help = new QAction(MainWindow);
        action_Help->setObjectName(QStringLiteral("action_Help"));
        action_About = new QAction(MainWindow);
        action_About->setObjectName(QStringLiteral("action_About"));
        action_Quit = new QAction(MainWindow);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        radioButtonFileIOtestmode = new QRadioButton(centralWidget);
        radioButtonFileIOtestmode->setObjectName(QStringLiteral("radioButtonFileIOtestmode"));
        radioButtonFileIOtestmode->setGeometry(QRect(10, 20, 201, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        radioButtonFileIOtestmode->setFont(font);
        radioButtonFileIOtestmode->setChecked(true);
        radioButtonMemoryOnlyTestMode = new QRadioButton(centralWidget);
        radioButtonMemoryOnlyTestMode->setObjectName(QStringLiteral("radioButtonMemoryOnlyTestMode"));
        radioButtonMemoryOnlyTestMode->setGeometry(QRect(10, 60, 201, 20));
        radioButtonMemoryOnlyTestMode->setFont(font);
        textEditCmdLineBuf = new QTextEdit(centralWidget);
        textEditCmdLineBuf->setObjectName(QStringLiteral("textEditCmdLineBuf"));
        textEditCmdLineBuf->setGeometry(QRect(270, 40, 391, 41));
        textEditCmdLineBuf->setAcceptRichText(true);
        line_sep_main = new QFrame(centralWidget);
        line_sep_main->setObjectName(QStringLiteral("line_sep_main"));
        line_sep_main->setGeometry(QRect(10, 95, 801, 21));
        line_sep_main->setFrameShape(QFrame::HLine);
        line_sep_main->setFrameShadow(QFrame::Sunken);
        buttonRunTest = new QPushButton(centralWidget);
        buttonRunTest->setObjectName(QStringLiteral("buttonRunTest"));
        buttonRunTest->setGeometry(QRect(690, 40, 93, 41));
        buttonRunTest->setFont(font);
        label_cmd_line = new QLabel(centralWidget);
        label_cmd_line->setObjectName(QStringLiteral("label_cmd_line"));
        label_cmd_line->setEnabled(true);
        label_cmd_line->setGeometry(QRect(270, 20, 91, 16));
        label_cmd_line->setFont(font);
        groupBoxBuffers = new QGroupBox(centralWidget);
        groupBoxBuffers->setObjectName(QStringLiteral("groupBoxBuffers"));
        groupBoxBuffers->setEnabled(true);
        groupBoxBuffers->setGeometry(QRect(20, 120, 371, 101));
        groupBoxBuffers->setFont(font);
        checkBoxRandomBufferFill = new QCheckBox(groupBoxBuffers);
        checkBoxRandomBufferFill->setObjectName(QStringLiteral("checkBoxRandomBufferFill"));
        checkBoxRandomBufferFill->setEnabled(true);
        checkBoxRandomBufferFill->setGeometry(QRect(10, 20, 331, 20));
        checkBoxRandomBufferFill->setFont(font);
        labelBufferSize = new QLabel(groupBoxBuffers);
        labelBufferSize->setObjectName(QStringLiteral("labelBufferSize"));
        labelBufferSize->setEnabled(true);
        labelBufferSize->setGeometry(QRect(120, 60, 221, 21));
        labelBufferSize->setFont(font);
        lineEditBufferSize = new QLineEdit(groupBoxBuffers);
        lineEditBufferSize->setObjectName(QStringLiteral("lineEditBufferSize"));
        lineEditBufferSize->setGeometry(QRect(10, 60, 91, 21));
        groupBoxFiles = new QGroupBox(centralWidget);
        groupBoxFiles->setObjectName(QStringLiteral("groupBoxFiles"));
        groupBoxFiles->setGeometry(QRect(420, 120, 381, 131));
        groupBoxFiles->setFont(font);
        textEditFileDirName = new QTextEdit(groupBoxFiles);
        textEditFileDirName->setObjectName(QStringLiteral("textEditFileDirName"));
        textEditFileDirName->setGeometry(QRect(10, 20, 151, 41));
        lineEditFileSize = new QLineEdit(groupBoxFiles);
        lineEditFileSize->setObjectName(QStringLiteral("lineEditFileSize"));
        lineEditFileSize->setGeometry(QRect(10, 71, 91, 21));
        lineEditFilesNumber = new QLineEdit(groupBoxFiles);
        lineEditFilesNumber->setObjectName(QStringLiteral("lineEditFilesNumber"));
        lineEditFilesNumber->setGeometry(QRect(10, 100, 91, 22));
        labelFileDirName = new QLabel(groupBoxFiles);
        labelFileDirName->setObjectName(QStringLiteral("labelFileDirName"));
        labelFileDirName->setGeometry(QRect(170, 30, 181, 20));
        labelFileDirName->setFont(font);
        labelFileSize = new QLabel(groupBoxFiles);
        labelFileSize->setObjectName(QStringLiteral("labelFileSize"));
        labelFileSize->setEnabled(true);
        labelFileSize->setGeometry(QRect(120, 80, 231, 16));
        labelFileSize->setFont(font);
        labelFilesNumber = new QLabel(groupBoxFiles);
        labelFilesNumber->setObjectName(QStringLiteral("labelFilesNumber"));
        labelFilesNumber->setGeometry(QRect(120, 100, 241, 16));
        groupBoxMemoryMode = new QGroupBox(centralWidget);
        groupBoxMemoryMode->setObjectName(QStringLiteral("groupBoxMemoryMode"));
        groupBoxMemoryMode->setGeometry(QRect(420, 260, 381, 141));
        groupBoxMemoryMode->setFont(font);
        checkBoxMemoryOnlyTest = new QCheckBox(groupBoxMemoryMode);
        checkBoxMemoryOnlyTest->setObjectName(QStringLiteral("checkBoxMemoryOnlyTest"));
        checkBoxMemoryOnlyTest->setGeometry(QRect(10, 20, 341, 21));
        checkBoxAutoMemoryTest = new QCheckBox(groupBoxMemoryMode);
        checkBoxAutoMemoryTest->setObjectName(QStringLiteral("checkBoxAutoMemoryTest"));
        checkBoxAutoMemoryTest->setGeometry(QRect(10, 50, 341, 20));
        lineEditAutoMemBufSize = new QLineEdit(groupBoxMemoryMode);
        lineEditAutoMemBufSize->setObjectName(QStringLiteral("lineEditAutoMemBufSize"));
        lineEditAutoMemBufSize->setGeometry(QRect(10, 80, 113, 22));
        labelAutoMemBufSize = new QLabel(groupBoxMemoryMode);
        labelAutoMemBufSize->setObjectName(QStringLiteral("labelAutoMemBufSize"));
        labelAutoMemBufSize->setGeometry(QRect(130, 80, 221, 20));
        groupBoxGeneralOptions = new QGroupBox(centralWidget);
        groupBoxGeneralOptions->setObjectName(QStringLiteral("groupBoxGeneralOptions"));
        groupBoxGeneralOptions->setGeometry(QRect(19, 229, 371, 171));
        groupBoxGeneralOptions->setFont(font);
        checkBoxLoopForever = new QCheckBox(groupBoxGeneralOptions);
        checkBoxLoopForever->setObjectName(QStringLiteral("checkBoxLoopForever"));
        checkBoxLoopForever->setGeometry(QRect(20, 20, 221, 20));
        checkBoxTimeProfiling = new QCheckBox(groupBoxGeneralOptions);
        checkBoxTimeProfiling->setObjectName(QStringLiteral("checkBoxTimeProfiling"));
        checkBoxTimeProfiling->setGeometry(QRect(20, 50, 201, 20));
        lineEditPassesNumber = new QLineEdit(groupBoxGeneralOptions);
        lineEditPassesNumber->setObjectName(QStringLiteral("lineEditPassesNumber"));
        lineEditPassesNumber->setGeometry(QRect(20, 80, 81, 22));
        lineEditThreadsNumber = new QLineEdit(groupBoxGeneralOptions);
        lineEditThreadsNumber->setObjectName(QStringLiteral("lineEditThreadsNumber"));
        lineEditThreadsNumber->setGeometry(QRect(20, 110, 81, 21));
        labelPassesNumber = new QLabel(groupBoxGeneralOptions);
        labelPassesNumber->setObjectName(QStringLiteral("labelPassesNumber"));
        labelPassesNumber->setGeometry(QRect(110, 80, 221, 16));
        labelThreadsNumber = new QLabel(groupBoxGeneralOptions);
        labelThreadsNumber->setObjectName(QStringLiteral("labelThreadsNumber"));
        labelThreadsNumber->setGeometry(QRect(110, 110, 221, 16));
        checkBoxVerbose = new QCheckBox(groupBoxGeneralOptions);
        checkBoxVerbose->setObjectName(QStringLiteral("checkBoxVerbose"));
        checkBoxVerbose->setGeometry(QRect(20, 140, 211, 20));
        MainWindow->setCentralWidget(centralWidget);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 841, 26));
        menu_File = new QMenu(menuBar);
        menu_File->setObjectName(QStringLiteral("menu_File"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menu_File->menuAction());
        menu_File->addAction(action_Help);
        menu_File->addAction(action_About);
        menu_File->addAction(action_Quit);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0));
        action_Help->setText(QApplication::translate("MainWindow", "&Help", 0));
        action_About->setText(QApplication::translate("MainWindow", "&About", 0));
        action_Quit->setText(QApplication::translate("MainWindow", "&Quit", 0));
        radioButtonFileIOtestmode->setText(QApplication::translate("MainWindow", "File I/O testing", 0));
        radioButtonMemoryOnlyTestMode->setText(QApplication::translate("MainWindow", "Memory testing only", 0));
        buttonRunTest->setText(QApplication::translate("MainWindow", "Run", 0));
        label_cmd_line->setText(QApplication::translate("MainWindow", "Command line", 0));
        groupBoxBuffers->setTitle(QApplication::translate("MainWindow", "Buffer(s)", 0));
        checkBoxRandomBufferFill->setText(QApplication::translate("MainWindow", "(-r) fill buffer randomly", 0));
        labelBufferSize->setText(QApplication::translate("MainWindow", "(-s) buffer size (MB)", 0));
        groupBoxFiles->setTitle(QApplication::translate("MainWindow", "File(s)", 0));
        labelFileDirName->setText(QApplication::translate("MainWindow", "(-t) working catalog", 0));
        labelFileSize->setText(QApplication::translate("MainWindow", "(-f) file size (MB)", 0));
        labelFilesNumber->setText(QApplication::translate("MainWindow", "(-n) files/buffers number", 0));
        groupBoxMemoryMode->setTitle(QApplication::translate("MainWindow", "Memory test options", 0));
        checkBoxMemoryOnlyTest->setText(QApplication::translate("MainWindow", "(-e) memory only test ", 0));
        checkBoxAutoMemoryTest->setText(QApplication::translate("MainWindow", "(-B) auto all memory testing", 0));
        labelAutoMemBufSize->setText(QApplication::translate("MainWindow", "(-b) auto buffer size (MB)", 0));
        groupBoxGeneralOptions->setTitle(QApplication::translate("MainWindow", "General options", 0));
        checkBoxLoopForever->setText(QApplication::translate("MainWindow", "(-l) loop forever", 0));
        checkBoxTimeProfiling->setText(QApplication::translate("MainWindow", "(-Z) time profiling", 0));
        labelPassesNumber->setText(QApplication::translate("MainWindow", "(-p) passes number", 0));
        labelThreadsNumber->setText(QApplication::translate("MainWindow", "(-j) threads number", 0));
        checkBoxVerbose->setText(QApplication::translate("MainWindow", "(-v) verbose", 0));
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
