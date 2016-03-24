// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

#include "Plot2d_ViewManager.h"
#include "Plot2d_ViewModel.h"
#include "Plot2d_ViewWindow.h"
#include "Plot2d_ViewFrame.h"

#include "SUIT_PreferenceMgr.h"
#include "SUIT_ResourceMgr.h"

/*!
  Constructor
*/
Plot2d_ViewManager::Plot2d_ViewManager( SUIT_Study* study, SUIT_Desktop* desk ) 
: SUIT_ViewManager( study, desk, new Plot2d_Viewer() )
{
  setTitle( tr( "PLOT2D_VIEW_TITLE" ) );
}

/*!
  Destructor
*/
Plot2d_ViewManager::~Plot2d_ViewManager()
{
}

/*!
  \return corresponding viewer
*/
Plot2d_Viewer* Plot2d_ViewManager::getPlot2dModel() const
{
  return (Plot2d_Viewer*)myViewModel;
}

/*!
  Adds new view
  \param theView - view to be added
*/
bool Plot2d_ViewManager::insertView( SUIT_ViewWindow* theView )
{
  bool res = SUIT_ViewManager::insertView( theView );
  if ( res )
  {
    Plot2d_ViewWindow* view = (Plot2d_ViewWindow*)theView;
    connect( view, SIGNAL( cloneView() ), this, SLOT( onCloneView() ) );

    Plot2d_ViewFrame* aViewFrame = view->getViewFrame();
    Plot2d_Viewer* aViewer = getPlot2dModel();
    connect( aViewFrame, SIGNAL( clicked(const QVariant&, int) ),
             aViewer, SLOT( onClicked(const QVariant&, int) ) );
  }
  return res;
}

/*!
  Creates new view
*/
void Plot2d_ViewManager::createView()
{
  createViewWindow();
}

/*!
  SLOT: called if action "Clone view" is activated, emits signal cloneView()
*/
void Plot2d_ViewManager::onCloneView()
{
  if( sender() && sender()->inherits( "Plot2d_ViewWindow" ) )
  {
    Plot2d_ViewWindow* srcWnd = ( Plot2d_ViewWindow* )sender();
    cloneView( srcWnd );
  }
}

/*!
  \brief Creates clone of source window
  \param srcWnd source window
  \return Pointer on the new window
  \sa onCloneView()
*/
Plot2d_ViewWindow* Plot2d_ViewManager::cloneView( Plot2d_ViewWindow* srcWnd )
{
  SUIT_ViewWindow* vw = createViewWindow();

  Plot2d_ViewWindow* newWnd = 0;
  if( vw && vw->inherits( "Plot2d_ViewWindow" ) )
    newWnd = ( Plot2d_ViewWindow* )vw;
  
  if( newWnd && srcWnd )
    emit cloneView( srcWnd->getViewFrame(), newWnd->getViewFrame() );

  return newWnd;
}

/*!
  Fills preference manager for viewer
*/
int Plot2d_ViewManager::fillPreferences( SUIT_PreferenceMgr* thePrefMgr, const int theId )
{
  int aGrpId = thePrefMgr->addItem( tr( "PREF_GROUP_PLOT2DVIEWER" ), theId,
				    SUIT_PreferenceMgr::GroupBox );

  thePrefMgr->addItem( tr( "PREF_SHOW_LEGEND" ), aGrpId,
		       SUIT_PreferenceMgr::Bool, "Plot2d", "ShowLegend" );
  
  int legendPosition = thePrefMgr->addItem( tr( "PREF_LEGEND_POSITION" ), aGrpId,
					    SUIT_PreferenceMgr::Selector, "Plot2d", "LegendPos" );
  QStringList aLegendPosList;
  aLegendPosList.append( tr("PREF_LEFT") );
  aLegendPosList.append( tr("PREF_RIGHT") );
  aLegendPosList.append( tr("PREF_TOP") );
  aLegendPosList.append( tr("PREF_BOTTOM") );

  QList<QVariant> anIndexesList;
  anIndexesList.append(0);
  anIndexesList.append(1);
  anIndexesList.append(2);
  anIndexesList.append(3);

  thePrefMgr->setItemProperty( "strings", aLegendPosList, legendPosition );
  thePrefMgr->setItemProperty( "indexes", anIndexesList, legendPosition );

  int curveType = thePrefMgr->addItem( tr( "PREF_CURVE_TYPE" ), aGrpId,
				       SUIT_PreferenceMgr::Selector, "Plot2d", "CurveType" );
  QStringList aCurveTypesList;
  aCurveTypesList.append( tr("PREF_POINTS") );
  aCurveTypesList.append( tr("PREF_LINES") );
  aCurveTypesList.append( tr("PREF_SPLINE") );

  anIndexesList.clear();
  anIndexesList.append(0);
  anIndexesList.append(1);
  anIndexesList.append(2);

  thePrefMgr->setItemProperty( "strings", aCurveTypesList, curveType );
  thePrefMgr->setItemProperty( "indexes", anIndexesList, curveType );

  int markerSize = thePrefMgr->addItem( tr( "PREF_MARKER_SIZE" ), aGrpId,
					SUIT_PreferenceMgr::IntSpin, "Plot2d", "MarkerSize" );

  thePrefMgr->setItemProperty( "min", 0, markerSize );
  thePrefMgr->setItemProperty( "max", 100, markerSize );

  QStringList aScaleModesList;
  aScaleModesList.append( tr("PREF_LINEAR") );
  aScaleModesList.append( tr("PREF_LOGARITHMIC") );

  anIndexesList.clear();
  anIndexesList.append(0);
  anIndexesList.append(1);

  int horScale = thePrefMgr->addItem( tr( "PREF_HOR_AXIS_SCALE" ), aGrpId,
				      SUIT_PreferenceMgr::Selector, "Plot2d", "HorScaleMode" );

  thePrefMgr->setItemProperty( "strings", aScaleModesList, horScale );
  thePrefMgr->setItemProperty( "indexes", anIndexesList, horScale );

  int verScale = thePrefMgr->addItem( tr( "PREF_VERT_AXIS_SCALE" ), aGrpId,
				      SUIT_PreferenceMgr::Selector, "Plot2d", "VerScaleMode" );

  thePrefMgr->setItemProperty( "strings", aScaleModesList, verScale );
  thePrefMgr->setItemProperty( "indexes", anIndexesList, verScale );

  thePrefMgr->addItem( tr( "PREF_VIEWER_BACKGROUND" ), aGrpId,
		       SUIT_PreferenceMgr::Color, "Plot2d", "Background" );

  return aGrpId;
}
