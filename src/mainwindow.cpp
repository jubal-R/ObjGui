#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "QScrollBar"
#include "QSettings"
#include "QInputDialog"

// For debugging
#include "iostream"

#include "files.h"
#include "dataStructures/functionlist.h"
#include "highlighter.h"
#include "objdumper.h"

using namespace std;

Files files;
FunctionList functionList;
SectionList sectionList;
QSettings settings;
ObjDumper objDumper;
Highlighter *disHighlighter = NULL;
Highlighter *symbolsHighlighter = NULL;
Highlighter *relocationsHighlighter = NULL;
Highlighter *hexHighlighter = NULL;
Highlighter *headersHighlighter = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->hexAddressBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->hexBrowser->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(ui->hexBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->hexAddressBrowser->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(ui->hexBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->asciiBrowser->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(ui->asciiBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->hexBrowser->verticalScrollBar(), SLOT(setValue(int)));

    /*
     * Setup builtin fonts
    */

    // Sans serif
    int sansid = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Regular.ttf");
    QString sansfamily = QFontDatabase::applicationFontFamilies(sansid).at(0);
    QFont sans(sansfamily);
    sans.setPointSize(11);

    this->setFont(sans);
    ui->tabWidget->setFont(sans);
    ui->syntaxLabel->setFont(sans);
    ui->disassemblyFlagLabel->setFont(sans);
    ui->headersLabel->setFont(sans);
    ui->functionListLabel->setFont(sans);
    ui->functionList->setFont(sans);

    // Sans serif bold
    int sansBoldId = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Bold.ttf");
    QString sansBoldFamily = QFontDatabase::applicationFontFamilies(sansBoldId).at(0);
    QFont sansBold(sansBoldFamily);
    sansBold.setPointSize(11);
    sansBold.setBold(true);

    ui->syntaxLabel->setFont(sansBold);
    ui->disassemblyFlagLabel->setFont(sansBold);
    ui->headersLabel->setFont(sansBold);

    // Monospace
    int monoid = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro.ttf");
    QString monofamily = QFontDatabase::applicationFontFamilies(monoid).at(0);
    QFont mono(monofamily);
    mono.setPointSize(12);

    ui->codeBrowser->setFont(mono);
    ui->symbolsBrowser->setFont(mono);
    ui->relocationsBrowser->setFont(mono);
    ui->hexAddressBrowser->setFont(mono);
    ui->hexBrowser->setFont(mono);
    ui->asciiBrowser->setFont(mono);
    ui->headersBrowser->setFont(mono);

    // Monospace Bold
    int monoBoldId = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro B.ttf");
    QString monoBoldFamily = QFontDatabase::applicationFontFamilies(monoBoldId).at(0);
    QFont monoBold(monoBoldFamily);
    monoBold.setPointSize(13);
    monoBold.setBold(true);
    ui->addressLabel->setFont(monoBold);
    ui->functionLabel->setFont(monoBold);
    ui->sectionLabel->setFont(monoBold);
    ui->hexAddressLabel->setFont(monoBold);
    ui->hexLabel->setFont(monoBold);
    ui->asciiLabel->setFont(monoBold);

    this->setWindowTitle("ObjGUI");

    // Set Window Size
    MainWindow::resize(settings.value("windowWidth", 1000).toInt(), settings.value("windowHeight", 600).toInt());
    ui->splitter->restoreState(settings.value("splitterSizes").toByteArray());

    // Indicate Current Preferences
    if (settings.value("syntax", "intel") == "intel"){
        ui->actionIntel->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(0);
        objDumper.setOutputSyntax("intel");
    }else if (settings.value("syntax", "intel") == "att"){
        ui->actionAtt->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(1);
        objDumper.setOutputSyntax("att");
    }
    ui->allHeadersCheckBox->toggle();

    if (settings.value("useCustomBinary", false).toBool()){
        ui->customBinaryCheckBox->setChecked(true);
        objDumper.setUseCustomBinary(true);
    }
    objDumper.setobjdumpBinary(settings.value("customBinary", "").toString());
    ui->customBinaryLineEdit->setText(settings.value("customBinary", "").toString());

    // Style
    QString tabWidgetStyle = "QTabBar::tab:selected{color: #fafafa; background-color: #3ba1a1;}"
          "QTabBar::tab {background-color: #fafafa; min-width: 80px;}"
          "QTabWidget::tab-bar {left: 5px;}"
          "QTabWidget::pane {border: none;}"
          "QTextBrowser {border: 2px soild red;}"
          "QComboBox {background-color: #fafafa; color: #555555;}";
    ui->tabWidget->setStyleSheet(tabWidgetStyle);
    QString menuStyle = "QMenu::item:selected {background-color: #3ba1a1; color: #fafafa;}"
            "QMenuBar::item {background-color: #fafafa; color: #555555;}"
            "QMenuBar {border-bottom: 1px solid #cccccc;}";
    ui->menuBar->setStyleSheet(menuStyle);

    disHighlighter = new Highlighter("dis", "default", ui->codeBrowser->document());
    symbolsHighlighter = new Highlighter("sym", "default", ui->symbolsBrowser->document());
    relocationsHighlighter = new Highlighter("sym", "default", ui->relocationsBrowser->document());
    hexHighlighter = new Highlighter("hexdump", "default", ui->hexBrowser->document());
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
    settings.setValue("windowWidth", windowRect.width());
    settings.setValue("windowHeight", windowRect.height());
    settings.setValue("splitterSizes", ui->splitter->saveState());

    delete ui;
}

//  Open Binary
void MainWindow::open(QString file){

    if (file != ""){
        this->setWindowTitle("ObjGUI - " + file);

        /*
         *  Clear Old Values
        */
        while (ui->functionList->count() > 0){
            ui->functionList->takeItem(0);
        }
        ui->addressLabel->clear();
        ui->functionLabel->clear();
        ui->sectionLabel->clear();
        ui->codeBrowser->clear();
        ui->hexAddressBrowser->clear();
        ui->hexBrowser->clear();
        ui->asciiBrowser->clear();
        ui->fileFormatlabel->clear();
        ui->symbolsBrowser->clear();
        ui->relocationsBrowser->clear();
        ui->headersBrowser->clear();


        /*
         *  Disassemble Binary and Display Values
        */

        // Disassemble and get function list
        functionList = objDumper.getFunctionList(file);

        // If functionlist is empty
        if (functionList.isEmpty()){
            ui->codeBrowser->setPlainText("File format not recognized.");
            ui->addressLabel->setText("");
            ui->functionLabel->setText("");

          // If objdump returned error message
        } else if (functionList.getLength() == 1 and functionList.getFunction(0).getName() == ""){
            QString message = functionList.getFunction(0).getContents();

            // If format is ambigous message, let user user select format from list of matching formats
            if (message.contains("Matching formats")){
                QStringList formats = message.split(":");
                if (formats.length() == 2){
                    formats = formats.at(1).split(" ", QString::SkipEmptyParts);

                    if (!formats.isEmpty()){
                        // Get target format and set flag
                        QString format = QInputDialog::getItem(this, "Select matching format", "Format is ambigous, select matching format:", formats, 0, false);
                        objDumper.setTarget("-b " + format);

                        // Generate new fumnctionlist
                        functionList = objDumper.getFunctionList(file);
                        displayFunctionData();
                    } else {
                        // Display error message
                        ui->codeBrowser->setPlainText(message);
                    }
                }

            } else {
                // Display error message
                ui->codeBrowser->setPlainText(message);
            }

          // If all good, display disassembly data
        } else {
            displayFunctionData();
        }

        // Get section list and set hex values
        sectionList = objDumper.getSectionList(file);
        int len = sectionList.getLength();
        setUpdatesEnabled(false);
        for (int i = 1; i < len; i++){
            Section section = sectionList.getSection(i);

            ui->hexBrowser->insertPlainText(section.getSectionName() + "\n");
            ui->hexAddressBrowser->insertPlainText("\n" + section.getAddressList().join("\n") + "\n");
            ui->hexBrowser->insertPlainText(section.getHexList().join("\n") + "\n");
            ui->asciiBrowser->insertPlainText("\n" + section.getAsciiList().join("\n") + "\n");
        }
        setUpdatesEnabled(true);

        // Set file format value in statusbar
        setUpdatesEnabled(false);
        ui->fileFormatlabel->setText(objDumper.getFileFormat(file));
        ui->symbolsBrowser->setPlainText(objDumper.getSymbolsTable(file));
        ui->relocationsBrowser->setPlainText(objDumper.getRelocationEntries(file));
        ui->headersBrowser->setPlainText(objDumper.getHeaders(file));
        setUpdatesEnabled(true);

        // Reset specified target
        objDumper.setTarget("");

    }
}

// Disassemble
void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), files.getCurrentDirectory(), tr("All (*)"));
    files.setCurrentDirectory(file);
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

void MainWindow::displayFunctionText(QString functionName){
    if (!functionList.isEmpty()){
        Function function = functionList.getFunction(functionName);
        setUpdatesEnabled(false);
        ui->addressLabel->setText(function.getAddress());
        ui->functionLabel->setText(function.getName());
        ui->sectionLabel->setText(function.getSection());
        ui->codeBrowser->setPlainText(function.getContents());
        setUpdatesEnabled(true);
    }
}

void MainWindow::displayFunctionData(){
    if (!functionList.isEmpty()){
        // Populate function/section list in sidebar
        ui->functionList->addItems(functionList.getFunctionNames());

        // Display main function by default if it exists
        if (functionList.containsFunction("main"))
            displayFunctionText("main");
        else {
            QString firstIndexName = functionList.getFunction(0).getName();
            displayFunctionText(firstIndexName);
        }
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
    files.openFileManager(files.getCurrentDirectory());
}

void MainWindow::on_actionIntel_triggered()
{
    settings.setValue("syntax", "intel");
    objDumper.setOutputSyntax("intel");
    ui->actionIntel->setChecked(true);
    ui->actionAtt->setChecked(false);
    ui->syntaxComboBox->setCurrentIndex(0);
}

void MainWindow::on_actionAtt_triggered()
{
    settings.setValue("syntax", "att");
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

void MainWindow::on_checkBox_toggled(bool checked)
{
    if (checked){
        objDumper.setOptionalFlags("-C");
    } else {
        objDumper.setOptionalFlags("");
    }
}

void MainWindow::on_customBinaryButton_clicked()
{
    QString objdumpBinary = QFileDialog::getOpenFileName(this, tr("Select Binary"), files.getCurrentDirectory(), tr("All (*)"));
    if (objdumpBinary != ""){
        ui->customBinaryLineEdit->setText(objdumpBinary);
        objDumper.setobjdumpBinary(objdumpBinary);
        settings.setValue("customBinary", objdumpBinary);
    }
}

void MainWindow::on_customBinaryCheckBox_toggled(bool checked)
{
    if (checked){
        ui->customBinaryButton->setEnabled(true);
        settings.setValue("useCustomBinary", true);
        objDumper.setUseCustomBinary(true);
    } else {
        ui->customBinaryButton->setEnabled(false);
        settings.setValue("useCustomBinary", false);
        objDumper.setUseCustomBinary(false);
    }
}
