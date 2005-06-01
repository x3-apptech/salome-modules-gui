#ifndef SVTK_VIEWMANAGER_H
#define SVTK_VIEWMANAGER_H

#include "SUIT_ViewManager.h"
#include "SVTK.h"

class SUIT_Desktop;

class SVTK_EXPORT SVTK_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  SVTK_ViewManager( SUIT_Study* study, SUIT_Desktop* );
  virtual ~SVTK_ViewManager();

protected:
  void setViewName( SUIT_ViewWindow* theView );

private:
  int               myId;
  static  int       _SVTKViewMgr_Id;
};

#endif
