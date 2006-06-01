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
//  File   : CAF_Tools.h
//  Author : UI team

#ifndef CAF_TOOLS_H
#define CAF_TOOLS_H

#include <CAF.h>

#include <SUIT_Tools.h>

#include <qcolor.h>

#include <Quantity_Color.hxx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

/*!
  \class CAF_Tools
  Prodives set of auxiliary static methods
*/
class CAF_EXPORT CAF_Tools : public SUIT_Tools
{
public:
  static QString                    toQString( const TCollection_AsciiString& );
  static QString                    toQString( const TCollection_ExtendedString& );

  static TCollection_ExtendedString toExtString( const QString& );
  static TCollection_AsciiString    toAsciiString( const QString& );

  static Quantity_Color             color( const QColor& );
  static QColor                     color( const Quantity_Color& );
};

#endif
