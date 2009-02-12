//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef SUIT_VIEWMODEL_H
#define SUIT_VIEWMODEL_H

#include "SUIT.h"

#include <QObject>
#include <QMap>

class QMenu;

class SUIT_Desktop;
class SUIT_ViewWindow;
class SUIT_ViewManager;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif
/*!
 * This object manages the definition and behaviour of a View Window. \n
 * In case of definition of this object in an external lib that lib must \n
 * have an exported function "createViewModel" which returns newly created \n
 * instance of SUIT_ViewModel.
 */
class SUIT_EXPORT SUIT_ViewModel : public QObject
{
	Q_OBJECT
public:
  enum HotOperation { PAN, ZOOM, ROTATE, FIT_AREA };

  typedef QMap<HotOperation, Qt::KeyboardModifier> StatesMap;
  typedef QMap<HotOperation, Qt::MouseButton>      ButtonsMap;
  
  SUIT_ViewModel();
  virtual ~SUIT_ViewModel();

  virtual SUIT_ViewWindow* createView( SUIT_Desktop* theDesktop );

  virtual void      setViewManager(SUIT_ViewManager* theViewManager);
  SUIT_ViewManager* getViewManager() const;

  virtual QString   getType() const { return "SUIT_ViewModel"; }

  virtual void      contextMenuPopup( QMenu* ) {}

  static void       setHotButton( HotOperation theOper, Qt::KeyboardModifier theState,
				  Qt::MouseButton theButton );
  static void       getHotButton( HotOperation theOper, Qt::KeyboardModifier& theState,
				  Qt::MouseButton& theButton );

protected:
  SUIT_ViewManager* myViewManager;

public:
  static StatesMap  myStateMap;
  static ButtonsMap myButtonMap;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

extern "C"
{
  typedef SUIT_ViewModel* (*VIEWMODEL_CREATE_FUNC)();
}

#define VIEWMODEL_CREATE_NAME   "createViewModel"

#endif
