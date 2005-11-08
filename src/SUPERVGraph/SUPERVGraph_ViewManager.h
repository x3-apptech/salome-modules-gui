#ifndef SUPERVGRAPH_VIEWMANAGER_H
#define SUPERVGRAPH_VIEWMANAGER_H

#include "SUPERVGraph_ViewModel.h"
#include "SUIT_ViewManager.h"

class SUIT_Desktop;

class /*SUPERVGRAPH_EXPORT*/ SUPERVGraph_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  SUPERVGraph_ViewManager( SUIT_Study* theStudy, SUIT_Desktop* theDesktop, SUIT_ViewModel* theViewModel = 0 );
  ~SUPERVGraph_ViewManager();

  virtual void      contextMenuPopup( QPopupMenu* thePopup );

protected:
  void              setViewName(SUIT_ViewWindow* theView);

private:
  static int        myMaxId;
  int               myId;

};

#endif
