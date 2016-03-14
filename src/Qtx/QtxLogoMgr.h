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

// File:      QtxLogoMgr.h
// Author:    Sergey TELKOV
//
#ifndef QTXLOGOMGR_H
#define QTXLOGOMGR_H

#include "Qtx.h"

#include <QObject>
#include <QList>
#include <QPixmap>

class QMenuBar;
class QMovie;

#ifdef WIN32
#pragma warning( disable : 4251 )
#endif

class QTX_EXPORT QtxLogoMgr : public QObject
{
  Q_OBJECT

  class LogoBox;

public:
  QtxLogoMgr( QMenuBar* );
  virtual ~QtxLogoMgr();

  int        count() const;

  void       insert( const QString&, QMovie*, const int = -1 );
  void       insert( const QString&, const QPixmap&, const int = -1 );
  void       remove( const QString& );
  void       clear();

  void       startAnimation( const QString& = QString() );
  void       stopAnimation( const QString& = QString() );
  void       pauseAnimation( const bool, const QString& = QString() );

  QMenuBar*  menuBar() const;

private:
  typedef struct { QString id; QPixmap pix; QMovie* mov; } LogoInfo;
  typedef QList<LogoInfo>                                  LogoList;

private:
  void       generate();
  int        find( const QString& ) const;
  LogoInfo&  insert( const QString&, const int );
  void       movies( const QString&, QList<QMovie*>& ) const;

private:
  LogoBox*   myBox;         //!< widget containing logox
  LogoList   myLogos;       //!< list of logo data
};

#ifdef WIN32
#pragma warning( default : 4251 )
#endif

#endif // QTXLOGOMGR_H
