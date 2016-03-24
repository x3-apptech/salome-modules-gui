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

// File:      QtxPathEdit.h
// Author:    Sergey TELKOV
//
#ifndef QTXPATHEDIT_H
#define QTXPATHEDIT_H

#include "Qtx.h"

#include <QFrame>

class QLineEdit;

class QTX_EXPORT QtxPathEdit : public QFrame
{
  Q_OBJECT

public:
  QtxPathEdit( const Qtx::PathType, QWidget* = 0, bool = false );
  QtxPathEdit( QWidget* = 0, bool = false );
  virtual ~QtxPathEdit();

  QString       path() const;
  void          setPath( const QString& );

  Qtx::PathType pathType() const;
  void          setPathType( const Qtx::PathType );

  QString       pathFilter() const;
  void          setPathFilter( const QString& );

private slots:
  void          onBrowse( bool = false );

protected:
  QLineEdit*    lineEdit() const;

private:
  void          initialize();
  void          updateState();

private:
  QLineEdit*    myPath;
  Qtx::PathType myType;
  QString       myFilter;
  bool          myBrowse;
};

#endif
