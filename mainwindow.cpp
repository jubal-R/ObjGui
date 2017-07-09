#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"
#include "qmessagebox.h"

// For debugging
#include "iostream"

#include "files.h"
#include "highlighter.h"
#include "objdumper.h"

using namespace std;

Files files;
ObjDumper objDumper;
Highlighter *disHighlighter = NULL;
Highlighter *symbolsHighlighter = NULL;
Highlighter *relocationsHighlighter = NULL;
Highlighter *stringsHighlighter = NULL;

QString currentDirectory = QString::fromStdString(files.getHomeDir());

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ObjGUI");

    // Style
    QString style = "QTabBar::tab:selected{color: #fafafa; background-color: #3ba1a1;}"
          "QTabBar::tab {color: font-size:9pt; padding: 1px 5px;}";
   ui->tabWidget->setStyleSheet(style);

    disHighlighter = new Highlighter("dis", "default", ui->codeBrowser->document());
    symbolsHighlighter = new Highlighter("sym", "default", ui->symbolsBrowser->document());
    relocationsHighlighter = new Highlighter("sym", "default", ui->relocationsBrowser->document());
    stringsHighlighter = new Highlighter("str", "default", ui->stringsBrowser->document());

    connect(ui->codeBrowser, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//  Open Binary
void MainWindow::open(QString file){

    if (file != ""){
        this->setWindowTitle("ObjGUI - " + file);

        ui->codeBrowser->setText(objDumper.getDisassembly(file));
        ui->symbolsBrowser->setText(objDumper.getSymbolsTable(file));
        ui->relocationsBrowser->setText(objDumper.getRelocationEntries(file));
        ui->stringsBrowser->setText(objDumper.getStrings(file));

        // Clear old function/section list from sidebar
        while (ui->sectionList->count() > 0){
            ui->sectionList->takeItem(0);
        }

        // Populate function/section list in sidebar
        QStringList sections = objDumper.getFunctionsList(objDumper.getDisassembly(file));
        ui->sectionList->addItems(sections);

    }
}
// Select Binary
void MainWindow::on_actionOpen_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open File"), currentDirectory, tr("All (*)"));
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

/*
 * Window
*/
void MainWindow::on_actionProject_triggered()
{
    QString aboutStr = "ObjGui - GUI for objdump\n"
                       "Project page:\n"
                       "https://github.com/jubal-R/ObjGui";
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

