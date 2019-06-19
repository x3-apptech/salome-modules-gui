// Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxRubberBand.cxx
// Author:    Alexander A. BORODIN
//
#include "QtxRubberBand.h"

#include <QBitmap>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>
#include <QPalette>
#include <QShowEvent>
#include <QVectorIterator>

#include <math.h>

/*!
  \class QtxAbstractRubberBand
  \brief Analog of class QRubberBand with possibility of creation non-rectangular contour for selection.
  
  Currently this class does not support Style functionality in full.
*/

/*!
  \brief Constructor
  \param theParent parent widget
 */

QtxAbstractRubberBand::QtxAbstractRubberBand( QWidget* theParent)
  : QWidget( theParent/*,Qt::ToolTip*/ ),
    myPoints(),
    myIsClosed( false )
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
#ifndef WIN32
  setAttribute(Qt::WA_NoSystemBackground);
#endif //WIN32
  setAttribute(Qt::WA_WState_ExplicitShowHide);
  setVisible(false);
  theParent->installEventFilter(this);
  setGeometry( QRect(QPoint(0,0), theParent->size() ) );
}

/*!
  \brief Destructor
 */
QtxAbstractRubberBand::~QtxAbstractRubberBand()
{
}

void QtxAbstractRubberBand::clearGeometry()
{
  myPoints.clear();
}

bool QtxAbstractRubberBand::isClosed()
{
  return myIsClosed;
}

void QtxAbstractRubberBand::paintEvent( QPaintEvent* theEvent )
{
  if ( !myPoints.empty() )
    {
      QPixmap tiledPixmap(16, 16);
     
      QPainter pixmapPainter(&tiledPixmap);
      pixmapPainter.setPen(Qt::NoPen);
      pixmapPainter.setBrush(QBrush( Qt::black, Qt::Dense4Pattern ));
      pixmapPainter.setBackground(QBrush( Qt::white ));
      pixmapPainter.setBackgroundMode(Qt::OpaqueMode);
      pixmapPainter.drawRect(0, 0, tiledPixmap.width(), tiledPixmap.height());
      pixmapPainter.end();
      // ### workaround for borked XRENDER
      tiledPixmap = QPixmap::fromImage(tiledPixmap.toImage());


      
      QPainter aPainter( this );
      aPainter.setRenderHint( QPainter::Antialiasing );
      QRect r = myPoints.boundingRect();
      //aPainter.setClipRegion( r.normalized().adjusted( -1, -1, 2, 2 ) );
      aPainter.drawTiledPixmap( 0, 0, width(), height(), tiledPixmap);

      aPainter.end();

    /*



#ifdef WIN32
      QPixmap anImage( size() );
#else
      QImage anImage( size(), QImage::Format_ARGB32_Premultiplied );
#endif

      anImage.fill( Qt::transparent );
      QPainter aImgPainter( &anImage );
      aImgPainter.setRenderHint( QPainter::Antialiasing );
      aImgPainter.setCompositionMode(QPainter::CompositionMode_Source);

      QPen aPen( Qt::black );
      aPen.setWidth( 2 );
      aImgPainter.setPen( aPen );
    
      aImgPainter.drawPolyline( myPoints );
      if ( myIsClosed && myPoints.last() != myPoints.first() )
        aImgPainter.drawLine( myPoints.last(), myPoints.first() );

      //aImgPainter.setPen(Qt::NoPen);
      //aImgPainter.setBrush(QBrush( Qt::white, Qt::Dense4Pattern));
      //aImgPainter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
      //aImgPainter.drawRect(0, 0, width(), height());
      aImgPainter.end();

      QPainter aPainter( this );
      aPainter.drawPolyline( myPoints );
      if ( myIsClosed && myPoints.last() != myPoints.first() )
        aPainter.drawLine( myPoints.last(), myPoints.first() );
      
#ifdef WIN32
      aPainter.drawPixmap( 0, 0, anImage );
#else
      aPainter.drawImage( 0, 0, anImage );
      #endif
      aPainter.end();*/
      
    }
}

void QtxAbstractRubberBand::showEvent( QShowEvent* theEvent )
{
  raise();
  theEvent->ignore();
}

void QtxAbstractRubberBand::moveEvent( QMoveEvent* )
{
}

void QtxAbstractRubberBand::resizeEvent( QResizeEvent* )
{
}

bool QtxAbstractRubberBand::eventFilter( QObject* obj, QEvent* e )
{
  if ( obj && obj == parent() && e->type() == QEvent::Resize )
    {
      QWidget* p = (QWidget*)parent();
      setGeometry( QRect(QPoint(0,0), p->size() ) );
    }
  return QWidget::eventFilter( obj, e );
}

QRegion createRegion( const QPointF& p1, const QPointF& p2 )
{
  if ( p1 == p2 )
    return QRegion();

  QLineF n = QLineF( p1, p2 ).normalVector();//.unitVector();
  n.setLength( 1 );
  n.translate( p1 * -1 );
  QPointF nPoint = n.p2();

  QPolygonF p;
  p << p1 + nPoint << p2 + nPoint << p2 - nPoint << p1 - nPoint << p1 + nPoint;

  return QRegion( p.toPolygon() );
}

void QtxAbstractRubberBand::updateMask()
{
  QRegion r;

  QVectorIterator<QPoint> it(myPoints);
  while( it.hasNext() )
    {
      QPoint p = it.next();
      if( !it.hasNext() )
        break;

      QPoint np = it.peekNext();
      
      if ( p == np ) continue;

      r += createRegion( p, np );
    }

  if ( isClosed() )
    r += createRegion( myPoints.last(), myPoints.first() );

  if ( !r.isEmpty() )
    setMask( r );
}


QtxRectRubberBand::QtxRectRubberBand(QWidget* parent)
  :QtxAbstractRubberBand( parent )      
{
  myPoints.resize( 4 );
  myIsClosed = true;
}

QtxRectRubberBand::~QtxRectRubberBand()
{
}

void QtxRectRubberBand::initGeometry( const QRect& theRect )
{
  myPoints.clear();
  myPoints << theRect.topLeft() << theRect.topRight() << theRect.bottomRight() << theRect.bottomLeft();
  //setMask( QRegion( myPoints ) );
  updateMask();
}

void QtxRectRubberBand::setStartPoint( const QPoint& thePoint )
{
  myPoints[0] = thePoint;
  myPoints[1].setY( thePoint.y() );
  myPoints[3].setX( thePoint.x() );
  updateMask();
}

void QtxRectRubberBand::setEndPoint( const QPoint& thePoint)
{
  myPoints[2] = thePoint;       
  myPoints[1].setX( thePoint.x() );
  myPoints[3].setY( thePoint.y() );
  updateMask();
}

void QtxRectRubberBand::clearGeometry()
{
  QMutableVectorIterator<QPoint> i(myPoints);
  while (i.hasNext())
    {
      i.next();
      i.setValue( QPoint( -1, -1 ) );
    }
}


QtxPolyRubberBand::QtxPolyRubberBand(QWidget* parent)
  :QtxAbstractRubberBand( parent )
{
}

QtxPolyRubberBand::~QtxPolyRubberBand()
{
}

void QtxPolyRubberBand::initGeometry( const QPolygon& thePoints )
{
  myPoints = thePoints;
  updateMask();
}

void QtxPolyRubberBand::initGeometry( const QPoint& thePoint )
{
  myPoints.clear();  
  myPoints << thePoint;
  updateMask();
}

void QtxPolyRubberBand::addNode( const QPoint& thePoint )
{
  myPoints << thePoint;
  updateMask();
}

void QtxPolyRubberBand::replaceLastNode( const QPoint& thePoint )
{
  if ( !myPoints.empty() )
    {
      myPoints.pop_back();
      myPoints << thePoint;
      updateMask();
    }
}

void QtxPolyRubberBand::removeLastNode()
{
  if ( !myPoints.empty() )
    {
      myPoints.pop_back();
      updateMask();
    }
}

void QtxPolyRubberBand::setClosed( bool theFlag )
{
  if (myIsClosed != theFlag )
    {
      myIsClosed = theFlag;
      updateMask();
    }
}

QtxCircleRubberBand::QtxCircleRubberBand(QWidget* parent)
  :QtxAbstractRubberBand(parent), myHasCenter(false)
{
  myPoints.resize(2);
  myIsClosed = true;
}

QtxCircleRubberBand::~QtxCircleRubberBand()
{
}

void QtxCircleRubberBand::initGeometry(const QPoint& thePoint)
{
  myIsClosed = false;
  myHasCenter = true;
  myPoints.clear();
  myPoints << thePoint;
  updateMask();
}

void QtxCircleRubberBand::setRadius(const QPoint& thePoint)
{
  if (myPoints.size() == 1)
    myPoints << thePoint;
  else
    myPoints.setPoint(1, thePoint);
  myIsClosed = true;
  updateMask();
}

void QtxCircleRubberBand::updateMask()
{
  int aLen = radius();
  if (aLen > MIN_RADIUS) {
    QRegion aReg1(myPoints[0].x() - aLen,
      myPoints[0].y() - aLen, aLen * 2, aLen * 2, QRegion::Ellipse);
    QRegion aReg2(myPoints[0].x() - aLen + 2,
      myPoints[0].y() - aLen + 2, aLen * 2 - 4, aLen * 2 - 4, QRegion::Ellipse);
    setMask(aReg1 - aReg2);
  }
}

bool QtxCircleRubberBand::isCenterDefined() const
{
  return myHasCenter;
}

void QtxCircleRubberBand::clearGeometry()
{
  QtxAbstractRubberBand::clearGeometry();
  myHasCenter = false;
  myIsClosed = false;
}

QPoint rotatePoint(const QPoint& theStart, const QPoint& theCenter, double theAngle)
{
  double cosTheta = cos(theAngle);
  double sinTheta = sin(theAngle);
  int aX = (int)(cosTheta * (theStart.x() - theCenter.x()) -
    sinTheta * (theStart.y() - theCenter.y()) + theCenter.x());
  int aY = (int)(sinTheta * (theStart.x() - theCenter.x()) +
    cosTheta * (theStart.y() - theCenter.y()) + theCenter.y());
  return QPoint(aX, aY);
}

static double m_pi = 4 * atan(1);
static double angle_deg = 360. / CIRCLE_NB_POINTS;
static double angle_rad = angle_deg * (m_pi / 180.);


void QtxCircleRubberBand::getPoligon(QPolygon* thePoints) const
{
  int aLen = radius();
  if (aLen > MIN_RADIUS) {
    thePoints->clear();
    QPoint aCenter = myPoints[0];
    QPoint aStart = myPoints[1];
    for (int i = 0; i < CIRCLE_NB_POINTS; i++) {
      thePoints->append(aStart);
      aStart = rotatePoint(aStart, aCenter, angle_rad);
    }
  }
}

int QtxCircleRubberBand::radius() const
{
  if (myPoints.size() < 2)
    return -1;
  QPoint aDist = myPoints[1] - myPoints[0];
  return (int)std::sqrt(std::pow(aDist.x(), 2) + std::pow(aDist.y(), 2));
}