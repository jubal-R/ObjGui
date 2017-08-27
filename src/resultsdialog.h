#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include "QDialog"
#include "ui_resultsdialog.h"

class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    ResultsDialog(QWidget *parent = 0);
    void setResultsText(QString results);
    void setResultsLabelText(QString resultsLabel);
private:
    Ui::ResultsDialog *ui;

};

#endif // RESULTSDIALOG_H
