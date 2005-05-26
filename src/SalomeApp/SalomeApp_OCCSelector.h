#ifndef SALOMEAPP_OCCSELECTOR_H
#define SALOMEAPP_OCCSELECTOR_H

#include "SalomeApp.h"

#include <SUIT_Selector.h>

#include <OCCViewer_ViewModel.h>

#include <qobject.h>

class Handle_AIS_InteractiveObject;

class SALOMEAPP_EXPORT SalomeApp_OCCSelector : public QObject, public SUIT_Selector
{
  Q_OBJECT

public:
  SalomeApp_OCCSelector( OCCViewer_Viewer*, SUIT_SelectionMgr* );
  virtual ~SalomeApp_OCCSelector();

  OCCViewer_Viewer* viewer() const;

  virtual QString type() const { return OCCViewer_Viewer::Type(); }

private slots:
  virtual void      onSelectionChanged();

protected:
  virtual void      getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void      setSelection( const SUIT_DataOwnerPtrList& );

  QString           entry( const Handle_AIS_InteractiveObject& ) const;

private:
  OCCViewer_Viewer* myViewer;
};

#endif
