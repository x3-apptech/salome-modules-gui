//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxWorkspace.h
// Author:    Sergey TELKOV
//
#ifndef QTXWORKSPACE_H
#define QTXWORKSPACE_H

#include "Qtx.h"

#include <QWorkspace>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxWorkspace : public QWorkspace
{
  Q_OBJECT

public:
  QtxWorkspace( QWidget* = 0 );
  virtual ~QtxWorkspace();

public slots:
  void        tileVertical();
  void        tileHorizontal();
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
