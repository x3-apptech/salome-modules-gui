
#ifndef LIGHTAPP_DATASUBOWNER_H
#define LIGHTAPP_DATASUBOWNER_H

#include <LightApp.h>
#include <LightApp_DataOwner.h>

/*!
  Class provide sub owner.
 */
class LIGHTAPP_EXPORT LightApp_DataSubOwner : public LightApp_DataOwner
{
public:
    LightApp_DataSubOwner( const QString&, const int );
    virtual ~LightApp_DataSubOwner();

    virtual bool isEqual( const SUIT_DataOwner& ) const;
    int          index() const;

private:
    int          myIndex;
};

#endif
