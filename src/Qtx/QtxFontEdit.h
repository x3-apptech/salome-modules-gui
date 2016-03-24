// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxFontEdit.h
// Author:    Sergey TELKOV
//
#ifndef QTXFONTEDIT_H
#define QTXFONTEDIT_H

#include "Qtx.h"

#include <QFrame>

class QtxComboBox;
class QComboBox;
class QToolButton;
class QFontComboBox;

class QTX_EXPORT QtxFontEdit : public QFrame
{
  Q_OBJECT

public:
  //! Font widget features
  typedef enum { 
    Family    = 0x01,                                      //!< show font family selection widget
    Size      = 0x02,                                      //!< show font size widget
    UserSize  = 0x04,                                      //!< allow font size direct change
    Bold      = 0x08,                                      //!< show 'bold' widget
    Italic    = 0x10,                                      //!< show 'italic' widget
    Underline = 0x20,                                      //!< show 'underline' widget
    Shadow    = 0x40,                                      //!< show 'shadow' widget
    Preview   = 0x80,                                      //!< show font preview widget
    Scripting = Bold | Italic | Underline,                 //!< show font scripting widgets ('bold','italic','underline')
    All = Family | Size | UserSize | Scripting | Preview   //!< show all font widgets
  } Features;

  typedef enum 
  {
    Native, //!< Native mode intended for working with system fonts
    Custom  //!< Custom mode intended for working with manually defined set of fonts
  } Mode;

public:
  QtxFontEdit( const int, QWidget* = 0 );
  QtxFontEdit( QWidget* = 0 );
  virtual ~QtxFontEdit();

  QFont          currentFont() const;
  void           setCurrentFont( const QFont& );

  int            fontSize() const;
  QString        fontFamily() const;
  int            fontScripting() const;

  void           setFontSize( const int );
  void           setFontFamily( const QString& );
  void           setFontScripting( const int );

  int            features() const;
  void           setFeatures( const int );

  void           setMode( const int );
  int            mode() const;

  void           setFonts( const QStringList& );
  QStringList    fonts() const;

  void           setSizes( const QList<int>& = QList<int>() );
  QList<int>     sizes() const;

signals:
  void           changed( const QFont& );

private slots:
  void           onPreview( bool );
  void           onFontChanged( const QFont& );
  void           onPropertyChanged();
 
private:
  void           initialize();
  void           updateState();

private:
  QtxComboBox*   mySize;
  QFontComboBox* myFamily;
  QToolButton*   myPreview;
  int            myFeatures;
  QToolButton    *myB, *myI, *myU, *myS;
  int            myMode;
  QComboBox*     myCustomFams;
};

#endif // QTXFONTEDIT_H
