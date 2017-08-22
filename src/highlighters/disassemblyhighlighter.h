#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QTextDocument;

class DisassemblyHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    DisassemblyHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat registerFormat;
    QTextCharFormat instructionFormat;
    QTextCharFormat addressFormat;
    QTextCharFormat functionFormat;

    QColor registerColor;
    QColor instructionColor;
    QColor functionsColor;
    QColor valueColor;
    QColor numColor;
    QColor commentColor;
    QColor varColor;
};

#endif // HIGHLIGHTER_H
