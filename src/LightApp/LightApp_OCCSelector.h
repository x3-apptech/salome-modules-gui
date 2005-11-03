#ifndef LIGHTAPP_OCCSELECTOR_H
#define LIGHTAPP_OCCSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>

#include <OCCViewer_ViewModel.h>

class Handle_AIS_InteractiveObject;

class LIGHTAPP_EXPORT LightApp_OCCSelector : public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_OCCSelector( OCCViewer_Viewer*, SUIT_SelectionMgr* );
  virtual ~LightApp_OCCSelector();

  OCCViewer_Viewer* viewer() const;

  virtual QString   type() const { return OCCViewer_Viewer::Type(); }

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
