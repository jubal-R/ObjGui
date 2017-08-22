#include <QtGui>
#include "sectionhighlighter.h"

SectionHighlighter::SectionHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    sectionColor = QColor(85, 85, 85);

    // Sections
    sectionFormat.setForeground(sectionColor);
    sectionFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("[_.-]+[a-zA-Z0-9_.-]+\\b");
    rule.format = sectionFormat;
    highlightingRules.append(rule);

}

void SectionHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

}
