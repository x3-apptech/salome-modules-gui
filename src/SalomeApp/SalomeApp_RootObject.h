#ifndef SALOMEAPP_ROOTOBJECT_H
#define SALOMEAPP_ROOTOBJECT_H

#include "SalomeApp.h"
#include "SUIT_DataObject.h"

class SalomeApp_Study;

/*!
  SalomeApp_RootObject - class to be instanciated by only one object - 
  root object of SalomeApp data object tree.  This object is not shown
  in object browser (invisible), so it has no re-definition of name(), icon(),
  etc. methods.  The goal of this class is to provide a unified access
  to SalomeApp_Study object from SalomeApp_DataObject instances.
*/
class SALOMEAPP_EXPORT SalomeApp_RootObject : public SUIT_DataObject
{
public:
  SalomeApp_RootObject( SalomeApp_Study* study )
    : myStudy( study ) {}

  virtual ~SalomeApp_RootObject() {}
    
  void                        setStudy( SalomeApp_Study* study ) { myStudy = study; }
  SalomeApp_Study*            study() const                      { return myStudy;  } 
  
private:
  SalomeApp_Study*            myStudy;

};

#endif
