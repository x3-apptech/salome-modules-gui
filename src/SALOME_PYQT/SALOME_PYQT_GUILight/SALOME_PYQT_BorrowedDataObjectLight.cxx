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
#include "SALOME_PYQT_BorrowedDataObjectLight.h"
#include <LightApp_Application.h>
#include <SUIT_Session.h>
#include <utilities.h>
#include <SUIT_ResourceMgr.h>


#include <CAM_DataModel.h>
#include <CAM_Module.h>


/*!
 *  Class:       SALOME_PYQT_BorrowedDataObjectLight
 *  Description: LIGHT PYTHON module's Borrowed data object: reference objects from other modules.
 *  Used to propagate selection in a light module client of other modules publishing objects in study.
 *  (copied from SALOME_PYQT_DataObjectLight)
 */

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight()
// purpose  : constructor
//=================================================================================
SALOME_PYQT_BorrowedDataObjectLight::SALOME_PYQT_BorrowedDataObjectLight ( QString entry )
  : CAM_DataObject(0),
    LightApp_DataObject( 0 )

{
  myEntry = entry;
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight()
// purpose  : destructor
//=================================================================================
SALOME_PYQT_BorrowedDataObjectLight::~SALOME_PYQT_BorrowedDataObjectLight()
{
  
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::entry()
// purpose  : return entry of object
//=================================================================================
QString SALOME_PYQT_BorrowedDataObjectLight::entry() const
{
  return myEntry;
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::refEntry()
// purpose  : return entry of the data object referenced by this one (if any)
//=================================================================================
QString SALOME_PYQT_BorrowedDataObjectLight::refEntry() const
{
  return myRefEntry;
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::setRefEntry()
// purpose  : sets entry of the data object referenced by this one
//=================================================================================
void SALOME_PYQT_BorrowedDataObjectLight::setRefEntry( const QString& refEntry )
{
  myRefEntry = refEntry;
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::name()
// purpose  : return name of object
//=================================================================================
QString SALOME_PYQT_BorrowedDataObjectLight::name() const
{
  return myName;
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::icon()
// purpose  : return icon of object
//=================================================================================
QPixmap SALOME_PYQT_BorrowedDataObjectLight::icon(const int index) const
{
  if(index == NameId)
    return myIcon;
  else
    return LightApp_DataObject::icon( index );
}


//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::toolTip()
// purpose  : return toolTip of object
//=================================================================================
QString SALOME_PYQT_BorrowedDataObjectLight::toolTip(const int index) const
{
  return myToolTip;
}

//=================================================================================
// function : SALOME_PYQT_BorrowedDataObjectLight::toolTip()
// purpose  : return toolTip of object
//=================================================================================
QColor SALOME_PYQT_BorrowedDataObjectLight::color( const ColorRole role, const int id ) const
{
  QColor c;

  switch ( role )
  {
  case Text:
  case Foreground:
    if ( !isReference() )
      c = myColor;
    break;

  default:
    break;
  }

  // Issue 21379: LightApp_DataObject::color() defines colors for valid references
  if ( !c.isValid() )
    c = LightApp_DataObject::color( role, id );

  return c;
}

bool SALOME_PYQT_BorrowedDataObjectLight::setName(const QString& name)
{
  myName = name;
  return true;
}

void SALOME_PYQT_BorrowedDataObjectLight::setIcon(const QString& iconname)
{
  if(!iconname.isEmpty()) {
    LightApp_Application* anApp = dynamic_cast<LightApp_Application*>( SUIT_Session::session()->activeApplication() );
    if(anApp) {
      QString modulename = anApp->activeModule()->name();
      if(!modulename.isEmpty())
        {
          myIcon = SUIT_Session::session()->resourceMgr()->loadPixmap(modulename,
                                                                      QObject::tr(iconname.toLatin1()));
        }
    }
  }
}

void SALOME_PYQT_BorrowedDataObjectLight::setToolTip(const QString& tooltip)
{
  myToolTip = tooltip;
}

void SALOME_PYQT_BorrowedDataObjectLight::setColor(const QColor& color)
{
  myColor = color;
}
