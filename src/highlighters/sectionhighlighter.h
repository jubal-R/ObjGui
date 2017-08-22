#ifndef SECTIONHIGHLIGHTER_H
#define SECTIONHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QTextDocument;

class SectionHighlighter : public QSyntaxHighlighter
{
public:
    SectionHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat sectionFormat;

    QColor sectionColor;
};

#endif // SECTIONHIGHLIGHTER_H
