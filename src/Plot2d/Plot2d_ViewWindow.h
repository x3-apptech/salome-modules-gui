#ifndef PLOT2D_VIEWWINDOW_H
#define PLOT2D_VIEWWINDOW_H

#include "Plot2d.h"

#include "Plot2d_ViewModel.h"
#include "Plot2d_ViewFrame.h"

#include <SUIT_Desktop.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ToolButton.h>

#include <QtxAction.h>

#include <qstring.h>
#include <qstring.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class PLOT2D_EXPORT Plot2d_ViewWindow : public SUIT_ViewWindow  
{
  Q_OBJECT

public:
  Plot2d_ViewWindow( SUIT_Desktop* theDesktop, Plot2d_Viewer* theModel );
  virtual ~Plot2d_ViewWindow();

  Plot2d_Viewer*    getModel() { return myModel; }
  void              putInfo(QString theMsg);
  Plot2d_ViewFrame* getViewFrame() { return myViewFrame; };
  QToolBar*         getToolBar() { return myToolBar; };
  void              contextMenuPopup( QPopupMenu* thePopup );

private:
  bool eventFilter(QObject* watched, QEvent* e);

  void createActions();
  void createToolBar();

public slots:
  void onChangeHorMode();
  void onChangeVerMode();
  void onChangeCurveMode();
  void onChangeLegendMode();
  
  void onDumpView();
  void onFitAll();
  void onFitRect();
  void onZoom();
  void onPanning();
  void onGlobalPanning();
  void onViewHorMode();
  void onViewVerMode();
  void onLegend();
  void onCurves();

signals:
  void cloneView();

protected:
  enum { DumpId, FitAllId, FitRectId, ZoomId, PanId, GlobalPanId, HorId,
         VerId, LegendId, CurvPointsId, CurvLinesId, CurvSplinesId, CurvSettingsId, CloneId,
         PModeXLinearId, PModeXLogarithmicId, PModeYLinearId, PModeYLogarithmicId };
  typedef QMap<int, QtxAction*> ActionsMap;
  ActionsMap        myActionsMap;

private:
  Plot2d_Viewer*    myModel;
  Plot2d_ViewFrame* myViewFrame;
  QToolBar*         myToolBar;

  SUIT_ToolButton*  myCurveBtn;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
