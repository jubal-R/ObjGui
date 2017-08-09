#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

// For debugging
#include "iostream"

#include "files.h"
#include "functionlist.h"
#include "highlighter.h"
#include "objdumper.h"
#include "settings.h"

using namespace std;

Files files;
FunctionList functionList;
Settings settings;
ObjDumper objDumper;
Highlighter *disHighlighter = NULL;
Highlighter *symbolsHighlighter = NULL;
Highlighter *relocationsHighlighter = NULL;
Highlighter *contentsHighlighter = NULL;
Highlighter *headersHighlighter = NULL;

QString currentDirectory = QString::fromStdString(files.getHomeDir());

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setup application fonts
    int sansid = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Regular.ttf");
    QString sansfamily = QFontDatabase::applicationFontFamilies(sansid).at(0);
    QFont sans(sansfamily);
    sans.setPointSize(11);

    this->setFont(sans);
    ui->tabWidget->setFont(sans);

//    sans.setPointSize(10);
    ui->functionList->setFont(sans);

    int monoid = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro.ttf");
    QString monofamily = QFontDatabase::applicationFontFamilies(monoid).at(0);
    QFont mono(monofamily);
    mono.setPointSize(12);

    ui->codeBrowser->setFont(mono);
    ui->symbolsBrowser->setFont(mono);
    ui->relocationsBrowser->setFont(mono);
    ui->contentsBrowser->setFont(mono);
    ui->headersBrowser->setFont(mono);

    mono.setBold(true);
    ui->addressLabel->setFont(mono);
    ui->functionLabel->setFont(mono);

    this->setWindowTitle("ObjGUI");

    // Set Window Size
    MainWindow::resize(settings.getWindowWidth(), settings.getWindowHeight());

    // Indicate Current Preferences
    if (settings.getSyntax() == "intel"){
        ui->actionIntel->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(0);
        objDumper.setOutputSyntax("intel");
    }else if (settings.getSyntax() == "att"){
        ui->actionAtt->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(1);
        objDumper.setOutputSyntax("att");
    }
    ui->allHeadersCheckBox->toggle();

    // Style
    QString style = "QTabBar::tab:selected{color: #fafafa; background-color: #3ba1a1;}"
          "QTabBar::tab {color: font-size:9pt; padding: 1px 5px;}";
   ui->tabWidget->setStyleSheet(style);

    disHighlighter = new Highlighter("dis", "default", ui->codeBrowser->document());
    symbolsHighlighter = new Highlighter("sym", "default", ui->symbolsBrowser->document());
    relocationsHighlighter = new Highlighter("sym", "default", ui->relocationsBrowser->document());
    contentsHighlighter = new Highlighter("str", "default", ui->contentsBrowser->document());
    headersHighlighter = new Highlighter("sym", "default", ui->headersBrowser->document());

    connect(ui->codeBrowser, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

}

MainWindow::~MainWindow()
{
    /*
     *  Save Settings
    */

    // Get Window Size
    QRect windowRect = MainWindow::normalGeometry();
    settings.setWindowWidth(windowRect.width());
    settings.setWindowHeight(windowRect.height());

    settings.saveSettings();

    delete ui;
}

//  Open Binary
void MainWindow::open(QString file){

    if (file != ""){
        this->setWindowTitle("ObjGUI - " + file);

        // Clear old function/section list from sidebar
        while (ui->functionList->count() > 0){
            ui->functionList->takeItem(0);
        }

        // Set file format value in statusbar
        ui->fileFormatlabel->setText(objDumper.getFileFormat(file));


        functionList = objDumper.getFunctionList(file);

        // Populate function/section list in sidebar
        ui->functionList->addItems(functionList.getFunctionNames());

        // Set disassembly text
        if (!functionList.isEmpty()){
            // Display main function by default if it exists
            if (functionList.containsFunction("main"))
                displayFunctionText("main");
            else {
                QString firstIndexName = functionList.getFunction(0).getName();
                displayFunctionText(firstIndexName);
            }

        } else {
           ui->codeBrowser->setText("File format not recognized.");
           ui->addressLabel->setText("");
           ui->functionLabel->setText("");
        }


        ui->symbolsBrowser->setText(objDumper.getSymbolsTable(file));
        ui->relocationsBrowser->setText(objDumper.getRelocationEntries(file));
        ui->contentsBrowser->setText(objDumper.getContents(file));
        ui->headersBrowser->setText(objDumper.getHeaders(file));

    }
}
// Disassemble
void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), currentDirectory, tr("All (*)"));
    currentDirectory = getDirectory(file);
    open(file);
}

//  Highlight Current Line
void MainWindow::highlightCurrentLine(){
   QColor lineColor = QColor(215, 215, 215);

   QList<QTextEdit::ExtraSelection> extraSelections;

   QTextEdit::ExtraSelection selections;
   selections.format.setBackground(lineColor);
   selections.format.setProperty(QTextFormat::FullWidthSelection, true);
   selections.cursor = ui->codeBrowser->textCursor();
   selections.cursor.clearSelection();
   extraSelections.append(selections);

   ui->codeBrowser->setExtraSelections(extraSelections);

}

// Get directory given file path
QString MainWindow::getDirectory(QString filepath){
    int lastIndex = filepath.lastIndexOf("/");
    filepath.chop(filepath.length() - lastIndex);

    return filepath;
}

void MainWindow::displayFunctionText(QString functionName){
    if (!functionList.isEmpty()){
        Function function = functionList.getFunction(functionName);
        ui->addressLabel->setText(function.getAddress());
        ui->functionLabel->setText(function.getName());
        //QString display = "  " + function.getAddress() + "\t\t" + function.getName() + "\n\n" + function.getContents();
        ui->codeBrowser->setText(function.getContents());
    }
}

/*
 * Window
*/
void MainWindow::on_actionProject_triggered()
{
    QString aboutStr = "ObjGui - GUI frontend for objdump\n"
                       "Project page: https://github.com/jubal-R/ObjGui\n\n"

                       "Copyright (C) 2017\n\n"

                       "This program is free software: you can redistribute it and/or modify "
                       "it under the terms of the GNU General Public License as published by "
                       "the Free Software Foundation, either version 3 of the License, or "
                       "(at your option) any later version.\n\n"

                       "This program is distributed in the hope that it will be useful,"
                       "but WITHOUT ANY WARRANTY; without even the implied warranty of "
                       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                       "GNU General Public License for more details.\n\n"

                       "You should have received a copy of the GNU General Public License "
                       "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n";
    QMessageBox::information(this, tr("About ObjGui"), aboutStr,QMessageBox::Close);
}

void MainWindow::on_actionExit_triggered()
{
    QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Exit", "Are you sure you want to exit?", QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {
          QApplication::quit();
        }
}

void MainWindow::on_actionFullscreen_triggered()
{
    if(MainWindow::isFullScreen())
        {
            MainWindow::showNormal();
        }else{
            MainWindow::showFullScreen();
        }
}


void MainWindow::on_actionShow_Containing_Folder_triggered()
{
    // Open current directory in file manager
    files.openFileManager(currentDirectory);
}

void MainWindow::on_actionIntel_triggered()
{
    settings.setSyntax("intel");
    objDumper.setOutputSyntax("intel");
    ui->actionIntel->setChecked(true);
    ui->actionAtt->setChecked(false);
    ui->syntaxComboBox->setCurrentIndex(0);
}

void MainWindow::on_actionAtt_triggered()
{
    settings.setSyntax("att");
    objDumper.setOutputSyntax("att");
    ui->actionAtt->setChecked(true);
    ui->actionIntel->setChecked(false);
    ui->syntaxComboBox->setCurrentIndex(1);
}

// Syntax Option ComboBox
void MainWindow::on_syntaxComboBox_currentIndexChanged(int index)
{
    if (index == 0){
        on_actionIntel_triggered();
    }else if(index == 1){
        on_actionAtt_triggered();
    }
}

void MainWindow::on_functionList_itemDoubleClicked(QListWidgetItem *item)
{
    displayFunctionText(item->text());
}



void MainWindow::on_disassemblyFlagcheckBox_toggled(bool checked)
{
    if (checked)
        objDumper.setDisassemblyFlag("-D");
    else
        objDumper.setDisassemblyFlag("-d");
}

void MainWindow::on_allHeadersCheckBox_toggled(bool checked)
{
    if (checked){
        objDumper.setHeaderFlags("-x");
        // Check all boxes
        ui->archiveHeadersCheckBox->setChecked(true);
        ui->fileHeadersCheckBox->setChecked(true);
        ui->privateHeadersCheckBox->setChecked(true);
        // Disable other options
        ui->archiveHeadersCheckBox->setEnabled(false);
        ui->fileHeadersCheckBox->setEnabled(false);
        ui->privateHeadersCheckBox->setEnabled(false);
    } else {
        // Re-enble other options
        ui->archiveHeadersCheckBox->setEnabled(true);
        ui->fileHeadersCheckBox->setEnabled(true);
        ui->privateHeadersCheckBox->setEnabled(true);
    }
}

QString MainWindow::getHeaderFlags(){
    QString flags = "";
    if (!ui->allHeadersCheckBox->isChecked()){
        if (ui->archiveHeadersCheckBox->isChecked())
            flags.append("-a ");
        if (ui->fileHeadersCheckBox->isChecked())
            flags.append("-f ");
        if (ui->privateHeadersCheckBox->isChecked())
            flags.append("-p ");

        return flags;
    } else {
        return "-x";
    }
}

void MainWindow::on_archiveHeadersCheckBox_toggled(bool checked)
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}

void MainWindow::on_fileHeadersCheckBox_toggled(bool checked)
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}

void MainWindow::on_privateHeadersCheckBox_toggled(bool checked)
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}
