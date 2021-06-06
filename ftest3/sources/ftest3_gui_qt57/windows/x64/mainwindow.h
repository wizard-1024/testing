#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

extern int  debug_print;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


private slots:
    void Help();
    void Quit();
    void About();

    void EnableFileTestItems();
    void EnableMemoryTestItems();

    void BuildCommandLine();
    void DisplayCommandLine();

    void on_buttonRunTest_clicked();
    void on_radioButtonFileIOtestmode_toggled(bool checked);
    void on_radioButtonMemoryOnlyTestMode_toggled(bool checked);
    void checkBoxVerbose_changed(int state);
    void checkBoxAutoMemoryTest_changed(int state);
    void checkBoxRandomBufferFill_changed(int state);
    void checkBoxLoopForever_changed(int state);
    void checkBoxTimeProfiling_changed(int state);
    void checkBoxMemoryOnlyTest_changed(int state);
    void lineEditBufferSize_changed(const QString & value);
    void lineEditPassesNumber_changed(const QString & value);
    void lineEditAutoMemBufSize_changed(const QString & value);
    void lineEditFileSize_changed(const QString & value);
    void lineEditFilesNumber_changed(const QString & value);
    void lineEditThreadsNumber_changed(const QString & value);
    void textEditCmdLineBuf_changed();

private:
    Ui::MainWindow *ui;
    QTextEdit *editor;

    int  verbose = 1;
    int  random_fill = 1;
    int  loop_forever = 0;
    int  time_profiling = 1;
    int  memory_test_only = 0;
    int  autodetect_memory_test = 0;

    unsigned long int pass_num = 1;
    int threads_num = 1;
    unsigned long int file_size = 16;
    int buffers_num = 1;
    long int buf_size = 4;
    long int auto_buf_size = 1000;

    QString * p_temp_dir;
    QString * p_program_path;
    QString * p_command_line;
    QString user_command_line;
};

#endif // MAINWINDOW_H
