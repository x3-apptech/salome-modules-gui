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
