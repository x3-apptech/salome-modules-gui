// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
#ifndef PLOT2D_VIEWWINDOW_H
#define PLOT2D_VIEWWINDOW_H

#include "Plot2d.h"
#include <SUIT_ViewWindow.h>

#include <qimage.h>

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

  virtual QString   getVisualParameters();
  virtual void      setVisualParameters( const QString& parameters );
  
  virtual void      RefreshDumpImage();

protected:
  virtual QImage    dumpView();
  virtual QString   filter() const;
  virtual bool      dumpViewToFormat( const QImage&, const QString& fileName, const QString& format );

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

  QImage myDumpImage;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
