#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include <QResource>
#include <QFile>
#include <QVBoxLayout>
#include <QProcess>
#include <QDir>
#include <QTextEdit>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    if (debug_print) qDebug() << "MainWindow::MainWindow()";

    ui->setupUi(this);
    MainWindow::setWindowTitle("FTest3 GUI Interface Program");

    QString text1 = QString::number(auto_buf_size);
    ui->lineEditAutoMemBufSize->setText(text1);

    QString text2 = QString::number(file_size);
    ui->lineEditFileSize->setText(text2);

    QString text3 = QString::number(buffers_num);
    ui->lineEditFilesNumber->setText(text3);

    QString text4 = QString::number(threads_num);
    ui->lineEditThreadsNumber->setText(text4);

    QString text5 = QString::number(pass_num);
    ui->lineEditPassesNumber->setText(text5);

    QString text6 = QString::number(buf_size);
    ui->lineEditBufferSize->setText(text6);

    if (verbose) ui->checkBoxVerbose->setChecked(true);
    else ui->checkBoxVerbose->setChecked(false);

    if (random_fill) ui->checkBoxRandomBufferFill->setChecked(true);
    else ui->checkBoxRandomBufferFill->setChecked(false);

    if (loop_forever) ui->checkBoxLoopForever->setChecked(true);
    else ui->checkBoxLoopForever->setChecked(false);

    if (time_profiling) ui->checkBoxTimeProfiling->setChecked(true);
    else ui->checkBoxTimeProfiling->setChecked(false);

    if (autodetect_memory_test) ui->checkBoxAutoMemoryTest->setChecked(true);
    else ui->checkBoxAutoMemoryTest->setChecked(false);

#if defined(_WIN32)
    p_temp_dir = new QString("c:\\temp");
#else
    p_temp_dir = new QString("/tmp");
#endif
    ui->textEditFileDirName->setText(*p_temp_dir);

    BuildCommandLine();
    DisplayCommandLine();

    if (memory_test_only) {
        ui->checkBoxMemoryOnlyTest->setChecked(false);
        EnableMemoryTestItems();
    }
    else {
        ui->checkBoxMemoryOnlyTest->setChecked(true);
        EnableFileTestItems();
    }

    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(About()));
    connect(ui->action_Quit, SIGNAL(triggered()), this, SLOT(Quit()));
    connect(ui->action_Help, SIGNAL(triggered()), this, SLOT(Help()));

    connect(ui->checkBoxVerbose, SIGNAL(stateChanged(int)), this, SLOT(checkBoxVerbose_changed(int)));
    connect(ui->checkBoxAutoMemoryTest, SIGNAL(stateChanged(int)), this, SLOT(checkBoxAutoMemoryTest_changed(int)));
    connect(ui->checkBoxRandomBufferFill, SIGNAL(stateChanged(int)), this, SLOT(checkBoxRandomBufferFill_changed(int)));
    connect(ui->checkBoxLoopForever, SIGNAL(stateChanged(int)), this, SLOT(checkBoxLoopForever_changed(int)));
    connect(ui->checkBoxTimeProfiling, SIGNAL(stateChanged(int)), this, SLOT(checkBoxTimeProfiling_changed(int)));
    connect(ui->checkBoxMemoryOnlyTest, SIGNAL(stateChanged(int)), this, SLOT(checkBoxMemoryOnlyTest_changed(int)));

    connect(ui->lineEditBufferSize, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditBufferSize_changed(const QString &)));
    connect(ui->lineEditPassesNumber, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditPassesNumber_changed(const QString &)));
    connect(ui->lineEditAutoMemBufSize, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditAutoMemBufSize_changed(const QString &)));
    connect(ui->lineEditFileSize, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditFileSize_changed(const QString &)));
    connect(ui->lineEditFilesNumber, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditFilesNumber_changed(const QString &)));
    connect(ui->lineEditThreadsNumber, SIGNAL(textChanged(const QString &)), this, SLOT(lineEditThreadsNumber_changed(const QString &)));
    //connect(ui->textEditCmdLineBuf, SIGNAL(textChanged(const QString &)), this, SLOT(textEditCmdLineBuf_changed(const QString &)));
    connect(ui->textEditCmdLineBuf, SIGNAL(textChanged()), this, SLOT(textEditCmdLineBuf_changed()));

    if (debug_print) qDebug() << "MainWindow::MainWindow() done";
}


MainWindow::~MainWindow()
{
    if (debug_print) qDebug() << "MainWindow::~MainWindow()";
    delete ui;
    if (debug_print) qDebug() << "MainWindow::~MainWindow() done";
}


void MainWindow::About()
{
    if (debug_print) qDebug() << "MainWindow::About()";
    QMessageBox::about(this, "About Ftest3 GUI",
                       "This program is written by Dmitry Stefankov\n"
                       "Copyright (C) 2016");
    if (debug_print) qDebug() << "MainWindow::About() done";
}



void MainWindow::Help()
{
    if (debug_print) qDebug() << "MainWindow::Help()";
    if (debug_print) qDebug() << "MainWindow:: QFile file";
    QFile file(":/ftest3.help");
    editor = new QTextEdit;
    if (file.open(QFile::ReadOnly | QFile::Text)) {
            if (debug_print) qDebug() << "MainWindow:: editor fill with values";
            editor->setPlainText(file.readAll());
            editor->setReadOnly(true);
            //editor->setDocumentTitle("Help for ftest3 program");
            editor->setWindowTitle("Help for ftest3 program");
            editor->resize(380,330);
            editor->show();
            if (debug_print) qDebug() << "MainWindow:: editor show()";
    }
    if (debug_print) qDebug() << "MainWindow::Help() done";
}


void MainWindow::Quit()
{
    if (debug_print) qDebug() << "MainWindow::Quit()";
    QMessageBox::information(this, "About Ftest3 GUI",
                       "Thank you! Bye-Bye!");
    if (debug_print) qDebug() << "MainWindow::Quit() done";
    qApp->quit();
}



void MainWindow::on_buttonRunTest_clicked()
{
    if (debug_print) qDebug() << "MainWindow::RunTest()";
    QString command = *p_program_path;
    QString our_params = ui->textEditCmdLineBuf->toPlainText();
    if (debug_print) qDebug() << command;
    if (debug_print) qDebug() << our_params;
    QStringList parameters;
    QString sOldPath = QDir::currentPath();
    if (debug_print) qDebug() <<  "sOldPath: " << sOldPath;
#if defined(_WIN32)
    QString sPath = "c:\\ftest3";
#else
    QString sPath = "/usr/local/bin";
#endif
    //QDir::setCurrent( sPath );
    QProcess * process = new QProcess(this);
#if defined(_WIN32)
    process->startDetached("cmd.exe", QStringList() << "/k" << our_params );
#else
    process->startDetached("/bin/sh", QStringList() << "-c" << our_params );
#endif
    QDir::setCurrent( sOldPath );
    if (debug_print) qDebug() << "MainWindow::RunTest() done";
}


void MainWindow::on_radioButtonFileIOtestmode_toggled(bool checked)
{
  if (checked) {
      memory_test_only = 0;
      EnableFileTestItems();
  }
  else {
      memory_test_only = 1;
      EnableMemoryTestItems();
  }
  BuildCommandLine();
  DisplayCommandLine();
}


void MainWindow::on_radioButtonMemoryOnlyTestMode_toggled(bool checked)
{
    if (checked) {
        //memory_test_only = 1;
    }
    else {
        //memory_test_only = 0;
    }
}


void MainWindow::EnableFileTestItems()
{
    ui->checkBoxMemoryOnlyTest->setEnabled(false);
    ui->checkBoxAutoMemoryTest->setEnabled(false);
    ui->lineEditAutoMemBufSize->setEnabled(false);
    ui->textEditFileDirName->setEnabled(true);
    ui->lineEditFileSize->setEnabled(true);
    //ui->lineEditFilesNumber->setEnabled(true);
}


void MainWindow::EnableMemoryTestItems()
{
    ui->checkBoxMemoryOnlyTest->setEnabled(true);
    ui->checkBoxAutoMemoryTest->setEnabled(true);
    ui->lineEditAutoMemBufSize->setEnabled(true);
    ui->textEditFileDirName->setEnabled(false);
    ui->lineEditFileSize->setEnabled(false);
    //ui->lineEditFilesNumber->setEnabled(false);
}


void MainWindow::BuildCommandLine()
{
    if (debug_print) qDebug() << "MainWindow::BuildCommandLine()";
#if defined(_WIN32)
    p_program_path  = new QString("ftest3.exe");
#else
    p_program_path  = new QString("./ftest3");
#endif
    QString text = *p_program_path;

    text.append("  ");
    if (verbose) text.append(" -v");
    if (random_fill) text.append(" -r");
    if (loop_forever) text.append(" -l");
    if (time_profiling) text.append(" -Z");

    if (pass_num > 0) {
        if (!loop_forever) {
            text.append(" -p ");
            text.append(QString::number(pass_num));
        }
    }

    if (threads_num > 0) {
        text.append(" -j ");
        text.append(QString::number(threads_num));
    }

    if (buffers_num > 0) {
        text.append(" -n ");
        text.append(QString::number(buffers_num));
    }

    if (buf_size > 0) {
        text.append(" -s ");
        text.append(QString::number(buf_size));
    }

    if (debug_print) qDebug() << "memory_test_only = " << memory_test_only;

    if (memory_test_only) {
        text.append(" -e ");
        if (autodetect_memory_test) {
            text.append(" -B ");
           if (auto_buf_size > 0) {
              text.append(" -b ");
              text.append(QString::number(auto_buf_size));
           }
        }
    }
    else {
        if (file_size > 0) {
            text.append(" -f ");
            text.append(QString::number(file_size));
        }
        text.append(" -t ");
        text.append(p_temp_dir);
    }

    p_command_line = new QString(text);
    if (debug_print) qDebug() << text;
    if (debug_print) qDebug() << *p_command_line;

    if (debug_print) qDebug() << "MainWindow::BuildCommandLine() done";
}


void MainWindow::DisplayCommandLine()
{
    ui->textEditCmdLineBuf->setText(*p_command_line);
}


void MainWindow::checkBoxVerbose_changed(int state)
{
    if (state) {
      verbose = 1;
    }
    else {
      verbose = 0;
    }
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::checkBoxAutoMemoryTest_changed(int state)
{
    if (state) {
      autodetect_memory_test = 1;
    }
    else {
      autodetect_memory_test = 0;
    }
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::checkBoxRandomBufferFill_changed(int state)
{
    if (state) {
      random_fill = 1;
    }
    else {
      random_fill = 0;
    }
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::checkBoxLoopForever_changed(int state)
{
    if (state) {
      loop_forever = 1;
    }
    else {
      loop_forever = 0;
    }
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::checkBoxTimeProfiling_changed(int state)
{
    if (state) {
      time_profiling = 1;
    }
    else {
      time_profiling = 0;
    }
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::checkBoxMemoryOnlyTest_changed(int state)
{
    if (state) {
      memory_test_only = 1;
    }
    else {
      memory_test_only = 0;
    }
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::lineEditBufferSize_changed(const QString & value)
{
    bool ok;
    buf_size = value.toLong(&ok, 10);
    if (!ok) buf_size = 4;
    if (buf_size < 1) buf_size = 4;
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::lineEditPassesNumber_changed(const QString & value)
{
    bool ok;
    pass_num = value.toLongLong(&ok,10);
    if (!ok) pass_num = 1;
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::lineEditAutoMemBufSize_changed(const QString & value)
{
    bool ok;
    auto_buf_size = value.toLong(&ok,10);
    if (!ok) auto_buf_size = 1000;
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::lineEditFileSize_changed(const QString & value)
{
    bool ok;
    file_size = value.toLongLong(&ok,10);
    if (!ok) file_size = 1;
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::lineEditFilesNumber_changed(const QString & value)
{
    bool ok;
    buffers_num = value.toInt(&ok,10);
    if (!ok) buffers_num = 1;
    if (buffers_num < 1) buffers_num = 1;
    if (buffers_num > 100) buffers_num = 100;
    BuildCommandLine();
    DisplayCommandLine();
}


void MainWindow::lineEditThreadsNumber_changed(const QString & value)
{
    bool ok;
    threads_num = value.toInt(&ok,10);
    if (!ok) threads_num = 1;
    if (threads_num < 1) threads_num = 1;
    if (threads_num > 500) threads_num = 500;
    BuildCommandLine();
    DisplayCommandLine();
}


//void MainWindow::textEditCmdLineBuf_changed(const QString & value)
void MainWindow::textEditCmdLineBuf_changed()
{
    //user_command_line = value;
    user_command_line = ui->textEditCmdLineBuf->toPlainText();
    if (debug_print) qDebug() << "user_command_line: " << user_command_line;
}

