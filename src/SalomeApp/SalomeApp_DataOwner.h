
#ifndef SALOMEAPP_DATAOWNER_H
#define SALOMEAPP_DATAOWNER_H

#include "SUIT_DataOwner.h"
#include "SALOME_InteractiveObject.hxx"

/*!
  This class provide data owner objects.
*/
class SalomeApp_DataOwner : public SUIT_DataOwner
{
public:
    SalomeApp_DataOwner( const Handle(SALOME_InteractiveObject)& theIO );
    SalomeApp_DataOwner( const QString& );
    virtual ~SalomeApp_DataOwner();

    virtual bool isEqual( const SUIT_DataOwner& ) const;
    const Handle(SALOME_InteractiveObject)& IO() const;
    QString entry() const;

private:
    QString  myEntry;
    Handle(SALOME_InteractiveObject) myIO;
};

typedef SMART(SalomeApp_DataOwner) SalomeApp_DataOwnerPtr;

#endif
