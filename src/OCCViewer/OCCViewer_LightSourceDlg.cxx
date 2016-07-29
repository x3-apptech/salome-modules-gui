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
#include "OCCViewer_LightSourceDlg.h"
#include "OCCViewer.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewModel.h"

// GUI includes
#include <SUIT_Session.h>
#include <QtxDoubleSpinBox.h>
#include <QtxColorButton.h>

// Qt includes
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QStackedLayout>
#include <QRadioButton>

/*!
  \class OCCViewer_LightSourceDlg
  \brief Dialog allowing to assign parameters of light source
*/

/*!
  \brief Constructor
  \param view - parent widget
  \param model - viewer
*/
OCCViewer_LightSourceDlg::OCCViewer_LightSourceDlg( OCCViewer_ViewWindow* view, OCCViewer_Viewer* model )
  :QDialog( view ),
  myModel( model )
{
  setObjectName( "OCCViewer_LightSourceDlg" );
  setWindowTitle( tr( "LIGHT_SOURCE" ) );
  setModal( false );

  setAttribute( Qt::WA_DeleteOnClose, true );

  // Create layout for this dialog
  QVBoxLayout* dlglayout = new QVBoxLayout( this );
  dlglayout->setSpacing( 6 ); dlglayout->setMargin( 11 );

  QGroupBox* typeGroup = new QGroupBox( tr( "TYPE" ), this );
  QHBoxLayout* typeLayout = new QHBoxLayout( typeGroup );
  typeLayout->setSpacing( 6 ); typeLayout->setMargin( 11 );

  myDirType = new QRadioButton( tr( "DIRECTIONAL" ), typeGroup );
  myPosType = new QRadioButton( tr( "POSITIONAL" ), typeGroup );

  typeLayout->addWidget( myDirType );
  typeLayout->addWidget( myPosType );

  myStackedLayout = new QStackedLayout();

  const double min = -RealLast();
  const double max = RealLast();
  const int precision = 3;

  /**********************   Directional light   **********************/
  /* Controls for directional light:
     Dx, Dy, Dz - direction
     Headlight - headlight flag
     Color - the color of a light source
  */

  QWidget* dirWidget = new QWidget( this );
  QVBoxLayout* dirLayout = new QVBoxLayout( dirWidget );
  dirLayout->setSpacing( 6 ); dirLayout->setMargin( 11 );

  // Create "Direction" group

  const double dir_step = 0.1;

  QGroupBox* dirCoordGroup = new QGroupBox( tr( "DIRECTION" ), this );
  QHBoxLayout* dirCoordLayout = new QHBoxLayout( dirCoordGroup );
  dirCoordLayout->setSpacing( 6 ); dirCoordLayout->setMargin( 11 );

  QLabel* dxLabel = new QLabel( tr("Dx:"), dirCoordGroup );
  myDx = new QtxDoubleSpinBox( min, max, dir_step, dirCoordGroup );
  myDx->setValue( 0.0 );
  myDx->setMinimumWidth( 80 );

  QLabel* dyLabel = new QLabel( tr("Dy:"), dirCoordGroup );
  myDy = new QtxDoubleSpinBox( min, max, dir_step, dirCoordGroup );
  myDy->setValue( 0.0 );
  myDy->setMinimumWidth( 80 );

  QLabel* dzLabel = new QLabel( tr("Dz:"), dirCoordGroup );
  myDz = new QtxDoubleSpinBox( min, max, dir_step, dirCoordGroup );
  myDz->setValue( -1.0 );
  myDz->setMinimumWidth( 80 );

  dirCoordLayout->addWidget( dxLabel );
  dirCoordLayout->addWidget( myDx );
  dirCoordLayout->addWidget( dyLabel );
  dirCoordLayout->addWidget( myDy );
  dirCoordLayout->addWidget( dzLabel );
  dirCoordLayout->addWidget( myDz );

  // Create "Parameters" group

  QGroupBox* dirParamGroup = new QGroupBox( dirWidget );
  QHBoxLayout* dirParamLayout = new QHBoxLayout( dirParamGroup );
  dirParamLayout->setSpacing( 6 ); dirParamLayout->setMargin( 11 );

  myDirHeadLight = new QCheckBox( tr("HEADLIGHT"), dirParamGroup );
  myDirHeadLight->setChecked( false );

  QLabel* aColorLabel = new QLabel( tr( "COLOR" ), dirParamGroup );
  myDirColor = new QtxColorButton( dirParamGroup );
  myDirColor->setColor( Qt::white );

  dirParamLayout->addWidget( myDirHeadLight );
  dirParamLayout->addWidget( aColorLabel );
  dirParamLayout->addWidget( myDirColor );
  dirParamLayout->addStretch();

  dirLayout->addWidget( dirCoordGroup );
  dirLayout->addWidget( dirParamGroup );

  /**********************   Positional light   **********************/
  /* Controls for positional light:
     X, Y, Z - position
     Headlight - headlight flag
     Color - the color of a light source
  */

  QWidget* posWidget = new QWidget( this );
  QVBoxLayout* posLayout = new QVBoxLayout( posWidget );
  posLayout->setSpacing( 6 ); posLayout->setMargin( 11 );

  // Create "Position" group

  const double pos_step = 1.0;

  QGroupBox* posCoordGroup = new QGroupBox( tr( "POSITION" ), posWidget );
  QHBoxLayout* posCoordLayout = new QHBoxLayout( posCoordGroup );
  posCoordLayout->setSpacing( 6 ); posCoordLayout->setMargin( 11 );

  QLabel* xLabel = new QLabel( tr("X:"), posCoordGroup );
  myX = new QtxDoubleSpinBox( min, max, pos_step, posCoordGroup );
  myX->setValue( 0.0 );
  myX->setMinimumWidth( 80 );

  QLabel* yLabel = new QLabel( tr("Y:"), posCoordGroup );
  myY = new QtxDoubleSpinBox( min, max, pos_step, posCoordGroup );
  myY->setValue( 0.0 );
  myY->setMinimumWidth( 80 );

  QLabel* zLabel = new QLabel( tr("Z:"), posCoordGroup );
  myZ = new QtxDoubleSpinBox( min, max, pos_step, posCoordGroup );
  myZ->setValue( 0.0 );
  myZ->setMinimumWidth( 80 );

  posCoordLayout->addWidget( xLabel );
  posCoordLayout->addWidget( myX );
  posCoordLayout->addWidget( yLabel );
  posCoordLayout->addWidget( myY );
  posCoordLayout->addWidget( zLabel );
  posCoordLayout->addWidget( myZ );

  // Create "Parameters" group

  QGroupBox* posParamGroup = new QGroupBox( posWidget );
  QHBoxLayout* posParamLayout = new QHBoxLayout( posParamGroup );
  posParamLayout->setSpacing( 6 ); posParamLayout->setMargin( 11 );

  myPosHeadLight = new QCheckBox( tr("HEADLIGHT"), posParamGroup );
  myPosHeadLight->setChecked( false );

  aColorLabel = new QLabel( tr( "COLOR" ), posParamGroup );
  myPosColor = new QtxColorButton( posParamGroup );
  myPosColor->setColor( Qt::white );

  posParamLayout->addWidget( myPosHeadLight );
  posParamLayout->addWidget( aColorLabel );
  posParamLayout->addWidget( myPosColor );
  posParamLayout->addStretch();

  posLayout->addWidget( posCoordGroup );
  posLayout->addWidget( posParamGroup );

  // add widgets in the stacked layout
  myStackedLayout->addWidget( dirWidget );
  myStackedLayout->addWidget( posWidget );

  // Create "Buttons" group

  QGroupBox* buttonGroup = new QGroupBox( this );
  QHBoxLayout* buttonLayout = new QHBoxLayout( buttonGroup );
  buttonLayout->setSpacing( 6 );
  buttonLayout->setMargin( 11 );

  QPushButton* okButton = new QPushButton( tr( "BUT_APPLY_AND_CLOSE" ), buttonGroup );
  okButton->setDefault( true );
  QPushButton* defaultButton = new QPushButton( tr( "BUT_DEFAULT" ), buttonGroup );
  QPushButton* closeButton = new QPushButton( tr( "BUT_CLOSE" ), buttonGroup );
  QPushButton* helpButton = new QPushButton( tr( "GEOM_BUT_HELP" ), buttonGroup );

  buttonLayout->addWidget( okButton );
  buttonLayout->addWidget( defaultButton );
  buttonLayout->addStretch();
  buttonLayout->addWidget( closeButton );
  buttonLayout->addWidget( helpButton );

  dlglayout->addWidget( typeGroup );
  dlglayout->addLayout( myStackedLayout );
  dlglayout->addWidget( buttonGroup );

  this->setLayout( dlglayout );

  // Initializations
  initParam();
  isBusy = false;

  // Signals and slots connections
  connect( myDirType,      SIGNAL( clicked( bool ) ),        this, SLOT( onTypeChanged() ) );
  connect( myPosType,      SIGNAL( clicked( bool ) ),        this, SLOT( onTypeChanged() ) );

  connect( myDx,           SIGNAL( valueChanged( double ) ), this, SLOT( onDirChanged() ) );
  connect( myDy,           SIGNAL( valueChanged( double ) ), this, SLOT( onDirChanged() ) );
  connect( myDz,           SIGNAL( valueChanged( double ) ), this, SLOT( onDirChanged() ) );
  connect( myDirHeadLight, SIGNAL( clicked( bool ) ),        this, SLOT( onDirChanged() ) );
  connect( myDirColor,     SIGNAL( changed( QColor ) ),      this, SLOT( onDirChanged() ) );

  connect( myX,            SIGNAL( valueChanged( double ) ), this, SLOT( onPosChanged() ) );
  connect( myY,            SIGNAL( valueChanged( double ) ), this, SLOT( onPosChanged() ) );
  connect( myZ,            SIGNAL( valueChanged( double ) ), this, SLOT( onPosChanged() ) );
  connect( myPosHeadLight, SIGNAL( clicked( bool ) ),        this, SLOT( onPosChanged() ) );
  connect( myPosColor,     SIGNAL( changed( QColor ) ),      this, SLOT( onPosChanged() ) );

  connect( okButton,       SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( defaultButton,  SIGNAL( clicked() ), this, SLOT( ClickOnDefault() ) );
  connect( closeButton,    SIGNAL( clicked() ), this, SLOT( ClickOnClose() ) );
  connect( helpButton,     SIGNAL( clicked() ), this, SLOT( ClickOnHelp() ) );

  resize( minimumSizeHint() );
}

/*!
  \brief Destructor
*/
OCCViewer_LightSourceDlg::~OCCViewer_LightSourceDlg()
{
  ( myInType == V3d_DIRECTIONAL ) ? myModel->getViewer3d()->DelLight( myPosLight ) :
		                            myModel->getViewer3d()->DelLight( myDirLight );
}

QString OCCViewer_LightSourceDlg::getName()
{
  // return the name of object
  return QString( "OCCViewer_LightSourceDlg" );
}

/*!
  Initialization of initial values of widgets
*/
void OCCViewer_LightSourceDlg::initParam( bool theIsDefault )
{
  Handle(V3d_Light) aLight;
  myModel->getViewer3d()->InitDefinedLights();
  while ( myModel->getViewer3d()->MoreDefinedLights() )
  {
    aLight = myModel->getViewer3d()->DefinedLight();
    if ( aLight->Type() == V3d_DIRECTIONAL ) {
      myDirLight = Handle(V3d_DirectionalLight)::DownCast( aLight );
      myPosLight = new V3d_PositionalLight( myModel->getViewer3d(), 0, 0, 0 );
      myDirType->setChecked( true );
      myStackedLayout->setCurrentIndex(0);
      break;
    }
    if ( aLight->Type() == V3d_POSITIONAL ) {
      myPosLight = Handle(V3d_PositionalLight)::DownCast( aLight );
      myDirLight = new V3d_DirectionalLight( myModel->getViewer3d() );
      myPosType->setChecked( true );
      myStackedLayout->setCurrentIndex(1);
      break;
    }
    myModel->getViewer3d()->NextDefinedLights();
  }

  double aX, aY, aZ;
  Quantity_Color aColor = aLight->Color();
  if( myDirType->isChecked() ) {
    myDirColor->setColor( OCCViewer::color( aColor ) );
    myDirLight->Direction( aX, aY, aZ );
    myDx->setValue( aX );
    myDy->setValue( aY );
    myDz->setValue( aZ );
    myDirHeadLight->setChecked( myDirLight->Headlight() );
  }
  else if( myPosType->isChecked() ) {
    myPosColor->setColor( OCCViewer::color( aColor ) );
    myPosLight->Position( aX, aY, aZ );
    myX->setValue( aX );
    myY->setValue( aY );
    myZ->setValue( aZ );
    myPosHeadLight->setChecked( myPosLight->Headlight() );
  }

  if ( !theIsDefault ) {
    myInX = aX; myInY = aY; myInZ = aZ;
    myInColor = aColor;
    myInHeadLight = aLight->Headlight();
    myInType = aLight->Type();
  }
}

/*!
  SLOT: called on type of light source changed
*/
void OCCViewer_LightSourceDlg::onTypeChanged()
{
  if( isBusy )
    return;
  myStackedLayout->setCurrentIndex( myPosType->isChecked() );
  if( myPosType->isChecked() ) {
    myModel->getViewer3d()->SetLightOff( myDirLight );
    onPosChanged();
  }
  else if( myDirType->isChecked() ) {
    myModel->getViewer3d()->SetLightOff( myPosLight );
    onDirChanged();
  }
}

/*!
  SLOT: called on value of directional light source changed
*/
void OCCViewer_LightSourceDlg::onDirChanged()
{
  if( isBusy )
    return;
  myModel->getViewer3d()->SetLightOff( myDirLight );
  if ( !( myDx->value() == 0 && myDy->value() == 0 && myDz->value() == 0 ) ) {
    myDirLight->SetDirection( myDx->value(), myDy->value(), myDz->value() );
    myDirLight->SetColor( OCCViewer::color( myDirColor->color() ) );
    myDirLight->SetHeadlight( myDirHeadLight->isChecked() );
    myModel->getViewer3d()->SetLightOn( myDirLight );
  }
  myModel->getViewer3d()->UpdateLights();
}

/*!
  SLOT: called on value of positional light source changed
*/
void OCCViewer_LightSourceDlg::onPosChanged()
{
  if( isBusy )
    return;
  myModel->getViewer3d()->SetLightOff( myPosLight );
  myPosLight->SetPosition( myX->value(), myY->value(), myZ->value() );
  myPosLight->SetColor( OCCViewer::color( myPosColor->color() ) );
  myPosLight->SetHeadlight( myPosHeadLight->isChecked() );
  myModel->getViewer3d()->SetLightOn( myPosLight );
  myModel->getViewer3d()->UpdateLights();
}

/*!
  \brief SLOT on "Apply and Close" button click: sets current light source and closes dialog
*/
void OCCViewer_LightSourceDlg::ClickOnOk()
{
  // need save a current type for deleting other light when dialog will be closed
  myInType = myDirType->isChecked() ? V3d_DIRECTIONAL : V3d_POSITIONAL;
  close();
}

/*!
  \brief SLOT on "Default" button click: sets default light source
*/
void OCCViewer_LightSourceDlg::ClickOnDefault()
{
  isBusy = true;
  myModel->setDefaultLights();
  initParam( true );
  myModel->getViewer3d()->UpdateLights();
  isBusy = false;
}

/*!
  \brief SLOT on "Close" button click: sets initial light source and closes dialog
*/
void OCCViewer_LightSourceDlg::ClickOnClose()
{
  if( myInType == V3d_DIRECTIONAL ) {
    myDirLight->SetDirection( myInX, myInY, myInZ );
    myDirLight->SetColor( myInColor );
    myDirLight->SetHeadlight( myInHeadLight );
    myModel->getViewer3d()->SetLightOn( myDirLight );
  }
  else {
    myPosLight->SetPosition( myInX, myInY, myInZ );
    myPosLight->SetColor( myInColor );
    myPosLight->SetHeadlight( myInHeadLight );
    myModel->getViewer3d()->SetLightOn( myPosLight );
  }
  close();
}

/*!
  \brief SLOT on help button click: opens a help page
*/
void OCCViewer_LightSourceDlg::ClickOnHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "occ_3d_viewer_page.html", "light_source" );
}
