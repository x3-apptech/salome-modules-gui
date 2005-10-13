#ifndef PLOT2D_VIEWWINDOW_H
#define PLOT2D_VIEWWINDOW_H

#include "Plot2d.h"
#include <SUIT_ViewWindow.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_Desktop;
class Plot2d_Viewer;
class Plot2d_ViewFrame;
class QtxAction;
class SUIT_ToolButton;

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

protected:
  QImage dumpView();

private:
  bool eventFilter(QObject* watched, QEvent* e);

  void createActions();
  void createToolBar();

public slots:
  void onChangeHorMode();
  void onChangeVerMode();
  void onChangeCurveMode();
  void onChangeLegendMode();
  
  void onFitAll();
  void onFitRect();
  void onZoom();
  void onPanning();
  void onGlobalPanning();
  void onViewHorMode();
  void onViewVerMode();
  void onLegend();
  void onCurves();

  void onDumpView();

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
