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

// File   : Plot2d_SetupCurveDlg.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#include "Plot2d_SetupCurveDlg.h"

#include <QtxColorButton.h>
#ifndef NO_SUIT
#include <SUIT_Tools.h>
#endif

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QPainter>

const int MARGIN_SIZE     = 11;
const int SPACING_SIZE    = 6;
const int MIN_COMBO_WIDTH = 100;
const int MIN_SPIN_WIDTH  = 50;
const int MAX_LINE_WIDTH  = 10;
const int MSIZE           = 9;

/*!
  \class Plot2d_SetupCurveDlg
  \brief Dialog box for modifying 2d curve settings.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
Plot2d_SetupCurveDlg::Plot2d_SetupCurveDlg( QWidget* parent )
: QDialog( parent )
{
  setModal( true );
  setWindowTitle( tr("TLT_SETUP_CURVE") );
  setSizeGripEnabled( true );

  // curve type
  QLabel* aLineTypeLab = new QLabel( tr( "CURVE_LINE_TYPE_LAB" ), this );
  myLineCombo = new QComboBox( this );
  myLineCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLineCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  QSize lsz( 40, 16 );
  myLineCombo->setIconSize( lsz );

  // curve width
  QLabel* aLineWidthLab = new QLabel( tr( "CURVE_LINE_WIDTH_LAB" ), this );
  myLineSpin = new QSpinBox( this );
  myLineSpin->setMinimum( 0 );
  myLineSpin->setMaximum( MAX_LINE_WIDTH );
  myLineSpin->setSingleStep( 1 );
  myLineSpin->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myLineSpin->setMinimumWidth( MIN_SPIN_WIDTH );

  // marker type
  QLabel* aMarkerLab = new QLabel( tr( "CURVE_MARKER_TYPE_LAB" ), this );
  myMarkerCombo = new QComboBox( this );
  myMarkerCombo->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myMarkerCombo->setMinimumWidth( MIN_COMBO_WIDTH );
  QSize sz(16, 16);
  myMarkerCombo->setIconSize(sz);

  // curve color
  QLabel* aColorLab = new QLabel( tr( "CURVE_COLOR_LAB" ), this );
  myColorBtn = new QtxColorButton( this );

  // preview
  QLabel* aPreviewLab = new QLabel( tr( "CURVE_PREVIEW_LAB" ), this );
  myPreview = new QLabel( this );
  myPreview->setFrameStyle( QLabel::Box | QLabel::Sunken );
  myPreview->setAlignment( Qt::AlignCenter );
  myPreview->setScaledContents( false );

  myOkBtn     = new QPushButton( tr( "BUT_OK" ),     this );
  myCancelBtn = new QPushButton( tr( "BUT_CANCEL" ), this );

  // layouting widgets
  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->setMargin( MARGIN_SIZE );

  topLayout->addWidget( aLineTypeLab,  0, 0 );
  topLayout->addWidget( myLineCombo,   0, 1, 1, 2 );
  topLayout->addWidget( aLineWidthLab, 1, 0 );
  topLayout->addWidget( myLineSpin,    1, 1, 1, 2 );
  topLayout->addWidget( aMarkerLab,    2, 0 );
  topLayout->addWidget( myMarkerCombo, 2, 1, 1, 2 );
  topLayout->addWidget( aColorLab,     3, 0 );
  topLayout->addWidget( myColorBtn,    3, 1 );
  topLayout->addWidget( aPreviewLab,   4, 0 );
  topLayout->addWidget( myPreview,     4, 1, 1, 2 );
  topLayout->setColumnStretch( 2, 5 );

  QHBoxLayout* btnLayout = new QHBoxLayout;
  btnLayout->setSpacing( SPACING_SIZE );
  btnLayout->setMargin( 0 );

  btnLayout->addWidget( myOkBtn );
  btnLayout->addSpacing( 20 );
  btnLayout->addStretch();
  btnLayout->addWidget( myCancelBtn );

  topLayout->addLayout( btnLayout, 5, 0, 1, 3 );

  // fill then combo boxes
  QColor cl = myLineCombo->palette().color( QPalette::Text );

  myLineCombo->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::NoPen ),      tr( "NONE_LINE_LBL" ) );
  myLineCombo->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::Solid ),      tr( "SOLID_LINE_LBL" ) );
  myLineCombo->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::Dash ),       tr( "DASH_LINE_LBL" ) );
  myLineCombo->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::Dot ),        tr( "DOT_LINE_LBL" ) );
  myLineCombo->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::DashDot ),    tr( "DASHDOT_LINE_LBL" ) );
  myLineCombo->addItem( Plot2d::lineIcon( lsz, cl, Plot2d::DashDotDot ), tr( "DAHSDOTDOT_LINE_LBL" ) );

  cl = myMarkerCombo->palette().color( QPalette::Text );

  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::None ),      tr( "NONE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Circle ),    tr( "CIRCLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Rectangle ), tr( "RECTANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Diamond ),   tr( "DIAMOND_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::DTriangle ), tr( "DTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::UTriangle ), tr( "UTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::LTriangle ), tr( "LTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::RTriangle ), tr( "RTRIANGLE_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::Cross ),     tr( "CROSS_MARKER_LBL" ) );
  myMarkerCombo->addItem( Plot2d::markerIcon( sz, cl, Plot2d::XCross ),    tr( "XCROSS_MARKER_LBL" ) );

  // default settings
  setLine( Plot2d::Solid, 0 );   // solid line, width = 0
  setMarker( Plot2d::Circle );   // circle
  setColor( QColor( 0, 0, 0 ) ); // black

  // connections
  connect( myLineCombo,   SIGNAL( activated( int ) ),    this, SLOT( updatePreview() ) );
  connect( myLineSpin,    SIGNAL( valueChanged( int ) ), this, SLOT( updatePreview() ) );
  connect( myMarkerCombo, SIGNAL( activated( int ) ),    this, SLOT( updatePreview() ) );
  connect( myColorBtn,    SIGNAL( changed( QColor ) ),   this, SLOT( updatePreview() ) );
  connect( myOkBtn,       SIGNAL( clicked() ),           this, SLOT( accept() ) );
  connect( myCancelBtn,   SIGNAL( clicked() ),           this, SLOT( reject() ) );

#ifndef NO_SUIT
  SUIT_Tools::centerWidget( this, parent );
#endif
  updatePreview();
}

/*!
  \brief Destructor.
*/
Plot2d_SetupCurveDlg::~Plot2d_SetupCurveDlg()
{
}

/*!
  \brief Set curve line type and width.
  \param type curve line type
  \param width curve line width
  \sa getLine(), getLineWidth()
*/
void Plot2d_SetupCurveDlg::setLine( Plot2d::LineType type, const int width )
{
  myLineCombo->setCurrentIndex( (int)type );
  if ( width > myLineSpin->maximum() )
    myLineSpin->setMaximum( width );
  myLineSpin->setValue( width );
  updatePreview();
}

/*!
  \brief Get curve line type.
  \return chosen curve line type
  \sa setLine(), getLineWidth()
*/
Plot2d::LineType Plot2d_SetupCurveDlg::getLine() const
{
  return (Plot2d::LineType)myLineCombo->currentIndex();
}

/*!
  \brief Get curve line width.
  \return chosen curve line width
  \sa setLine(), getLine()
*/
int Plot2d_SetupCurveDlg::getLineWidth() const
{
  return myLineSpin->value();
}

/*!
  \brief Set curve marker type.
  \param type curve marker type
  \sa getMarker()
*/
void Plot2d_SetupCurveDlg::setMarker( Plot2d::MarkerType type )
{
  myMarkerCombo->setCurrentIndex( (int)type );
  updatePreview();
}

/*!
  \brief Get curve marker type.
  \return chosen curve marker type
  \sa setMarker()
*/
Plot2d::MarkerType Plot2d_SetupCurveDlg::getMarker() const
{
  return (Plot2d::MarkerType)myMarkerCombo->currentIndex();
}

/*!
  \brief Set curve color.
  \param color curve color
  \sa getColor()
*/
void Plot2d_SetupCurveDlg::setColor( const QColor& color )
{
  myColorBtn->setColor( color );
  updatePreview();
}

/*!
  \brief Get curve color.
  \return curve color
  \sa setColor()
*/
QColor Plot2d_SetupCurveDlg::getColor() const
{
  return myColorBtn->color();
}

/*
  \brief Update preview widget.
*/
void Plot2d_SetupCurveDlg::updatePreview()
{
  QSize sz( 150, 20 );
  QPixmap px( sz );
  px.fill( palette().color( QPalette::Background ) );

  QPainter p( &px );

  Plot2d::drawLine( &p, 5+MSIZE/2, sz.height()/2, sz.width()-5-MSIZE/2, sz.height()/2,
                    getLine(), getColor(), getLineWidth() );
  Plot2d::drawMarker( &p, 5+MSIZE/2, sz.height()/2, MSIZE, MSIZE,
                      getMarker(), getColor() );
  Plot2d::drawMarker( &p, sz.width()-5-MSIZE/2, sz.height()/2, MSIZE, MSIZE,
                      getMarker(), getColor() );

  myPreview->setPixmap( px );
}
