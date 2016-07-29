// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "OCCViewer.h"

/*!
  \brief Convert QColor object to Quantity_Color object.
  \param c color object in Qt format
  \return color object in OCC format
*/
Quantity_Color OCCViewer::color( const QColor& c )
{
  Quantity_Color aColor;
  if ( c.isValid() )
    aColor = Quantity_Color( c.red()   / 255., c.green() / 255.,
                             c.blue()  / 255., Quantity_TOC_RGB );
  return aColor;
}

/*!
  \brief Convert Quantity_Color object to QColor object.
  \param c color object in OCC format
  \return color object in Qt format
*/
QColor OCCViewer::color( const Quantity_Color& c )
{
  return QColor ( int( c.Red()   * 255 ),
                  int( c.Green() * 255 ),
                  int( c.Blue()  * 255 ) );
}
