#ifndef LIGHTAPP_GLSELECTOR_H
#define LIGHTAPP_GLSELECTOR_H

#include "LightApp.h"

#include <SUIT_Selector.h>

#include <GLViewer_Viewer2d.h>

class LIGHTAPP_EXPORT LightApp_GLSelector : public SUIT_Selector
{
  Q_OBJECT

public:
  LightApp_GLSelector( GLViewer_Viewer2d*, SUIT_SelectionMgr* );
  virtual ~LightApp_GLSelector();

  GLViewer_Viewer2d* viewer() const;

  virtual QString type() const { return GLViewer_Viewer2d::Type(); }

private slots:
  void                onSelectionChanged();

protected:
  virtual void        getSelection( SUIT_DataOwnerPtrList& ) const;
  virtual void        setSelection( const SUIT_DataOwnerPtrList& );

private:
  GLViewer_Viewer2d*  myViewer;
};

#endif
