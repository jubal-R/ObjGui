#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"
#include "QScrollBar"
#include "QInputDialog"
#include "QProgressDialog"
#include "QFuture"
#include "QFile"
#include "QTextStream"
#include "QtConcurrent/QtConcurrent"

#include "QDebug"

#include "resultsdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->hexAddressBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->hexBrowser->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(ui->hexBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->hexAddressBrowser->verticalScrollBar(), SLOT(setValue(int)));

    QObject::connect(ui->stringsAddressBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->stringsBrowser->verticalScrollBar(), SLOT(setValue(int)));
    QObject::connect(ui->stringsBrowser->verticalScrollBar(), SIGNAL(valueChanged(int)), ui->stringsAddressBrowser->verticalScrollBar(), SLOT(setValue(int)));

    /*
     *  Setup builtin fonts
    */

    // Sans serif
    int sansid = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Regular.ttf");
    QString sansfamily = QFontDatabase::applicationFontFamilies(sansid).at(0);
    QFont sans(sansfamily);
    sans.setPointSize(11);

    this->setFont(sans);
    ui->disTabWidget->setFont(sans);
    ui->syntaxLabel->setFont(sans);
    ui->disassemblyFlagLabel->setFont(sans);
    ui->functionListLabel->setFont(sans);
    ui->functionList->setFont(sans);
    ui->customBinaryButton->setFont(sans);
    ui->stringsAddressBrowser->setFont(sans);
    ui->stringsBrowser->setFont(sans);
    ui->symbolsBrowser->setFont(sans);
    ui->relocationsBrowser->setFont(sans);
    ui->headersBrowser->setFont(sans);

    // Sans serif bold
    int sansBoldId = QFontDatabase::addApplicationFont(":/fonts/NotoSans-Bold.ttf");
    QString sansBoldFamily = QFontDatabase::applicationFontFamilies(sansBoldId).at(0);
    QFont sansBold(sansBoldFamily);
    sansBold.setPointSize(11);
    sansBold.setBold(true);

    ui->syntaxLabel->setFont(sansBold);
    ui->disassemblyFlagLabel->setFont(sansBold);
    ui->functionLabel->setFont(sansBold);
    ui->addressLabel->setFont(sansBold);
    ui->fileOffsetLabel->setFont(sansBold);
    ui->sectionLabel->setFont(sansBold);
    ui->hexAddressLabel->setFont(sansBold);
    ui->hexLabel->setFont(sansBold);
    ui->symbolsTableLabel->setFont(sansBold);
    ui->relocationsLabel->setFont(sansBold);
    ui->stringsAddressLabel->setFont(sansBold);
    ui->stringsLabel->setFont(sansBold);

    // Monospace
    int monoid = QFontDatabase::addApplicationFont(":/fonts/Anonymous Pro.ttf");
    QString monofamily = QFontDatabase::applicationFontFamilies(monoid).at(0);
    QFont mono(monofamily);
    mono.setPointSize(12);

    ui->codeBrowser->setFont(mono);
    ui->hexAddressBrowser->setFont(mono);
    ui->hexBrowser->setFont(mono);
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

    ui->searchBar->hide();
    currentSearchTerm = "";

    /*
     *  Set options from saved settings
    */

    // Syntax
    if (settings.value("syntax", "intel") == "intel"){
        ui->actionIntel->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(0);
        disassemblyCore.setOutputSyntax("intel");

    }else if (settings.value("syntax", "intel") == "att"){
        ui->actionAtt->setChecked(true);
        ui->syntaxComboBox->setCurrentIndex(1);
        disassemblyCore.setOutputSyntax("att");
    }

    // Optional flags
    if (settings.value("demangle", false) == true){
        ui->demanlgeCheckBox->setChecked(true);
        disassemblyCore.setDemangleFlag("-C");
    }

    // Custom binary
    if (settings.value("useCustomBinary", false).toBool()){
        ui->customBinaryCheckBox->setChecked(true);
        disassemblyCore.setUseCustomBinary(true);
    }
    disassemblyCore.setobjdumpBinary(settings.value("customBinary", "").toString());
    ui->customBinaryLineEdit->setText(settings.value("customBinary", "").toString());

    // Style
    disHighlighter = new DisassemblyHighlighter(ui->codeBrowser->document(), "Default");
    headerHighlighter = new HeaderHighlighter(ui->headersBrowser->document());

    QString theme = settings.value("theme", "default").toString();

    if (theme == "dark"){
            on_actionDark_triggered();
    } else if (theme == "solarized"){
        on_actionSolarized_triggered();
    }else if (theme == "solarizedDark"){
        on_actionSolarized_Dark_triggered();
    } else {
        on_actionDefault_triggered();
    }

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

    delete ui;
}

/*
 *  Load Disassembly
*/

//  Load binary and display disassembly
void MainWindow::loadBinary(QString file){

    if (file != ""){
        this->setWindowTitle("ObjGUI - " + file);

        clearUi();

        if (canDisassemble(file)) {
            QProgressDialog progress("Loading Disassembly", "", 0, 4, this);
            progress.setCancelButton(0);
            progress.setWindowModality(Qt::WindowModal);
            progress.setMinimumDuration(500);
            progress.setValue(0);

            // Disassemble in seperate thread
            QFuture<void> disassemblyThread = QtConcurrent::run(&disassemblyCore, &DisassemblyCore::disassemble, file);

            while (!disassemblyThread.isFinished()){
                qApp->processEvents(QEventLoop::ExcludeUserInputEvents);
            }

            progress.setValue(1);

            if (!disassemblyCore.disassemblyIsLoaded()){
                ui->codeBrowser->setPlainText("File format not recognized.");
                ui->addressLabel->setText("");
                ui->functionLabel->setText("");
		int num1 = 0;
	   	std::string s1 = ("Instruction count: "+std::to_string(num1));
	    	QString arg1 = QString::fromLocal8Bit(s1.c_str());
	    	ui->fileInstructionCountlabel->setText(arg1);
            } else {
                // If all good, display disassembly data
                displayFunctionData();
		
		//Display number of instructions detected
		int num1 = 0;
		QStringList arg;
		arg << "-d" << file;
		QProcess *proc = new QProcess();
		proc->start(ui->customBinaryLineEdit->text(), arg);
		proc->waitForFinished();
		QString result=proc->readAllStandardOutput();
		QRegularExpression re("[0-9a-fA-F]+:\t");
		QRegularExpressionMatchIterator i = re.globalMatch(result);
		while(i.hasNext()) {
			QRegularExpressionMatch match = i.next();
			(void)match; //Suppress -Wunused-parameter
			num1=num1+1;
		}
		//stops here
	   	std::string s1 = ("Instruction count: "+std::to_string(num1));
	    	QString arg1 = QString::fromLocal8Bit(s1.c_str());
	    	ui->fileInstructionCountlabel->setText(arg1);

                // Add initial location to history
                addToHistory(currentFunctionIndex, 0);

                enableMenuItems();
            }

            progress.setValue(2);

            displayHexData();

            progress.setValue(3);

            setUpdatesEnabled(false);

            ui->fileFormatlabel->setText(disassemblyCore.getFileFormat(file));

            ui->symbolsBrowser->setPlainText(disassemblyCore.getSymbolsTable(file));
            ui->relocationsBrowser->setPlainText(disassemblyCore.getRelocationEntries(file));
            ui->headersBrowser->setPlainText(disassemblyCore.getHeaders(file));
            setUpdatesEnabled(true);

            // Clear specified target
            disassemblyCore.setTarget("");

            // Load strings data
            ui->stringsAddressBrowser->setPlainText(disassemblyCore.getStringsAddresses());
            ui->stringsBrowser->setPlainText(disassemblyCore.getStrings());

            progress.setValue(4);
        }
    }
}

// Disassemble
void MainWindow::on_actionOpen_triggered()
{
    // Prompt user for file
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), files.getCurrentDirectory(), tr("All (*)"));

    // Update current directory and load file
    if (file != ""){
        files.setCurrentDirectory(file);

        loadBinary(file);
        ui->disTabWidget->setCurrentIndex(0);
        ui->codeBrowser->setFocus();
    }

}

//Dump File
void MainWindow::on_actionDumpFile_triggered()
{
	QString filename = "objdumpOutput.txt";
	QFile file2(filename);
	if(file2.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text)) {
		QTextStream stream(&file2);

		//dump functions
		QStringList funcs = disassemblyCore.getFunctionNames();
		QVector<QString> baseOffsets = disassemblyCore.getBaseOffsets();
		for(const auto& func : funcs) {
			Function currFunc = disassemblyCore.getFunction(func);
			stream << "F|"+currFunc.getName().remove("@plt")+"|"+currFunc.getAddress()<<endl;
			//write here using stream << "something" << endl;
		}

		// dump instructions
		// regex for parsing instruction nmeumonics: [\s]+\t(...)[.]*[a-z]*
		// after regexing for those, regex for: (...)[.]*[a-z]*
		// use [\s][a-fA-F0-9]+[:] regex to grab address
		// out of those regex matches to grab JUST the instruction mnemonic
		// TO-DO AFTER IMPLEMENTING ABOVE: Make a regex to grab the instruction address on the first column of objdump output
		
		QStringList arg;
		arg << "-d" << disassemblyCore.getFileName();
		QProcess *proc = new QProcess();
		proc->start(ui->customBinaryLineEdit->text(), arg);
		proc->waitForFinished();
		QString result=proc->readAllStandardOutput();
		QString line;
		QTextStream stream2(&result);
		while (stream2.readLineInto(&line)) {
			QString address;
			QString nmeumonic;
			QRegularExpression addressRegex("[\\s][a-fA-F0-9]+[:]");

			QRegularExpressionMatch match = addressRegex.match(line);
			if(match.hasMatch()) {
				QString matched = match.captured(0);
				address = matched.mid(1, (matched.length()-2));
 			} else {
				continue;
			}


			QRegularExpression nmeumonicRegex("[\\s]+\t(...)[.]*[a-z]*");
			QRegularExpressionMatch match2 = nmeumonicRegex.match(line);
			if(match2.hasMatch()) {
				nmeumonic = match2.captured(0).simplified();
				nmeumonic.remove("\t");
				if(nmeumonic.contains(" ")) {
					nmeumonic = nmeumonic.split(" ").at(0);
				}
				/*
				QRegularExpression nmeumonicRegex2("(...)[.]*[a-z]*");
				QRegularExpressionMatch match3 = nmeumonicRegex2.match(line2);
				if(match3.hasMatch()) {
					qDebug() << "MATCH 3 BEFORE: "<<match3.captured(0)<<endl;
					nmeumonic = match3.captured(0).simplified();
					nmeumonic.remove('\t');
					qDebug() << "MATCH 3 AFTER: "<<nmeumonic<<endl;
				} else {
					continue;
				}
				*/
			} else {
				continue;
			}
			while(address.size() < 8) {
                                address = "0"+address;
                        }
			stream << "I|"+nmeumonic<<"|"<<address<<endl;
		}


	}
	file2.close();
}


bool MainWindow::canDisassemble(QString file){
    // Check for errors or invalid file
    QString errorMsg = disassemblyCore.getObjdumpErrorMsg(file);
    bool canDisassemble = true;

    // If format is ambigous message, let user user select format from list of matching formats
    if (!errorMsg.isEmpty()){
        if (errorMsg.contains("Matching formats")){
            QStringList formats = errorMsg.split(":");
            if (formats.length() == 2){
                formats = formats.at(1).split(" ", QString::SkipEmptyParts);

                if (!formats.isEmpty()){
                    // Get target format and set flag
                    QString format = QInputDialog::getItem(this, "Select matching format", "Format is ambigous, select matching format:", formats, 0, false);
                    disassemblyCore.setTarget("--target=" + format);
                } else {
                    // Display error message
                    ui->codeBrowser->setPlainText(errorMsg);
                    canDisassemble = false;
                }
            }

        } else {
            // Display error message
            ui->codeBrowser->setPlainText(errorMsg);
            canDisassemble = false;
        }
    }
    return canDisassemble;
}

/*
 *  Display Disassembly Data
*/

// Set lables and code browser to display function info and contents
void MainWindow::displayFunctionText(QString functionName){
    if (disassemblyCore.disassemblyIsLoaded()){
        Function function = disassemblyCore.getFunction(functionName);

        setUpdatesEnabled(false);
        ui->addressValueLabel->setText(function.getAddress());
        ui->fileOffsetValueLabel->setText(function.getFileOffset());
        ui->functionLabel->setText(function.getName());
        ui->sectionValueLabel->setText(function.getSection());
        ui->codeBrowser->setPlainText(function.getContents());
        setUpdatesEnabled(true);

        int index = disassemblyCore.getFunctionIndex(functionName);
        if (index >= 0){
            currentFunctionIndex = index;
        }
    }
}

void MainWindow::displayFunctionText(int functionIndex){
    if (disassemblyCore.disassemblyIsLoaded()){
        Function function = disassemblyCore.getFunction(functionIndex);

        // If index is out of range an empty function will be returned
        if (function.getAddress() != ""){
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
}
// Setup functionlist and display function data
void MainWindow::displayFunctionData(){
    if (disassemblyCore.disassemblyIsLoaded()){
        // Populate function list in sidebar
        ui->functionList->addItems(disassemblyCore.getFunctionNames());

	int num = 0;
	for(const auto& i : disassemblyCore.getFunctionNames()) {
		(void)i; //Suppress -Wunused-parameter
		num = num + 1;
	}
	std::string s = ("Functions ["+std::to_string(num)+"]");
	QString arg = QString::fromLocal8Bit(s.c_str());
	ui->functionListLabel->setText(arg);

        // Display main function by default if it exists
        if (disassemblyCore.functionExists("main"))
            displayFunctionText("main");
        else {
            QString firstIndexName = disassemblyCore.getFunction(0).getName();
            displayFunctionText(firstIndexName);
        }

    }
}

//  Highlight current line of function
void MainWindow::highlightCurrentLine(){
   QList<QTextEdit::ExtraSelection> extraSelections;

   QTextEdit::ExtraSelection selections;
   selections.format.setBackground(lineColor);
   selections.format.setProperty(QTextFormat::FullWidthSelection, true);
   selections.cursor = ui->codeBrowser->textCursor();
   selections.cursor.clearSelection();
   extraSelections.append(selections);

   ui->codeBrowser->setExtraSelections(extraSelections);

}

void MainWindow::displayHexData(){
    // Set hex view values
    setUpdatesEnabled(false);
    ui->hexAddressBrowser->setPlainText(disassemblyCore.getSectionAddressDump());
    ui->hexBrowser->setPlainText(disassemblyCore.getSectionHexDump());
    setUpdatesEnabled(true);
}

void MainWindow::clearUi(){
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
    ui->fileFormatlabel->clear();
    ui->symbolsBrowser->clear();
    ui->relocationsBrowser->clear();
    ui->headersBrowser->clear();
    ui->stringsAddressBrowser->clear();
    ui->stringsBrowser->clear();

    // Clear history
    history.clear();
}

void MainWindow::enableMenuItems(){
    // Enable navigation and tools
    ui->actionGo_To_Address->setEnabled(true);
    ui->actionGo_to_Address_at_Cursor->setEnabled(true);
    ui->actionGet_Offset->setEnabled(true);
    ui->actionGet_File_Offset_of_Current_Line->setEnabled(true);
    ui->actionFind_References->setEnabled(true);
    ui->actionFind_Calls_to_Current_Function->setEnabled(true);
    ui->actionFind_Calls_to_Current_Location->setEnabled(true);
}

/*
 *  Navigation
*/

// Go to virtual memory address
void MainWindow::goToAddress(QString targetAddress){
    if (targetAddress != ""){
        if (!targetAddress.startsWith("0x")){
            targetAddress = "0x" + targetAddress;
        }

        // Find address index
        QVector<int> location = disassemblyCore.getAddressLocation(targetAddress);

        // Check if address was found
        if (location[0] >= 0){
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
            ui->disTabWidget->setCurrentIndex(0);
            ui->codeBrowser->setFocus();
            setUpdatesEnabled(true);

            // Add new location to history
            addToHistory(currentFunctionIndex, location[1]);

        } else {
            // Search strings
            int stringsIndex = disassemblyCore.getStringIndexByAddress(targetAddress);

            if (stringsIndex >= 0){
                ui->infoTabWidget->setCurrentIndex(0);
                QTextCursor cursor(ui->stringsBrowser->document()->findBlockByLineNumber(stringsIndex));
                cursor.select(QTextCursor::LineUnderCursor);
                ui->stringsBrowser->setTextCursor(cursor);
                ui->stringsBrowser->setFocus();

            } else {
                QMessageBox::warning(this, tr("Go to Address"), "Address not found.",QMessageBox::Ok);
            }

        }

    }
}

// Go to Address triggered
void MainWindow::on_actionGo_To_Address_triggered()
{
    bool ok = true;
    QString targetAddress = QInputDialog::getText(this, tr("Go to Address"),tr("Address"), QLineEdit::Normal,"", &ok).trimmed();
    if (ok)
        goToAddress(targetAddress);

}

// Go to Address at Cursor triggered
void MainWindow::on_actionGo_to_Address_at_Cursor_triggered()
{
    QTextCursor cursor = ui->codeBrowser->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    QString targetAddress = cursor.selectedText();

    goToAddress(targetAddress);
}

// Display function clicked in sidebar
void MainWindow::on_functionList_itemDoubleClicked(QListWidgetItem *item)
{
    // Display function
    displayFunctionText(item->text());
    ui->disTabWidget->setCurrentIndex(0);
    // Add new location to history
    addToHistory(currentFunctionIndex, 0);
}

// Get file offset of current line of disassembly
void MainWindow::on_actionGet_Offset_triggered()
{
    bool ok;
    QString targetAddress = QInputDialog::getText(this, tr("Get File Offset"),tr("Address"), QLineEdit::Normal,"", &ok).trimmed();
    if (ok && !targetAddress.isEmpty()){
        // Get file offset of address
        QVector<QString> offset = disassemblyCore.getFileOffset(targetAddress);

        if(!offset[0].isEmpty()){
            QString offsetMsg = "File Offset of Address " + targetAddress+ "\nHex: " + offset[0] + "\nInt: " + offset[1];
            QMessageBox::information(this, tr("File Offset"), offsetMsg,QMessageBox::Close);
        } else {
            QMessageBox::warning(this, tr("File Offset"), "Invalid address.",QMessageBox::Close);
        }

    } else {
        QMessageBox::warning(this, tr("File Offset"), "No address entered.",QMessageBox::Close);
    }
}

// Get Offset of Current Line triggered
void MainWindow::on_actionGet_File_Offset_of_Current_Line_triggered()
{
    if (disassemblyCore.disassemblyIsLoaded()){
        int currentTab = ui->disTabWidget->currentIndex();
        QString offsetMsg = "";

        if (currentTab == 0 && ui->codeBrowser->hasFocus()){
            Function function = disassemblyCore.getFunction(currentFunctionIndex);
            QTextCursor cursor = ui->codeBrowser->textCursor();
            int lineNum = cursor.blockNumber();
            // Get address
            QString currentLineAddressStr = function.getAddressAt(lineNum);

            if (!currentLineAddressStr.isEmpty()){
                // Get file offset of address
                QVector<QString> offset = disassemblyCore.getFileOffset(currentLineAddressStr);
                offsetMsg = "File Offset of Address " + currentLineAddressStr + "\nHex: " + offset[0] + "\nInt: " + offset[1];
            }

        } else if (ui->infoTabWidget->currentIndex() == 0 && ui->stringsBrowser->hasFocus()){
            QTextCursor cursor = ui->stringsBrowser->textCursor();
            int lineNum = cursor.blockNumber();
            // Get address
            QString currentLineAddressStr = disassemblyCore.getStringAddressAt(lineNum);

            if (!currentLineAddressStr.isEmpty()){
                // Get file offset of address
                QVector<QString> offset = disassemblyCore.getFileOffset(currentLineAddressStr);
                offsetMsg = "File Offset of Address " + currentLineAddressStr + "\nHex: " + offset[0] + "\nInt: " + offset[1];
            }
        }

        if (!offsetMsg.isEmpty())
            QMessageBox::information(this, tr("File Offset"), offsetMsg,QMessageBox::Close);

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

// Back button
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
        ui->disTabWidget->setCurrentIndex(0);
        ui->codeBrowser->setFocus();
        setUpdatesEnabled(true);
    }
}

// Forward button
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
        ui->disTabWidget->setCurrentIndex(0);
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

void MainWindow::displayResults(QVector< QVector<QString> > results, QString resultsLabel){
    if (!results.isEmpty()){
        QString resultsStr = "";
        for (int i = 0; i < results.length(); i++){
            QVector<QString> result = results[i];
            if (result.length() == 2)
                resultsStr.append(result[1] + "    " + result[0] + "\n");
        }

        // Display results
        ResultsDialog resultsDialog;
        resultsDialog.setWindowModality(Qt::WindowModal);
        resultsDialog.setResultsLabelText(resultsLabel);
        resultsDialog.setResultsText(resultsStr);
        resultsDialog.exec();
    }
}

// Find calls to the current function
void MainWindow::on_actionFind_Calls_to_Current_Function_triggered()
{
    QString functionName = disassemblyCore.getFunction(currentFunctionIndex).getName();
    QVector< QVector<QString> > results = disassemblyCore.findCallsToFunction(functionName);

    if (!results.isEmpty()){
        // Display results
        displayResults(results, "Calls to function " + functionName);

    } else {
        QMessageBox::information(this, tr("Calls to Function"), "No calls found to function " + functionName,QMessageBox::Close);
    }
}

// Find all references to a target location
void MainWindow::findReferencesToLocation(QString target){
    if (!target.isEmpty()){
        QVector< QVector<QString> > results = disassemblyCore.findReferences(target);

        if (!results.isEmpty()){
            // Display results
            displayResults(results, "References to " + target);

        } else {
            QMessageBox::information(this, tr("References"), "No references found to " + target,QMessageBox::Close);
        }

    } else {
        QMessageBox::warning(this, tr("Search failed"), "Cannot search for empty string.",QMessageBox::Close);
    }

}

// Find References
void MainWindow::on_actionFind_References_triggered()
{
    bool ok = true;
    QString targetAddress = QInputDialog::getText(this, tr("Find References"),tr("Find References to"), QLineEdit::Normal,"", &ok).trimmed();
    if (ok)
        findReferencesToLocation(targetAddress);
}

// Find all calls to current location
void MainWindow::on_actionFind_Calls_to_Current_Location_triggered(){
    if (disassemblyCore.disassemblyIsLoaded()){
        QTextCursor cursor = ui->codeBrowser->textCursor();
        int lineNum = cursor.blockNumber();
        QString targetLocation = disassemblyCore.getFunction(currentFunctionIndex).getAddressAt(lineNum).mid(2);

        QVector< QVector<QString> > results = disassemblyCore.findReferences(targetLocation);

        if (!results.isEmpty()){
            // Display results
            displayResults(results, "Calls to address " + targetLocation);

        } else {
            QMessageBox::information(this, tr("Calls to address"), "No calls found to address " + targetLocation,QMessageBox::Close);
        }
    }
}

// Toggle searchbar
void MainWindow::on_actionFind_2_triggered()
{
    if (ui->searchBar->isHidden()){
        ui->searchBar->show();
        ui->findLineEdit->setFocus();
    }else {
        ui->searchBar->hide();
    }
}

// Find and highlight search term in the target widget
void MainWindow::find(QString searchTerm, QPlainTextEdit *targetWidget, bool searchBackwords){
    if (targetWidget != NULL){
        QTextCursor cursor = targetWidget->textCursor();
        int currentPosition = cursor.position();
        bool found = false;

        // Start new search from begining of document
        if (searchTerm != currentSearchTerm){
            if (!searchBackwords)
                cursor.movePosition(QTextCursor::Start);
            else
                cursor.movePosition(QTextCursor::End);

            targetWidget->setTextCursor(cursor);

            currentSearchTerm = searchTerm;
            if (!searchBackwords)
                found = targetWidget->find(searchTerm);
            else
                found = targetWidget->find(searchTerm, QTextDocument::FindBackward);

            // Call vertical scrollbar value changed to keep widgets scrolling synced
            targetWidget->verticalScrollBar()->valueChanged(targetWidget->verticalScrollBar()->value());

            // If not found move cursor back to original position and display not found message
            if(!found){
                cursor.setPosition(currentPosition);
                targetWidget->setTextCursor(cursor);
                QMessageBox::information(this, tr("Not Found"), "\"" + searchTerm + "\" not found.", QMessageBox::Close);
            }

        } else {
            if (!searchBackwords)
                found = targetWidget->find(searchTerm);
            else
                found = targetWidget->find(searchTerm, QTextDocument::FindBackward);

            // Call vertical scrollbar value changed to keep widgets scrolling synced
            targetWidget->verticalScrollBar()->valueChanged(targetWidget->verticalScrollBar()->value());

            // If not found wrap to begining and search again
            if (!found){
                if (!searchBackwords)
                    cursor.movePosition(QTextCursor::Start);
                else
                    cursor.movePosition(QTextCursor::End);

                targetWidget->setTextCursor(cursor);

                if (!searchBackwords)
                    found = targetWidget->find(searchTerm);
                else
                    found = targetWidget->find(searchTerm, QTextDocument::FindBackward);

                // Call vertical scrollbar value changed to keep widgets scrolling synced
                targetWidget->verticalScrollBar()->valueChanged(targetWidget->verticalScrollBar()->value());
                if (!found){
                    cursor.setPosition(currentPosition);
                    targetWidget->setTextCursor(cursor);
                    QMessageBox::information(this, tr("Not Found"), "\"" + searchTerm + "\" not found.", QMessageBox::Close);
                }
            }
        }

    }
}

void MainWindow::on_findButton_clicked()
{
    QString searchTerm = ui->findLineEdit->text();
    int currentTabIndex = ui->disTabWidget->currentIndex();
    QPlainTextEdit *targetWidget = NULL;

    // Set pointer to target widget given current tab index
    switch (currentTabIndex) {
    case 0:
        targetWidget = ui->codeBrowser;
        break;
    case 1:
        targetWidget = ui->hexBrowser;
        break;
    default:
        break;
    }

    find(searchTerm, targetWidget, false);

}

void MainWindow::on_findLineEdit_returnPressed()
{
    on_findButton_clicked();
}

// Find Prev (search backwards)
void MainWindow::on_findPrevButton_clicked()
{
    QString searchTerm = ui->findLineEdit->text();
    int currentTabIndex = ui->disTabWidget->currentIndex();
    QPlainTextEdit *targetWidget = NULL;

    // Set pointer to target widget given current tab index
    switch (currentTabIndex) {
    case 0:
        targetWidget = ui->codeBrowser;
        break;
    case 1:
        targetWidget = ui->hexBrowser;
        break;
    default:
        break;
    }

    find(searchTerm, targetWidget, true);
}

void MainWindow::on_stringsSearchBar_returnPressed()
{
    QString searchTerm = ui->stringsSearchBar->text();
    QPlainTextEdit *stringsBrowser = ui->stringsBrowser;
    find(searchTerm, stringsBrowser, false);
}

/*
 *  Options
*/

void MainWindow::on_actionIntel_triggered()
{
    settings.setValue("syntax", "intel");
    disassemblyCore.setOutputSyntax("intel");
    ui->actionIntel->setChecked(true);
    ui->actionAtt->setChecked(false);
    ui->syntaxComboBox->setCurrentIndex(0);
}

void MainWindow::on_actionAtt_triggered()
{
    settings.setValue("syntax", "att");
    disassemblyCore.setOutputSyntax("att");
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
        disassemblyCore.setDisassemblyFlag("-D");
    else
        disassemblyCore.setDisassemblyFlag("-d");
}

void MainWindow::on_demanlgeCheckBox_toggled(bool checked)
{
    if (checked){
        disassemblyCore.setDemangleFlag("-C");
        settings.setValue("demangle", true);
    } else {
        disassemblyCore.setDemangleFlag("");
        settings.setValue("demangle", false);
    }
}

void MainWindow::on_customBinaryButton_clicked()
{
    QString objdumpBinary = QFileDialog::getOpenFileName(this, tr("Select Binary"), files.getCurrentDirectory(), tr("All (*)"));
    if (objdumpBinary != ""){
        ui->customBinaryLineEdit->setText(objdumpBinary);
        disassemblyCore.setobjdumpBinary(objdumpBinary);
        settings.setValue("customBinary", objdumpBinary);
    }
}

void MainWindow::on_customBinaryCheckBox_toggled(bool checked)
{
    if (checked){
        ui->customBinaryButton->setEnabled(true);
        settings.setValue("useCustomBinary", true);
        disassemblyCore.setUseCustomBinary(true);
    } else {
        ui->customBinaryButton->setEnabled(false);
        settings.setValue("useCustomBinary", false);
        disassemblyCore.setUseCustomBinary(false);
    }
}

/*
 * Themes
*/

// Style central widget
void MainWindow::setCentralWidgetStyle(QString foregroundColor2, QString backgroundColor2){
    QString centralWidgetStyle = "background-color: " + backgroundColor2 + ";"
            "color: " + foregroundColor2 + ";";
    ui->centralWidget->setStyleSheet(centralWidgetStyle);
}

void MainWindow::setMainStyle(QString foregroundColor, QString backgroundColor, QString backgroundColor3){
    QString mainStyle = "QScrollBar:vertical {"
                        "background: "+ backgroundColor3 +";"
                        "width: 10px;"
                    "}"
                    "QScrollBar:horizontal {"
                        "background: " + backgroundColor3 +";"
                        "height: 10px;"
                    "}"
                    "QScrollBar::handle:vertical,QScrollBar::handle:horizontal {"
                        "background-color: " + backgroundColor + ";"
                        "border: 1px solid " + backgroundColor3 + ";"
                        "border-radius: 5px;"
                     "}"
                     "QScrollBar::add-line:vertical,QScrollBar::add-line:horizontal {"
                          "background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                          "stop: 0 rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                          "height: 0px;"
                          "subcontrol-position: bottom;"
                          "subcontrol-origin: margin;"
                      "}"
                      "QScrollBar::sub-line:vertical,QScrollBar::sub-line:horizontal {"
                            "background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                            "stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                            "height: 0 px;"
                            "subcontrol-position: top;"
                            "subcontrol-origin: margin;"
                      "}"
                      "QPlainTextEdit {"
                          "background-color: "+ backgroundColor +";"
                          "color:"+ foregroundColor +";"
                          "border: 0px; selection-background-color: #404f4f;"
                      "}";
    ui->main->setStyleSheet(mainStyle);
}

// Style tab widget
void MainWindow::setTabWidgetStyle(QString foregroundColor, QString foregroundColor2, QString backgroundColor, QString backgroundColor2){
    QString style = "#disTab, #hexTab, #optionsTab {"
                "background-color: " + backgroundColor + ";"
                "color: " + foregroundColor + ";"
            "}"
            "#hexAddressBrowser {"
                "color: " + foregroundColor2 + ";"
            "}"
            "QTabBar::tab:selected{"
                "color: " + foregroundColor2 +";"
                "border-bottom:2px solid " + foregroundColor2 +";"
            "}"
            "QTabBar::tab {"
                "background-color: " + backgroundColor2 +";"
                "border-bottom:2px solid " + backgroundColor2 +";"
                "min-width: 102px;"
                "height: 30px;"
            "}"
            "QTabWidget::tab-bar {"
                "left: 5px;"
            "}"
            "QTabWidget::pane {"
                "border: 1px solid #c0c0c0;"
            "}"
            "QComboBox {"
                "background-color: #fafafa;"
                "color: #555555;"
            "}"
            "QCheckBox {"
                "background-color: " + backgroundColor + ";"
                "color: " + foregroundColor + ";"
            "}"
            "QLabel {"
                "background-color: " + backgroundColor + ";"
                "color: " + foregroundColor + ";"
            "}";
   ui->disTabWidget->setStyleSheet(style);

}

void MainWindow::setInfoTabWidgetStyle(QString foregroundColor, QString foregroundColor2, QString backgroundColor, QString backgroundColor2){
    QString style = "#symbolsTab, #relocationsTab, #stringsTab, #headersTab {"
                "background-color: " + backgroundColor + ";"
                "color: " + foregroundColor + ";"
             "}"
             "QTabBar::tab:selected {"
                "border-bottom:2px solid " + foregroundColor2 +";"
             "}"
             "QTabBar::tab {"
                "background-color: " + backgroundColor2 +";"
                "min-width: 70px;"
             "}"
             "QTabWidget::pane {"
                "border: 1px solid #c0c0c0;"
             "}"
             "QPlainTextEdit {"
                "background-color: "+ backgroundColor +";"
                "color:"+ foregroundColor +";"
                "border: 0px;"
                "selection-background-color: #404f4f;"
                "font-size: 10pt;"
             "}"
             "QLabel {"
                "background-color: " + backgroundColor + ";"
                "color: " + foregroundColor + ";"
                "font-size: 10pt;"
             "}"
             "QLineEdit {"
                "background-color: " + backgroundColor + ";"
                "color: " + foregroundColor + ";"
                "border: 1px solid "+ foregroundColor + ";"
             "}"
             "QScrollBar:vertical {"
                "background: "+ backgroundColor +";"
             "}"
             "QScrollBar:horizontal {"
                "background: "+ backgroundColor +";"
             "}";
    ui->infoTabWidget->setStyleSheet(style);
}

void MainWindow::setSidebarStyle(QString foregroundColor, QString backgroundColor){
    QString sidebarStyle = "#functionList {background-color: " + backgroundColor + "; color: " + foregroundColor + "; font-size: 10pt; border: 1px solid #c0c0c0;}";
    ui->sidebar_2->setStyleSheet(sidebarStyle);
}

void MainWindow::setMenuStyle(QString foregroundColor, QString backgroundColor, QString selectedColor){
    QString menuStyle = "QMenu::item:selected {background-color: " + selectedColor + "; color: #fafafa;}"
            "QMenu::item::disabled {color: #aaaaaa}"
            "QMenu::item {background-color: " + backgroundColor + "; color: " + foregroundColor + ";}"
            "QMenuBar::item {background-color: " + backgroundColor + "; color: " + foregroundColor + ";}"
            "QMenuBar {background-color: " + backgroundColor + ";}";
    ui->menuBar->setStyleSheet(menuStyle);
}

void MainWindow::setNavbarStyle(QString foregroundColor, QString backgroundColor){
    (void)foregroundColor; //Suppress -Wunused-parameter

    QString navBarStyle = "#navBar {background-color: " + backgroundColor + "; border-bottom: 1px solid #d4d4d4;}";
    ui->navBar->setStyleSheet(navBarStyle);

//    QString disTabStyle = "QTabBar {background-color: " + backgroundColor + ";}";
//    ui->disTabWidget->setStyleSheet(disTabStyle);
}

// Set theme default
void MainWindow::on_actionDefault_triggered()
{
    settings.setValue("theme", "default");

    QString fgc = "#4c4c4c";
    QString bgc = "#f6f6f6";
    QString fgc2 = "#4c4c4c";
    QString bgc2 = "#e0e0e0";
    QString addrc = "#268BD2";
    QString bgc3 = "#d7d7d7";

    setCentralWidgetStyle(fgc2, bgc2);
    setMainStyle(fgc, bgc, bgc3);
    setTabWidgetStyle(fgc, addrc, bgc, bgc2);
    setInfoTabWidgetStyle(fgc, addrc, bgc, bgc2);
    setSidebarStyle(fgc, bgc);
    setMenuStyle(fgc, bgc, bgc3);
    setNavbarStyle(fgc, bgc);

    disHighlighter->setTheme("Default");

    lineColor = QColor(215,215,215);
    highlightCurrentLine();
}

// Set dark theme
void MainWindow::on_actionDark_triggered()
{
    settings.setValue("theme", "dark");

    QString fgc = "#fafafa";
    QString bgc = "#333333";
    QString fgc2 = "#4c4c4c";
    QString bgc2 = "#e0e0e0";
    QString addrc = "#268BD2";
    QString bgc3 = "#414141";

    setCentralWidgetStyle(fgc, bgc3);
    setMainStyle(fgc, bgc, bgc3);
    setTabWidgetStyle(fgc, addrc, bgc, bgc3);
    setInfoTabWidgetStyle(fgc, addrc, bgc, bgc3);
    setSidebarStyle(fgc, bgc);
    setMenuStyle(fgc, bgc, bgc3);
    setNavbarStyle(fgc, bgc);

    disHighlighter->setTheme("Default");

    lineColor = QColor(65,65,65);
    highlightCurrentLine();
}

// Set theme solarized
void MainWindow::on_actionSolarized_triggered()
{
    settings.setValue("theme", "solarized");

    QString fgc = "#839496";
    QString bgc = "#fdf6e3";
    QString fgc2 = "#4c4c4c";
    QString bgc2 = "#e0e0e0";
    QString addrc = "#268BD2";
    QString bgc3 = "#eee8d5";

    setCentralWidgetStyle(fgc, bgc3);
    setMainStyle(fgc, bgc, bgc3);
    setTabWidgetStyle(fgc, addrc, bgc, bgc3);
    setInfoTabWidgetStyle(fgc, addrc, bgc, bgc3);
    setSidebarStyle(fgc, bgc);
    setMenuStyle(fgc, bgc, bgc3);
    setNavbarStyle(fgc, bgc);

    disHighlighter->setTheme("solarized");

    lineColor = QColor(238, 232, 213);
    highlightCurrentLine();
}

// Set theme solarized dark
void MainWindow::on_actionSolarized_Dark_triggered()
{
    settings.setValue("theme", "solarizedDark");

    QString fgc = "#839496";
    QString bgc = "#002b36";
    QString fgc2 = "#4c4c4c";
    QString bgc2 = "#073642";
    QString addrc = "#268BD2";
    QString bgc3 = "#073638";

    setCentralWidgetStyle(fgc, bgc3);
    setMainStyle(fgc, bgc, bgc3);
    setTabWidgetStyle(fgc, addrc, bgc, bgc2);
    setInfoTabWidgetStyle(fgc, addrc, bgc, bgc3);
    setSidebarStyle(fgc, bgc);
    setMenuStyle(fgc, bgc, bgc3);
    setNavbarStyle(fgc, bgc);

    disHighlighter->setTheme("solarized");

    lineColor = QColor(7, 54, 66);
    highlightCurrentLine();
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
