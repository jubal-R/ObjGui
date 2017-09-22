#include <QtGui>
#include "disassemblyhighlighter.h"

DisassemblyHighlighter::DisassemblyHighlighter(QTextDocument *parent, QString theme)
    : QSyntaxHighlighter(parent)
{
    setupHighlighter(theme);
}

void DisassemblyHighlighter::setupHighlighter(QString theme){
    HighlightingRule rule;

    if (theme == "solarized"){
        registerColor = QColor(42, 161, 152);
        jumpColor = QColor(181, 137, 0);
        callColor = QColor(133, 153, 0);
        numColor = QColor(38, 139, 210);
        logicColor = QColor(108,113,196);
        stringColor = QColor(42, 161, 152);
    } else {
        registerColor = QColor(29, 160, 185);
        jumpColor = QColor(249, 38, 114);
        callColor = QColor(79, 153, 0);
        numColor = QColor(38, 139, 210);
        logicColor = QColor(174,129,255);
        stringColor = QColor(230,219,116);
    }

    // Addresses
    addressFormat.setForeground(numColor);
    rule.pattern = QRegExp("\\b0x[0-9a-f\\.]+\\b");
    rule.format = addressFormat;
    highlightingRules.append(rule);

    // Logical operations
    logicFormat.setForeground(logicColor);
    QStringList logicPatterns;
    logicPatterns << "\\bx?or\\b"
                     << "\\bsh[lr]\\b"
                     << "\\band\\b"
                     << "\\bnot\\b"
                     << "\\bcmp\\b";

    foreach (const QString &pattern, logicPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = logicFormat;
        highlightingRules.append(rule);
    }

    // Jumps
    jumpFormat.setForeground(jumpColor);
    jumpFormat.setFontWeight(QFont::Bold);
    rule.pattern = QRegExp("j[mpneglabsocz]+\\s(0x)?[0-9a-f\\.]+");
    rule.format = jumpFormat;
    highlightingRules.append(rule);

    // Calls
    callFormat.setForeground(callColor);
    rule.pattern = QRegExp("call\\s[0-9a-z\\.]+(\\s[<>a-zA-Z0-9@_-\\+]+)?");
    rule.format = callFormat;
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

    // Strings
    stringFormat.setForeground(stringColor);
    rule.pattern = QRegExp("\".*\"");
    rule.format = stringFormat;
    highlightingRules.append(rule);

}

void DisassemblyHighlighter::setTheme(QString theme){
    setupHighlighter(theme);
    rehighlight();
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
