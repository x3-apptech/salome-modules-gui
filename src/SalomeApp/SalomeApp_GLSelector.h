#ifndef SALOMEAPP_GLSELECTOR_H
#define SALOMEAPP_GLSELECTOR_H

#include "SalomeApp.h"

#include <SUIT_Selector.h>

#include <GLViewer_Viewer2d.h>

class SALOMEAPP_EXPORT SalomeApp_GLSelector : public SUIT_Selector
{
  Q_OBJECT

public:
  SalomeApp_GLSelector( GLViewer_Viewer2d*, SUIT_SelectionMgr* );
  virtual ~SalomeApp_GLSelector();

  GLViewer_Viewer2d* viewer() const;

  virtual QString type() const { return GLViewer_Viewer2d::Type(); }

private slots:
  void                onSelectionChanged();

protected:
  virtual void        getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void        setSelection( const SUIT_DataOwnerPtrList& );

private:
  GLViewer_Viewer2d* myViewer;
};

#endif
