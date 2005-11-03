
#ifndef LIGHTAPP_DATAOWNER_H
#define LIGHTAPP_DATAOWNER_H

#include "LightApp.h"
#include "SUIT_DataOwner.h"
#include "SALOME_InteractiveObject.hxx"

/*!
  This class provide data owner objects.
*/
class LIGHTAPP_EXPORT LightApp_DataOwner : public SUIT_DataOwner
{
public:
    LightApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO );
    LightApp_DataOwner( const QString& );
    virtual ~LightApp_DataOwner();

    virtual bool isEqual( const SUIT_DataOwner& ) const;
    const Handle(SALOME_InteractiveObject)& IO() const;
    QString entry() const;

private:
    QString  myEntry;
    Handle(SALOME_InteractiveObject) myIO;
};

typedef SMART(LightApp_DataOwner) LightApp_DataOwnerPtr;

#endif
