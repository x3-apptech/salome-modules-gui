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
#include "SALOME_PYQT_DataObjectLight.h"
#include <LightApp_Application.h>
#include <SUIT_Session.h>
#include <utilities.h>
#include <SUIT_ResourceMgr.h>


#include <CAM_DataModel.h>
#include <CAM_Module.h>


/*!
 *  Class:       SALOME_PYQT_DataObjectLight
 *  Description: LIGHT PYTHON module's data object
 */

static int _ID = 0;

//=================================================================================
// function : SALOME_PYQT_DataObjectLight()
// purpose  : constructor
//=================================================================================
SALOME_PYQT_DataObjectLight::SALOME_PYQT_DataObjectLight ( SUIT_DataObject* parent )
  : CAM_DataObject(parent),
    LightApp_DataObject( parent )

{
  _ID++;
  myEntry = QString("PYLIGHT_OBJ_%1").arg(_ID);
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight()
// purpose  : destructor
//=================================================================================
SALOME_PYQT_DataObjectLight::~SALOME_PYQT_DataObjectLight()
{
  
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight::entry()
// purpose  : return entry of object
//=================================================================================
QString SALOME_PYQT_DataObjectLight::entry() const
{
  return myEntry;
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight::refEntry()
// purpose  : return entry of the data object referenced by this one (if any)
//=================================================================================
QString SALOME_PYQT_DataObjectLight::refEntry() const
{
  return myRefEntry;
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight::setRefEntry()
// purpose  : sets entry of the data object referenced by this one
//=================================================================================
void SALOME_PYQT_DataObjectLight::setRefEntry( const QString& refEntry )
{
  myRefEntry = refEntry;
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight::name()
// purpose  : return name of object
//=================================================================================
QString SALOME_PYQT_DataObjectLight::name() const
{
  return myName;
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight::icon()
// purpose  : return icon of object
//=================================================================================
QPixmap SALOME_PYQT_DataObjectLight::icon(const int index) const
{
  if(index == NameId)
    return myIcon;
  else
    return LightApp_DataObject::icon( index );
}


//=================================================================================
// function : SALOME_PYQT_DataObjectLight::toolTip()
// purpose  : return toolTip of object
//=================================================================================
QString SALOME_PYQT_DataObjectLight::toolTip(const int index) const
{
  return myToolTip;
}

//=================================================================================
// function : SALOME_PYQT_DataObjectLight::toolTip()
// purpose  : return toolTip of object
//=================================================================================
QColor SALOME_PYQT_DataObjectLight::color( const ColorRole role, const int id ) const
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

bool SALOME_PYQT_DataObjectLight::setName(const QString& name)
{
  myName = name;
  return true;
}

void SALOME_PYQT_DataObjectLight::setIcon(const QString& iconname)
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

void SALOME_PYQT_DataObjectLight::setToolTip(const QString& tooltip)
{
  myToolTip = tooltip;
}

void SALOME_PYQT_DataObjectLight::setColor(const QColor& color)
{
  myColor = color;
}
