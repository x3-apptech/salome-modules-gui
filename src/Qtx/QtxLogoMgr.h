// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef QTX_LOGOMGR_H
#define QTX_LOGOMGR_H

#include "Qtx.h"

class QMenuBar;

#include <qobject.h>
#include <qpixmap.h>

#ifdef WIN32
#pragma warning( disable : 4251 )
#endif

class QTX_EXPORT QtxLogoMgr : public QObject
{
  Q_OBJECT

public:
  QtxLogoMgr( QMenuBar* );
  virtual ~QtxLogoMgr();

  int        count() const;

  void       insert( const QString&, const QPixmap&, const int = -1 );
  void       remove( const QString& );
  void       clear();

  QMenuBar*  menuBar() const;

private:
  void       generate();
  int        find( const QString& ) const;

private:
  typedef struct { QString id; QPixmap pix; } LogoInfo;
  typedef QValueList<LogoInfo>                LogoList;

private:
  int        myId;
  QMenuBar*  myMenus;
  LogoList   myLogos;
};

#ifdef WIN32
#pragma warning( default : 4251 )
#endif

#endif
