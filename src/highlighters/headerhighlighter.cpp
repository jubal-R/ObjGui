#include <QtGui>
#include "headerhighlighter.h"

HeaderHighlighter::HeaderHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    numColor = QColor(150, 150, 150);

    // Labels
    labelFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\b[^:]+:");
    rule.format = labelFormat;
    highlightingRules.append(rule);

    // Addresses
    numberFormat.setForeground(numColor);
    rule.pattern = QRegExp("\\b0x?[0-9a-f\\.]{3,}\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

}

void HeaderHighlighter::highlightBlock(const QString &text)
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
