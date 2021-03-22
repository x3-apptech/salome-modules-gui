// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef QTXINFOPANEL_H
#define QTXINFOPANEL_H

#include "Qtx.h"

#include <QWidget>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QAction;

class QTX_EXPORT QtxInfoPanel : public QWidget
{
  Q_OBJECT

  class Container;
  class Title;

public:
  QtxInfoPanel( QWidget* = 0 );
  ~QtxInfoPanel();

  void                setTitle( const QString& );
  int                 addLabel( const QString&, const int = -1 );
  int                 addLabel( const QString&, Qt::Alignment, const int = -1 );
  int                 addAction( QAction*, const int = -1 );
  int                 addGroup( const QString&, const int = -1 );

  void                remove( const int );
  void                clear( const int = -1 );

  void                setVisible( const int, bool );
  void                setEnabled( const int, bool );

private:
  int                 generateId() const;
  QWidget*            find( const int ) const;

private:
  Title*              title;
  Container*          container;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // QTXINFOPANEL_H
