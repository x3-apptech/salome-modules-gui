// Copyright (C) 2015-2019  OPEN CASCADE
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
// File   : PyEditor_Keywords.h
// Author : Sergey TELKOV, Open CASCADE S.A.S. (sergey.telkov@opencascade.com)
//

#ifndef PYEDITOR_KEYWORDS_H
#define PYEDITOR_KEYWORDS_H

#include <QObject>
#include <QColor>
#include <QList>
#include <QMap>

class PyEditor_Keywords : public QObject
{
  Q_OBJECT

public:
  PyEditor_Keywords( QObject* = 0 );
  virtual ~PyEditor_Keywords();

  QList<int>       types() const;
  QList<QColor>    colors() const;

  int              type( const QString& ) const;
  QColor           color( const QString& ) const;

  QStringList      keywords() const;
  QStringList      keywords( int ) const;
  QStringList      keywords( const QColor& ) const;

  void             append( const QString&, int, const QColor& = QColor() );
  void             append( const QStringList&, int, const QColor& = QColor() );

  void             remove( int );
  void             remove( const QString& );
  void             remove( const QStringList& );

  void             clear();

Q_SIGNALS:
  void             keywordsChanged();

private:
  typedef struct { int type; QColor color; } KeywordInfo;
  typedef QMap<QString, KeywordInfo>         KeywordMap;

private:
  KeywordMap       myKeywords;
};

class PyEditor_StandardKeywords : public PyEditor_Keywords
{
  Q_OBJECT

public:
  typedef enum { Base, Exceptions } KeywordType;

public:
  PyEditor_StandardKeywords( QObject* = 0 );
  virtual ~PyEditor_StandardKeywords();
};

#endif
