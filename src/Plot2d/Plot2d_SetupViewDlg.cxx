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

// File   : Plot2d_SetupViewDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "Plot2d_SetupViewDlg.h"

#ifndef NO_SUIT
#include <SUIT_Session.h>
#include <SUIT_Application.h>
#endif
#include <QtxColorButton.h>
#include <QtxFontEdit.h>

#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>

const int MARGIN_SIZE     = 11;
const int SPACING_SIZE    = 6;
const int MIN_EDIT_WIDTH  = 200;
const int MIN_COMBO_WIDTH = 100;
const int MIN_SPIN_WIDTH  = 70;

/*!
  \class Plot2d_SetupViewDlg
  \brief Dialog box to setup Plot2d view window.
*/

/*!
  \brief Constructor.
  \param parent parent widget
  \param showDefCheck if \c true, show "Set settings as default" check box
  \param secondAxisY if \c true, show widgets for the second (right) vertical axis
*/
Plot2d_SetupViewDlg::Plot2d_SetupViewDlg( QWidget* parent,
                                          bool showDefCheck,
                                          bool secondAxisY )
: QDialog( parent ),
  mySecondAxisY( secondAxisY )
{
  setModal( true );
  setWindowTitle( tr("TLT_SETUP_PLOT2D_VIEW") );
  setSizeGripEnabled( true );

  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->setMargin( MARGIN_SIZE );

  // main title
  myTitleCheck = new QCheckBox( tr( "PLOT2D_ENABLE_MAIN_TITLE" ), this );
  myTitleEdit  = new QLineEdit( this );
  myTitleEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myTitleEdit->setMinimumWidth( MIN_EDIT_WIDTH );

  // curve type : points, lines, spline
  QLabel* aCurveLab = new QLabel( tr( "PLOT2D_CURVE_TYPE_LBL" ), this );
  myCurveCombo      = new QComboBox( this );
  myCurveCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myCurveCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myCurveCombo->addItem( tr( "PLOT2D_CURVE_TYPE_POINTS" ) );
  myCurveCombo->addItem( tr( "PLOT2D_CURVE_TYPE_LINES" ) );
  myCurveCombo->addItem( tr( "PLOT2D_CURVE_TYPE_SPLINE" ) );

  // marker size
  QLabel* aMarkerLab  = new QLabel( tr( "PLOT2D_MARKER_SIZE_LBL" ), this );
  myMarkerSpin = new QSpinBox( this );
  myMarkerSpin->setMinimum( 0 );
  myMarkerSpin->setMaximum( 100 );
  myMarkerSpin->setSingleStep( 1 );
  myMarkerSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myMarkerSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  // background color
  QLabel* aBGLab  = new QLabel( tr( "PLOT2D_BACKGROUND_COLOR_LBL" ), this );
  myBackgroundBtn = new QtxColorButton( this );

  // selection color
  QLabel* aSelectionLab  = new QLabel( tr( "PLOT2D_SELECTION_COLOR_LBL" ), this );
  mySelectionBtn = new QtxColorButton( this );

  QHBoxLayout* ViewerColorLayout = new QHBoxLayout;
  ViewerColorLayout->addWidget( aBGLab );
  ViewerColorLayout->addWidget( myBackgroundBtn );
  ViewerColorLayout->addStretch();
  ViewerColorLayout->addWidget( aSelectionLab );
  ViewerColorLayout->addWidget( mySelectionBtn );
  ViewerColorLayout->addStretch();

  // legend
  QGroupBox* aLegendGrp = new QGroupBox( tr( "PLOT2D_LEGEND_GROUP" ), this );
  QGridLayout* aLegendLayout = new QGridLayout( aLegendGrp );
  aLegendLayout->setMargin( MARGIN_SIZE ); aLegendLayout->setSpacing( SPACING_SIZE );
  aLegendGrp->setLayout( aLegendLayout );

  myLegendCheck = new QCheckBox( tr( "PLOT2D_ENABLE_LEGEND" ), this );

  QLabel* aLegendPosition = new QLabel( tr( "PLOT2D_LEGEND_POSITION" ), this );
  myLegendCombo = new QComboBox( this );
  myLegendCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLegendCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myLegendCombo->addItem( tr( "PLOT2D_LEGEND_POSITION_LEFT" ) );
  myLegendCombo->addItem( tr( "PLOT2D_LEGEND_POSITION_RIGHT" ) );
  myLegendCombo->addItem( tr( "PLOT2D_LEGEND_POSITION_TOP" ) );
  myLegendCombo->addItem( tr( "PLOT2D_LEGEND_POSITION_BOTTOM" ) );

  QLabel* aLegendSymbolType = new QLabel( tr( "PLOT2D_LEGEND_SYMBOL_TYPE" ), this );
  myLegendSymbolType = new QComboBox( this );
  myLegendSymbolType->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLegendSymbolType->setMinimumWidth( MIN_COMBO_WIDTH );
  myLegendSymbolType->addItem( tr( "PLOT2D_MARKER_ON_LINE" ) );
  myLegendSymbolType->addItem( tr( "PLOT2D_MARKER_ABOVE_LINE" ) );

  QLabel* aLegendFontLab = new QLabel( tr( "PLOT2D_LEGEND_FONT" ), this );
  myLegendFont = new QtxFontEdit( this );

  QLabel* aLegendFontColor = new QLabel( tr( "PLOT2D_LEGEND_FONT_COLOR" ), this );
  myLegendFontColor = new QtxColorButton( this );

  QLabel* aSelectLegendFontColor = new QLabel( tr( "PLOT2D_SELECTED_LEGEND_FONT_COLOR" ), this );
  mySelectedLegendFontColor = new QtxColorButton( this );

  QHBoxLayout* FontLayout = new QHBoxLayout;
  FontLayout->addWidget( aLegendFontColor );
  FontLayout->addWidget( myLegendFontColor );
  FontLayout->addStretch();
  FontLayout->addWidget( aSelectLegendFontColor );
  FontLayout->addWidget( mySelectedLegendFontColor );
  FontLayout->addStretch();

  aLegendLayout->addWidget( myLegendCheck,      0, 0 );
  aLegendLayout->addWidget( aLegendPosition,    1, 0 );
  aLegendLayout->addWidget( myLegendCombo,      1, 1 );
  aLegendLayout->addWidget( aLegendSymbolType,  2, 0 );
  aLegendLayout->addWidget( myLegendSymbolType, 2, 1 );
  aLegendLayout->addWidget( aLegendFontLab,     3, 0 );
  aLegendLayout->addWidget( myLegendFont,       3, 1 );
  aLegendLayout->addLayout( FontLayout,         4, 0, 1, 2 );

  //Deviation marker parameters
  QGroupBox* aDeviationGrp = new QGroupBox( tr( "PLOT2D_DEVIATION_MARKER_TLT" ), this );
  QHBoxLayout* aDeviationLayout = new QHBoxLayout(aDeviationGrp);

  //Deviation marker parameters : Line width
  QLabel* aDeviationLwLbl  = new QLabel( tr( "PLOT2D_DEVIATION_LW_LBL" ), aDeviationGrp );
  myDeviationLw  = new QSpinBox( aDeviationGrp );
  myDeviationLw->setMinimum( 1 );
  myDeviationLw->setMaximum( 5 );
  myDeviationLw->setSingleStep( 1 );
  myDeviationLw->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  //Deviation marker parameters : Line width
  QLabel* aDeviationTsLbl  = new QLabel( tr( "PLOT2D_DEVIATION_TS_LBL" ), aDeviationGrp );
  myDeviationTs  =  new QSpinBox( aDeviationGrp );
  myDeviationTs->setMinimum( 1 );
  myDeviationTs->setMaximum( 5 );
  myDeviationTs->setSingleStep( 1 );
  myDeviationTs->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  //Deviation marker parameters : Color
  QLabel* aDeviationClLbl  = new QLabel( tr( "PLOT2D_DEVIATION_CL_LBL" ), aDeviationGrp );
  myDeviationCl = new QtxColorButton( aDeviationGrp );

  aDeviationLayout->addWidget( aDeviationLwLbl );
  aDeviationLayout->addWidget( myDeviationLw );
  aDeviationLayout->addWidget( aDeviationTsLbl );
  aDeviationLayout->addWidget( myDeviationTs );
  aDeviationLayout->addWidget( aDeviationClLbl );
  aDeviationLayout->addWidget( myDeviationCl );

  // normalize mode
  QGroupBox* aNormalizeGrp = new QGroupBox( tr( "PLOT2D_NORMALIZE_TLT" ), this );
  QGridLayout* aNormalizeLayout = new QGridLayout( aNormalizeGrp );
  aNormalizeLayout->setMargin( MARGIN_SIZE ); aNormalizeLayout->setSpacing( SPACING_SIZE );
  aNormalizeGrp->setLayout( aNormalizeLayout );
  QLabel* aYLeftLab  = new QLabel( tr( "PLOT2D_NORMALIZE_LEFT_AXIS" ), aNormalizeGrp );
  myNormLMinCheck = new QCheckBox( tr( "PLOT2D_NORMALIZE_MODE_MIN" ), aNormalizeGrp );
  myNormLMaxCheck = new QCheckBox( tr( "PLOT2D_NORMALIZE_MODE_MAX" ), aNormalizeGrp );
  QLabel* aYRightLab  = new QLabel( tr( "PLOT2D_NORMALIZE_RIGHT_AXIS" ), aNormalizeGrp );
  myNormRMinCheck = new QCheckBox( tr( "PLOT2D_NORMALIZE_MODE_MIN" ), aNormalizeGrp );
  myNormRMaxCheck = new QCheckBox( tr( "PLOT2D_NORMALIZE_MODE_MAX" ), aNormalizeGrp );

  aNormalizeLayout->addWidget( aYLeftLab,    0, 0 );
  aNormalizeLayout->addWidget( myNormLMaxCheck,    0, 1 );
  aNormalizeLayout->addWidget( myNormLMinCheck, 0, 2 );
  aNormalizeLayout->addWidget( aYRightLab,    1, 0 );
  aNormalizeLayout->addWidget( myNormRMaxCheck,    1, 1 );
  aNormalizeLayout->addWidget( myNormRMinCheck, 1, 2 );

  // scale mode
  QGroupBox* aScaleGrp = new QGroupBox( tr( "PLOT2D_SCALE_TLT" ), this );
  QGridLayout* aScaleLayout = new QGridLayout( aScaleGrp );
  aScaleLayout->setMargin( MARGIN_SIZE ); aScaleLayout->setSpacing( SPACING_SIZE );
  aScaleGrp->setLayout( aScaleLayout );

  QLabel* xScaleLab = new QLabel( tr( "PLOT2D_SCALE_MODE_HOR" ), aScaleGrp );
  myXModeCombo = new QComboBox( aScaleGrp );
  myXModeCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXModeCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myXModeCombo->addItem( tr( "PLOT2D_SCALE_MODE_LINEAR" ) );
  myXModeCombo->addItem( tr( "PLOT2D_SCALE_MODE_LOGARITHMIC" ) );

  QLabel* yScaleLab = new QLabel( tr( "PLOT2D_SCALE_MODE_VER" ), aScaleGrp );
  myYModeCombo = new QComboBox( aScaleGrp );
  myYModeCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myYModeCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myYModeCombo->addItem( tr( "PLOT2D_SCALE_MODE_LINEAR" ) );
  myYModeCombo->addItem( tr( "PLOT2D_SCALE_MODE_LOGARITHMIC" ) );

  aScaleLayout->addWidget( xScaleLab,    0, 0 );
  aScaleLayout->addWidget( myXModeCombo, 0, 1 );
  aScaleLayout->addWidget( yScaleLab,    0, 2 );
  aScaleLayout->addWidget( myYModeCombo, 0, 3 );

  // tab widget for choose properties of axis
  QTabWidget* aTabWidget = new QTabWidget( this );

  // widget for parameters on Ox
  QWidget* aXWidget = new QWidget(aTabWidget);
  QGridLayout* aXLayout = new QGridLayout( aXWidget );
  aXLayout->setSpacing( SPACING_SIZE );
  aXLayout->setMargin( MARGIN_SIZE );

  // axis title
  myTitleXCheck = new QCheckBox( tr( "PLOT2D_ENABLE_HOR_TITLE" ), aXWidget );
  myTitleXEdit  = new QLineEdit( aXWidget );
  myTitleXEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myTitleXEdit->setMinimumWidth( MIN_EDIT_WIDTH );
  aXLayout->addWidget( myTitleXCheck,1,    0    );
  aXLayout->addWidget( myTitleXEdit, 1, 1, 1, 3 );

  // grid
  QGroupBox* aGridGrpX = new QGroupBox( tr( "PLOT2D_GRID_TLT" ), aXWidget );
  QGridLayout* aGridLayoutX = new QGridLayout( aGridGrpX );
  aGridLayoutX->setMargin( MARGIN_SIZE );
  aGridLayoutX->setSpacing( SPACING_SIZE );

  myXGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_HOR_MAJOR" ), aGridGrpX );

  QLabel* aXMajLbl  = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpX);
  myXGridSpin       = new QSpinBox( aGridGrpX );
  myXGridSpin->setMinimum( 1 );
  myXGridSpin->setMaximum( 100 );
  myXGridSpin->setSingleStep( 1 );
  myXGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  myXMinGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_HOR_MINOR" ), aGridGrpX );

  QLabel* aXMinLbl     = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpX);
  myXMinGridSpin       = new QSpinBox( aGridGrpX );
  myXMinGridSpin->setMinimum( 1 );
  myXMinGridSpin->setMaximum( 100 );
  myXMinGridSpin->setSingleStep( 1 );
  myXMinGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXMinGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  aGridLayoutX->addWidget( myXGridCheck,    0, 0 );
  aGridLayoutX->addWidget( aXMajLbl,        0, 1 );
  aGridLayoutX->addWidget( myXGridSpin,     0, 2 );
  aGridLayoutX->addWidget( myXMinGridCheck, 1, 0 );
  aGridLayoutX->addWidget( aXMinLbl,        1, 1 );
  aGridLayoutX->addWidget( myXMinGridSpin,  1, 2 );
  aXLayout->addWidget( aGridGrpX, 3, 0, 1, 4 );

  aTabWidget->addTab( aXWidget, tr( "INF_AXES_X" ) );

  // widget for parameters on Oy
  QWidget* aYWidget = new QWidget(aTabWidget);
  QGridLayout* aYLayout = new QGridLayout( aYWidget );
  aYLayout->setSpacing( SPACING_SIZE );
  aYLayout->setMargin( MARGIN_SIZE );

  // axis title
  myTitleYCheck = new QCheckBox( tr( "PLOT2D_ENABLE_VER_TITLE" ), aYWidget );
  myTitleYEdit  = new QLineEdit( aYWidget );
  myTitleYEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myTitleYEdit->setMinimumWidth( MIN_EDIT_WIDTH );
  aYLayout->addWidget( myTitleYCheck,1,    0    );
  aYLayout->addWidget( myTitleYEdit, 1, 1, 1, 3 );

  // grid
  QGroupBox* aGridGrpY = new QGroupBox( tr( "PLOT2D_GRID_TLT" ), aYWidget );
  QGridLayout* aGridLayoutY = new QGridLayout( aGridGrpY );
  aGridGrpY->setLayout( aGridLayoutY );
  aGridLayoutY->setMargin( MARGIN_SIZE ); aGridLayoutY->setSpacing( SPACING_SIZE );

  myYGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MAJOR" ), aGridGrpY );

  QLabel* aYMajLbl  = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY);
  myYGridSpin       = new QSpinBox( aGridGrpY );
  myYGridSpin->setMinimum( 1 );
  myYGridSpin->setMaximum( 100 );
  myYGridSpin->setSingleStep( 1 );
  myYGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myYGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  myYMinGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MINOR" ), aGridGrpY );

  QLabel* aYMinLbl     = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY);
  myYMinGridSpin       = new QSpinBox( aGridGrpY );
  myYMinGridSpin->setMinimum( 1 );
  myYMinGridSpin->setMaximum( 100 );
  myYMinGridSpin->setSingleStep( 1 );
  myYMinGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myYMinGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  aGridLayoutY->addWidget( myYGridCheck,    0, 0 );
  aGridLayoutY->addWidget( aYMajLbl,       0, 1 );
  aGridLayoutY->addWidget( myYGridSpin,     0, 2 );
  aGridLayoutY->addWidget( myYMinGridCheck, 1, 0 );
  aGridLayoutY->addWidget( aYMinLbl,        1, 1 );
  aGridLayoutY->addWidget( myYMinGridSpin,  1, 2 );
  aYLayout->addWidget( aGridGrpY, 3, 0, 1, 4 );

  aTabWidget->addTab( aYWidget, tr( "INF_AXES_Y_LEFT" ) );

  // if exist second axis Oy, addition new tab widget for right axis
  if ( mySecondAxisY ) {
    // widget for parameters on Oy
    QWidget* aYWidget2 = new QWidget( aTabWidget );
    QGridLayout* aYLayout2 = new QGridLayout( aYWidget2 );
    aYLayout2->setSpacing( SPACING_SIZE );
    aYLayout2->setMargin( MARGIN_SIZE );

    // axis title
    myTitleY2Check = new QCheckBox( tr( "PLOT2D_ENABLE_VER_TITLE" ), aYWidget2 );
    myTitleY2Edit  = new QLineEdit( aYWidget2 );
    myTitleY2Edit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    myTitleY2Edit->setMinimumWidth( MIN_EDIT_WIDTH );
    aYLayout2->addWidget( myTitleY2Check,1,    0    );
    aYLayout2->addWidget( myTitleY2Edit, 1, 1, 1, 3 );

    // grid
    QGroupBox* aGridGrpY2 = new QGroupBox( tr( "PLOT2D_GRID_TLT" ), aYWidget2 );
    QGridLayout* aGridLayoutY2 = new QGridLayout( aGridGrpY2 );
    aGridGrpY2->setLayout( aGridLayoutY2 );
    aGridLayoutY2->setMargin( MARGIN_SIZE ); aGridLayoutY2->setSpacing( SPACING_SIZE );

    myY2GridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MAJOR" ), aGridGrpY2 );

    QLabel* aY2MajLbl  = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY2);
    myY2GridSpin       = new QSpinBox( aGridGrpY2 );
    myY2GridSpin->setMinimum( 1 );
    myY2GridSpin->setMaximum( 100 );
    myY2GridSpin->setSingleStep( 1 );
    myY2GridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    myY2GridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

    myY2MinGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MINOR" ), aGridGrpY2 );

    QLabel* aY2MinLbl     = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY2);
    myY2MinGridSpin       = new QSpinBox( aGridGrpY2 );
    myY2MinGridSpin->setMinimum( 1 );
    myY2MinGridSpin->setMaximum( 100 );
    myY2MinGridSpin->setSingleStep( 1 );
    myY2MinGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    myY2MinGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

    aGridLayoutY2->addWidget( myY2GridCheck,    0, 0 );
    aGridLayoutY2->addWidget( aY2MajLbl,        0, 1 );
    aGridLayoutY2->addWidget( myY2GridSpin,     0, 2 );
    aGridLayoutY2->addWidget( myY2MinGridCheck, 1, 0 );
    aGridLayoutY2->addWidget( aY2MinLbl,        1, 1 );
    aGridLayoutY2->addWidget( myY2MinGridSpin,  1, 2 );
    aYLayout2->addWidget( aGridGrpY2, 3, 0, 1, 4 );

    aTabWidget->addTab( aYWidget2, tr( "INF_AXES_Y_RIGHT" ) );
  }
  else {
    myTitleY2Check   = 0;
    myTitleY2Edit    = 0;
    myY2GridCheck    = 0;
    myY2GridSpin     = 0;
    myY2MinGridCheck = 0;
    myY2MinGridSpin  = 0;
    myY2ModeCombo    = 0;
  }

  aTabWidget->setCurrentIndex( 0 );
  /* "Set as default" check box */

  myDefCheck = new QCheckBox( tr( "PLOT2D_SET_AS_DEFAULT_CHECK" ), this );

  /* OK/Cancel buttons */
  myOkBtn = new QPushButton( tr( "BUT_OK" ), this );
  myOkBtn->setAutoDefault( true );
  myOkBtn->setDefault( true );
  myCancelBtn = new QPushButton( tr( "BUT_CANCEL" ), this );
  myCancelBtn->setAutoDefault( true );
  myHelpBtn = new QPushButton( tr( "BUT_HELP" ), this );
  myHelpBtn->setAutoDefault( true );
  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->addWidget( myOkBtn );
  btnLayout->addStretch();
  btnLayout->addWidget( myCancelBtn );
  btnLayout->addWidget( myHelpBtn );

  // layout widgets
  topLayout->addWidget( myTitleCheck,      0, 0       );
  topLayout->addWidget( myTitleEdit,       0, 1, 1, 3 );
  topLayout->addWidget( aCurveLab,         1, 0       );
  topLayout->addWidget( myCurveCombo,      1, 1       );
  topLayout->addWidget( aMarkerLab,        2, 0       );
  topLayout->addWidget( myMarkerSpin,      2, 1       );
  topLayout->addLayout( ViewerColorLayout, 3, 0, 1, 4 );
  topLayout->addWidget( aLegendGrp,        4, 0, 1, 4 );
  topLayout->addWidget( aDeviationGrp,     5, 0, 1, 4 );
  topLayout->addWidget( aNormalizeGrp,     6, 0, 1, 4 );
  topLayout->addWidget( aScaleGrp,         7, 0, 1, 4 );
  topLayout->addWidget( aTabWidget,        8, 0, 1, 4 );
  topLayout->addWidget( myDefCheck,        9, 0, 1, 4 );
  topLayout->setRowStretch( 10, 5 );
  topLayout->addLayout( btnLayout,         11, 0, 1, 4 );

  if ( !showDefCheck )
    myDefCheck->hide();

  connect( myTitleCheck,    SIGNAL( clicked() ), this, SLOT( onMainTitleChecked() ) );
  connect( myTitleXCheck,   SIGNAL( clicked() ), this, SLOT( onXTitleChecked() ) );
  connect( myTitleYCheck,   SIGNAL( clicked() ), this, SLOT( onYTitleChecked() ) );
  connect( myLegendCheck,   SIGNAL( clicked() ), this, SLOT( onLegendChecked() ) );
  connect( myXGridCheck,    SIGNAL( clicked() ), this, SLOT( onXGridMajorChecked() ) );
  connect( myYGridCheck,    SIGNAL( clicked() ), this, SLOT( onYGridMajorChecked() ) );
  connect( myXMinGridCheck, SIGNAL( clicked() ), this, SLOT( onXGridMinorChecked() ) );
  connect( myYMinGridCheck, SIGNAL( clicked() ), this, SLOT( onYGridMinorChecked() ) );
  connect( myNormLMaxCheck, SIGNAL( clicked() ), this, SLOT( onNormLMaxChecked() ) );
  connect( myNormLMinCheck, SIGNAL( clicked() ), this, SLOT( onNormLMinChecked() ) );
  connect( myNormRMaxCheck, SIGNAL( clicked() ), this, SLOT( onNormRMaxChecked() ) );
  connect( myNormRMinCheck, SIGNAL( clicked() ), this, SLOT( onNormRMinChecked() ) );


  connect( myOkBtn,         SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( myCancelBtn,     SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( myHelpBtn,       SIGNAL( clicked() ), this, SLOT( onHelp() ) );

  if ( mySecondAxisY ) {
    connect( myTitleY2Check,   SIGNAL( clicked() ), this, SLOT( onY2TitleChecked() ) );
    connect( myY2GridCheck,    SIGNAL( clicked() ), this, SLOT( onY2GridMajorChecked() ) );
    connect( myY2MinGridCheck, SIGNAL( clicked() ), this, SLOT( onY2GridMinorChecked() ) );
  }

  // init fields
  setBackgroundColor( Qt::gray );
  onMainTitleChecked();
  onXTitleChecked();
  onYTitleChecked();
  onLegendChecked();
  onXGridMajorChecked();
  onYGridMajorChecked();
  onXGridMinorChecked();
  onNormLMaxChecked();
  onNormLMinChecked();
  onNormRMaxChecked();
  onNormRMinChecked();
  if ( mySecondAxisY ) {
    onY2TitleChecked();
    onY2GridMajorChecked();
    onY2GridMinorChecked();
  }
}

/*!
  \brief Destructor.
*/
Plot2d_SetupViewDlg::~Plot2d_SetupViewDlg()
{
}

/*!
  \brief Set main title attributes.
  \param enable if \c true main title is enabled
  \param title main title
  \sa isMainTitleEnabled(), getMainTitle()
*/
void Plot2d_SetupViewDlg::setMainTitle( bool enable, const QString& title )
{
  myTitleCheck->setChecked( enable );
  if ( !title.isNull() )
    myTitleEdit->setText( title );
  onMainTitleChecked();
}

/*!
  \brief Check if main title is enabled.
  \return \c true if main title is enabled
  \sa setMainTitle()
*/
bool Plot2d_SetupViewDlg::isMainTitleEnabled()
{
  return myTitleCheck->isChecked();
}

/*!
  \brief Get main title.
  \return main title
  \sa setMainTitle()
*/
QString Plot2d_SetupViewDlg::getMainTitle()
{
  return myTitleEdit->text();
}

/*!
  \brief Set horizontal axis title attributes.
  \param enable if \c true horizontal axis title is enabled
  \param title horizontal axis title
  \sa isXTitleEnabled(), getXTitle()
*/
void Plot2d_SetupViewDlg::setXTitle( bool enable, const QString& title )
{
  myTitleXCheck->setChecked( enable );
  if ( !title.isNull() )
    myTitleXEdit->setText( title );
  onXTitleChecked();
}

/*!
  \brief Check if main title is enabled.
  \return \c true if horizontal axis title is enabled
  \sa setXTitle()
*/
bool Plot2d_SetupViewDlg::isXTitleEnabled()
{
  return myTitleXCheck->isChecked();
}

/*!
  \brief Get horizontal axis title.
  \return horizontal axis title
  \sa setXTitle()
*/
QString Plot2d_SetupViewDlg::getXTitle()
{
  return myTitleXEdit->text();
}

/*!
  \brief Set left vertical axis title attributes.
  \param enable if \c true left vertical axis title is enabled
  \param title left vertical axis title
  \sa setY2Title(), isYTitleEnabled(), getYTitle()
*/
void Plot2d_SetupViewDlg::setYTitle( bool enable, const QString& title )
{
  myTitleYCheck->setChecked( enable );
  if ( !title.isNull() )
    myTitleYEdit->setText( title );
  onYTitleChecked();
}

/*!
  \brief Set right vertical axis title attributes.
  \param enable if \c true right vertical axis title is enabled
  \param title right vertical axis title
  \sa setYTitle(), isY2TitleEnabled(), getY2Title()
*/
void Plot2d_SetupViewDlg::setY2Title( bool enable, const QString& title )
{
  myTitleY2Check->setChecked( enable );
  if ( !title.isNull() )
    myTitleY2Edit->setText( title );
  onY2TitleChecked();
}

/*!
  \brief Check if left vertical axis title is enabled.
  \return \c true if right vertical axis title is enabled
  \sa setYTitle()
*/
bool Plot2d_SetupViewDlg::isYTitleEnabled()
{
  return myTitleYCheck->isChecked();
}

/*!
  \brief Check if right vertical axis title is enabled.
  \return \c true if right vertical axis title is enabled
  \sa setY2Title()
*/
bool Plot2d_SetupViewDlg::isY2TitleEnabled()
{
  return myTitleY2Check->isChecked();
}

/*!
  \brief Get left vertical axis title.
  \return left vertical axis title
  \sa setYTitle()
*/
QString Plot2d_SetupViewDlg::getYTitle()
{
  return myTitleYEdit->text();
}

/*!
  \brief Get right vertical axis title.
  \return right vertical axis title
  \sa setY2Title()
*/
QString Plot2d_SetupViewDlg::getY2Title()
{
  return myTitleY2Edit->text();
}

/*!
  \brief Set curve type.
  \param type curve type: 0 (points), 1 (lines) or 2 (splines)
  \sa getCurveType()
*/
void Plot2d_SetupViewDlg::setCurveType( const int type )
{
  myCurveCombo->setCurrentIndex( type );
}

/*!
  \brief Get curve type.
  \return curve type: 0 (points), 1 (lines) or 2 (splines)
  \sa setCurveType()
*/
int Plot2d_SetupViewDlg::getCurveType()
{
  return myCurveCombo->currentIndex();
}

/*!
  \brief Set normalization to maximum by left Y axis.
  \param type normalizatoin type: true,false
  \sa getMaxNormMode()
*/
void Plot2d_SetupViewDlg::setLMaxNormMode( const bool type )
{
  myNormLMaxCheck->setChecked( type );
}

/*!
  \brief Check if normalization to maximum by left Y axis sets.
  \return curve normalizatoin type: true,false
  \sa setMaxNormMode()
*/
bool Plot2d_SetupViewDlg::getLMaxNormMode()
{
  return myNormLMaxCheck->isChecked();
}

/*!
  \brief Set normalization to minimum by left Y axis.
  \param type normalizatoin type: true,false
  \sa getMinNormMode()
*/
void Plot2d_SetupViewDlg::setLMinNormMode( const bool type )
{
  myNormLMinCheck->setChecked( type );
}

/*!
  \brief Check if normalization to minimum by left Y axis sets.
  \return curve normalizatoin type: true,false
  \sa setMinNormMode()
*/
bool Plot2d_SetupViewDlg::getLMinNormMode()
{
  return myNormLMinCheck->isChecked();
}

/*!
  \brief Set normalization to maximum by right Y axis.
  \param type normalizatoin type: true,false
  \sa getMaxNormMode()
*/
void Plot2d_SetupViewDlg::setRMaxNormMode( const bool type )
{
  myNormRMaxCheck->setChecked( type );
}

/*!
  \brief Check if normalization to maximum by right Y axis sets.
  \return curve normalizatoin type: true,false
  \sa setMaxNormMode()
*/
bool Plot2d_SetupViewDlg::getRMaxNormMode()
{
  return myNormRMaxCheck->isChecked();
}

/*!
  \brief Set normalization to minimum by right Y axis.
  \param type normalizatoin type: true,false
  \sa getMinNormMode()
*/
void Plot2d_SetupViewDlg::setRMinNormMode( const bool type )
{
  myNormRMinCheck->setChecked( type );
}

/*!
  \brief Check if normalization to minimum by right Y axis sets.
  \return curve normalizatoin type: true,false
  \sa setMinNormMode()
*/
bool Plot2d_SetupViewDlg::getRMinNormMode()
{
  return myNormRMinCheck->isChecked();
}

/*!
  \brief Set legend attribute.
  \param if \c true legend is shown
  \param pos legend position: 0 (left), 1 (right), 2 (top), 3 (bottom)
  \param fnt legend font
  \param col legend font color
  \sa isLegendEnabled(), getLegendPos(), getLegendFont()
*/
void Plot2d_SetupViewDlg::setLegend( bool enable, int pos, int symbolType,
                                     const QFont& fnt, const QColor& fontColor, const QColor& selFontColor )
{
  myLegendCheck->setChecked( enable );
  myLegendCombo->setCurrentIndex( pos );
  myLegendSymbolType->setCurrentIndex( symbolType );
  myLegendFont->setCurrentFont( fnt );
  myLegendFontColor->setColor( fontColor );
  mySelectedLegendFontColor->setColor( selFontColor );
  onLegendChecked();
}

/*!
  \brief Check if legend is enabled.
  \return \c true if legend is enabled
  \sa setLegend()
*/
bool Plot2d_SetupViewDlg::isLegendEnabled()
{
  return myLegendCheck->isChecked();
}

/*!
  \brief Get legend position.
  \return legend position: 0 (left), 1 (right), 2 (top), 3 (bottom)
  \sa setLegend()
*/
int Plot2d_SetupViewDlg::getLegendPos()
{
  return myLegendCombo->currentIndex();
}

/*!
  \brief Get legend symbol type.
  \return legend symbol type: 0 (marker on line), 1 (marker above line)
  \sa setLegend()
*/
int Plot2d_SetupViewDlg::getLegendSymbolType()
{
  return myLegendSymbolType->currentIndex();
}

/*!
  \brief Get legend font.
  \return legend font
  \sa setLegend()
*/
QFont Plot2d_SetupViewDlg::getLegendFont()
{
  return myLegendFont->currentFont();
}

/*!
  \brief Get legend font color.
  \return legend font color
  \sa setLegend()
*/
QColor Plot2d_SetupViewDlg::getLegendColor()
{
  return myLegendFontColor->color();
}

/*!
  \brief Get selected legend font color.
  \return selected legend font color
  \sa setLegend()
*/
QColor Plot2d_SetupViewDlg::getSelectedLegendColor()
{
  return mySelectedLegendFontColor->color();
}

/*!
  \brief Set marker size.
  \param size marker size
  \sa getMarkerSize()
*/
void Plot2d_SetupViewDlg::setMarkerSize( const int size )
{
  myMarkerSpin->setValue( size );
}

/*!
  \brief Get marker size.
  \return marker size
  \sa setMarkerSize()
*/
int Plot2d_SetupViewDlg::getMarkerSize()
{
  return myMarkerSpin->value();
}
/*!
  \brief Set deviation marker line width.
  \param width marker line width
  \sa getDeviationMarkerLw()
*/
void Plot2d_SetupViewDlg::setDeviationMarkerLw( const int width ){
  myDeviationLw->setValue(width);
}

/*!
  \brief Get deviation marker line width.
  \return marker line width
  \sa setMarkerSize()
*/
int Plot2d_SetupViewDlg::getDeviationMarkerLw() const {
  return myDeviationLw->value();
}

/*!
  \brief Set deviation marker tick size.
  \param size marker tick size
  \sa getDeviationMarkerTs()
*/
void Plot2d_SetupViewDlg::setDeviationMarkerTs( const int size) {
  myDeviationTs->setValue(size);
}

/*!
  \brief Get deviation marker tick size.
  \return marker tick size
  \sa setDeviationMarkerTs()
*/
int Plot2d_SetupViewDlg::getDeviationMarkerTs() const {
  return myDeviationTs->value();
}

/*!
  \brief Set color of the deviation marker.
  \param color marker color
  \sa getDeviationMarkerCl()
*/
void Plot2d_SetupViewDlg::setDeviationMarkerCl( const QColor& col) {
  myDeviationCl->setColor( col );
}

/*!
  \brief Get color of the deviation marker.
  \return marker color
  \sa setDeviationMarkerCl()
*/
QColor Plot2d_SetupViewDlg::getDeviationMarkerCl() const {
 return myDeviationCl->color();
}

/*!
  \brief Set background color.
  \param color background color
  \sa getBackgroundColor()
*/
void Plot2d_SetupViewDlg::setBackgroundColor( const QColor& color )
{
  myBackgroundBtn->setColor( color );
}

/*!
  \brief Get background color.
  \return background color
  \sa setBackgroundColor()
*/
QColor Plot2d_SetupViewDlg::getBackgroundColor()
{
  return myBackgroundBtn->color();
}

/*!
  \brief Set selection color.
  \param color selection color
  \sa getSelectionColor()
*/
void Plot2d_SetupViewDlg::setSelectionColor( const QColor& color )
{
  mySelectionBtn->setColor( color );
}

/*!
  \brief Get selection color.
  \return selection color
  \sa setSelectionColor()
*/
QColor Plot2d_SetupViewDlg::getSelectionColor()
{
  return mySelectionBtn->color();
}

/*!
  \brief Set major grid parameters.
  \param enableX if \c true, horizontal major grid is enabled
  \param divX maximum number of ticks for horizontal major grid
  \param enableY if \c true, left vertical major grid is enabled
  \param divY maximum number of ticks for left vertical major grid
  \param enableY2 if \c true, right vertical major grid is enabled
  \param divY2 maximum number of ticks for right vertical major grid
  \sa getMajorGrid()
*/
void Plot2d_SetupViewDlg::setMajorGrid( bool enableX, const int divX,
                                        bool enableY, const int divY,
                                        bool enableY2, const int divY2 )
{
  myXGridCheck->setChecked( enableX );
  myXGridSpin->setValue( divX );
  myYGridCheck->setChecked( enableY );
  myYGridSpin->setValue( divY );
  onXGridMajorChecked();
  onYGridMajorChecked();
  if (mySecondAxisY) {
    myY2GridCheck->setChecked( enableY2 );
    myY2GridSpin->setValue( divY2 );
    onY2GridMajorChecked();
  }
}

/*!
  \brief Get major grid parameters.
  \param enableX \c true if horizontal major grid is enabled
  \param divX maximum number of ticks for horizontal major grid
  \param enableY \c true if left vertical major grid is enabled
  \param divY maximum number of ticks for left vertical major grid
  \param enableY2 \c true if right vertical major grid is enabled
  \param divY2 maximum number of ticks for right vertical major grid
  \sa setMajorGrid()
*/
void Plot2d_SetupViewDlg::getMajorGrid( bool& enableX, int& divX,
                                        bool& enableY, int& divY,
                                        bool& enableY2, int& divY2 )
{
  enableX  = myXGridCheck->isChecked();
  divX     = myXGridSpin->value();
  enableY  = myYGridCheck->isChecked();
  divY     = myYGridSpin->value();
  if (mySecondAxisY) {
    enableY2 = myY2GridCheck->isChecked();
    divY2    = myY2GridSpin->value();
  }
  else {
    enableY2 = false;
    divY2    = 1;
  }
}

/*!
  \brief Set minor grid parameters.
  \param enableX if \c true, horizontal minor grid is enabled
  \param divX maximum number of ticks for horizontal minor grid
  \param enableY if \c true, left vertical minor grid is enabled
  \param divY maximum number of ticks for left vertical minor grid
  \param enableY2 if \c true, right vertical minor grid is enabled
  \param divY2 maximum number of ticks for right vertical minor grid
  \sa getMinorGrid()
*/
void Plot2d_SetupViewDlg::setMinorGrid( bool enableX, const int divX,
                                        bool enableY, const int divY,
                                        bool enableY2, const int divY2 )
{
  myXMinGridCheck->setChecked( enableX );
  myXMinGridSpin->setValue( divX );
  myYMinGridCheck->setChecked( enableY );
  myYMinGridSpin->setValue( divY );
  onXGridMinorChecked();
  onYGridMinorChecked();
  if (mySecondAxisY) {
    myY2MinGridCheck->setChecked( enableY2 );
    myY2MinGridSpin->setValue( divY2 );
    onY2GridMinorChecked();
  }
}

/*!
  \brief Get minor grid parameters.
  \param enableX \c true if horizontal minor grid is enabled
  \param divX maximum number of ticks for horizontal minor grid
  \param enableY \c true if left vertical minor grid is enabled
  \param divY maximum number of ticks for left vertical minor grid
  \param enableY2 \c true if right vertical minor grid is enabled
  \param divY2 maximum number of ticks for right vertical minor grid
  \sa setMinorGrid()
*/
void Plot2d_SetupViewDlg::getMinorGrid( bool& enableX, int& divX,
                                        bool& enableY, int& divY,
                                        bool& enableY2, int& divY2 )
{
  enableX  = myXMinGridCheck->isChecked();
  divX     = myXMinGridSpin->value();
  enableY  = myYMinGridCheck->isChecked();
  divY     = myYMinGridSpin->value();
  if ( mySecondAxisY ) {
    enableY2 = myY2MinGridCheck->isChecked();
    divY2    = myY2MinGridSpin->value();
  }
  else {
    enableY2 = false;
    divY2    = 1;
  }
}

/*!
  \brief Set scale mode for horizontal and vertical axes.
  \param xMode horizontal axis scale mode: 0 (linear), 1 (logarithmic)
  \param yMode vertical axis scale mode: 0 (linear), 1 (logarithmic)
  \sa getXScaleMode(), getYScaleMode()
*/
void Plot2d_SetupViewDlg::setScaleMode( const int xMode, const int yMode )
{
  myXModeCombo->setCurrentIndex( xMode );
  myYModeCombo->setCurrentIndex( yMode );
}

/*!
  \brief Get scale mode for horizontal axis.
  \return horizontal axis scale mode: 0 (linear), 1 (logarithmic)
  \sa setScaleMode()
*/
int Plot2d_SetupViewDlg::getXScaleMode()
{
  return myXModeCombo->currentIndex();
}

/*!
  \brief Get scale mode for vertical axis.
  \return vertical axis scale mode: 0 (linear), 1 (logarithmic)
  \sa setScaleMode()
*/
int  Plot2d_SetupViewDlg::getYScaleMode()
{
  return myYModeCombo->currentIndex();
}

/*!
  \brief Called when user clicks "Show main title" check box.
*/
void Plot2d_SetupViewDlg::onMainTitleChecked()
{
  myTitleEdit->setEnabled( myTitleCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Show horizontal axis title" check box.
*/
void Plot2d_SetupViewDlg::onXTitleChecked()
{
  myTitleXEdit->setEnabled( myTitleXCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Show vertical left axis title" check box.
*/
void Plot2d_SetupViewDlg::onYTitleChecked()
{
  myTitleYEdit->setEnabled( myTitleYCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Show vertical right axis title" check box.
*/
void Plot2d_SetupViewDlg::onY2TitleChecked()
{
  myTitleY2Edit->setEnabled( myTitleY2Check->isChecked() );
}

/*!
  \brief Called when user clicks "Show Legend" check box.
*/
void Plot2d_SetupViewDlg::onLegendChecked()
{
  myLegendCombo->setEnabled( myLegendCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Enable horizontal major grid" check box.
*/
void Plot2d_SetupViewDlg::onXGridMajorChecked()
{
  myXMinGridCheck->setEnabled( myXGridCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Enable left vertical major grid" check box.
*/
void Plot2d_SetupViewDlg::onYGridMajorChecked()
{
  myYMinGridCheck->setEnabled( myYGridCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Enable right vertical major grid" check box.
*/
void Plot2d_SetupViewDlg::onY2GridMajorChecked()
{
  myY2MinGridCheck->setEnabled( myY2GridCheck->isChecked() );
}

/*!
  \brief Called when user clicks "Enable horizontal minor grid" check box.
*/
void Plot2d_SetupViewDlg::onXGridMinorChecked()
{
}

/*!
  \brief Called when user clicks "Enable left vertical minor grid" check box.
*/
void Plot2d_SetupViewDlg::onYGridMinorChecked()
{
}

/*!
  \brief Called when user clicks "Enable right vertical minor grid" check box.
*/
void Plot2d_SetupViewDlg::onY2GridMinorChecked()
{
}

/*!
  \brief Called when user clicks "Left Y Axis: Normalize to maximum" check box.
*/
void Plot2d_SetupViewDlg::onNormLMaxChecked()
{
}

/*!
  \brief Called when user clicks "Left Y Axis: Normalize to minimum" check box.
*/
void Plot2d_SetupViewDlg::onNormLMinChecked()
{
}

/*!
  \brief Called when user clicks "Right Y Axis: Normalize to maximum" check box.
*/
void Plot2d_SetupViewDlg::onNormRMaxChecked()
{
}

/*!
  \brief Called when user clicks "Right Y Axis: Normalize to minimum" check box.
*/
void Plot2d_SetupViewDlg::onNormRMinChecked()
{
}

/*!
  \brief Get "Set settings as default" check box value.
  \return \c true if "Set settings as default" check box is on
*/
bool Plot2d_SetupViewDlg::isSetAsDefault()
{
  return myDefCheck->isChecked();
}

/*!
  Slot, called when user clicks "Help" button
*/
void Plot2d_SetupViewDlg::onHelp()
{
#ifndef NO_SUIT
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "plot2d_viewer_page.html", "settings" );
#endif
}
