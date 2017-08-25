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
    QString getHeaderFlags();

    void highlightCurrentLine();

    void on_actionOpen_triggered();

    void open(QString file);

    void displayFunctionText(QString functionName);

    void displayFunctionText(int functionIndex);

    void displayFunctionData();

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

    void on_checkBox_toggled(bool checked);

    void on_customBinaryButton_clicked();

    void on_customBinaryCheckBox_toggled(bool checked);

    void on_archiveHeadersCheckBox_clicked();

    void on_fileHeadersCheckBox_clicked();

    void on_privateHeadersCheckBox_clicked();

    void on_sectionHeadersCheckbox_clicked();

    void on_actionGo_To_Address_triggered();

    void on_actionGet_Offset_triggered();

private:
    Ui::MainWindow *ui;
    int currentFunctionIndex;
};

#endif // MAINWINDOW_H
