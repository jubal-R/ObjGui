#include <QtGui>

#include "disassemblyhighlighter.h"

DisassemblyHighlighter::DisassemblyHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    registerColor = QColor(29, 160, 185);
    instructionColor = QColor(249, 38, 114);
    functionsColor = QColor(79, 153, 0);
    valueColor = QColor(230, 218, 117);
    numColor = QColor(38, 139, 210);
    commentColor = QColor(178, 179, 191);
    varColor = QColor(102, 217, 239);


    // Addresses
    addressFormat.setForeground(numColor);
    rule.pattern = QRegExp("\\b[0-9a-fx\\.]{3,}\\b");
    rule.format = addressFormat;
    highlightingRules.append(rule);

    // Instructions
    instructionFormat.setForeground(instructionColor);
    instructionFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("\\s\\s[a-z]+\\s\\s");
    rule.format = instructionFormat;
    highlightingRules.append(rule);

    // Functions
    functionFormat.setForeground(functionsColor);
    rule.pattern = QRegExp("<[a-zA-Z0-9@_-\\+]+>");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // x86_64 Registers
    registerFormat.setForeground(registerColor);
    registerFormat.setFontWeight(QFont::Bold);
    QStringList registerPatterns;
    registerPatterns << "\\b[re]?[sabcdi][xhlpi]l?\\b"
                     << "\\br1?[890-5][dwb]?\\b";

    foreach (const QString &pattern, registerPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = registerFormat;
        highlightingRules.append(rule);
    }


}


void DisassemblyHighlighter::highlightBlock(const QString &text)
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
