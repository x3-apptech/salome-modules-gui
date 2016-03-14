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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#ifndef SALOME_PYQT_DATAOBJECTLIGHT_H
#define SALOME_PYQT_DATAOBJECTLIGHT_H


#include "SALOME_PYQT_GUILight.h"
#include <LightApp_DataObject.h>
#include <LightApp_Study.h>

#include <qstring.h>

/*!
 * SALOME_PYQT_DataObjectLight - PYTHON LIGHT module's data object class
 */
class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_DataObjectLight : public virtual LightApp_DataObject
{

 public:
  SALOME_PYQT_DataObjectLight( SUIT_DataObject* = 0 );

  virtual ~SALOME_PYQT_DataObjectLight();
  
  virtual QString    entry() const;

  virtual QString    refEntry() const;
  void               setRefEntry( const QString& refEntry );
  
  virtual QString    name()    const;
  virtual QPixmap    icon(const int = NameId)    const;
  virtual QString    toolTip(const int = NameId) const;

  bool               setName(const QString& name);
  void               setIcon(const QString& icon);
  void               setToolTip(const QString& tooltip);

  virtual QColor     color( const ColorRole, const int = NameId ) const;
  void               setColor(const QColor& color);

 private:
  QString myEntry;
  QString myRefEntry;
  QString myName;
  QString myToolTip;
  QPixmap myIcon;
  QColor  myColor;
}; 

#endif // SALOME_PYQT_DATAOBJECTLIGHT_H
