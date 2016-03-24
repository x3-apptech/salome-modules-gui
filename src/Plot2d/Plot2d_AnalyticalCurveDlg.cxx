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

// File   : Plot2d_AnalyticalCurveDlg.cxx
// Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)



// TODO : check what happens if invalid formula is given, e.g. x/0
// TODO : check what happens if curve formala is valid in general but some there are some problems with calculating (e.g. logarithmic formulas and negative x)

//Local includes
#include "Plot2d_AnalyticalCurveDlg.h"
#include "Plot2d_AnalyticalCurve.h"
#include "Plot2d_ViewFrame.h"

//Qtx includes
#include <QtxIntSpinBox.h>
#include <QtxColorButton.h>

//SUIT includes
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_Application.h>

//Qt includes
#include <QListWidget>
#include <QGroupBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QPushButton>

//qwt includes
#include <qwt_plot.h>

// Controls
const int MIN_NB_INTERVALS =     1;
const int MAX_NB_INTERVALS = 10000;
const int STEP             =     1;
const int MIN_LINE_WIDTH   =     0;
const int MAX_LINE_WIDTH   =    10;

const char* PROP_TITLE       = "title";
const char* PROP_VISIBLE     = "visible";
const char* PROP_STATUS      = "status";
const char* PROP_FORMULA     = "formula";
const char* PROP_NBINTERVALS = "nb_intervals";
const char* PROP_AUTOASSIGN  = "auto_assign";
const char* PROP_MARKERTYPE  = "marker_type";
const char* PROP_LINETYPE    = "line_type";
const char* PROP_LINEWIDTH   = "line_width";
const char* PROP_COLOR       = "color";

/*
  \class Plot2d_AnalyticalCurveDlg::UpdateLocker
  \brief Update locker class
  \internal
*/
class Plot2d_AnalyticalCurveDlg::UpdateLocker
{
public:
  UpdateLocker( QObject* );
  ~UpdateLocker();
private:
  QObject* myObject;
  bool     myLocked;
};

Plot2d_AnalyticalCurveDlg::UpdateLocker::UpdateLocker( QObject* object ) : myObject( object )
{
  myLocked = myObject->blockSignals( true );
}

Plot2d_AnalyticalCurveDlg::UpdateLocker::~UpdateLocker()
{
  myObject->blockSignals( myLocked );
}

/*
  \class Plot2d_AnalyticalCurveDlg::Updater
  \brief Auxiliary class used for handle update requests from sub-widgets
  \internal
*/

Plot2d_AnalyticalCurveDlg::Updater::Updater( QWidget* parent ) : QObject( parent )
{/*nothing to do*/}

Plot2d_AnalyticalCurveDlg::Updater::~Updater()
{/*nothing to do*/}

/*!
  Constructor 
*/
Plot2d_AnalyticalCurveDlg::Plot2d_AnalyticalCurveDlg( Plot2d_CurveContainer* container, QWidget* parent )
  : QDialog( parent ), myContainer( container )
{
  setModal( true );
  setWindowTitle( tr( "ANALYTICAL_CURVE_TLT" ) );
  setSizeGripEnabled( true );

  // Curves list widget
  myCurvesList = new QListWidget( this );
  myCurvesList->setSelectionMode( QAbstractItemView::SingleSelection );

  // Curve parameters group box
  myCurveParams =  new QGroupBox( tr( "AC_CURVE_PARAMS" ), this );

  QLabel* formulaLabel     = new QLabel( tr( "AC_FORMULA" ), myCurveParams );
  myFormula                = new QLineEdit( myCurveParams );
  QLabel* nbIntervalsLabel = new QLabel( tr( "AC_NB_INTERVALS" ), myCurveParams );
  myNbIntervals            = new QtxIntSpinBox( MIN_NB_INTERVALS, MAX_NB_INTERVALS, STEP, myCurveParams );

  QGridLayout* paramsLayout = new QGridLayout( myCurveParams );
  paramsLayout->addWidget( formulaLabel,     0, 0 );
  paramsLayout->addWidget( myFormula,        0, 1 );
  paramsLayout->addWidget( nbIntervalsLabel, 1, 0 );
  paramsLayout->addWidget( myNbIntervals,    1, 1 );

  // Curve properties group box
  myCurveProps = new QGroupBox( tr( "AC_CURVE_PROPS" ), this );

  myAutoAssign           = new QCheckBox( tr( "AC_AUTO_ASSIGN" ), myCurveProps );
  myPropsGrp             = new QWidget( myCurveProps );
  QLabel* markerLabel    = new QLabel( tr( "AC_MARKER_TYPE" ), myPropsGrp );  
  myMarkerType           = new QComboBox( myPropsGrp );
  QLabel* lineTypeLabel  = new QLabel( tr( "AC_LINE_TYPE" ), myPropsGrp );  
  myLineType             = new QComboBox( myPropsGrp );
  QLabel* lineWidthLabel = new QLabel( tr( "AC_LINE_WIDTH" ), myPropsGrp );  
  myLineWidth            = new QtxIntSpinBox( MIN_LINE_WIDTH, MAX_LINE_WIDTH, STEP, myPropsGrp );
  QLabel* colorLabel     = new QLabel( tr("AC_CURVE_COLOR"), myPropsGrp );
  myColor                = new QtxColorButton( myPropsGrp );

  QGridLayout* propsGrpLayout = new QGridLayout( myPropsGrp );
  propsGrpLayout->addWidget( markerLabel,    0, 0 );
  propsGrpLayout->addWidget( myMarkerType,   0, 1 );
  propsGrpLayout->addWidget( lineTypeLabel,  1, 0 );
  propsGrpLayout->addWidget( myLineType,     1, 1 );
  propsGrpLayout->addWidget( lineWidthLabel, 2, 0 );
  propsGrpLayout->addWidget( myLineWidth,    2, 1 );
  propsGrpLayout->addWidget( colorLabel,     3, 0 );
  propsGrpLayout->addWidget( myColor,        3, 1 );

  QVBoxLayout* propsLayout = new QVBoxLayout( myCurveProps );
  propsLayout->addWidget( myAutoAssign );
  propsLayout->addWidget( myPropsGrp );

  // Add && Remove buttons
  myAddButton    = new QPushButton( tr("AC_ADD_BTN"), this );
  myAddButton->setAutoDefault( true );
  myRemoveButton = new QPushButton( tr("AC_REM_BTN"), this );
  myRemoveButton->setAutoDefault( true );

  // OK, Apply, Close, Help buttons
  QPushButton* okButton     = new QPushButton( tr( "AC_OK_BTN" ),     this );
  okButton->setDefault( true );
  okButton->setAutoDefault( true );

  QPushButton* applyButton     = new QPushButton( tr( "AC_APPLY_BTN" ),     this );
  applyButton->setAutoDefault( true );

  QPushButton* cancelButton = new QPushButton( tr( "AC_CLOSE_BTN" ), this );
  cancelButton->setAutoDefault( true );
  QPushButton* helpButton   = new QPushButton( tr( "AC_HELP_BTN" ),   this );
  helpButton->setAutoDefault( true );
  QHBoxLayout* btnLayout    = new QHBoxLayout;
  btnLayout->addWidget( okButton );
  btnLayout->addWidget( applyButton );
  btnLayout->addStretch();
  btnLayout->addWidget( cancelButton );
  btnLayout->addWidget( helpButton );

  QGridLayout* mainLayout = new QGridLayout( this );
  mainLayout->addWidget( myCurvesList,   0, 0, 3, 1 );
  mainLayout->addWidget( myCurveParams,  0, 1, 1, 2 );
  mainLayout->addWidget( myCurveProps,   1, 1, 1, 2 );
  mainLayout->addWidget( myAddButton,    2, 1 );
  mainLayout->addWidget( myRemoveButton, 2, 2 );
  mainLayout->addLayout( btnLayout,      3, 0, 1, 3 );

  // Fill combo boxes
  QColor cl = myMarkerType->palette().color( QPalette::Text ); // color to draw markers
  QSize  sz = QSize( 16, 16 );                                 // size of the icons for markers
  myMarkerType->setIconSize( sz );

  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::None ),      tr( "NONE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Circle ),    tr( "CIRCLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Rectangle ), tr( "RECTANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Diamond ),   tr( "DIAMOND_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::DTriangle ), tr( "DTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::UTriangle ), tr( "UTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::LTriangle ), tr( "LTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::RTriangle ), tr( "RTRIANGLE_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Cross ),     tr( "CROSS_MARKER_LBL" ) );
  myMarkerType->addItem( Plot2d::markerIcon( sz, cl, Plot2d::XCross ),    tr( "XCROSS_MARKER_LBL" ) );

  cl = myLineType->palette().color( QPalette::Text ); // color to draw line types
  sz = QSize( 40, 16 );                               // size of the icons for line types
  myLineType->setIconSize( sz );

  myLineType->addItem( Plot2d::lineIcon( sz, cl, Plot2d::NoPen ),      tr( "NONE_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( sz, cl, Plot2d::Solid ),      tr( "SOLID_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( sz, cl, Plot2d::Dash ),       tr( "DASH_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( sz, cl, Plot2d::Dot ),        tr( "DOT_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( sz, cl, Plot2d::DashDot ),    tr( "DASHDOT_LINE_LBL" ) );
  myLineType->addItem( Plot2d::lineIcon( sz, cl, Plot2d::DashDotDot ), tr( "DAHSDOTDOT_LINE_LBL" ) );

  // Connections
  myUpdater = new Updater( this );
  connect( myUpdater,      SIGNAL( update()  ),                       this, SLOT( updateCurve() ) );
  connect( myFormula,      SIGNAL( textChanged( QString ) ),          myUpdater, SIGNAL( update() ) );
  connect( myNbIntervals,  SIGNAL( valueChanged( int ) ),             myUpdater, SIGNAL( update() ) );
  connect( myAutoAssign,   SIGNAL( stateChanged( int ) ),             myUpdater, SIGNAL( update() ) );
  connect( myMarkerType,   SIGNAL( activated( int ) ),                myUpdater, SIGNAL( update() ) );
  connect( myLineType,     SIGNAL( activated( int ) ),                myUpdater, SIGNAL( update() ) );
  connect( myLineWidth,    SIGNAL( valueChanged( int ) ),             myUpdater, SIGNAL( update() ) );
  connect( myColor,        SIGNAL( changed( QColor ) ),               myUpdater, SIGNAL( update() ) );
  connect( myCurvesList,   SIGNAL( itemChanged( QListWidgetItem* ) ), myUpdater, SIGNAL( update() ) );
  connect( myCurvesList,   SIGNAL( itemSelectionChanged() ),          this, SLOT( selectionChanged() ) );
  connect( myAddButton,    SIGNAL( clicked() ),                       this, SLOT( addCurve() ) );
  connect( myRemoveButton, SIGNAL( clicked()),                        this, SLOT( removeCurve() ) );
  connect( okButton,       SIGNAL( clicked() ),                       this, SLOT( accept() ) );
  connect( applyButton,    SIGNAL( clicked() ),                       this, SLOT( apply() ) );
  connect( cancelButton,   SIGNAL( clicked() ),                       this, SLOT( reject() ) );
  connect( helpButton,     SIGNAL( clicked() ),                       this, SLOT( help() ) );

  // Initialize dialog box
  init();
}

/*!
  Destructor
*/
Plot2d_AnalyticalCurveDlg::~Plot2d_AnalyticalCurveDlg()
{
}

void Plot2d_AnalyticalCurveDlg::init()
{
  AnalyticalCurveList curves = myContainer->getAnalyticalCurves();
  foreach ( Plot2d_AnalyticalCurve* curve, curves ) {
    QListWidgetItem* item = new QListWidgetItem( curve->getName() );
    item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
    item->setCheckState( curve->isActive() ? Qt::Checked : Qt::Unchecked );
    initPropsFromCurve(curve);
    QVariant var;
    var.setValue( (void*)curve );
    item->setData( Qt::UserRole, var );
    myCurvesList->addItem( item );

    if ( !myCurvesList->currentItem() )
      myCurvesList->setCurrentItem( item );
  }
  
  selectionChanged();
}

/*!
 /brief Store curve properties in the local map.
*/
void Plot2d_AnalyticalCurveDlg::initPropsFromCurve(Plot2d_AnalyticalCurve* curve) {
  myProperties[ curve ][ PROP_TITLE ]         = curve->getName();
  myProperties[ curve ][ PROP_VISIBLE ]       = curve->isActive();
  myProperties[ curve ][ PROP_FORMULA ]       = curve->getExpression();
  myProperties[ curve ][ PROP_NBINTERVALS ]   = (int)curve->getNbIntervals();
  myProperties[ curve ][ PROP_AUTOASSIGN ]    = curve->isAutoAssign();
  myProperties[ curve ][ PROP_MARKERTYPE ]  = curve->getMarker();
  myProperties[ curve ][ PROP_LINETYPE ]    = curve->getLine();
  myProperties[ curve ][ PROP_LINEWIDTH ]   = curve->getLineWidth();
  myProperties[ curve ][ PROP_COLOR ]       = curve->getColor();
  myProperties[ curve ][ PROP_STATUS ]        = ItemExisting;
}

QwtPlot* Plot2d_AnalyticalCurveDlg::getPlot() {
  Plot2d_ViewFrame* frame = dynamic_cast<Plot2d_ViewFrame*>(myContainer);
  if( frame )
	return frame->getPlot();
  return 0;
}

/*!
 /brief Store local copy on the curves properties into curves.
*/
bool Plot2d_AnalyticalCurveDlg::processCurves() {
  // update curves

  QwtPlot* plot = getPlot();
  if(!plot)
	  return false;

  PropMap::Iterator it;
  for ( it = myProperties.begin(); it != myProperties.end(); ++it ) {
    Plot2d_AnalyticalCurve* curve = it.key();
    if ( propStatus( curve ) == ItemRemoved ) {
      myContainer->removeAnalyticalCurve( curve );
      continue;
    }
    curve->setName( propTitle( curve ) );
    curve->setActive( propVisible( curve ) );
    curve->setExpression( propFormula( curve ) );
    curve->setNbIntervals( propIntervals(curve) );
    bool prevValue = curve->isAutoAssign();
    curve->setAutoAssign( propAutoAssign( curve ) );
    if ( !curve->isAutoAssign() ) {
      curve->setMarker( propMarkerType( curve ) );
      curve->setLine( propLineType( curve ) );
      curve->setLineWidth( propLineWidth( curve ) );
      curve->setColor( propColor( curve ) );
    }  else {
      if(!prevValue){
	curve->autoFill(plot);
      }
    }
    
    if (! curve->checkCurve(plot) ) {
      QListWidgetItem* item = getItem(curve);
      if(item) {
	myCurvesList->setCurrentItem( item );
	SUIT_MessageBox::critical( this, tr( "ERR_ERROR" ), tr( "AC_CANT_CALCULATE" ) );
      }
      return false;	
    }
    
    if ( propStatus( curve ) == ItemAdded ) {
      myContainer->addAnalyticalCurve( curve );
      myProperties[ curve ][ PROP_STATUS ] = ItemExisting;
    }
  }
  return true;
}

/*!
  \brief Private slot. Called when "Apply" button is clicked
*/
void Plot2d_AnalyticalCurveDlg::apply() {
  if(processCurves()) {
    Plot2d_ViewFrame* f = dynamic_cast<Plot2d_ViewFrame*>(myContainer);
    if(f)
      f->updateAnalyticalCurves();
    
    AnalyticalCurveList curves = myContainer->getAnalyticalCurves();
    foreach ( Plot2d_AnalyticalCurve* curve, curves ) {
      initPropsFromCurve(curve);
    }
    selectionChanged();
  }
}

/*!
  \brief Private slot. Called when "Ok" button is clicked
*/
void Plot2d_AnalyticalCurveDlg::accept()
{
  if(processCurves())
    QDialog::accept();
}

/*!
  \brief Private slot. Called when "Close" button is clicked
*/
void Plot2d_AnalyticalCurveDlg::reject()
{
  // clean-up curves just created
  PropMap::Iterator it;
  for ( it = myProperties.begin(); it != myProperties.end(); ++it ) {
    Plot2d_AnalyticalCurve* curve = it.key();
    if ( propStatus( curve ) == ItemAdded )
      delete curve;
  }
  QDialog::reject();
}

/*!
  \brief Private slot. Called when "Add curve" button is clicked
*/
void Plot2d_AnalyticalCurveDlg::addCurve()
{
  Plot2d_AnalyticalCurve* curve = new Plot2d_AnalyticalCurve();
  
  if(curve->isAutoAssign()) {
    QwtPlot* plot = getPlot();
    if(plot)
      curve->autoFill(plot);
  }
  
  QListWidgetItem* item = new QListWidgetItem(curve->getName()); 
  item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled );
  item->setCheckState( Qt::Checked );
  QVariant var;
  var.setValue( (void*)curve );
  item->setData( Qt::UserRole, var );
  myCurvesList->addItem( item );
  
  myProperties[ curve ][ PROP_STATUS ] = ItemAdded;
  
  myCurvesList->setCurrentItem( item );
}

/*!
  \brief Private slot. Called when "Remove curve" button is clicked
*/
void Plot2d_AnalyticalCurveDlg::removeCurve()
{
  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  foreach( QListWidgetItem* item, items ) {
    Plot2d_AnalyticalCurve* curve = (Plot2d_AnalyticalCurve*)( item->data( Qt::UserRole ).value<void*>() );
    delete item;
    if ( propStatus( curve ) == ItemAdded ) {
      myProperties.remove( curve );
      delete curve;
    }
    else {
      myProperties[ curve ][ PROP_STATUS ] = ItemRemoved;
    }
  }
}

/*!
  \brief Private slot. Called when any curve property is changed.
*/
void Plot2d_AnalyticalCurveDlg::updateCurve()
{
  UpdateLocker lock( myUpdater );
  
  QListWidgetItem* item = selected();
  Plot2d_AnalyticalCurve* curve = selectedCurve();
  if ( item && curve ) {
    myProperties[ curve ][ PROP_TITLE ]       = item->text();
    myProperties[ curve ][ PROP_VISIBLE ]     = item->checkState() == Qt::Checked;
    myProperties[ curve ][ PROP_FORMULA ]     = myFormula->text();
    myProperties[ curve ][ PROP_NBINTERVALS ] = myNbIntervals->value();
    myProperties[ curve ][ PROP_AUTOASSIGN ]  = myAutoAssign->isChecked();
    myProperties[ curve ][ PROP_MARKERTYPE ]  = myMarkerType->currentIndex();
    myProperties[ curve ][ PROP_LINETYPE ]    = myLineType->currentIndex();
    myProperties[ curve ][ PROP_LINEWIDTH ]   = myLineWidth->value();
    myProperties[ curve ][ PROP_COLOR ]       = myColor->color();
  }
  
  updateState();
}

/*!
  \brief Private slot. Update widgets state.
*/
void Plot2d_AnalyticalCurveDlg::updateState()
{
  myPropsGrp->setEnabled( !myAutoAssign->isChecked() );
  myCurveParams->setEnabled( selectedCurve() != 0 );
  myCurveProps->setEnabled( selectedCurve() != 0 );
  myRemoveButton->setEnabled( selectedCurve() != 0 );
}

/*!
  \brief Private slot. Called when selection in the curve list is changed.
*/
void Plot2d_AnalyticalCurveDlg::selectionChanged()
{  
  UpdateLocker lock( myUpdater );

  Plot2d_AnalyticalCurve* curve = selectedCurve();

  myFormula->setText( propFormula( curve ) );
  myNbIntervals->setValue( propIntervals( curve ) );
  myMarkerType->setCurrentIndex( (int)propMarkerType( curve ) );
  myLineType->setCurrentIndex( (int)propLineType( curve ) );
  myLineWidth->setValue( propLineWidth( curve ) );
  myColor->setColor( propColor( curve ) );
  myAutoAssign->setChecked( propAutoAssign( curve ) );

  updateState();
}

/*!
  \brief Show help page
*/
void Plot2d_AnalyticalCurveDlg::help()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "plot2d_viewer_page.html", "analytical_curve" );
}

/*!
  \brief Get currently selected list widget item
*/
QListWidgetItem* Plot2d_AnalyticalCurveDlg::selected() const
{
  QList<QListWidgetItem*> items = myCurvesList->selectedItems();
  return items.count() > 0 ? items[0] : 0;
}

/*!
  \brief Get widget item by the curve
*/
QListWidgetItem* Plot2d_AnalyticalCurveDlg::getItem(Plot2d_AnalyticalCurve* theCurve) const
{
  int nb = myCurvesList->count();
  QListWidgetItem* item = 0;
  for(int i = 0; i < nb ; i++) {	
    item = myCurvesList->item(i);
    if(item->data( Qt::UserRole ).value<void*>() == theCurve)
      break;
  }
  return item;
}

/*!
  \brief Get currently selected curve
*/
Plot2d_AnalyticalCurve* Plot2d_AnalyticalCurveDlg::selectedCurve() const
{
  return selected() ? (Plot2d_AnalyticalCurve*)( selected()->data( Qt::UserRole ).value<void*>() ) : 0;
}

/*!
  \brief Get curve property: status
*/
int Plot2d_AnalyticalCurveDlg::propStatus( Plot2d_AnalyticalCurve* curve, const int def )
{
  int val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_STATUS ) )
      myProperties[ curve ][ PROP_STATUS ] = def;
    QVariant v = myProperties[ curve ][ PROP_STATUS ];
    if ( v.isValid() && v.type() == QVariant::Int ) val = v.toInt();
  }
  return val;
}

/*!
  \brief Get curve property: title
*/
QString Plot2d_AnalyticalCurveDlg::propTitle( Plot2d_AnalyticalCurve* curve, const QString& def )
{
  QString val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_TITLE ) )
      myProperties[ curve ][ PROP_TITLE ] = def;
    QVariant v = myProperties[ curve ][ PROP_TITLE ];
    if ( v.isValid() && v.type() == QVariant::String ) val = v.toString();
  }
  return val;
}

/*!
  \brief Get curve property: visible flag
*/
bool Plot2d_AnalyticalCurveDlg::propVisible( Plot2d_AnalyticalCurve* curve, bool def )
{
  bool val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_VISIBLE ) )
      myProperties[ curve ][ PROP_VISIBLE ] = def;
    QVariant v = myProperties[ curve ][ PROP_VISIBLE ];
    if ( v.isValid() && v.type() == QVariant::Bool ) val = v.toBool();
  }
  return val;
}

/*!
  \brief Get curve property: formula
*/
QString Plot2d_AnalyticalCurveDlg::propFormula( Plot2d_AnalyticalCurve* curve, const QString& def )
{
  QString val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_FORMULA ) )
      myProperties[ curve ][ PROP_FORMULA ] = def;
    QVariant v = myProperties[ curve ][ PROP_FORMULA ];
    if ( v.isValid() && v.type() == QVariant::String ) val = v.toString();
  }
  return val;
}

/*!
  \brief Get curve property: nb intervals
*/
int Plot2d_AnalyticalCurveDlg::propIntervals( Plot2d_AnalyticalCurve* curve, int def )
{
  int val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_NBINTERVALS ) )
      myProperties[ curve ][ PROP_NBINTERVALS ] = def;
    QVariant v = myProperties[ curve ][ PROP_NBINTERVALS ];
    if ( v.isValid() && v.type() == QVariant::Int ) val = v.toInt();
  }
  return val;
}

/*!
  \brief Get curve property: marker type
*/
Plot2d::MarkerType Plot2d_AnalyticalCurveDlg::propMarkerType( Plot2d_AnalyticalCurve* curve, Plot2d::MarkerType def )
{
  Plot2d::MarkerType val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_MARKERTYPE ) )
      myProperties[ curve ][ PROP_MARKERTYPE ] = def;
    QVariant v = myProperties[ curve ][ PROP_MARKERTYPE ];
    if ( v.isValid() && v.type() == QVariant::Int ) val = (Plot2d::MarkerType)( v.toInt() );
  }
  return val;
}

/*!
  \brief Get curve property: line type
*/
Plot2d::LineType Plot2d_AnalyticalCurveDlg::propLineType( Plot2d_AnalyticalCurve* curve, Plot2d::LineType def )
{
  Plot2d::LineType val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_LINETYPE ) )
      myProperties[ curve ][ PROP_LINETYPE ] = def;
    QVariant v = myProperties[ curve ][ PROP_LINETYPE ];
    if ( v.isValid() && v.type() == QVariant::Int ) val = (Plot2d::LineType)( v.toInt() );
  }
  return val;
}

/*!
  \brief Get curve property: line width
*/
int Plot2d_AnalyticalCurveDlg::propLineWidth( Plot2d_AnalyticalCurve* curve, int def )
{
  int val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_LINEWIDTH ) )
      myProperties[ curve ][ PROP_LINEWIDTH ] = def;
    QVariant v = myProperties[ curve ][ PROP_LINEWIDTH ];
    if ( v.isValid() && v.type() == QVariant::Int ) val = v.toInt();
  }
  return val;
}

/*!
  \brief Get curve property: color
*/
QColor Plot2d_AnalyticalCurveDlg::propColor( Plot2d_AnalyticalCurve* curve, const QColor& def )
{
  QColor val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_COLOR ) )
      myProperties[ curve ][ PROP_COLOR ] = def;
    QVariant v = myProperties[ curve ][ PROP_COLOR ];
    if ( v.isValid() && v.type() == QVariant::Color ) val = v.value<QColor>();
  }
  return val;
}

/*!
  \brief Get curve property: auto-assign flag
*/
bool Plot2d_AnalyticalCurveDlg::propAutoAssign( Plot2d_AnalyticalCurve* curve, bool def )
{
  bool val = def;
  if ( curve ) {
    if ( !myProperties.contains( curve ) )
      myProperties[ curve ] = CurveProps();
    if ( !myProperties[ curve ].contains( PROP_AUTOASSIGN ) )
      myProperties[ curve ][ PROP_AUTOASSIGN ] = def;
    QVariant v = myProperties[ curve ][ PROP_AUTOASSIGN ];
    if ( v.isValid() && v.type() == QVariant::Bool ) val = v.toBool();
  }
  return val;
}
