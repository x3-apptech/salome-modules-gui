// Copyright (C) 2015-2020  OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File   : PyEditor_Keywords.cxx
// Author : Sergey TELKOV, Open CASCADE S.A.S. (sergey.telkov@opencascade.com)
//

#include "PyEditor_Keywords.h"

#include <QSet>

/*!
  \brief PyEditor_Keywords
*/

/*!
  \brief Constructor.
*/
PyEditor_Keywords::PyEditor_Keywords( QObject* parent )
  : QObject( parent )
{
}

/*!
  \brief Destructor.
*/
PyEditor_Keywords::~PyEditor_Keywords()
{
}


QList<int> PyEditor_Keywords::types() const
{
  QMap<int, bool> map;
  for ( KeywordMap::const_iterator it = myKeywords.begin(); it != myKeywords.end(); ++it ) {
    map.insert( it.value().type, false );
  }
  return map.keys();
}

/*!
  \brief Gets the colors list.
  \return color list
*/
QList<QColor> PyEditor_Keywords::colors() const
{
  QList<QColor> list;
  QSet<QRgb> set;
  for ( KeywordMap::const_iterator it = myKeywords.begin(); it != myKeywords.end(); ++it ) {
    const QColor& color = it.value().color;
    if ( !set.contains( color.rgba() ) ) {
      list.append( color );
      set.insert( color.rgba() );
    }
  }
  return list;
}

/*!
  \brief Gets the keyword type.
  \return type number
*/
int PyEditor_Keywords::type( const QString& keyword ) const
{
  return myKeywords.contains(keyword) ? myKeywords[keyword].type : -1;
}

/*!
  \brief Gets the keyword color.
  \return color
*/
QColor PyEditor_Keywords::color( const QString& keyword ) const
{
  return myKeywords.contains(keyword) ? myKeywords[keyword].color : QColor();
}

/*!
  \brief Gets all keywords.
  \return keywords string list
*/
QStringList PyEditor_Keywords::keywords() const
{
  return myKeywords.keys();
}

/*!
  \brief Gets all keywords of specified type.
  \return keywords string list
*/
QStringList PyEditor_Keywords::keywords( int type ) const
{
  QStringList keywords;
  for ( KeywordMap::const_iterator it = myKeywords.begin(); it != myKeywords.end(); ++it ) {
    if ( it.value().type == type )
      keywords.append( it.key() );
  }
  return keywords;
}

/*!
  \brief Gets all keywords with specified color.
  \return keywords string list
*/
QStringList PyEditor_Keywords::keywords( const QColor& color ) const
{
  QStringList keywords;
  for ( KeywordMap::const_iterator it = myKeywords.begin(); it != myKeywords.end(); ++it ) {
    if ( it.value().color == color )
      keywords.append( it.key() );
  }
  return keywords;
}

/*!
  \brief Append keyword with type and color.
*/
void PyEditor_Keywords::append( const QString& keyword,
				int type, const QColor& color )
{
  append( QStringList() << keyword, type, color );
}

/*!
  \brief Append keyword list with type and color.
*/
void PyEditor_Keywords::append( const QStringList& keywords,
				int type, const QColor& color )
{
  bool modif = false;
  for ( QStringList::const_iterator it = keywords.begin(); it != keywords.end(); ++it ) {
    const QString& kw = *it;
    bool changed = false;
    if ( !myKeywords.contains( kw ) ) {
      myKeywords.insert( kw, KeywordInfo() );
      changed = true;
    }
    KeywordInfo& info = myKeywords[kw];
    changed = changed || info.type != type || info.color != color;
    info.type = type;
    info.color = color;

    modif = modif || changed;
  }

  if ( modif )
    Q_EMIT keywordsChanged();
}

/*!
  \brief Remove all keywords with specified type.
*/
void PyEditor_Keywords::remove( int type )
{
  remove( keywords( type ) );
}

/*!
  \brief Remove keyword.
*/
void PyEditor_Keywords::remove( const QString& keyword )
{
  remove( QStringList() << keyword );
}

/*!
  \brief Remove keywords.
*/
void PyEditor_Keywords::remove( const QStringList& keywords )
{
  bool changed = false;
  for ( QStringList::const_iterator it = keywords.begin(); it != keywords.end(); ++it ) {
    if ( myKeywords.contains( *it ) ) {
      myKeywords.remove( *it );
      changed = true;
    }
  }
  if ( changed )
    Q_EMIT keywordsChanged();
}

/*!
  \brief Remove all keywords.
*/
void PyEditor_Keywords::clear()
{
  if ( !myKeywords.isEmpty() ) {
    myKeywords.clear();
    Q_EMIT keywordsChanged();
  }
}

/*!
  \brief PyEditor_StandardKeywords
*/

PyEditor_StandardKeywords::PyEditor_StandardKeywords( QObject* parent )
  : PyEditor_Keywords( parent )
{
  QStringList aBase;
  aBase << "and" << "as" << "assert" << "break" << "class" << "continue"
	<< "def" << "elif" << "else" << "except" << "exec" << "finally"
	<< "False" << "for" << "from" << "global" << "if" << "import"
	<< "in" << "is" << "lambda" << "None" << "not" << "or" << "pass"
	<< "print" << "raise" << "return" << "True" << "try" << "while"
	<< "with" << "yield";
  append( aBase, Base, Qt::blue );

  QStringList anExcept;
  anExcept << "ArithmeticError" << "AssertionError" << "AttributeError"
	   << "EnvironmentError" << "EOFError" << "Exception"
	   << "FloatingPointError" << "ImportError" << "IndentationError"
	   << "IndexError" << "IOError" << "KeyboardInterrupt" << "KeyError"
	   << "LookupError" << "MemoryError" << "NameError" << "OSError"
	   << "NotImplementedError" << "OverflowError" << "ReferenceError"
	   << "RuntimeError" << "StandardError" << "StopIteration"
	   << "SyntaxError" << "SystemError" << "SystemExit" << "TabError"
	   << "TypeError" << "UnboundLocalError" << "UnicodeDecodeError"
	   << "UnicodeEncodeError" << "UnicodeError" << "UnicodeTranslateError"
	   << "ValueError" << "WindowsError" << "ZeroDivisionError"
	   << "Warning" << "UserWarning" << "DeprecationWarning"
	   << "PendingDeprecationWarning" << "SyntaxWarning"
	   << "OverflowWarning" << "RuntimeWarning" << "FutureWarning";
  append( anExcept, Exceptions, Qt::magenta );
}

PyEditor_StandardKeywords::~PyEditor_StandardKeywords()
{
}
