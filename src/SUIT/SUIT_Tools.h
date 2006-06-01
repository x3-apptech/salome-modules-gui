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
#ifndef SUIT_TOOLS_H
#define SUIT_TOOLS_H

#include "SUIT.h"

#include <Qtx.h>

#include <qrect.h>
#include <qfont.h>
#include <qstring.h>

/*!
  \class SUIT_Tools
  Prodives set of auxiliary static methods
*/
class SUIT_EXPORT SUIT_Tools : public Qtx
{
public:
  static void    trace( const char* pLog, const char* szFormat, ... );//!< Traces output to log-file.
  static QRect   makeRect( const int x1, const int y1, const int x2, const int y2 ); 

  static QString fontToString( const QFont& font );
  static QFont   stringToFont( const QString& fontDescription );

  static void    centerWidget( QWidget* src, const QWidget* ref );
};

#endif
