/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>

#include "highlighter.h"

Highlighter::Highlighter(QString type, QString theme, QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    if(theme == "default"){
        // default
        keywordColor = QColor(29, 160, 185);
        keyword2Color = QColor(249, 38, 114);
        functionsColor = QColor(79, 153, 0);
        valueColor = QColor(230, 218, 117);
        numColor = QColor(38, 139, 210);
        commentColor = QColor(178, 179, 191);
        varColor = QColor(102, 217, 239);
    }


    if(type == "dis"){
        //Functions
        classFormat.setForeground(functionsColor);
        rule.pattern = QRegExp("<[a-zA-Z0-9@_-\\+]+>");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Nums :D
        classFormat.setForeground(numColor);
        rule.pattern = QRegExp("\\b[0-9a-fx\\.]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Instructions
        keywordFormat.setForeground(keyword2Color);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns;
        keywordPatterns << "\\bcall\\b" << "\\bpush\\b" << "\\bpop\\b" << "\\blea\\b"
                        << "\\bret\\b" << "\\bcmp\\b" << "\\bmov\\b" << "\\bint\\b"
                        << "\\bjmp\\b" << "\\bje\\b" << "\\bjne\\b" << "\\bjl\\b"
                        << "\\bjg\\b" << "\\bjge\\b" << "\\bjle\\b" << "\\bjz\\b"
                        << "\\bjnz\\b" << "\\bjb\\b" << "\\bja\\b" << "\\bjae\\b"
                        << "\\bjbe\\b" << "\\bjs\\b" << "\\bjns\\b" << "\\bleave\\b"
                        << "\\bxor\\b" << "\\bnot\\b" << "\\band\\b" << "\\bor\\b"
                        << "\\bsal\\b" << "\\bsar\\b" << "\\bshl\\b" << "\\bshr\\b"
                        << "\\bnop\\b" << "\\btest\\b" << "\\bxchg\\b" << "\\bhlt\\b"
                        << "\\bmovsxd\\b" << "\\bmovsd\\b" << "\\bmovabs\\b" << "\\bmovzx\\b"
                        << "\\bcdqe\\b" << "\\bcdq\\b" << "\\bcmovs\\b" << "\\bsete\\b"
                        << "\\bimul\\b" << "\\bidiv\\b" << "\\binc\\b" << "\\bdec\\b"
                        << "\\badd\\b" << "\\bsub\\b" << "\\bmul\\b" << "\\bdiv\\b";

        foreach (const QString &pattern, keywordPatterns) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //Registers
        keywordFormat.setForeground(keywordColor);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns2;
        keywordPatterns2 << "\\brsp\\b" << "\\besp\\b" << "\\bsp\\b" << "\\bspl\\b"
                         << "\\brbp\\b" << "\\bebp\\b" << "\\bbp\\b" << "\\bbpl\\b"
                         << "\\brsi\\b" << "\\besi\\b" << "\\bsi\\b" << "\\bsil\\b"
                         << "\\brdi\\b" << "\\bedi\\b" << "\\bdi\\b" << "\\bdil\\b"
                         << "\\brax\\b" << "\\brbx\\b" << "\\brcx\\b" << "\\brdx\\b"
                         << "\\beax\\b" << "\\bax\\b" << "\\bah\\b" << "\\bal\\b"
                         << "\\bebx\\b" << "\\bbx\\b" << "\\bbh\\b" << "\\bbl\\b"
                         << "\\becx\\b" << "\\bcx\\b" << "\\bch\\b" << "\\bcl\\b"
                         << "\\bedx\\b" << "\\bdx\\b" << "\\bdh\\b" << "\\bdl\\b"
                         << "\\br[8-9]\\b" << "\\br[8-9][dwb]\\b"
                         << "\\br1[0-5]\\b" << "\\br1[0-5][dwb]\\b"
                         << "\\brip\\b" << "\\beip\\b" << "\\bip\\b";

        foreach (const QString &pattern, keywordPatterns2) {
            rule.pattern = QRegExp(pattern);
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        //Quotes
        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("\".*\"");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        quotationFormat.setForeground(valueColor);
        rule.pattern = QRegExp("'.*'");
        rule.format = quotationFormat;
        highlightingRules.append(rule);

        //Comments
        singleLineCommentFormat.setFontItalic(true);
        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp(";[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setFontItalic(true);
        multiLineCommentFormat.setForeground(commentColor);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");

    }else if(type == "str"){
        keywordFormat.setForeground(keyword2Color);
        rule.pattern = QRegExp("\\bContents+\\s+of+\\s+section\\b");
        rule.format = keywordFormat;
        highlightingRules.append(rule);

        //Functions
        classFormat.setForeground(functionsColor);
        classFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegExp("\\.+[a-zA-Z0-9_.-]+:");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Nums :D
        classFormat.setForeground(numColor);
        classFormat.setFontWeight(0);
        rule.pattern = QRegExp("\\b[0-9a-f]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Comments
        singleLineCommentFormat.setFontItalic(true);
        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp(";[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setFontItalic(true);
        multiLineCommentFormat.setForeground(commentColor);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");

    }else if(type == "sym"){
        //Text
        classFormat.setForeground(functionsColor);
        rule.pattern = QRegExp("\\b[a-zA-Z0-9_.-@]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Nums :D
        classFormat.setForeground(numColor);
        rule.pattern = QRegExp("\\b[0-9a-f]+\\b");
        rule.format = classFormat;
        highlightingRules.append(rule);

        //Comments
        singleLineCommentFormat.setFontItalic(true);
        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp("//[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        singleLineCommentFormat.setForeground(commentColor);
        rule.pattern = QRegExp(";[^\n]*");
        rule.format = singleLineCommentFormat;
        highlightingRules.append(rule);

        multiLineCommentFormat.setFontItalic(true);
        multiLineCommentFormat.setForeground(commentColor);

        commentStartExpression = QRegExp("/\\*");
        commentEndExpression = QRegExp("\\*/");

    }


}


void Highlighter::highlightBlock(const QString &text)
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

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}
