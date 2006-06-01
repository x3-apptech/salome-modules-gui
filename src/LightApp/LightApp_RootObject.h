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
#ifndef LIGHTAPP_ROOTOBJECT_H
#define LIGHTAPP_ROOTOBJECT_H

#include "LightApp.h"
#include "SUIT_DataObject.h"

class LightApp_Study;

/*!
  LightApp_RootObject - class to be instanciated by only one object - 
  root object of LightApp data object tree.  This object is not shown
  in object browser (invisible), so it has no re-definition of name(), icon(),
  etc. methods.  The goal of this class is to provide a unified access
  to LightApp_Study object from LightApp_DataObject instances.
*/
class LIGHTAPP_EXPORT LightApp_RootObject : public SUIT_DataObject
{
public:
  LightApp_RootObject( LightApp_Study* study )
   : myStudy( study ) 
  {}

  virtual ~LightApp_RootObject() {}
    
  void                   setStudy( LightApp_Study* study ) { myStudy = study; }
  LightApp_Study*        study() const                     { return myStudy;  } 
  
private:
  LightApp_Study*            myStudy;

};

#endif
