#ifndef SALOMEAPP_OBSELECTOR_H
#define SALOMEAPP_OBSELECTOR_H

#include "SalomeApp.h"

#include <SUIT_Selector.h>

#include <qobject.h>

class OB_Browser;
class SalomeApp_DataObject;

class SALOMEAPP_EXPORT SalomeApp_OBSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  SalomeApp_OBSelector( OB_Browser*, SUIT_SelectionMgr* );
  virtual ~SalomeApp_OBSelector();

  OB_Browser*  browser() const;

  void setBrowser(OB_Browser* theBrowser);
  
  virtual QString type() const { return "ObjectBrowser"; }

private slots:
  void         onSelectionChanged();
  void         onBrowserDestroyed();

protected:
  virtual void getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void setSelection( const SUIT_DataOwnerPtrList& );

private:
  void         fillEntries( QMap<QString, SalomeApp_DataObject*>& );

private:
  OB_Browser*  myBrowser;
};

#endif
