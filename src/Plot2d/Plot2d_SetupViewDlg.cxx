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
//  File   : Plot2d_SetupViewDlg.cxx
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#include "Plot2d_SetupViewDlg.h"

#include "SUIT_Session.h"
#include "SUIT_Application.h"

#include <qcheckbox.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qtabwidget.h>

#define MARGIN_SIZE          11
#define SPACING_SIZE         6
#define MIN_EDIT_WIDTH       200
#define MIN_COMBO_WIDTH      100
#define MIN_SPIN_WIDTH       70

/*!
  Constructor
*/
Plot2d_SetupViewDlg::Plot2d_SetupViewDlg( QWidget* parent, bool showDefCheck, bool secondAxisY )
    : QDialog( parent, "Plot2d_SetupViewDlg", true, 
         WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  mySecondAxisY = secondAxisY;
  setCaption( tr("TLT_SETUP_PLOT2D_VIEW") );
  setSizeGripEnabled( TRUE );
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
  myCurveCombo      = new QComboBox( false, this );
  myCurveCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myCurveCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myCurveCombo->insertItem( tr( "PLOT2D_CURVE_TYPE_POINTS" ) );
  myCurveCombo->insertItem( tr( "PLOT2D_CURVE_TYPE_LINES" ) );
  myCurveCombo->insertItem( tr( "PLOT2D_CURVE_TYPE_SPLINE" ) );
  // legend
  myLegendCheck = new QCheckBox( tr( "PLOT2D_ENABLE_LEGEND" ), this );
  myLegendCombo = new QComboBox( false, this );
  myLegendCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLegendCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myLegendCombo->insertItem( tr( "PLOT2D_LEGEND_POSITION_LEFT" ) );
  myLegendCombo->insertItem( tr( "PLOT2D_LEGEND_POSITION_RIGHT" ) );
  myLegendCombo->insertItem( tr( "PLOT2D_LEGEND_POSITION_TOP" ) );
  myLegendCombo->insertItem( tr( "PLOT2D_LEGEND_POSITION_BOTTOM" ) );
  // marker size
  QLabel* aMarkerLab  = new QLabel( tr( "PLOT2D_MARKER_SIZE_LBL" ), this );
  myMarkerSpin = new QSpinBox( 0, 100, 1, this );
  myMarkerSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myMarkerSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  // background color
  QLabel* aBGLab  = new QLabel( tr( "PLOT2D_BACKGROUND_COLOR_LBL" ), this );
  myBackgroundBtn = new QToolButton( this );
  myBackgroundBtn->setMinimumWidth(20);

  // scale mode
  QGroupBox* aScaleGrp = new QGroupBox( tr( "PLOT2D_SCALE_TLT" ), this );
  aScaleGrp->setColumnLayout(0, Qt::Vertical );
  aScaleGrp->layout()->setSpacing( 0 );  aScaleGrp->layout()->setMargin( 0 );
  QGridLayout* aScaleLayout = new QGridLayout( aScaleGrp->layout() );
  aScaleLayout->setMargin( MARGIN_SIZE ); aScaleLayout->setSpacing( SPACING_SIZE );

  QLabel* xScaleLab = new QLabel( tr( "PLOT2D_SCALE_MODE_HOR" ), aScaleGrp );
  myXModeCombo = new QComboBox( false, aScaleGrp );
  myXModeCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXModeCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myXModeCombo->insertItem( tr( "PLOT2D_SCALE_MODE_LINEAR" ) );
  myXModeCombo->insertItem( tr( "PLOT2D_SCALE_MODE_LOGARITHMIC" ) );
  QLabel* yScaleLab = new QLabel( tr( "PLOT2D_SCALE_MODE_VER" ), aScaleGrp );
  myYModeCombo = new QComboBox( false, aScaleGrp );
  myYModeCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myYModeCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  myYModeCombo->insertItem( tr( "PLOT2D_SCALE_MODE_LINEAR" ) );
  myYModeCombo->insertItem( tr( "PLOT2D_SCALE_MODE_LOGARITHMIC" ) );

  aScaleLayout->addWidget( xScaleLab,    0, 0 );
  aScaleLayout->addWidget( myXModeCombo, 0, 1 );
  aScaleLayout->addWidget( yScaleLab,    0, 2 );
  aScaleLayout->addWidget( myYModeCombo, 0, 3 );

  // tab widget for choose properties of axis 
  QTabWidget* aTabWidget = new QTabWidget( this, "tabWidget" );

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
  aXLayout->addWidget( myTitleXCheck,         1,    0    );
  aXLayout->addMultiCellWidget( myTitleXEdit, 1, 1, 1, 3 );
  // grid
  QGroupBox* aGridGrpX = new QGroupBox( tr( "PLOT2D_GRID_TLT" ), aXWidget );
  aGridGrpX->setColumnLayout(0, Qt::Vertical );
  aGridGrpX->layout()->setSpacing( 0 );  aGridGrpX->layout()->setMargin( 0 );
  QGridLayout* aGridLayoutX = new QGridLayout( aGridGrpX->layout() );
  aGridLayoutX->setMargin( MARGIN_SIZE ); aGridLayoutX->setSpacing( SPACING_SIZE );
  myXGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_HOR_MAJOR" ), aGridGrpX );
  QLabel* aXMajLbl  = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpX);
  myXGridSpin       = new QSpinBox( 1, 100, 1, aGridGrpX );
  myXGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );
  myXMinGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_HOR_MINOR" ), aGridGrpX );
  QLabel* aXMinLbl     = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpX);
  myXMinGridSpin       = new QSpinBox( 1, 100, 1, aGridGrpX );
  myXMinGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myXMinGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  aGridLayoutX->addWidget( myXGridCheck,    0, 0 );
  aGridLayoutX->addWidget( aXMajLbl,        0, 1 );
  aGridLayoutX->addWidget( myXGridSpin,     0, 2 );
  aGridLayoutX->addWidget( myXMinGridCheck, 1, 0 );
  aGridLayoutX->addWidget( aXMinLbl,        1, 1 );
  aGridLayoutX->addWidget( myXMinGridSpin,  1, 2 );
  aXLayout->addMultiCellWidget( aGridGrpX, 3, 3, 0, 3 );

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
  aYLayout->addWidget( myTitleYCheck,         1,    0    );
  aYLayout->addMultiCellWidget( myTitleYEdit, 1, 1, 1, 3 );
  // grid
  QGroupBox* aGridGrpY = new QGroupBox( tr( "PLOT2D_GRID_TLT" ), aYWidget );
  aGridGrpY->setColumnLayout(0, Qt::Vertical );
  aGridGrpY->layout()->setSpacing( 0 );  aGridGrpY->layout()->setMargin( 0 );
  QGridLayout* aGridLayoutY = new QGridLayout( aGridGrpY->layout() );
  aGridLayoutY->setMargin( MARGIN_SIZE ); aGridLayoutY->setSpacing( SPACING_SIZE );
  myYGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MAJOR" ), aGridGrpY );
  QLabel* aYMajLbl  = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY);
  myYGridSpin       = new QSpinBox( 1, 100, 1, aGridGrpY );
  myYGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myYGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );
  myYMinGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MINOR" ), aGridGrpY );
  QLabel* aYMinLbl     = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY);
  myYMinGridSpin       = new QSpinBox( 1, 100, 1, aGridGrpY );
  myYMinGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myYMinGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  aGridLayoutY->addWidget( myYGridCheck,    0, 0 );
  aGridLayoutY->addWidget( aYMajLbl,        0, 1 );
  aGridLayoutY->addWidget( myYGridSpin,     0, 2 );
  aGridLayoutY->addWidget( myYMinGridCheck, 1, 0 );
  aGridLayoutY->addWidget( aYMinLbl,        1, 1 );
  aGridLayoutY->addWidget( myYMinGridSpin,  1, 2 );
  aYLayout->addMultiCellWidget( aGridGrpY, 3, 3, 0, 3 );

  aTabWidget->addTab( aYWidget, tr( "INF_AXES_Y_LEFT" ) );

  // if exist second axis Oy, addition new tab widget for right axis
  if (mySecondAxisY) {
    // widget for parameters on Oy
    QWidget* aYWidget2 = new QWidget(aTabWidget);
    QGridLayout* aYLayout2 = new QGridLayout( aYWidget2 );
    aYLayout2->setSpacing( SPACING_SIZE );
    aYLayout2->setMargin( MARGIN_SIZE );
    // axis title
    myTitleY2Check = new QCheckBox( tr( "PLOT2D_ENABLE_VER_TITLE" ), aYWidget2 );
    myTitleY2Edit  = new QLineEdit( aYWidget2 );
    myTitleY2Edit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    myTitleY2Edit->setMinimumWidth( MIN_EDIT_WIDTH );
    aYLayout2->addWidget( myTitleY2Check,         1,    0    );
    aYLayout2->addMultiCellWidget( myTitleY2Edit, 1, 1, 1, 3 );
    // grid
    QGroupBox* aGridGrpY2 = new QGroupBox( tr( "PLOT2D_GRID_TLT" ), aYWidget2 );
    aGridGrpY2->setColumnLayout(0, Qt::Vertical );
    aGridGrpY2->layout()->setSpacing( 0 );  aGridGrpY2->layout()->setMargin( 0 );
    QGridLayout* aGridLayoutY2 = new QGridLayout( aGridGrpY2->layout() );
    aGridLayoutY2->setMargin( MARGIN_SIZE ); aGridLayoutY2->setSpacing( SPACING_SIZE );
    myY2GridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MAJOR" ), aGridGrpY2 );
    QLabel* aY2MajLbl  = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY2);
    myY2GridSpin       = new QSpinBox( 1, 100, 1, aGridGrpY2 );
    myY2GridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    myY2GridSpin->setMinimumWidth( MIN_SPIN_WIDTH );
    myY2MinGridCheck      = new QCheckBox( tr( "PLOT2D_GRID_ENABLE_VER_MINOR" ), aGridGrpY2 );
    QLabel* aY2MinLbl     = new QLabel( tr( "PLOT2D_MAX_INTERVALS" ), aGridGrpY2);
    myY2MinGridSpin       = new QSpinBox( 1, 100, 1, aGridGrpY2 );
    myY2MinGridSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
    myY2MinGridSpin->setMinimumWidth( MIN_SPIN_WIDTH );

    aGridLayoutY2->addWidget( myY2GridCheck,    0, 0 );
    aGridLayoutY2->addWidget( aY2MajLbl,        0, 1 );
    aGridLayoutY2->addWidget( myY2GridSpin,     0, 2 );
    aGridLayoutY2->addWidget( myY2MinGridCheck, 1, 0 );
    aGridLayoutY2->addWidget( aY2MinLbl,        1, 1 );
    aGridLayoutY2->addWidget( myY2MinGridSpin,  1, 2 );
    aYLayout2->addMultiCellWidget( aGridGrpY2, 3, 3, 0, 3 );

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
  aTabWidget->setCurrentPage( 0 );
  /* "Set as default" check box */
  myDefCheck = new QCheckBox( tr( "PLOT2D_SET_AS_DEFAULT_CHECK" ), this );

  /* OK/Cancel/Help buttons */
  myOkBtn = new QPushButton( tr( "BUT_OK" ), this );
  myOkBtn->setAutoDefault( TRUE );
  myOkBtn->setDefault( TRUE );
  myCancelBtn = new QPushButton( tr( "BUT_CANCEL" ), this );
  myCancelBtn->setAutoDefault( TRUE );
  myHelpBtn = new QPushButton( tr( "BUT_HELP" ), this );
  myHelpBtn->setAutoDefault( TRUE );
  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->addWidget( myOkBtn );
  btnLayout->addStretch();
  btnLayout->addWidget( myCancelBtn );
  btnLayout->addWidget( myHelpBtn );
  
  // layout widgets
  topLayout->addWidget( myTitleCheck,          0,    0    );
  topLayout->addMultiCellWidget( myTitleEdit,  0, 0, 1, 3 );
  topLayout->addWidget( aCurveLab,             1,    0    );
  topLayout->addWidget( myCurveCombo,          1,    1    );
  topLayout->addWidget( myLegendCheck,         1,    2    );
  topLayout->addWidget( myLegendCombo,         1,    3    );
  topLayout->addWidget( aMarkerLab,            2,    0    );
  topLayout->addWidget( myMarkerSpin,          2,    1    );
  QHBoxLayout* bgLayout = new QHBoxLayout;
  bgLayout->addWidget( myBackgroundBtn ); bgLayout->addStretch();
  topLayout->addWidget( aBGLab,                2,    2    );
  topLayout->addLayout( bgLayout,              2,    3    );
  topLayout->addMultiCellWidget( aScaleGrp,    3, 3, 0, 3 );
  topLayout->addMultiCellWidget( aTabWidget,   4, 4, 0, 3 );
  topLayout->addMultiCellWidget( myDefCheck,   5, 5, 0, 3 );
  topLayout->setRowStretch( 5, 5 );

  topLayout->addMultiCellLayout( btnLayout,    6, 6, 0, 3 );
  
  if ( !showDefCheck )
    myDefCheck->hide();

  connect( myTitleCheck,    SIGNAL( clicked() ), this, SLOT( onMainTitleChecked() ) );
  connect( myTitleXCheck,   SIGNAL( clicked() ), this, SLOT( onXTitleChecked() ) );
  connect( myTitleYCheck,   SIGNAL( clicked() ), this, SLOT( onYTitleChecked() ) );
  connect( myBackgroundBtn, SIGNAL( clicked() ), this, SLOT( onBackgroundClicked() ) );
  connect( myLegendCheck,   SIGNAL( clicked() ), this, SLOT( onLegendChecked() ) );
  connect( myXGridCheck,    SIGNAL( clicked() ), this, SLOT( onXGridMajorChecked() ) );
  connect( myYGridCheck,    SIGNAL( clicked() ), this, SLOT( onYGridMajorChecked() ) );
  connect( myXMinGridCheck, SIGNAL( clicked() ), this, SLOT( onXGridMinorChecked() ) );
  connect( myYMinGridCheck, SIGNAL( clicked() ), this, SLOT( onYGridMinorChecked() ) );

  connect( myOkBtn,         SIGNAL( clicked() ), this, SLOT( accept() ) );
  connect( myCancelBtn,     SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( myHelpBtn,       SIGNAL( clicked() ), this, SLOT( onHelp() ) );
  
  if (mySecondAxisY) {
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
  if (mySecondAxisY) {
    onY2TitleChecked();
    onY2GridMajorChecked();
    onY2GridMinorChecked();
  }
}

/*!
  Destructor
*/
Plot2d_SetupViewDlg::~Plot2d_SetupViewDlg()
{
}
/*!
  Sets main title attributes
*/
void Plot2d_SetupViewDlg::setMainTitle( bool enable, const QString& title )
{
  myTitleCheck->setChecked( enable );
  if ( !title.isNull() )
    myTitleEdit->setText( title );
  onMainTitleChecked();
}
/*!
  Returns TRUE if main title is enabled
*/
bool Plot2d_SetupViewDlg::isMainTitleEnabled()
{
  return myTitleCheck->isChecked();
}
/*!
  Gets main title
*/
QString Plot2d_SetupViewDlg::getMainTitle()
{
  return myTitleEdit->text();
}
/*!
  Sets horizontal axis title attributes
*/
void Plot2d_SetupViewDlg::setXTitle( bool enable, const QString& title )
{
  myTitleXCheck->setChecked( enable );
  if ( !title.isNull() )
    myTitleXEdit->setText( title );
  onXTitleChecked();
}
/*!
  Returns TRUE if horizontal axis title is enabled
*/
bool Plot2d_SetupViewDlg::isXTitleEnabled()
{
  return myTitleXCheck->isChecked();
}
/*!
  Gets horizontal axis title
*/
QString Plot2d_SetupViewDlg::getXTitle()
{
  return myTitleXEdit->text();
}
/*!
  Sets vertical left axis title attributes
*/
void Plot2d_SetupViewDlg::setYTitle( bool enable, const QString& title )
{
  myTitleYCheck->setChecked( enable );
  if ( !title.isNull() )
    myTitleYEdit->setText( title );
  onYTitleChecked();
}
/*!
  Sets vertical right axis title attributes
*/
void Plot2d_SetupViewDlg::setY2Title( bool enable, const QString& title )
{
  myTitleY2Check->setChecked( enable );
  if ( !title.isNull() )
    myTitleY2Edit->setText( title );
  onY2TitleChecked();
}
/*!
  Returns TRUE if vertical left axis title is enabled
*/
bool Plot2d_SetupViewDlg::isYTitleEnabled()
{
  return myTitleYCheck->isChecked();
}
/*!
  Returns TRUE if vertical right axis title is enabled
*/
bool Plot2d_SetupViewDlg::isY2TitleEnabled()
{
  return myTitleY2Check->isChecked();
}
/*!
  Gets vertical left axis title
*/
QString Plot2d_SetupViewDlg::getYTitle()
{
  return myTitleYEdit->text();
}
/*!
  Gets vertical right axis title
*/
QString Plot2d_SetupViewDlg::getY2Title()
{
  return myTitleY2Edit->text();
}
/*!
  Sets curve type : 0 - points, 1 - lines, 2 - splines
*/
void Plot2d_SetupViewDlg::setCurveType( const int type )
{
  myCurveCombo->setCurrentItem( type );
}
/*!
  Gets curve type : 0 - points, 1 - lines, 2 - splines
*/
int Plot2d_SetupViewDlg::getCurveType()
{
  return myCurveCombo->currentItem();
}
/*!
  Sets legend attributes : pos = 0 - left, 1 - right, 2 - top, 3 - bottom
*/
void Plot2d_SetupViewDlg::setLegend( bool enable, int pos )
{
  myLegendCheck->setChecked( enable );
  myLegendCombo->setCurrentItem( pos );
  onLegendChecked();
}
/*!
  Returns TRUE if legend is enabled
*/
bool Plot2d_SetupViewDlg::isLegendEnabled()
{
  return myLegendCheck->isChecked();
}
/*!
  Returns legend position
*/
int Plot2d_SetupViewDlg::getLegendPos()
{
  return myLegendCombo->currentItem();
}
/*!
  Sets marker size
*/
void Plot2d_SetupViewDlg::setMarkerSize( const int size )
{
  myMarkerSpin->setValue( size );
}
/*!
  Gets marker size
*/
int Plot2d_SetupViewDlg::getMarkerSize()
{
  return myMarkerSpin->value();
}
/*!
  Sets background color
*/
void Plot2d_SetupViewDlg::setBackgroundColor( const QColor& color )
{
  QPalette pal = myBackgroundBtn->palette();
  QColorGroup ca = pal.active();
  ca.setColor( QColorGroup::Button, color );
  QColorGroup ci = pal.inactive();
  ci.setColor( QColorGroup::Button, color );
  pal.setActive( ca );
  pal.setInactive( ci );
  myBackgroundBtn->setPalette( pal );
}
/*!
  Gets background color
*/
QColor Plot2d_SetupViewDlg::getBackgroundColor()
{
  return myBackgroundBtn->palette().active().button();
}
/*!
  Sets major grid parameters
*/
void Plot2d_SetupViewDlg::setMajorGrid( bool enableX, const int divX,
                                        bool enableY, const int divY,
                                        bool enableY2, const int divY2  )
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
  Gets major grid parameters
*/
void Plot2d_SetupViewDlg::getMajorGrid( bool& enableX, int& divX,
                                        bool& enableY, int& divY,
                                        bool& enableY2, int& divY2)
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
  Sets minor grid parameters
*/
void Plot2d_SetupViewDlg::setMinorGrid( bool enableX, const int divX,
                                        bool enableY, const int divY,
                                        bool enableY2, const int divY2)
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
  Gets minor grid parameters
*/
void Plot2d_SetupViewDlg::getMinorGrid( bool& enableX, int& divX,
                                        bool& enableY, int& divY,
                                        bool& enableY2, int& divY2)
{
  enableX  = myXMinGridCheck->isChecked();
  divX     = myXMinGridSpin->value();
  enableY  = myYMinGridCheck->isChecked();
  divY     = myYMinGridSpin->value();
  if (mySecondAxisY) {
    enableY2 = myY2MinGridCheck->isChecked();
    divY2    = myY2MinGridSpin->value();
  }
  else {
    enableY2 = false;
    divY2    = 1;
  }
}
/*!
  Sets scale mode for hor. and ver. axes : 0 - linear, 1 - logarithmic
*/
void Plot2d_SetupViewDlg::setScaleMode( const int xMode, const int yMode )
{
  myXModeCombo->setCurrentItem( xMode );
  myYModeCombo->setCurrentItem( yMode );
}
/*!
  Gets scale mode for hor. axis : 0 - linear, 1 - logarithmic
*/
int  Plot2d_SetupViewDlg::getXScaleMode()
{
  return myXModeCombo->currentItem();
}
/*!
  Gets scale mode for hor. axis : 0 - linear, 1 - logarithmic
*/
int  Plot2d_SetupViewDlg::getYScaleMode()
{
  return myYModeCombo->currentItem();
}
/*!
  Slot, called when user clicks "Show main title" check box
*/
void Plot2d_SetupViewDlg::onMainTitleChecked()
{
  myTitleEdit->setEnabled( myTitleCheck->isChecked() );
}
/*!
  Slot, called when user clicks "Show horizontal axis title" check box
*/
void Plot2d_SetupViewDlg::onXTitleChecked()
{
  myTitleXEdit->setEnabled( myTitleXCheck->isChecked() );
}
/*!
  Slot, called when user clicks "Show vertical left axis title" check box
*/
void Plot2d_SetupViewDlg::onYTitleChecked()
{
  myTitleYEdit->setEnabled( myTitleYCheck->isChecked() );
}
/*!
  Slot, called when user clicks "Show vertical right axis title" check box
*/
void Plot2d_SetupViewDlg::onY2TitleChecked()
{
  myTitleY2Edit->setEnabled( myTitleY2Check->isChecked() );
}
/*!
  Slot, called when user clicks "Change bacground color" button
*/
void Plot2d_SetupViewDlg::onBackgroundClicked()
{
  QColor color = QColorDialog::getColor( getBackgroundColor() );
  if ( color.isValid() ) {
    setBackgroundColor( color );
  }
}
/*!
  Slot, called when user clicks "Show Legend" check box
*/
void Plot2d_SetupViewDlg::onLegendChecked()
{
  myLegendCombo->setEnabled( myLegendCheck->isChecked() );
}
/*!
  Slot, called when user clicks "Enable hor. major grid" check box
*/
void Plot2d_SetupViewDlg::onXGridMajorChecked()
{
  myXMinGridCheck->setEnabled( myXGridCheck->isChecked() );
}
/*!
  Slot, called when user clicks  "Enable ver. major grid" check box
*/
void Plot2d_SetupViewDlg::onYGridMajorChecked()
{
  myYMinGridCheck->setEnabled( myYGridCheck->isChecked() );
}
/*!
  Slot, called when user clicks  "Enable ver. major grid" check box
*/
void Plot2d_SetupViewDlg::onY2GridMajorChecked()
{
  myY2MinGridCheck->setEnabled( myY2GridCheck->isChecked() );
}
/*!
  Slot, called when user clicks  "Enable hor. minor grid" check box
*/
void Plot2d_SetupViewDlg::onXGridMinorChecked()
{
}
/*!
  Slot, called when user clicks  "Enable ver. minor grid" check box
*/
void Plot2d_SetupViewDlg::onYGridMinorChecked()
{
}
/*!
  Slot, called when user clicks  "Enable ver. minor grid" check box
*/
void Plot2d_SetupViewDlg::onY2GridMinorChecked()
{
}
/*!
  Retursns true if "Set as default" check box is on
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
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if (app)
    app->onHelpContextModule("GUI", "plot2d_viewer_page.html#settings");
}
