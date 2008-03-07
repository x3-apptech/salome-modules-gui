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

#include "LightApp_EventFilter.h"

#include <SUIT_Desktop.h>

#include <qapplication.h>

LightApp_EventFilter* LightApp_EventFilter::myFilter = NULL;

/*!Constructor.*/
LightApp_EventFilter::LightApp_EventFilter()
: QObject()
{
  qApp->installEventFilter( this );
}

/*!Destructor.*/
LightApp_EventFilter::~LightApp_EventFilter()
{
  qApp->removeEventFilter( this );
}

/*!
  Custom event filter
*/
bool LightApp_EventFilter::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::WindowActivate && o->inherits("QDialog") )
    {
      QWidget* parent = ((QWidget*)o)->parentWidget();
      
      SUIT_Desktop* aDesktop = 0;
      
      while( parent )
	{
	  if ( aDesktop = dynamic_cast<SUIT_Desktop*>(parent) )
	    break;
	  parent = parent->parentWidget();
	}
      
      if ( aDesktop )
	aDesktop->emitActivated();
    }
  
  return QObject::eventFilter( o, e );
}

/*!Create new instance of LightApp_EventFilter*/
void LightApp_EventFilter::Init()
{
  if( myFilter==NULL )
    myFilter = new LightApp_EventFilter();
}

/*!Destroy filter.*/
void LightApp_EventFilter::Destroy()
{
  if( myFilter )
  {
    delete myFilter;
    myFilter = NULL;
  }
}
