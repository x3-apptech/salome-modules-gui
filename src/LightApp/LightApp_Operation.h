// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//  File   : LightApp_Operation.h
//  Author : Sergey LITONIN
//  Module : LIGHT
//
#ifndef LightApp_Operation_H
#define LightApp_Operation_H

#include "LightApp.h"
#include <SUIT_Operation.h>

class LightApp_Module;
class LightApp_SelectionMgr;
class LightApp_Dialog;
class SUIT_Desktop;

/*!
  \class LightApp_Operation
  \brief Base class for all operations
  Base class for all operations (see SUIT_Operation for more description)
*/
class LIGHTAPP_EXPORT LightApp_Operation : public SUIT_Operation
{
  Q_OBJECT

public:
  LightApp_Operation();
  virtual ~LightApp_Operation();

  virtual void              setModule( LightApp_Module* );
  LightApp_Module*          module() const;

  bool                      isAutoResumed() const;

  virtual LightApp_Dialog* dlg() const;

protected:

  // Methods redefined from base class

  virtual void              startOperation();
  virtual void              suspendOperation();
  virtual void              resumeOperation();
  virtual void              abortOperation();
  virtual void              commitOperation();

  // Additional virtual methods may be redefined by derived classes
  
  virtual void              setDialogActive( const bool );
  virtual void              activateSelection();
  virtual void              selectionDone();


  // Axiluary methods
  
  SUIT_Desktop*             desktop() const;
  SUIT_Operation*           activeOperation() const;
  LightApp_SelectionMgr*    selectionMgr() const;
  void                      update( const int );
  void                      setAutoResumed( const bool );
      
private slots:

  virtual void              onSelectionDone();

private:

  LightApp_Module*          myModule;         
  bool                      myIsAutoResumed;
};

#endif






