// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxBiColorTool.cxx
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "QtxBiColorTool.h"
#include "QtxColorButton.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QSlider>
#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionButton>

const int BICOLOR_MAX_DELTA = 100;

/*!
  \class QtxBiColorTool::ColorLabel
  \brief Draw colored label (for secondary color)
  \internal
*/
class QtxBiColorTool::ColorLabel: public QFrame
{
public:
  ColorLabel( QWidget* parent) : QFrame( parent )
  {
    setFrameStyle( QFrame::Panel | QFrame::Raised );
    //setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  }
  ~ColorLabel() {}
  QSize sizeHint() const
  {
    return minimumSizeHint();
  }
  QSize minimumSizeHint() const
  {
    if ( !mySizeHint.isValid() ) {
      int is = style()->pixelMetric( QStyle::PM_ButtonIconSize, 0, this );
      int pm = style()->pixelMetric( QStyle::PM_ButtonMargin );
      ColorLabel* that = const_cast<ColorLabel*>( this );
      that->mySizeHint = QSize( is + pm, is + pm );
    }
    return mySizeHint; 
  }
  void paintEvent( QPaintEvent* e )
  {
    QStylePainter sp(this);
    QStyleOptionButton option;
    option.initFrom(this);
    option.features = QStyleOptionButton::None;
    option.state |= QStyle::State_Raised;
    sp.drawControl( QStyle::CE_PushButton, option );

    QRect r = rect();
    r.setTopLeft( r.topLeft() + QPoint( 2, 2 ) );
    r.setBottomRight( r.bottomRight() - QPoint( 2, 2 ) );

    QPixmap pix( r.size() );
    pix.fill( palette().color( backgroundRole() ) );

    if ( myColor.isValid() ) {
      QPainter pixp( &pix );
      pixp.setPen( isEnabled() ? Qt::black : palette().mid().color() );
      pixp.fillRect( 1, 1, pix.width()-3, pix.height()- 3, QBrush( isEnabled() ? myColor : palette().mid().color() ) );
      pixp.drawRect( 1, 1, pix.width()-3, pix.height()- 3 );
      pixp.end();
    }
    else {
      QPainter pixp( &pix );
      pixp.setPen( palette().color( isEnabled() ? QPalette::WindowText : QPalette::Mid ) );
      pixp.drawRect( 2, 2, pix.width() - 4, pix.height() - 4 );
      pixp.fillRect( 3, 3, pix.width() - 6, pix.height() - 6,
		     QBrush( palette().color( isEnabled() ? QPalette::WindowText : QPalette::Mid ), Qt::BDiagPattern ) );
      pixp.end();
    }

    QPainter p( this );
    p.drawPixmap( r, pix );
    p.end();
  }
  void setColor( const QColor& c )
  {
    myColor = c;
    update();
  }
  
private:
  QSize  mySizeHint;
  QColor myColor;
};

/*!
  \class QtxBiColorTool
  \brief Implementation of the widget managing a couple of colors.

  The main color is specified explicitly. The secondary color is calculated
  by changing "value" of the main color in HSV notation to the specified delta.
*/

/*!
  \brief Constructor.
  \param parent parent widget
*/
QtxBiColorTool::QtxBiColorTool( QWidget* parent )
  : QWidget( parent )
{
  QHBoxLayout* l = new QHBoxLayout( this );
  l->setMargin( 0 );
  l->setSpacing( 5 );

  myMainColor = new QtxColorButton( this );
  //myMainColor->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  myExtraText = new QLabel( this );
  myRuler = new QSlider( Qt::Horizontal, this );
  myRuler->setMinimum( -BICOLOR_MAX_DELTA );
  myRuler->setMaximum( +BICOLOR_MAX_DELTA );
  myRuler->setSingleStep( 1 );
  myRuler->setPageStep( 10 );
  myRuler->setValue( 0 );
  myRuler->setTickPosition( QSlider::NoTicks );
  myDelta = new ColorLabel( this );

  l->addWidget( myMainColor );
  l->addWidget( myExtraText );
  l->addWidget( myRuler );
  l->addWidget( myDelta );
  myRuler->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

  connect( myMainColor, SIGNAL( changed( QColor ) ),   this, SLOT( updateState() ) );
  connect( myRuler,     SIGNAL( valueChanged( int ) ), this, SLOT( updateState() ) );

  updateState();
}

/*!
  \brief Destructor.
*/
QtxBiColorTool::~QtxBiColorTool()
{
}

/*!
  \brief Get currently selected main color

  Returns invalid QColor if no color is selected.

  \return selected main color
  \sa setMainColor()
*/
QColor QtxBiColorTool::mainColor() const
{
  return myMainColor->color();
}

/*!
  \brief Set main color.
  \param c color to be set as current main color
  \sa mainColor()
*/
void QtxBiColorTool::setMainColor( const QColor& c )
{
  myMainColor->setColor( c );
  updateState();
}

/*!
  \brief Get current value delta for the secondary color
  \return curent color value delta
  \sa setDelta(), secondaryColor()
*/
int QtxBiColorTool::delta() const
{
  return myRuler->value();
}

/*!
  \brief Set value delta for the secondary color
  \param d new color value delta
  \sa delta(), secondaryColor()
*/
void QtxBiColorTool::setDelta( int d )
{
  myRuler->setValue( d );
  updateState();
}

/*!
  \brief Get secondary color.

  Returns invalid QColor if no main color is selected.
  Secondary color is calculated by changing "value" of the main color
  in HSV notation to the specified delta.

  \return secondary color
  \sa mainColor(), setMainColor(), delta(), setDelta()
*/
QColor QtxBiColorTool::secondaryColor() const
{
  return Qtx::mainColorToSecondary( mainColor(), delta() );
}

/*!
  \brief Returns auxiliary text assigned to the widget
  \return current widget text
  \sa setText()
*/
QString QtxBiColorTool::text() const
{
  return myExtraText->text();
}

/*!
  \brief Assign auxiliary text to the widet
  \param txt new widget text
  \sa text()
*/
void QtxBiColorTool::setText( const QString& txt )
{
  myExtraText->setText( txt );
}

/*!
  \brief Get access to the internal label use for drawing 
  auxiliary test assigned to the widget
*/
QLabel* QtxBiColorTool::label()
{
  return myExtraText;
}

/*!
  \brief Update widget state
*/
void QtxBiColorTool::updateState()
{
  myDelta->setColor( secondaryColor() );
}
