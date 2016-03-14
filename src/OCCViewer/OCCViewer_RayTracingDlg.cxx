// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// internal includes
#include "OCCViewer_RayTracingDlg.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewFrame.h"

// GUI includes
#include <SUIT_Session.h>
#include <QtxIntSpinBox.h>

// QT Includes
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>

/*!
  \class OCCViewer_RayTracingDlg
  \brief Dialog allowing to assign parameters of ray tracing
*/

/*!
  \brief Constructor
  \param view - parent widget
*/
OCCViewer_RayTracingDlg::OCCViewer_RayTracingDlg( OCCViewer_ViewWindow* view )
  :QDialog( view )
{
  // get current view frame (OCCViewer_ViewWindow->QFrame->OCCViewer_ViewFrame)
  myViewFrame = dynamic_cast<OCCViewer_ViewFrame*>( view->parent()->parent() );
  myView3d = view->getViewPort()->getView();
  setObjectName( "OCCViewer_RayTracingDlg" );
  setWindowTitle( tr( "RAY_TRACING" ) );
  setModal( false );

  setAttribute( Qt::WA_DeleteOnClose, true );

  // Create layout for this dialog
  QVBoxLayout* dlglayout = new QVBoxLayout( this );
  dlglayout->setSpacing( 6 );
  dlglayout->setMargin( 11 );

  // Create "Ray tracing" group

  myRayTracingGroup = new QGroupBox( tr( "RAY_TRACING" ) );
  myRayTracingGroup->setCheckable( true );
  
  QGridLayout* rayTracingLayout = new QGridLayout( myRayTracingGroup );
  rayTracingLayout->setSpacing( 6 );
  rayTracingLayout->setMargin( 11 );

  myShadow = new QCheckBox( tr("SHADOW") );
  myReflection = new QCheckBox( tr("REFLECTION") );
  myAntialiasing = new QCheckBox( tr("ANTIALIASING") );
  myTransparentShadow = new QCheckBox( tr("TRANSPARENT_SHADOW") );
  QLabel* depthLabel = new QLabel( tr( "DEPTH" ) );
  myDepth = new QtxIntSpinBox( 1, 10 );

  rayTracingLayout->addWidget( depthLabel,          0, 0 );
  rayTracingLayout->addWidget( myDepth,             0, 1 );
  rayTracingLayout->addWidget( myReflection,        1, 0 );
  rayTracingLayout->addWidget( myAntialiasing,      1, 1 );
  rayTracingLayout->addWidget( myShadow,            2, 0 );
  rayTracingLayout->addWidget( myTransparentShadow, 2, 1 );

  // Create "Buttons" group

  QGroupBox* groupButtons = new QGroupBox( this );
  QHBoxLayout* groupButtonsLayout = new QHBoxLayout( groupButtons );
  groupButtonsLayout->setSpacing( 6 );
  groupButtonsLayout->setMargin( 11 );

  QPushButton* buttonClose = new QPushButton( tr( "BUT_CLOSE" ) );
  buttonClose->setDefault( true );

  QPushButton* buttonHelp = new QPushButton( tr( "GEOM_BUT_HELP" ) );

  groupButtonsLayout->addStretch();
  groupButtonsLayout->addWidget( buttonClose );
  groupButtonsLayout->addWidget( buttonHelp );

  dlglayout->addWidget( myRayTracingGroup );
  dlglayout->addWidget( groupButtons );

  // Initializations
  initParam();

  // Signals and slots connections
  connect( myRayTracingGroup,    SIGNAL( toggled(bool) ),            this, SLOT( onRayTracing(bool) ) );
  connect( myShadow,             SIGNAL( toggled(bool) ),            this, SLOT( onValueChanged() ) );
  connect( myReflection,         SIGNAL( toggled(bool) ),            this, SLOT( onValueChanged() ) );
  connect( myAntialiasing,       SIGNAL( toggled(bool) ),            this, SLOT( onValueChanged() ) );
  connect( myTransparentShadow,  SIGNAL( toggled(bool) ),            this, SLOT( onValueChanged() ) );
  connect( myDepth,              SIGNAL( valueChanged(int) ),        this, SLOT( onValueChanged() ) );
  connect( buttonClose,          SIGNAL( clicked() ),                this, SLOT( close() ) ) ;
  connect( buttonHelp,           SIGNAL( clicked() ),                this, SLOT( ClickOnHelp() ) );
}

/*!
  \brief Destructor
*/
OCCViewer_RayTracingDlg::~OCCViewer_RayTracingDlg()
{
}

QString OCCViewer_RayTracingDlg::getName()
{
  // return the name of object
  return QString( "OCCViewer_RayTracingDlg" );
}

/*!
  Initialization of initial values of widgets
*/
void OCCViewer_RayTracingDlg::initParam()
{
  Graphic3d_RenderingParams aParams = myView3d->RenderingParams();
  myRayTracingGroup->setChecked( aParams.Method == Graphic3d_RM_RAYTRACING );
  myDepth->setValue( aParams.RaytracingDepth );
  myReflection->setChecked( aParams.IsReflectionEnabled );
  myAntialiasing->setChecked( aParams.IsAntialiasingEnabled );
  myShadow->setChecked( aParams.IsShadowEnabled );
  myTransparentShadow->setChecked( aParams.IsTransparentShadowEnabled );
}

/*!
  SLOT: called on value of ray tracing changed
*/
void OCCViewer_RayTracingDlg::onValueChanged()
{
  for ( int i = OCCViewer_ViewFrame::BOTTOM_RIGHT; i <= OCCViewer_ViewFrame::TOP_RIGHT; i++ ) {
    if ( OCCViewer_ViewWindow* aViewWindow = myViewFrame->getView(i) ) {
      Handle(V3d_View) aView = aViewWindow->getViewPort()->getView();
      Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
      aParams.IsShadowEnabled = myShadow->isChecked();
      aParams.IsReflectionEnabled = myReflection->isChecked();
      aParams.IsAntialiasingEnabled = myAntialiasing->isChecked();
      aParams.IsTransparentShadowEnabled = myTransparentShadow->isChecked();
      aParams.RaytracingDepth = myDepth->value();
      aView->Redraw();
    }
  }
}

/*!
  SLOT on "Ray tracing" group click
*/
void OCCViewer_RayTracingDlg::onRayTracing( bool theIsChecked )
{
  for ( int i = OCCViewer_ViewFrame::BOTTOM_RIGHT; i <= OCCViewer_ViewFrame::TOP_RIGHT; i++ ) {
    if ( OCCViewer_ViewWindow* aViewWindow = myViewFrame->getView(i) ) {
      Handle(V3d_View) aView = aViewWindow->getViewPort()->getView();
      Graphic3d_RenderingParams& aParams = aView->ChangeRenderingParams();
      theIsChecked ? aParams.Method = Graphic3d_RM_RAYTRACING : aParams.Method = Graphic3d_RM_RASTERIZATION;
      aView->Redraw();
    }
  }
}

/*!
  SLOT on help button click: opens a help page
*/
void OCCViewer_RayTracingDlg::ClickOnHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "occ_3d_viewer_page.html", "ray_tracing" );
}
