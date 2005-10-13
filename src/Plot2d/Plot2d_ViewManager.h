#ifndef PLOT2D_VIEWMANAGER_H
#define PLOT2D_VIEWMANAGER_H

#include "Plot2d.h"
#include "SUIT_ViewManager.h"

class SUIT_Desktop;
class Plot2d_Viewer;
class Plot2d_ViewFrame;

class PLOT2D_EXPORT Plot2d_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  Plot2d_ViewManager( SUIT_Study*, SUIT_Desktop* );
  ~Plot2d_ViewManager();

  Plot2d_Viewer*     getPlot2dModel() const;

protected:
  void         setViewName(SUIT_ViewWindow* theView);
  bool         insertView(SUIT_ViewWindow* theView);

public slots:
  void         createView();

protected slots:
  void         onCloneView();

signals:
  void cloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* );

protected:
  static  int  myMaxId;
  int          myId;
};

#endif
