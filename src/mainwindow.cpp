#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "QScrollBar"
#include "QSettings"
#include "QInputDialog"

#include "QDebug"

#include "files.h"
#include "highlighters/disassemblyhighlighter.h"
#include "highlighters/sectionhighlighter.h"
#include "highlighters/headerhighlighter.h"
#include "objdumper.h"
#include "ui_loadingdialog.h"
#include "resultsdialog.h"

using namespace std;

Files files;
FunctionList functionList;
SectionList sectionList;
QSettings settings;
ObjDumper objDumper;
DisassemblyHighlighter *disHighlighter = NULL;
SectionHighlighter *sectionHighlighter = NULL;
HeaderHighlighter *headerHighlighter = NULL;

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
     *  Setup builtin fonts
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
    ui->customBinaryButton->setFont(sans);

    // Sans serif bold
    int sansBoldId = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Bold.ttf");
    QString sansBoldFamily = QFontDatabase::applicationFontFamilies(sansBoldId).at(0);
    QFont sansBold(sansBoldFamily);
    sansBold.setPointSize(11);
    sansBold.setBold(true);

    ui->syntaxLabel->setFont(sansBold);
    ui->disassemblyFlagLabel->setFont(sansBold);
    ui->headersLabel->setFont(sansBold);
    ui->functionLabel->setFont(sansBold);
    ui->addressLabel->setFont(sansBold);
    ui->fileOffsetLabel->setFont(sansBold);
    ui->sectionLabel->setFont(sansBold);
    ui->hexAddressLabel->setFont(sansBold);
    ui->hexLabel->setFont(sansBold);
    ui->asciiLabel->setFont(sansBold);
    ui->symbolsTableLabel->setFont(sansBold);
    ui->relocationsLabel->setFont(sansBold);

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
    ui->addressValueLabel->setFont(mono);
    ui->fileOffsetValueLabel->setFont(mono);
    ui->sectionValueLabel->setFont(mono);

    // Monospace Bold
    int monoBoldId = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro B.ttf");
    QString monoBoldFamily = QFontDatabase::applicationFontFamilies(monoBoldId).at(0);
    QFont monoBold(monoBoldFamily);
    monoBold.setPointSize(13);
    monoBold.setBold(true);


    this->setWindowTitle("ObjGUI");

    // Set Window Size
    MainWindow::resize(settings.value("windowWidth", 1000).toInt(), settings.value("windowHeight", 600).toInt());
    ui->splitter->restoreState(settings.value("splitterSizes").toByteArray());

    /*
     *  Set options from saved settings
    */

    // Syntax
    if (settings.value("syntax", "intel") == "intel"){
        ui->actionIntel->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(0);
        objDumper.setOutputSyntax("intel");

    }else if (settings.value("syntax", "intel") == "att"){
        ui->actionAtt->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(1);
        objDumper.setOutputSyntax("att");
    }

    // Optional flags
    if (settings.value("demangle", false) == true){
        ui->demanlgeCheckBox->setChecked(true);
        objDumper.setOptionalFlags("-C");
    }

    // Header flags
    QString headerFlags = settings.value("headerFlags", "").toString();
    if (headerFlags != ""){
        if (headerFlags == "-a -f -p -h "){
            ui->allHeadersCheckBox->setChecked(true);
        } else {
            if (headerFlags.contains('a'))
                ui->archiveHeadersCheckBox->setChecked(true);
            if (headerFlags.contains('f'))
                ui->fileHeadersCheckBox->setChecked(true);
            if (headerFlags.contains('p'))
                ui->privateHeadersCheckBox->setChecked(true);
            if (headerFlags.contains('h'))
                ui->sectionHeadersCheckbox->setChecked(true);
        }
        objDumper.setHeaderFlags(headerFlags);
    }

    // Custom binary
    if (settings.value("useCustomBinary", false).toBool()){
        ui->customBinaryCheckBox->setChecked(true);
        objDumper.setUseCustomBinary(true);
    }
    objDumper.setobjdumpBinary(settings.value("customBinary", "").toString());
    ui->customBinaryLineEdit->setText(settings.value("customBinary", "").toString());

    // Style
    QString tabWidgetStyle = "QTabBar::tab:selected{color: #fafafa; background-color: #3ba1a1; border-top: 2px solid #d4d4d4;}"
          "QTabBar::tab {background-color: #E0E0E0; min-width: 102px;}"
          "QTabWidget::tab-bar {left: 5px;}"
          "QTabWidget::pane {border: none;}"
          "QComboBox {background-color: #fafafa; color: #555555;}";
    ui->tabWidget->setStyleSheet(tabWidgetStyle);
    QString menuStyle = "QMenu::item:selected {background-color: #3ba1a1; color: #fafafa;}"
            "QMenu::item::disabled {color: #aaaaaa}"
            "QMenuBar::item {background-color: #fafafa; color: #555555;}"
            "QMenuBar {border-bottom: 1px solid #cccccc;}";
    ui->menuBar->setStyleSheet(menuStyle);

    disHighlighter = new DisassemblyHighlighter(ui->codeBrowser->document());
    sectionHighlighter = new SectionHighlighter(ui->hexBrowser->document());
    headerHighlighter = new HeaderHighlighter(ui->headersBrowser->document());

    connect(ui->codeBrowser, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    currentFunctionIndex = 0;

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
    settings.setValue("headerFlags", getHeaderFlags());

    delete ui;
}

/*
 *  Disassembly
*/

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
        ui->addressValueLabel->clear();
        ui->fileOffsetValueLabel->clear();
        ui->functionLabel->clear();
        ui->sectionValueLabel->clear();
        ui->codeBrowser->clear();
        ui->hexAddressBrowser->clear();
        ui->hexBrowser->clear();
        ui->asciiBrowser->clear();
        ui->fileFormatlabel->clear();
        ui->symbolsBrowser->clear();
        ui->relocationsBrowser->clear();
        ui->headersBrowser->clear();

        // Clear history
        history.clear();


        /*
         *  Disassemble Binary and Display Values
        */

        // Disassemble and get function list
        functionList.nukeList();
        functionList = objDumper.getFunctionList(file);

        // If functionlist is empty
        if (functionList.isEmpty() && functionList.successfullyCreated()){
            ui->codeBrowser->setPlainText("File format not recognized.");
            ui->addressLabel->setText("");
            ui->functionLabel->setText("");

          // If objdump returned error message
        } else if (!functionList.successfullyCreated()){
            QString message = functionList.getErrorMsg();

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
                        functionList.nukeList();
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
                // Disable navigation and tools
                ui->actionGo_To_Address->setEnabled(false);
                ui->actionGo_to_Address_at_Cursor->setEnabled(false);
                ui->actionGet_Offset->setEnabled(false);
            }

          // If all good, display disassembly data
        } else {
            displayFunctionData();
        }

        // Get section list and set hex values
        sectionList.nukeList();
        sectionList = objDumper.getSectionList(file);
        int len = sectionList.getLength();
        QByteArray addressStr;
        QByteArray hexStr;
        QByteArray asciiStr;

        for (int i = 1; i < len; i++){
            Section section = sectionList.getSection(i);

            addressStr.append("\n" + section.getAddressString() + "\n");
            hexStr.append(section.getSectionName() + "\n" + section.getHexString() + "\n");
            asciiStr.append("\n" + section.getAsciiString() + "\n");
        }
        setUpdatesEnabled(false);
        ui->hexAddressBrowser->setPlainText(addressStr);
        ui->hexBrowser->setPlainText(hexStr);
        ui->asciiBrowser->setPlainText(asciiStr);

        // Set file format value in statusbar
        ui->fileFormatlabel->setText(objDumper.getFileFormat(file));
        ui->symbolsBrowser->setPlainText(objDumper.getSymbolsTable(file));
        ui->relocationsBrowser->setPlainText(objDumper.getRelocationEntries(file));
        ui->headersBrowser->setPlainText(objDumper.getHeaders(file));
        setUpdatesEnabled(true);

        // Reset specified target
        objDumper.setTarget("");

        ui->tabWidget->setCurrentIndex(0);
        ui->codeBrowser->setFocus();

    }
}

// Disassemble
void MainWindow::on_actionOpen_triggered()
{
    // Setup loading message dialog
    QDialog* dialog = new QDialog(this, Qt::FramelessWindowHint);
    Ui_Dialog loadingDialogUi;
    loadingDialogUi.setupUi(dialog);
    dialog->show();

    // Prompt user for file
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), files.getCurrentDirectory(), tr("All (*)"));

    // Set and display loading message
    loadingDialogUi.label->setText("Disassembling binary...");
    qApp->processEvents();

    // Update current directory and call disassembly function
    if (file != ""){
        files.setCurrentDirectory(file);
        open(file);
    }
    // Delete loading dialog
    delete dialog;
}

/*
 *  Function Data
*/

void MainWindow::displayFunctionText(QString functionName){
    if (!functionList.isEmpty()){
        Function function = functionList.getFunction(functionName);
        setUpdatesEnabled(false);
        ui->addressValueLabel->setText(function.getAddress());
        ui->fileOffsetValueLabel->setText(function.getFileOffset());
        ui->functionLabel->setText(function.getName());
        ui->sectionValueLabel->setText(function.getSection());
        ui->codeBrowser->setPlainText(function.getContents());
        setUpdatesEnabled(true);

        int index = functionList.getFunctionIndex(functionName);
        if (index >= 0){
            currentFunctionIndex = index;
        }
    }
}

void MainWindow::displayFunctionText(int functionIndex){
    if (!functionList.isEmpty()){
        Function function = functionList.getFunction(functionIndex);
        setUpdatesEnabled(false);
        ui->addressValueLabel->setText(function.getAddress());
        ui->fileOffsetValueLabel->setText(function.getFileOffset());
        ui->functionLabel->setText(function.getName());
        ui->sectionValueLabel->setText(function.getSection());
        ui->codeBrowser->setPlainText(function.getContents());
        setUpdatesEnabled(true);

        currentFunctionIndex = functionIndex;
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

        // Add initial location to history
        addToHistory(currentFunctionIndex, 0);

        // Enable navigation and tools
        ui->actionGo_To_Address->setEnabled(true);
        ui->actionGo_to_Address_at_Cursor->setEnabled(true);
        ui->actionGet_Offset->setEnabled(true);
    }
}

//  Highlight current line of function
void MainWindow::highlightCurrentLine(){
   QColor lineColor = QColor(215,215,215);

   QList<QTextEdit::ExtraSelection> extraSelections;

   QTextEdit::ExtraSelection selections;
   selections.format.setBackground(lineColor);
   selections.format.setProperty(QTextFormat::FullWidthSelection, true);
   selections.cursor = ui->codeBrowser->textCursor();
   selections.cursor.clearSelection();
   extraSelections.append(selections);

   ui->codeBrowser->setExtraSelections(extraSelections);

}

/*
 *  Navigation
*/

// Go to virtual memory address
void MainWindow::goToAddress(QString targetAddress){
    if (targetAddress != ""){
        // Find address index
        QVector<int> location = functionList.getAddressLocation(targetAddress);

        // Check if address was found
        if(location[0] > 0){
            // Add old location to history
            QTextCursor prevCursor = ui->codeBrowser->textCursor();
            int lineNum = prevCursor.blockNumber();
            addToHistory(currentFunctionIndex, lineNum);

            setUpdatesEnabled(false);
            // Display function
            if (location[0] != currentFunctionIndex){
                displayFunctionText(location[0]);
                ui->functionList->setCurrentRow(location[0]);
            }
            // Go to Line
            QTextCursor cursor(ui->codeBrowser->document()->findBlockByLineNumber(location[1]));
            ui->codeBrowser->setTextCursor(cursor);
            ui->tabWidget->setCurrentIndex(0);
            ui->codeBrowser->setFocus();
            setUpdatesEnabled(true);

            // Add new location to history
            addToHistory(currentFunctionIndex, location[1]);

        } else {
            QMessageBox::information(this, tr("Go to Address"), "Address not found.",QMessageBox::Ok);
        }
    }
}

void MainWindow::on_actionGo_To_Address_triggered()
{
    bool ok = true;

    QString targetAddress = QInputDialog::getText(this, tr("Go to Address"),tr("Go to Address:"), QLineEdit::Normal,"", &ok).trimmed();
    goToAddress(targetAddress);

}

void MainWindow::on_actionGo_to_Address_at_Cursor_triggered()
{
    QTextCursor cursor = ui->codeBrowser->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString targetAddress = cursor.selectedText();

    goToAddress(targetAddress);
}

// Function list
void MainWindow::on_functionList_itemDoubleClicked(QListWidgetItem *item)
{
    // Display function
    displayFunctionText(item->text());
    ui->tabWidget->setCurrentIndex(0);
    // Add new location to history
    addToHistory(currentFunctionIndex, 0);
}

// Get file offset of current line of disassembly
void MainWindow::on_actionGet_Offset_triggered()
{
    /*
     * Offset calculated by [function offset] + [current line offset from function start]
     * Note: file offset often last part of virtual memory address, but not always.
    */
    if (!functionList.isEmpty()){
        Function function = functionList.getFunction(currentFunctionIndex);
        QTextCursor cursor = ui->codeBrowser->textCursor();
        int lineNum = cursor.blockNumber();

        if (function.getMatrixLen() > 0 && lineNum < function.getMatrixLen()){

            // Get addresses
            QString currentLineAddressStr = function.getAddressAt(lineNum);
            QString functionAddressStr = function.getAddress();

            // Convert addresses from hex string to longs
            bool currentAddrOk;
            bool functAddrOk;
            qlonglong addr = currentLineAddressStr.toLongLong(&currentAddrOk, 16);
            qlonglong functAddr = functionAddressStr.toLongLong(&functAddrOk, 16);

            if (currentAddrOk && functAddrOk){
                // Offset of current line from function start
                qlonglong lineOffset = addr - functAddr;

                // Get functions file offset
                bool functOffsetOk;
                qlonglong functOffset = function.getFileOffset().toLongLong(&functOffsetOk, 16);

                if (functOffsetOk){
                    // Calculate file offset of current line
                    qlonglong lineFileOffset = functOffset + lineOffset;
                    QString lineOffsetHexStr = "0x" + QString::number(lineFileOffset, 16);

                    QString offsetMsg = "File Offset of " + function.getAddressAt(lineNum) + "\nHex: " + lineOffsetHexStr + "\nDecimal: " + QString::number(lineFileOffset);

                    QMessageBox::information(this, tr("File Offset"), offsetMsg,QMessageBox::Close);

                }
            }
        }
    }
}


/*
 *  History
*/

// Add location to history and update iterator
void MainWindow::addToHistory(int functionIndex, int lineNum){
    QVector<int> item(2);
    item[0] = functionIndex;
    item[1] = lineNum;

    // Note: constEnd() points to imaginary item after last item
    if (historyIterator != history.constEnd() - 1)
        history = history.mid(0, historyIterator - history.constBegin() + 1);

    history.append(item);
    historyIterator = history.constEnd() - 1;
}

void MainWindow::on_backButton_clicked()
{
    if (!history.isEmpty() && historyIterator != history.constBegin()){
        historyIterator--;
        QVector<int> prevLocation = historyIterator.i->t();

        // Display prev function
        setUpdatesEnabled(false);
        if (currentFunctionIndex != prevLocation[0]){
            displayFunctionText(prevLocation[0]);
            ui->functionList->setCurrentRow(prevLocation[0]);
        }
        // Go to prev line
        QTextCursor cursor(ui->codeBrowser->document()->findBlockByLineNumber(prevLocation[1]));
        ui->codeBrowser->setTextCursor(cursor);
        ui->tabWidget->setCurrentIndex(0);
        ui->codeBrowser->setFocus();
        setUpdatesEnabled(true);
    }
}

void MainWindow::on_forwardButton_clicked()
{
    if (!history.isEmpty() && historyIterator != history.constEnd() - 1){
        historyIterator++;
        QVector<int> nextLocation = historyIterator.i->t();

        // Display prev function
        setUpdatesEnabled(false);
        if (currentFunctionIndex != nextLocation[0]){
            displayFunctionText(nextLocation[0]);
            ui->functionList->setCurrentRow(nextLocation[0]);
        }
        // Go to prev line
        QTextCursor cursor(ui->codeBrowser->document()->findBlockByLineNumber(nextLocation[1]));
        ui->codeBrowser->setTextCursor(cursor);
        ui->tabWidget->setCurrentIndex(0);
        ui->codeBrowser->setFocus();
        setUpdatesEnabled(true);
    }
}

void MainWindow::on_actionBack_triggered()
{
    on_backButton_clicked();
}

void MainWindow::on_actionForward_triggered()
{
    on_forwardButton_clicked();
}

/*
 *  Searching
*/

// Find calls to the current function
void MainWindow::on_actionFind_Calls_to_Current_Function_triggered()
{
    QString functionName = functionList.getFunction(currentFunctionIndex).getName();
    QVector< QVector<QString> > results = functionList.findCallsToFunction(functionName);

    if (!results.isEmpty()){
        QString resultsStr = "";
        for (int i = 0; i < results.length(); i++){
            QVector<QString> result = results[i];
            resultsStr.append(result[1] + "    " + result[0] + "\n");
        }

        // Display results
        ResultsDialog resultsDialog;
        resultsDialog.setWindowModality(Qt::WindowModal);
        resultsDialog.setResultsLabelText("Calls to function " + functionName);
        resultsDialog.setResultsText(resultsStr);
        resultsDialog.exec();

    } else {
        QMessageBox::information(this, tr("Calls to Function"), "No calls found to function " + functionName,QMessageBox::Close);
    }
}

// Find all calls to current location
void MainWindow::on_actionFind_Calls_to_Current_Location_triggered()
{
    QTextCursor cursor = ui->codeBrowser->textCursor();
    int lineNum = cursor.blockNumber();
    QString targetLocation = functionList.getFunction(currentFunctionIndex).getAddressAt(lineNum);
    QVector< QVector<QString> > results = functionList.findCallsToAddress(targetLocation);

    if (!results.isEmpty()){
        QString resultsStr = "";
        for (int i = 0; i < results.length(); i++){
            QVector<QString> result = results[i];
            resultsStr.append(result[1] + "  " + result[0] + "\n");
        }

        // Display results
        ResultsDialog resultsDialog;
        resultsDialog.setWindowModality(Qt::WindowModal);
        resultsDialog.setResultsLabelText("Calls to location " + targetLocation);
        resultsDialog.setResultsText(resultsStr);
        resultsDialog.exec();

    } else {
        QMessageBox::information(this, tr("Calls to Location"), "No calls found to location " + targetLocation,QMessageBox::Close);
    }
}


/*
 *  Options
*/

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
        // Check all boxes
        ui->archiveHeadersCheckBox->setChecked(true);
        ui->fileHeadersCheckBox->setChecked(true);
        ui->privateHeadersCheckBox->setChecked(true);
        ui->sectionHeadersCheckbox->setChecked(true);
        // Disable other options
        ui->archiveHeadersCheckBox->setEnabled(false);
        ui->fileHeadersCheckBox->setEnabled(false);
        ui->privateHeadersCheckBox->setEnabled(false);
        ui->sectionHeadersCheckbox->setEnabled(false);
    } else {
        // Re-enble other options
        ui->archiveHeadersCheckBox->setEnabled(true);
        ui->fileHeadersCheckBox->setEnabled(true);
        ui->privateHeadersCheckBox->setEnabled(true);
        ui->sectionHeadersCheckbox->setEnabled(true);
    }
    objDumper.setHeaderFlags(getHeaderFlags());
}

QString MainWindow::getHeaderFlags(){
    QString flags = "";

    if (ui->archiveHeadersCheckBox->isChecked())
        flags.append("-a ");
    if (ui->fileHeadersCheckBox->isChecked())
        flags.append("-f ");
    if (ui->privateHeadersCheckBox->isChecked())
        flags.append("-p ");
    if (ui->sectionHeadersCheckbox->isChecked())
        flags.append("-h ");

    return flags;
}

void MainWindow::on_archiveHeadersCheckBox_clicked()
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}

void MainWindow::on_fileHeadersCheckBox_clicked()
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}

void MainWindow::on_privateHeadersCheckBox_clicked()
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}

void MainWindow::on_sectionHeadersCheckbox_clicked()
{
    if (!ui->allHeadersCheckBox->isChecked())
        objDumper.setHeaderFlags(getHeaderFlags());
}

void MainWindow::on_demanlgeCheckBox_toggled(bool checked)
{
    if (checked){
        objDumper.setOptionalFlags("-C");
        settings.setValue("demangle", true);
    } else {
        objDumper.setOptionalFlags("");
        settings.setValue("demangle", false);
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

/*
 *  Window
*/

void MainWindow::on_actionProject_triggered()
{
    QString aboutStr = "ObjGui is a binary analysis tool and GUI frontend for objdump.\n\n"
                       "Project Page: https://github.com/jubal-R/ObjGui\n\n"

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
