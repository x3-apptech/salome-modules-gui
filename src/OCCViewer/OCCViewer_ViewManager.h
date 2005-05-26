#ifndef OCCVIEWER_VIEWMANAGER_H
#define OCCVIEWER_VIEWMANAGER_H

#include "OCCViewer_ViewModel.h"

#include "SUIT_ViewManager.h"

class SUIT_Desktop;

class OCCVIEWER_EXPORT OCCViewer_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  OCCViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* theDesktop, bool DisplayTrihedron = true );
  ~OCCViewer_ViewManager();

  OCCViewer_Viewer* getOCCViewer() { return (OCCViewer_Viewer*) myViewModel; }

  virtual void      contextMenuPopup( QPopupMenu* );

protected:
  void              setViewName(SUIT_ViewWindow* theView);

protected:
  static  int       myMaxId;
  int               myId;
};

#endif
