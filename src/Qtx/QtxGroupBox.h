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

// File:      QtxGroupBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXGROUPBOX_H
#define QTXGROUPBOX_H

#include "Qtx.h"

#include <QGroupBox>

class QTX_EXPORT QtxGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  QtxGroupBox( QWidget* = 0 );
  QtxGroupBox( const QString&, QWidget* = 0 );
  virtual ~QtxGroupBox();

  virtual void  insertTitleWidget( QWidget* );
  virtual void  removeTitleWidget( QWidget* );

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

  virtual bool  eventFilter( QObject*, QEvent* );

  QWidget*      widget() const;
  void          setWidget( QWidget* );

public slots:
  virtual void  setVisible( bool );

protected:
  virtual void  childEvent( QChildEvent* );
  virtual void  resizeEvent( QResizeEvent* );
  virtual void  customEvent( QEvent* );

private:
  void          initialize();
  void          updateTitle();
  QSize         titleSize() const;
  void          setInsideMargin( const int );
  QSize         expandTo( const QSize& ) const;

private:
  QWidget*      myContainer;
};

#endif
