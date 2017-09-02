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

    void loadBinary(QString file);

    void displayFunctionText(QString functionName);

    void displayFunctionText(int functionIndex);

    void displayFunctionData();

    void goToAddress(QString targetAddress);

    void addToHistory(int functionIndex, int lineNum);

    void findReferencesToLocation(QString location);

    void displayResults(QVector< QVector<QString> > results, QString resultsLabel);

    void setCentralWidgetStyle(QString foregroundColor, QString backgroundColor);

    void setTabWidgetStyle(QString foregroundColor, QString backgroundColor, QString backgroundColor2, QString addressColor);

    void setSidebarStyle(QString backgroundColor);

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

    void on_customBinaryButton_clicked();

    void on_customBinaryCheckBox_toggled(bool checked);

    void on_archiveHeadersCheckBox_clicked();

    void on_fileHeadersCheckBox_clicked();

    void on_privateHeadersCheckBox_clicked();

    void on_sectionHeadersCheckbox_clicked();

    void on_actionGo_To_Address_triggered();

    void on_actionGet_Offset_triggered();

    void on_actionGo_to_Address_at_Cursor_triggered();

    void on_demanlgeCheckBox_toggled(bool checked);

    void on_backButton_clicked();

    void on_forwardButton_clicked();

    void on_actionBack_triggered();

    void on_actionForward_triggered();

    void on_actionFind_Calls_to_Current_Function_triggered();

    void on_actionFind_Calls_to_Current_Location_triggered();

    void on_actionFind_2_triggered();

    void on_findButton_clicked();

    void on_findLineEdit_returnPressed();

    void on_findPrevButton_clicked();

    void on_actionFind_References_triggered();

    void on_actionGet_File_Offset_of_Current_Line_triggered();

    void on_actionDefault_triggered();

    void on_actionSolarized_Dark_triggered();

    void on_actionSolarized_triggered();

    void on_actionDark_triggered();

private:
    Ui::MainWindow *ui;
    int currentFunctionIndex;
    QList< QVector<int> > history;
    QList< QVector<int> >::const_iterator historyIterator;
    QVector<QString> baseOffsets;
    QString currentSearchTerm;
    QColor lineColor;
};

#endif // MAINWINDOW_H
