#ifndef LIGHTAPP_OBSELECTOR_H
#define LIGHTAPP_OBSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>

class OB_Browser;
class LightApp_DataObject;

class LIGHTAPP_EXPORT LightApp_OBSelector : public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_OBSelector( OB_Browser*, SUIT_SelectionMgr* );
  virtual ~LightApp_OBSelector();

  OB_Browser*  browser() const;

  /*!Return "ObjectBrowser"*/
  virtual QString type() const { return "ObjectBrowser"; }

private slots:
  void         onSelectionChanged();

protected:
  virtual void getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void setSelection( const SUIT_DataOwnerPtrList& );

private:
  void         fillEntries( QMap<QString, LightApp_DataObject*>& );

private:
  OB_Browser*  myBrowser;
};

#endif
