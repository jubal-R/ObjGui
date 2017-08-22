#ifndef HEADERHIGHLIGHTER_H
#define HEADERHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class HeaderHighlighter : public QSyntaxHighlighter
{
public:
    HeaderHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat labelFormat;
    QTextCharFormat numberFormat;

    QColor numColor;
};

#endif // HEADERHIGHLIGHTER_H
