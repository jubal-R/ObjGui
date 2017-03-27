#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qfiledialog.h"

#include "files.h"
#include "highlighter.h"
#include "objdumper.h"

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

//  Open
void MainWindow::open(QString file){

    if (file != ""){
        this->setWindowTitle("ObjGUI - " + file);

        ui->codeBrowser->setText(objDumper.getDisassembly(file));
        ui->symbolsBrowser->setText(objDumper.getSymbolsTable(file));
        ui->relocationsBrowser->setText(objDumper.getRelocationEntries(file));
        ui->stringsBrowser->setText(objDumper.getStrings(file));

    }
}
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
