#include "resultsdialog.h"

ResultsDialog::ResultsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Results");
}

void ResultsDialog::setResultsText(QString results){
    ui->resultsBrowser->setPlainText(results);
}

void ResultsDialog::setResultsLabelText(QString resultsLabelText){
    ui->resultsLabel->setText(resultsLabelText);
}
