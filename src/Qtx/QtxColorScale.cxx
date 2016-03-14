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

// File:      QtxColorScale.cxx
// Author:    Sergey TELKOV
//
#include "QtxColorScale.h"

#include <QMap>
#include <QRegExp>
#include <QStringList>
#include <QPixmap>
#include <QPainter>
#include <QTextDocument>

#include <math.h>

/*!
  \class QtxColorScale
  \brief Color Scale widget.
*/

/*!
  \brief Constructor.
  \param parent parent widget
  \param f widget flags
*/
QtxColorScale::QtxColorScale( QWidget* parent, Qt::WindowFlags f )
: QFrame( parent, f ),
  myMin( 0.0 ),
  myMax( 1.0 ),
  myTitle( "" ),
  myFormat( "%.4g" ),
  myInterval( 10 ),
  myDumpMode( NoDump ),
  myColorMode( Auto ),
  myLabelMode( Auto ),
  myFlags( AtBorder | WrapTitle ),
  myLabelPos( Right ),
  myTitlePos( Center )
{
  setWindowTitle( tr ( "Color scale" ) );
}

/*!
  \brief Constructor.
  \param num number of color scale intervals
  \param parent parent widget
  \param f widget flags
*/
QtxColorScale::QtxColorScale( const int num, QWidget* parent, Qt::WindowFlags f )
: QFrame( parent, f ),
  myMin( 0.0 ),
  myMax( 1.0 ),
  myTitle( "" ),
  myFormat( "%.4g" ),
  myInterval( num ),
  myDumpMode( NoDump ),
  myColorMode( Auto ),
  myLabelMode( Auto ),
  myFlags( AtBorder | WrapTitle ),
  myLabelPos( Right ),
  myTitlePos( Center )
{
  setWindowTitle( tr ( "Color scale" ) );
}

/*!
  \brief Destructor.

  Does nothing for the moment.
*/
QtxColorScale::~QtxColorScale()
{
}

/*!
  \brief Get color scale minimum value.
  \return lower limit of the color scale
*/
double QtxColorScale::minimum() const
{
  return myMin;
}

/*!
  \brief Get color scale maximum value.
  \return upper limit of the color scale
*/
double QtxColorScale::maximum() const
{
  return myMax;
}

/*!
  \brief Get color scale range.
  \param min returning lower limit of the color scale
  \param max returning upper limit of the color scale
*/
void QtxColorScale::range( double& min, double& max ) const
{
  min = myMin;
  max = myMax;
}

/*!
  \brief Get color scale title.
  \return current title
*/
QString QtxColorScale::title() const
{
  return myTitle;
}

/*!
  \brief Get current format of the number presentation.

  This format is used to output values in the color scale labels
  in "Auto" label mode. The format uses sprintf specification.

  \return current format
*/
QString QtxColorScale::format() const
{
  return myFormat;
}

/*!
  \brief Get Color scale dump mode.
  \return current dump mode (QtxColorScale::DumpMode)
*/
int QtxColorScale::dumpMode() const
{
  return myDumpMode;
}

/*!
  \brief Get label mode.
  \return current label mode (QtxColorScale::Mode)
*/
int QtxColorScale::labelMode() const
{
  return myLabelMode;
}

/*!
  \brief Get color mode.
  \return current color mode (QtxColorScale::Mode)
*/
int QtxColorScale::colorMode() const
{
  return myColorMode;
}

/*!
  \brief Get number of color scale intervals.
  \return number of intervals
*/
int QtxColorScale::intervalsNumber() const
{
  return myInterval;
}

/*!
  \brief Get user label for the specified color scale interval.
  \param idx interval index
  \return user label for specified interval
*/
QString QtxColorScale::label( const int idx ) const
{
  QString res;
  if ( idx >= 0 && idx < (int)myLabels.count() )
    res = myLabels[idx];
  return res;
}

/*!
  \brief Get user color for the specified color scale interval.
  \param idx interval index
  \return user color for specified interval
*/
QColor QtxColorScale::color( const int idx ) const
{
  QColor res;
  if ( idx >= 0 && idx < (int)myColors.count() )
    res = myColors[idx];
  return res;
}

/*!
  \brief Get user labels for all color scale intervals.
  \param list returning labels list
*/
void QtxColorScale::labels( QStringList& list ) const
{
  list = myLabels;
}

/*!
  \brief Get user colors for all color scale intervals.
  \param list returning colors list
*/
void QtxColorScale::colors( QList<QColor>& list ) const
{
  list = myColors;
}

/*!
  \brief Get label position.
  \return label position (QtxColorScale::Position)
*/
int QtxColorScale::labelPosition() const
{
  return myLabelPos;
}

/*!
  \brief Get title position.
  \return title position (QtxColorScale::Position)
*/
int QtxColorScale::titlePosition() const
{
  return myTitlePos;
}

/*!
  \brief Set color scale minimum value.
  \param val lower limit of the color scale
*/
void QtxColorScale::setMinimum( const double val )
{
  setRange( val, maximum() );
}

/*!
  \brief Set color scale maximum value.
  \param val upper limit of the color scale
*/
void QtxColorScale::setMaximum( const double val )
{
  setRange( minimum(), val );
}

/*!
  \brief Set color scale range.
  \param min lower limit of the color scale
  \param max upper limit of the color scale
*/
void QtxColorScale::setRange( const double min, const double max )
{
  if ( myMin == min && myMax == max )
    return;
  
  myMin = min;
  myMax = max;
  
  myPrecise = QString();

  if ( colorMode() == Auto || labelMode() == Auto )
    updateScale();
}

/*!
  \brief Set color scale title.
  \param str new title
*/
void QtxColorScale::setTitle( const QString& str )
{
  if ( myTitle == str )
    return;
  
  myTitle = str;
  updateScale();
}

/*!
  \brief Set current format of the number presentation.
  \sa format()
  \param format new number presentation format
*/
void QtxColorScale::setFormat( const QString& format )
{
  if ( myFormat == format )
    return;

  myFormat = format;
  myPrecise = QString();
  if ( colorMode() == Auto )
    updateScale();
}

/*!
  \brief Set number of color scale intervals.
  \param num number of intervals
*/
void QtxColorScale::setIntervalsNumber( const int num )
{
  if ( myInterval == num || num < 1 )
    return;
  
  myInterval = num;
  myPrecise = QString();
  
  updateScale();
}

/*!
  \brief Set user label for the specified color scale interval.

  If number of interval is negative then user label will be added
  as new to the end of list.

  \param txt user label
  \param idx interval index
*/
void QtxColorScale::setLabel( const QString& txt, const int idx )
{
  bool changed = false;
  int i = idx < 0 ? myLabels.count() : idx;
  if ( i < myLabels.count() )
  {
    changed = myLabels[i] != txt;
    myLabels[i] = txt;
  }
  else
  {
    changed = true;
    while ( i >= myLabels.count() )
      myLabels.append( "" );
    myLabels[i] = txt;
  }
  if ( changed )
    updateScale();
}

/*!
  \brief Set user color for the specified color scale interval.

  If number of interval is negative then user color will be added
  as new to the end of list.

  \param clr user color
  \param idx interval index
*/
void QtxColorScale::setColor( const QColor& clr, const int idx )
{
  bool changed = false;
  int i = idx < 0 ? myColors.count() : idx;
  if ( i < myColors.count() )
  {
    changed = myColors[i] != clr;
    myColors[i] = clr;
  }
  else
  {
    changed = true;
    while ( i >= myColors.count() )
      myColors.append( QColor() );
    myColors[i] = clr;
  }
  if ( changed )
    updateScale();
}

/*!
  \brief Set user labels for all color scale intervals.
  \param list new labels list
*/
void QtxColorScale::setLabels( const QStringList& list )
{
  if ( list.isEmpty() )
    return;

  myLabels = list;
  updateScale();
}

/*!
  \brief Set user colors for all color scale intervals.
  \param list new colors list
*/
void QtxColorScale::setColors( const QList<QColor>& list )
{
  if ( list.isEmpty() )
    return;

  myColors = list;
  updateScale();
}

/*!
  \brief Set color scale color mode.
  \param mode new color mode (QtxColorScale::Mode)
*/
void QtxColorScale::setColorMode( const int mode )
{
  if ( myColorMode == mode )
    return;
  
  myColorMode = mode;
  updateScale();
}

/*!
  \brief Set color scale dump mode.
  \param mode new dump mode (QtxColorScale::DumpMode)
*/
void QtxColorScale::setDumpMode( const int mode )
{
  myDumpMode = mode;
}

/*!
  \brief Set color scale label mode.
  \param mode new label mode (QtxColorScale::Mode)
*/
void QtxColorScale::setLabelMode( const int mode )
{
  if ( myLabelMode != mode )
  {
    myLabelMode = mode;
    updateScale();
  }
}

/*!
  \brief Set label position.
  \param pos new label position (QtxColorScale::Position)
*/
void QtxColorScale::setLabelPosition( const int pos )
{
  if ( myLabelPos != pos && pos >= None && pos <= Center )
  {
    myLabelPos = pos;
    updateScale();
  }
}

/*!
  \brief Set title position.
  \param pos new title position (QtxColorScale::Position)
*/
void QtxColorScale::setTitlePosition( const int pos )
{
  if ( myTitlePos != pos && pos >= None && pos <= Center )
  {
    myTitlePos = pos;
    updateScale();
  }
}

/*!
  \brief Set color scale flags.
  \param flags new flags
*/
void QtxColorScale::setFlags( const int flags )
{
  int prev = myFlags;
  myFlags |= flags;
  if ( prev != myFlags )
    updateScale();
}

/*!
  \brief Test color scale flags.
  \return \c true if specified flags are set
*/
bool QtxColorScale::testFlags( const int flags ) const
{
  return ( myFlags & flags ) == flags;
}

/*!
  \brief Clear (reset) color scale flags.
  \param flags color scale flags to be cleared
*/
void QtxColorScale::clearFlags( const int flags )
{
  int prev = myFlags;
  myFlags &= ~flags;
  if ( prev != myFlags )
    updateScale();
}

/*!
  \brief Get widget's minumum size hint.
  \return minimum size hint
*/
QSize QtxColorScale::minimumSizeHint() const
{
  QSize sz = calculateSize( true, myFlags, titlePosition() != None, labelPosition() != None, true );
  return sz + QSize( frameWidth(), frameWidth() );
}

/*!
  \brief Get widget's default size hint.
  \return size hint
*/
QSize QtxColorScale::sizeHint() const
{
  QSize sz = calculateSize( false, myFlags, titlePosition() != None, labelPosition() != None, true );
  return sz + QSize( frameWidth(), frameWidth() );
}

/*!
  \brief Calculate color scale size.
  \param min if \c true, color scale size is calculated to be as smallest as possible
  \param flags color scale flags
  \param title color scale title
  \param labels if \c true take into account labels
  \param colors if \c true take into account colors
  \return color scale size
*/
QSize QtxColorScale::calculateSize( const bool min, const int flags, const bool title,
                                    const bool labels, const bool colors ) const
{
  int num = intervalsNumber();
  
  int spacer = 5;
  int textWidth = 0;
  int textHeight = fontMetrics().height();
  int colorWidth = 20;
  
  if ( labels && colors )
  {
    QtxColorScale* that = (QtxColorScale*)this;
    QString fmt = that->myFormat;

    for ( int idx = 0; idx < num; idx++ )
      textWidth = qMax( textWidth, fontMetrics().width( getLabel( idx ) ) );
    
    if ( !min )
      that->myFormat = that->myFormat.replace( QRegExp( "g" ), "f" );
    
    for ( int index = 0; index < num; index++ )
      textWidth = qMax( textWidth, fontMetrics().width( getLabel( index ) ) );
    
    that->myFormat = fmt;
  }
  
  int scaleWidth = 0;
  int scaleHeight = 0;
  
  int titleWidth = 0;
  int titleHeight = 0;
  
  if ( flags & AtBorder )
  {
    num++;
    if ( min && title && !myTitle.isEmpty() )
      titleHeight += 10;
  }
  
  if ( colors )
  {
    scaleWidth = colorWidth + textWidth + ( textWidth ? 3 : 2 ) * spacer;
    if ( min )
      scaleHeight = qMax( 2 * num, 3 * textHeight );
    else
      scaleHeight = (int)( 1.5 * ( num + 1 ) * textHeight );
  }

  if ( title )
  {
    QTextDocument* srt = textDocument( flags );
    if ( srt )
    {
      QPainter p( (QtxColorScale*)this );
      if ( scaleWidth )
        srt->setTextWidth( scaleWidth );
      
      titleHeight = (int)srt->size().height() + spacer;
      titleWidth = (int)srt->size().width() + 10;
      
    }
    delete srt;
  }

  int W = qMax( titleWidth, scaleWidth ) + width() - contentsRect().width();
  int H = scaleHeight + titleHeight + height() - contentsRect().height();
  
  return QSize( W, H );
}

/*!
  \brief Dump color scale into pixmap with current size.
  \return generated pixmap
*/
QPixmap QtxColorScale::dump() const
{
  QPixmap aPix;
  
  if ( dumpMode() != NoDump )
  {
    aPix = QPixmap( size() );
    if ( !aPix.isNull() )
    {
      bool scale = ( myDumpMode == ScaleDump || myDumpMode == FullDump );
      bool label = ( myDumpMode == ScaleDump || myDumpMode == FullDump ) &&
        labelPosition() != None;
      bool title = ( myDumpMode == TitleDump || myDumpMode == FullDump ) &&
        titlePosition() != None;
      QColor bgc = palette().color( backgroundRole() );
      QPainter p;
      p.begin( &aPix );
      p.fillRect( 0, 0, aPix.width(), aPix.height(), bgc );
      drawScale( &p, bgc, false, 0, 0, aPix.width(), aPix.height(), title, label, scale );
      p.end();
    }
  }
  
  return aPix;
}

/*!
  \brief Dump color scale into pixmap with the specified size.
  \param w pixmap width
  \param h pixmap height
  \return generated pixmap
*/
QPixmap QtxColorScale::dump( const int w, const int h ) const
{
  return dump( palette().color( backgroundRole() ), w, h );
}

/*!
  \brief Dump color scale into pixmap with the specified size and background color.
  \param bg pixmap background color
  \param w pixmap width
  \param h pixmap height
  \return generated pixmap
*/
QPixmap QtxColorScale::dump( const QColor& bg, const int w, const int h ) const
{
  QPixmap aPix;
  if ( dumpMode() != NoDump )
  {
    bool scale = ( myDumpMode == ScaleDump || myDumpMode == FullDump );
    bool label = ( myDumpMode == ScaleDump || myDumpMode == FullDump ) &&
      labelPosition() != None;
    bool title = ( myDumpMode == TitleDump || myDumpMode == FullDump ) &&
      titlePosition() != None;
    
    int W = w;
    int H = h;
    if ( W < 0 || H < 0 )
    {
      QSize sz = calculateSize( false, myFlags & ~WrapTitle, title, label, scale );
      
      if ( W < 0 )
        W = sz.width();
      if ( H < 0 )
        H = sz.height();
    }
    
    aPix = QPixmap( W, H );
    if ( !aPix.isNull() )
    {
      QPainter p;
      p.begin( &aPix );
      p.fillRect( 0, 0, aPix.width(), aPix.height(), bg );
      drawScale( &p, bg, false, 0, 0, aPix.width(), aPix.height(), title, label, scale );
      p.end();
    }
  }
  
  return aPix;
}

/*!
  \brief Show color scale (reimplemented from QFrame).
*/
void QtxColorScale::show()
{
  QFrame::show();
}

/*!
  \brief Hide color scale (reimplemented from QFrame).
*/
void QtxColorScale::hide()
{
  QFrame::hide();
}

/*!
  \brief Paint widget
  \param e paint event
*/
void QtxColorScale::paintEvent( QPaintEvent* e )
{
  QPainter p( this );
  drawFrame( &p );
  drawContents( &p );
}

/*!
  \brief Draw color scale (reimplemented from QFrame).
  \param p painter
*/
void QtxColorScale::drawContents( QPainter* p )
{
  if ( !updatesEnabled() )
    return;
  
  QRect aDrawRect = contentsRect();
  
  drawScale( p, false/*testFlags( Transparent )*/, aDrawRect.x(),
             aDrawRect.y(), aDrawRect.width(), aDrawRect.height(),
             titlePosition() != None, labelPosition() != None, true );
}

/*!
  \brief Draw color scale contents.
  \param p painter
  \param transp if \c true color scale is drawn on transparent background
  \param X color scale x coordinate
  \param Y color scale y coordinate
  \param W color scale width
  \param H color scale height
  \param drawTitle if \c true, draw title
  \param drawLabel if \c true, draw labels
  \param drawColors if \c true, draw colors
*/
void QtxColorScale::drawScale( QPainter* p, const bool transp, const int X, const int Y,
                               const int W, const int H, const bool drawTitle,
                               const bool drawLabel, const bool drawColors ) const
{
  QPixmap cache( W, H );
  QPainter cp( &cache );
  
  drawScale( &cp, palette().color( backgroundRole() ), transp, 0, 0, W, H, drawTitle, drawLabel, drawColors );
  cp.end();
  
  p->drawPixmap( X, Y, cache );
}

/*!
  \brief Draw color scale contents.
  \param p painter
  \param bg background color
  \param transp if \c true color scale is drawn on transparent background
  \param X color scale x coordinate
  \param Y color scale y coordinate
  \param W color scale width
  \param H color scale height
  \param drawTitle if \c true, draw title
  \param drawLabel if \c true, draw labels
  \param drawColors if \c true, draw colors
*/
void QtxColorScale::drawScale( QPainter* p, const QColor& bg, const bool transp,
                               const int X, const int Y, const int W, const int H,
                               const bool drawTitle, const bool drawLabel, const bool drawColors ) const
{
  if ( !transp )
    p->fillRect( X, Y, W, H, bg );
  
  int num = intervalsNumber();
  
  int labPos = labelPosition();
  
  int spacer = 5;
  int textWidth = 0;
  int textHeight = p->fontMetrics().height();
  
  QString aTitle = title();
  
  int titleWidth = 0;
  int titleHeight = 0;
  
  if ( qGray( bg.rgb() ) < 128 )
    p->setPen( QColor( 255, 255, 255 ) );
  else
    p->setPen( QColor( 0, 0, 0 ) );
  
  // Draw title
  if ( drawTitle )
  {
    QTextDocument* srt = textDocument( myFlags );
    if ( srt )
    {
      srt->setTextWidth( W - 10 );
      titleHeight = (int)srt->size().height() + spacer;
      titleWidth = (int)srt->size().width();
      p->save();
      p->translate( X + 5, Y );
      srt->drawContents( p );
      p->restore();
    }
    delete srt;
  }

  bool reverse = testFlags( Reverse );
  
  QList<QColor>  colors;
  QList<QString> labels;
  for ( int idx = 0; idx < num; idx++ )
  {
    if ( reverse )
    {
      colors.append( getColor( idx ) );
      labels.append( getLabel( idx ) );
    }
    else
    {
      colors.prepend( getColor( idx ) );
      labels.prepend( getLabel( idx ) );
    }
  }
  
  if ( testFlags( AtBorder ) )
  {
    if ( reverse )
      labels.append( getLabel( num ) );
    else
      labels.prepend( getLabel( num ) );
    if ( drawLabel )
      textWidth = qMax( textWidth, p->fontMetrics().width( labels.last() ) );
  }
  
  if ( drawLabel )
  {
    const QFontMetrics& fm = p->fontMetrics();
    for ( QStringList::ConstIterator it = labels.begin(); it != labels.end(); ++it )
      textWidth = qMax( textWidth, fm.width( *it) );
  }
  
  int lab = labels.count();
  
  double spc = ( H - ( ( qMin( lab, 2 ) + qAbs( lab - num - 1 ) ) * textHeight ) - titleHeight );
  double val = spc != 0 ? 1.0 * ( lab - qMin( lab, 2 ) ) * textHeight / spc : 0;
  double iPart;
  double fPart = modf( val, &iPart );
  int filter = (int)iPart + ( fPart != 0 ? 1 : 0 );
  filter = qMax( filter, 1 );
  
  double step = 1.0 * ( H - ( lab - num + qAbs( lab - num - 1 ) ) * textHeight - titleHeight ) / num;
  
  int ascent = p->fontMetrics().ascent();
  int colorWidth = qMax( 5, qMin( 20, W - textWidth - 3 * spacer ) );
  if ( labPos == Center || !drawLabel )
    colorWidth = W - 2 * spacer;
  
  // Draw colors
  int x = X + spacer;
  switch ( labPos )
  {
  case Left:
    x += textWidth + ( textWidth ? 1 : 0 ) * spacer;
    break;
  }

  double offset = 1.0 * textHeight / 2 * ( lab - num + qAbs( lab - num - 1 ) ) + titleHeight;
  QList<QColor>::Iterator cit = colors.begin();
  uint ci = 0;
  for ( ci = 0; cit != colors.end() && drawColors; ++cit, ci++ )
  {
    int y = (int)( Y + ci * step + offset );
    int h = (int)( Y + ( ci + 1 ) * step + offset ) - y;
    p->fillRect( x, y, colorWidth, h, *cit );
  }
  
  if ( drawColors )
    p->drawRect( int( x - 1 ), int( Y + offset - 1 ), int( colorWidth + 2 ), int( ci * step + 2 ) );
  
  // Draw labels
  offset = 1.0 * qAbs( lab - num - 1 ) * ( step - textHeight ) / 2 +
    1.0 * qAbs( lab - num - 1 ) * textHeight / 2;
  offset += titleHeight;
  if ( drawLabel && !labels.isEmpty() )
  {
    int i1 = 0;
    int i2 = lab - 1;
    int last1( i1 ), last2( i2 );
    int x = X + spacer;
    switch ( labPos )
    {
    case Center:
      x += ( colorWidth - textWidth ) / 2;
      break;
    case Right:
      x += colorWidth + spacer;
      break;
    }
    while ( i2 - i1 >= filter || ( i2 == 0 && i1 == 0 ) )
    {
      int pos1 = i1;
      int pos2 = lab - 1 - i2;
      if ( filter && !( pos1 % filter ) )
      {
        p->drawText( x, (int)( Y + i1 * step + ascent + offset ), labels[i1] );
        last1 = i1;
      }
      if ( filter && !( pos2 % filter ) )
      {
        p->drawText( x, (int)( Y + i2 * step + ascent + offset ), labels[i2] );
        last2 = i2;
      }
      i1++;
      i2--;
    }
    int pos = i1;
    int i0 = -1;
    while ( pos <= i2 && i0 == -1 )
    {
      if ( filter && !( pos % filter ) &&
           qAbs( pos - last1 ) >= filter && qAbs( pos - last2 ) >= filter )
        i0 = pos;
      pos++;
    }

    if ( i0 != -1 )
      p->drawText( x, (int)( Y + i0 * step + ascent + offset ), labels[i0] );
  }
}

/*!
  \brief Generate number presentation format.
  \return format for number labels
*/
QString QtxColorScale::getFormat() const
{
  QString aFormat = format();
  
  if ( !testFlags( PreciseFormat ) || testFlags( Integer ) )
    return aFormat;
  
  if ( !myPrecise.isEmpty() )
    return myPrecise;
  
  if ( !aFormat.contains( QRegExp( "^(%[0-9]*.?[0-9]*[fegFEG])$" ) ) )
    return aFormat;
  
  int pos1 = aFormat.indexOf( '.' );
  int pos2 = aFormat.indexOf( QRegExp( "[fegFEG]") );
  
  QString aLocFormat;
  int precision = 1;
  if ( pos1 > 0 )
  {
    aLocFormat = aFormat.mid( 0, pos1 + 1 );
    precision = aFormat.mid( pos1 + 1, pos2 - pos1 - 1 ).toInt();
    if ( precision < 1 )
      precision = 1;
  }
  else
    return aFormat;
  
  QtxColorScale* that = (QtxColorScale*)this;
  
  // calculate format, maximum precision limited
  // to 7 digits after the decimal point.
  while ( myPrecise.isEmpty() && precision < 7 )
  {
    QString aTmpFormat = aLocFormat;
    aTmpFormat += QString( "%1" ).arg( precision );
    aTmpFormat += aFormat.mid( pos2 );
    
    QMap<QString, int> map;
    bool isHasTwinz = false;
    
    for ( int idx = 0; idx < intervalsNumber() && !isHasTwinz; idx++ )
    {
      double val = getNumber( idx );
      QString tmpname = QString().sprintf( aTmpFormat.toLatin1(), val );
      isHasTwinz = map.contains( tmpname );
      map.insert( tmpname, 1 );
    }
    
    if ( !isHasTwinz )
      that->myPrecise = aTmpFormat;
    precision++;
  }
  
  if ( !myPrecise.isEmpty() )
    aFormat = myPrecise;
  
  return aFormat;
}

/*!
  \brief Get color scale value corresponding to the specified interval.
  \param idx interval index
  \return color scale value
*/
double QtxColorScale::getNumber( const int idx ) const
{
  double val = 0;
  if ( intervalsNumber() > 0 )
    val = minimum() + idx * ( qAbs( maximum() - minimum() ) / intervalsNumber() );
  return val;
}

/*!
  \brief Get color scale label text corresponding to the specified interval.
  \param idx interval index
  \return color scale label text
*/
QString QtxColorScale::getLabel( const int idx ) const
{
  QString res;
  if ( labelMode() == User )
    res = label( idx );
  else
  {
    double val = getNumber( idx );
    res = QString().sprintf( getFormat().toLatin1(), testFlags( Integer ) ? (int)val : val );
  }
  return res;
}

/*!
  \brief Get color scale color corresponding to the specified interval.
  \param idx interval index
  \return color scale color
*/
QColor QtxColorScale::getColor( const int idx ) const
{
  QColor res;
  if ( colorMode() == User )
    res = color( idx );
  else
    res = Qtx::scaleColor( idx, 0, intervalsNumber() - 1 );
  return res;
}

/*!
  \brief Update color scale.
*/
void QtxColorScale::updateScale()
{
  update();
  updateGeometry();
}

/*!
  \brief Get text document (rich text) for the color scale title representation.

  If title is not defined (empty string) then null pointer is returned.
  The calling function is responsible for the returning object deleting.

  \param flags color scale flags (not used)
  \return text document or 0 if title is not set
*/
QTextDocument* QtxColorScale::textDocument( const int /*flags*/ ) const
{
  QTextDocument* doc = 0;

  QString aTitle;
  switch ( titlePosition() )
  {
  case Left:
    aTitle = QString( "<p align=\"left\">%1</p>" );
    break;
  case Right:
    aTitle = QString( "<p align=\"right\">%1</p>" );
    break;
  case Center:
    aTitle = QString( "<p align=\"center\">%1</p>" );
    break;
  case None:
  default:
    break;
  }
  
  if ( !aTitle.isEmpty() && !title().isEmpty() )
  {
    /*
    if ( !myStyleSheet )
    {
      QtxColorScale* that = (QtxColorScale*)this;
      that->myStyleSheet = new QStyleSheet( that );
    }
      
    if ( myStyleSheet )
    {
      QStyleSheetItem* item = myStyleSheet->item( "p" );
      if ( item )
      item->setWhiteSpaceMode( flags & WrapTitle ? QStyleSheetItem::WhiteSpaceNormal :
      QStyleSheetItem::WhiteSpaceNoWrap );
    }
    */
    aTitle = aTitle.arg( title() );
    doc = new QTextDocument( aTitle );
  }

  return doc;
}
