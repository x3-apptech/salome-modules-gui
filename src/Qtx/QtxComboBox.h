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

// File:      QtxComboBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXCOMBOBOX_H
#define QTXCOMBOBOX_H

#include "Qtx.h"

#include <QMap>
#include <QComboBox>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxComboBox : public QComboBox
{
  Q_OBJECT

  class Model;
  class ClearEvent;

public:
  QtxComboBox( QWidget* = 0 );
  virtual ~QtxComboBox();

  bool         isCleared() const;
  void         setCleared( const bool );

  QVariant     currentId() const;
  void         setCurrentId( const QVariant& );

  QVariant     id( const int ) const;
  int          index( const QVariant& ) const;

  bool         hasId( const int ) const;
  void         setId( const int, const QVariant& );

signals:
  void         activatedId( QVariant );
  void         activatedId( int );
  void         activatedId( QString );

private slots:
  void         onCurrentChanged( int );

protected:
  virtual void paintEvent( QPaintEvent* );
  virtual void childEvent( QChildEvent* );
  virtual void customEvent( QEvent* );

private:
  void         resetClear();

private:
  enum { IdRole = Qt::UserRole + 10 };

private:
  bool         myCleared;     //!< "cleared" state
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
