#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QListWidgetItem"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void highlightCurrentLine();
    void on_actionOpen_triggered();
    void open(QString file);
    void displayFunctionText(QString functionName);
    QString getHeaderFlags();
    QString getDirectory(QString file);

    void on_actionProject_triggered();

    void on_actionExit_triggered();

    void on_actionFullscreen_triggered();

    void on_actionShow_Containing_Folder_triggered();
    
    void on_actionIntel_triggered();

    void on_actionAtt_triggered();

    void on_functionList_itemDoubleClicked(QListWidgetItem *item);

    void on_syntaxComboBox_currentIndexChanged(int index);

    void on_disassemblyFlagcheckBox_toggled(bool checked);

    void on_allHeadersCheckBox_toggled(bool checked);

    void on_archiveHeadersCheckBox_toggled(bool checked);

    void on_fileHeadersCheckBox_toggled(bool checked);

    void on_privateHeadersCheckBox_toggled(bool checked);

    void on_checkBox_toggled(bool checked);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
