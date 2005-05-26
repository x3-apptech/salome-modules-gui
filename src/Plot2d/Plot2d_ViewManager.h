#ifndef PLOT2D_VIEWMANAGER_H
#define PLOT2D_VIEWMANAGER_H

#include "Plot2d.h"

#include "Plot2d_ViewWindow.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"

class SUIT_Desktop;

class PLOT2D_EXPORT Plot2d_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  Plot2d_ViewManager( SUIT_Study*, SUIT_Desktop* );
  ~Plot2d_ViewManager();

  Plot2d_Viewer*     getPlot2dModel() { return (Plot2d_Viewer*)myViewModel; };

protected:
  void         setViewName(SUIT_ViewWindow* theView);
  bool         insertView(SUIT_ViewWindow* theView);

public slots:
  void         createView();


protected:
  static  int  myMaxId;
  int          myId;
};

#endif
