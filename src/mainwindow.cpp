#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

// For debugging
#include "iostream"

#include "files.h"
#include "highlighter.h"
#include "objdumper.h"
#include "settings.h"

using namespace std;

Files files;
Settings settings;
ObjDumper *objDumper = NULL;
Highlighter *disHighlighter = NULL;
Highlighter *symbolsHighlighter = NULL;
Highlighter *relocationsHighlighter = NULL;
Highlighter *stringsHighlighter = NULL;
Highlighter *headersHighlighter = NULL;

QString currentDirectory = QString::fromStdString(files.getHomeDir());

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ObjGUI");

    // Set Window Size
    MainWindow::resize(settings.getWindowWidth(), settings.getWindowHeight());

    // Indicate Current Preferences
    if (settings.getSyntax() == "intel"){
        ui->actionIntel->setChecked(true);
    }else if (settings.getSyntax() == "att"){
        ui->actionAtt->setChecked(true);
    }

    // Style
    QString style = "QTabBar::tab:selected{color: #fafafa; background-color: #3ba1a1;}"
          "QTabBar::tab {color: font-size:9pt; padding: 1px 5px;}";
   ui->tabWidget->setStyleSheet(style);

    disHighlighter = new Highlighter("dis", "default", ui->codeBrowser->document());
    symbolsHighlighter = new Highlighter("sym", "default", ui->symbolsBrowser->document());
    relocationsHighlighter = new Highlighter("sym", "default", ui->relocationsBrowser->document());
    stringsHighlighter = new Highlighter("str", "default", ui->stringsBrowser->document());
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
        while (ui->sectionList->count() > 0){
            ui->sectionList->takeItem(0);
        }

        objDumper = new ObjDumper(file, settings.getSyntax());

        // Populate function/section list in sidebar
        ui->sectionList->addItems(objDumper->getFunctionsList());

        ui->codeBrowser->setText(objDumper->getDisassembly());
        ui->symbolsBrowser->setText(objDumper->getSymbolsTable());
        ui->relocationsBrowser->setText(objDumper->getRelocationEntries());
        ui->stringsBrowser->setText(objDumper->getStrings());
        ui->headersBrowser->setText(objDumper->getHeaders());

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

    ui->actionIntel->setChecked(true);
    ui->actionAtt->setChecked(false);
}

void MainWindow::on_actionAtt_triggered()
{
    settings.setSyntax("att");
    ui->actionAtt->setChecked(true);
    ui->actionIntel->setChecked(false);
}
