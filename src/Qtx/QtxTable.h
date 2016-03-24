// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:      QtxTable.h
// Author:    Sergey TELKOV
//
#ifndef QTXTABLE_H
#define QTXTABLE_H

#include "Qtx.h"

#include <qtable.h>

#ifndef QT_NO_TABLE

class QHeader;

class QTX_EXPORT QtxTable : public QTable
{
  Q_OBJECT

  class HeaderEditor;

public:
  QtxTable( QWidget* = 0, const char* = 0 );
  QtxTable( int, int, QWidget* = 0, const char* = 0 );
  virtual ~QtxTable();

  bool             headerEditable( Orientation ) const;

  bool             editHeader( Orientation, const int );
  void             endEditHeader( const bool = true );

  virtual bool     eventFilter( QObject*, QEvent* );

signals:
  void             headerEdited( QHeader*, int );
  void             headerEdited( Orientation, int );

public slots:
  virtual void     hide();
  virtual void     setHeaderEditable( Orientation, bool );

private slots:
  void             onScrollBarMoved( int );
  void             onHeaderSizeChange( int, int, int );

protected:
  virtual void     resizeEvent( QResizeEvent* );

  virtual bool     beginHeaderEdit( Orientation, const int );
  virtual void     endHeaderEdit( const bool = true );
  bool             isHeaderEditing() const;
  virtual QWidget* createHeaderEditor( QHeader*, const int, const bool = true );
  virtual void     setHeaderContentFromEditor( QHeader*, const int, QWidget* );

  QHeader*         header( Orientation o ) const;

private:
  void             updateHeaderEditor();
  void             beginHeaderEdit( Orientation, const QPoint& );
  QRect            headerSectionRect( QHeader*, const int ) const;

private:
  QWidget*         myHeaderEditor;
  QHeader*         myEditedHeader;
  int              myEditedSection;
  QMap<int, bool>  myHeaderEditable;
};

#endif

#endif // QTXTABLE_H
