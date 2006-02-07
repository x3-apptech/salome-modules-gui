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
// See http://www.salome-platform.org/
//
// File:      QtxDblValidator.cxx
// Author:    Alexandre SOLOVYOV

#include "QtxDblValidator.h"

QtxDblValidator::QtxDblValidator( const double bot, const double top, const int dec,
				  QObject* o, const char* name )
: QDoubleValidator( bot, top, dec, o, name )
{
}

QtxDblValidator::~QtxDblValidator()
{
}

void QtxDblValidator::fixup( QString& str ) const
{
  bool ok = false;
  double d = str.toDouble( &ok );
  if( ok )
  {
    if( d<bottom() )
      str = QString::number( bottom() );
    else if( d>top() )
      str = QString::number( top() );
  }
  else
    str = "0";
}
