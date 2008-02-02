/*****
*
* TOra - An Oracle Toolkit for DBA's and developers
* Copyright (C) 2003-2005 Quest Software, Inc
* Portions Copyright (C) 2005 Other Contributors
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation;  only version 2 of
* the License is valid for this program.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*      As a special exception, you have permission to link this program
*      with the Oracle Client libraries and distribute executables, as long
*      as you follow the requirements of the GNU GPL in regard to all of the
*      software in the executable aside from Oracle client libraries.
*
*      Specifically you are not permitted to link this program with the
*      Qt/UNIX, Qt/Windows or Qt Non Commercial products of TrollTech.
*      And you are not permitted to distribute binaries compiled against
*      these libraries without written consent from Quest Software, Inc.
*      Observe that this does not disallow linking to the Qt Free Edition.
*
*      You may link this product with any GPL'd Qt library such as Qt/Free
*
* All trademarks belong to their respective owners.
*
*****/

#ifndef TOHIGHLIGHTEDTEXT_H
#define TOHIGHLIGHTEDTEXT_H

#include "config.h"

#include "tomarkedtext.h"

#include <Qsci/qscilexer.h>
#include <Qsci/qsciapis.h>

#include <QString>
#include <QKeyEvent>

#include <list>
#include <map>
#include <qtimer.h>
#include <qstringlist.h>
#include <QListWidget>

class QListWidgetItem;
class QPainter;
class toSyntaxSetup;
class toHighlightedText;

/** This class implements a syntax parser to provide information to
 * a syntax highlighted editor.
 */

class toSyntaxAnalyzer
{
public:
    /** Highlighting categories (joins more categories in qscintilla into
     * simplier ones
     */
    enum infoType
    {
        Default = 0,
        Comment = 1,
        Number = 2,
        Keyword = 3,
        String = 4,
        DefaultBg = 5,
        ErrorBg = 6,
        DebugBg = 7
    };
private:
    /** Indicate if colors are updated, can't do this in constructor since QApplication
     * isn't initialized yet.
     */
    bool ColorsUpdated;
    /** Colors allocated for the different @ref infoType values.
     */
    QColor Colors[8];

    /** marker per linea contenente errori
      */
    int errorMarker;
    /** marker per linea corrente
      */
    int debugMarker;
    /** Keeps track of possible hits found so far.
     */
    struct posibleHit
    {
        posibleHit(const char *);
        /** Where you are in this word to find a hit.
         */
        int Pos;
        /** The text to hit, points into keywords array.
         */
        const char *Text;
    };
    /** An array of lists of keywords, indexed on the first character.
     */
    std::list<const char *> Keywords[256];
protected:
    /** Check if this is part of a symbol or not.
     */
    bool isSymbol(QChar c)
    {
        return (c.isLetterOrNumber() || c == '_' || c == '#' || c == '$' || c == '.');
    }
private:
    /** Get a colordefinition from a @ref infoType value.
     * @param def Color to fill out.
     * @param pos @ref infoType to get color for.
     */
    void readColor(const QColor &def, infoType pos);
    /** Get a string representation of an @ref infoType.
     * @param typ @ref infoType to get string for.
     * @return Description of infotype.
     */
    static QString typeString(infoType typ);
    /** Get an @ref infoType from a string representation of it.
     * @param str Description of @ref infoType.
     * @return @ref infoType described by string.
     */
    static infoType typeString(const QString &str);
    /** Update configuration settings from this class color values.
     */
    void updateSettings(void);
public:
    /** Create a syntax analysed
     * @param keywords A list of keywords.
     */
    toSyntaxAnalyzer(const char **keywords);
    virtual ~toSyntaxAnalyzer()
    { }

    /** Get the character used to quote names of functions etc for the database
     */
    virtual QChar quoteCharacter()
    {
        return '\"';
    }
    /** True if declare keyword starts block.
     */
    virtual bool declareBlock()
#ifdef TO_NO_ORACLE
    { return false;
    }
#else
    { return true;
    }
#endif

    /** Get a colordefinition for a @ref infoType value.
     * @param typ @ref infoType to get color for.
     * @return Color of that type.
     */
    QColor getColor(infoType typ);

    /** Check if a word is reserved.
     * @param word Word to check.
     * @return True if word is reserved.
     */
    bool reservedWord(const QString &word);

    friend class toSyntaxSetup;
    /** Get the default syntax analyzer.
     * @return Reference to the default analyzer.
     */
    static toSyntaxAnalyzer &defaultAnalyzer();
};

class toComplPopup : public QListWidget {
    Q_OBJECT;

private:
    toHighlightedText* editor;

public:
    toComplPopup(toHighlightedText* high);
    virtual ~toComplPopup();

public slots:
    void hide(void);

protected:
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void focusOutEvent(QFocusEvent *e);
};
/**
 * A simple editor which supports syntax highlighting.
 *
 * This needs to be heavily re-implemented/simplified to use QScintilla syntax
 * colouring. For now it only stubs used API from previous version of
 * toHighlightedText. The rest of the API comes unchanged from toMarkedText
 * which is now derived from QScintilla.
 */

class toHighlightedText : public toMarkedText
{
private:
    Q_OBJECT

    // Associated lexer (may be not used)
    QsciLexer *lexer;       // NOTE: this should be used in instead of toSyntaxAnalyzer
    bool syntaxColoring;
    /** Map of rows with errors and their error message.
     */
    std::map<int, QString> Errors;
    QsciAPIs* complAPI;
    QTimer* timer;
protected:
    int debugMarker;
    int errorMarker;
    toComplPopup* popup;

public:
    friend class toComplPopup;

    /** Create a new editor.
     * @param parent Parent of widget.
     * @param name Name of widget.
     */
    toHighlightedText(QWidget *parent, const char *name = NULL);

    /**
     * Cleaning up done here
     */
    virtual ~toHighlightedText();

public:
    /**
     * Set the lexer to use.
     * @param lexer to use,
     *        0 if no syntax colouring
     */
    void setLexer(QsciLexer *lexer);

    /**
     * Get the current lexer.
     * @return lexer used or 0 if no syntax colouring.
     */
    QsciLexer * getLexer(void)
    {
        return lexer;
    }

    /**
     * Overriden to set font for lexer as well.
     * @param font the font to set
     */
    void setFont (const QFont & font);

public:
    // ------------------ API used by TOra classes ----------------------
    // NOTE: currently all stubs

    /** Convert a linenumber after a change of the buffer to another linenumber. Can be
     * used to convert a specific linenumber after receiving a @ref insertedLines call.
     * @param line Line number.
     * @param start Start of change.
     * @param diff Lines added or removed.
     * @return New linenumber or -1 if line doesn't exist anymore.
     */
    static int convertLine(int line, int start, int diff)
    {
        return line;
    }

    /** Set current line. Will be indicated with a different background.
     * @param current Current line.
     */
    void setCurrent(int current);

    /** Returns true if the editor has any errors.
     */
    bool hasErrors();

    /** Set the error list map.
     * @param errors A map of linenumbers to errorstrings. These will be displayed in the
     *               statusbar if the cursor is placed on the line.
     */
    void setErrors(const std::map<int, QString> &errors);

    /**
     * DEPRECATED: should use setLexer() instead!!!
     *
     * Set the syntax highlighter to use.
     * @param analyzer Analyzer to use.
     */
    void setAnalyzer(toSyntaxAnalyzer &analyzer) {}

    /**
     * DEPRECATED: should use getLexer() instead!!!
     *
     * Get the current syntaxhighlighter.
     * @return Analyzer used.
     */
    toSyntaxAnalyzer &analyzer(void)
    {
        return toSyntaxAnalyzer::defaultAnalyzer();
    }

    /**
     * Set keyword upper flag. If this is set keywords will be converted
     * to uppercase when painted.
     *
     * NOTE: this may be quite tricky to implement - have to check
     *       how the Scintilla Lexers are working
     *
     * @param val New value of keyword to upper flag.
     */
    void setKeywordUpper(bool val) {}

    /**
     * Sets the syntax colouring flag.
     */
    void setSyntaxColoring(bool val);

    /** Get the tablename currently under the cursor.
     * @param owner Filled with owner or table or QString::null if no owner specified.
     * @param table Filled with tablename.
     * @param highlight If true mark the extracted tablename
     */
    void tableAtCursor(QString &owner, QString &table, bool highlight = false);

    void updateSyntaxColor(toSyntaxAnalyzer::infoType t);

protected:
    QStringList getCompletionList(QString* partial);
    void completeWithText(QString itemText);

private:
    bool invalidToken(int line, int col);

    // ------------------ END OF API used by TOra classes ----------------------

public slots:
    /** Go to next error.
     */
    void nextError(void);
    /** Go to previous error.
     */
    void previousError(void);

    virtual void autoCompleteFromAPIs();

    void positionChanged(int row, int col);

    virtual void completeFromAPI(QListWidgetItem * item);

private slots:
    void setStatusMessage(void);
};

#endif