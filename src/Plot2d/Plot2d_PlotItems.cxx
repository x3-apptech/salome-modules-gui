// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//  File   : Plot2d_HistogramItem.cxx
//  Author : Natalia ERMOLAEVA, Open CASCADE S.A.S. (natalia.donis@opencascade.com)

#include "Plot2d_PlotItems.h"
#include "Plot2d_Object.h"

#include <QPainter>
#include <QPalette>
#include <QLayout>
#include <QLine>
#include <QVariant>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_scale_map.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_dict.h>

const char* yAxisLeft[] = {
  "12 12 2 1",
  "  c None",
  ". c #000000",
  "            ",
  "   .        ",
  "  ...       ",
  " . . .      ",
  "   .        ",
  "   .        ",
  "   .    .   ",
  "   .     .  ",
  "   ........ ",
  "         .  ",
  "        .   ",
  "            "};

const char* yAxisRight[] = {
  "12 12 2 1",
  "  c None",
  ". c #000000",
  "            ",
  "        .   ",
  "       ...  ",
  "      . . . ",
  "        .   ",
  "        .   ",
  "   .    .   ",
  "  .     .   ",
  " ........   ",
  "  .         ",
  "   .        ",
  "            "};

/*!
  Constructor of Plot2d_QwtLegendItem
*/
Plot2d_QwtLegendItem::Plot2d_QwtLegendItem( QWidget* parent ) :
  QwtLegendItem( parent ),
  myYAxisIdentifierMode( IM_None ),
  myIsSelected(false)
{
  myYAxisLeftIcon = yAxisLeft;
  myYAxisRightIcon = yAxisRight;
  int anIconWidth = qMax( myYAxisLeftIcon.width(), myYAxisRightIcon.width() );

  mySpacingCollapsed = spacing();
  mySpacingExpanded = anIconWidth - mySpacingCollapsed;
}

/*!
  Destructor of Plot2d_QwtLegendItem
*/
Plot2d_QwtLegendItem::~Plot2d_QwtLegendItem()
{
}

/*!
  Set Y axis identifier displaying mode
*/
void Plot2d_QwtLegendItem::setYAxisIdentifierMode( const int theMode )
{
  myYAxisIdentifierMode = theMode;
  setSpacing( theMode == IM_None ? mySpacingCollapsed : mySpacingExpanded );
}

/*!
  Redefined method of drawing identifier of legend item
*/
void Plot2d_QwtLegendItem::drawIdentifier( QPainter* painter, const QRect& rect ) const
{
  QwtLegendItem::drawIdentifier( painter, rect );

  if( myYAxisIdentifierMode != IM_None ) {
    QPixmap aPixmap( myYAxisIdentifierMode == IM_Left ? yAxisLeft : yAxisRight );
    painter->save();
    painter->drawPixmap( rect.topRight() + QPoint( mySpacingExpanded/2, mySpacingExpanded/2 ), aPixmap );
    painter->restore();
  }
}

/*!
  Update highliting on the item.
*/
void Plot2d_QwtLegendItem::updateHighlit() {
  QwtText txt = text();
  if(isSelected()) {
    QColor highlightColor = Plot2d_Object::selectionColor();
    if(highlightColor != txt.backgroundBrush().color()) {
      txt.setBackgroundBrush(highlightColor);
      setText(txt);
    }    
  } else if( QWidget* parent = qobject_cast<QWidget*>(this->parent()->parent()) ) {
    QPalette aPal = parent->palette();
    if(aPal.color(QPalette::Background) != txt.backgroundBrush().color()) {
      txt.setBackgroundBrush(aPal.color(QPalette::Background));
      setText(txt);
    }
  }
}

/*!
  Sets selected property.
*/
void Plot2d_QwtLegendItem::setSelected(const bool on) {
  myIsSelected = on;
}

/*!
  Gets selected property.
*/
bool Plot2d_QwtLegendItem::isSelected() const {
  return myIsSelected;
}


/*
  Draw text of the item.
*/
void  Plot2d_QwtLegendItem::drawText(QPainter * painter, const QRect &rect) {
  painter->setPen( isSelected() ? Plot2d_Object::highlightedLegendTextColor() : 
		   getColorFromPalette( QPalette::Text) );
  
  QwtLegendItem::drawText( painter, rect );
}

/*
  Get color from the legend pallete by 'role' flag.
*/
QColor Plot2d_QwtLegendItem::getColorFromPalette(QPalette::ColorRole role) {
  QWidget* pw = parentWidget(); 
  QColor  col = palette().color( role );
  while( pw ) {
    if ( qobject_cast<QwtLegend*>( pw ) ) {
      col = pw->palette().color(role );
      break;
    }
    pw = pw->parentWidget();
  } 
  return col;
}
/*
 * Internal class to store deviation data on the curve.
 */
class Plot2d_QwtPlotCurve::Plot2d_DeviationData {
public:
  Plot2d_DeviationData(const double *min, const double *max,const QList<int>& idx)
  { 
    foreach(int index,idx) {
      myMin[index] = min[index];
      myMax[index] = max[index];
    }
  }
  ~Plot2d_DeviationData(){}

  size_t size() const 
  { 
    return qwtMin(myMin.size(), myMax.size()); 
  }
  bool values(size_t i, double &min, double &max) {
    if(myMin.contains(i) && myMax.contains(i)) {
      min = myMin[i];
      max = myMax[i];
      return true;
    }
    return false;
  }
private:
  QMap<int,double> myMin;
  QMap<int,double> myMax;
};


/*!
  Constructor of Plot2d_QwtPlotCurve
*/
Plot2d_QwtPlotCurve::Plot2d_QwtPlotCurve( const QString& title,
                                          QwtPlot::Axis yAxis /*const int index*/ ) :
  Plot2d_SelectableItem(),    					  
  QwtPlotCurve( title ),
  myYAxis( yAxis ),
  myYAxisIdentifierEnabled( false ),
  myDeviationData(0)
{  
}

/*!
  Destructor of Plot2d_QwtPlotCurve
*/
Plot2d_QwtPlotCurve::~Plot2d_QwtPlotCurve()
{
  clearDeviationData();
}

/*!
  Enable / disable Y axis identifier
*/
void Plot2d_QwtPlotCurve::setYAxisIdentifierEnabled( const bool on )
{
  myYAxisIdentifierEnabled = on;
}

/*!
  Redefined method, which updates legend of the curve
*/
void Plot2d_QwtPlotCurve::updateLegend( QwtLegend* legend ) const
{
  if ( !legend )
    return; 

  QWidget* widget = legend->find( this );

  if ( testItemAttribute(QwtPlotItem::Legend)) {   

    if ( widget == NULL ) {
      widget = legendItem();
      if ( widget ) {
	if ( widget->inherits("QwtLegendItem") ) {
	  QwtLegendItem *label = (QwtLegendItem *)widget;
	  label->setItemMode(legend->itemMode());
		
	  if ( plot() ) {
	    QObject::connect(label, SIGNAL(clicked()),
			     plot(), SLOT(legendItemClicked()));
	    QObject::connect(label, SIGNAL(checked(bool)),
			     plot(), SLOT(legendItemChecked(bool)));
	  }
	}
	legend->contentsWidget()->layout()->addWidget(widget);
	legend->insert(this, widget);
      }
    }
    
    QwtPlotCurve::updateLegend( legend );
    
    
    if( Plot2d_QwtLegendItem* anItem = dynamic_cast<Plot2d_QwtLegendItem*>( widget ) ) {
      int aMode = Plot2d_QwtLegendItem::IM_None;
      if( myYAxisIdentifierEnabled )
	aMode = myYAxis == QwtPlot::yRight ?
	  Plot2d_QwtLegendItem::IM_Right :
	  Plot2d_QwtLegendItem::IM_Left;
      anItem->setYAxisIdentifierMode( aMode );
      if(isSelected()) {
	anItem->setCurvePen(legendPen());
	anItem->setSymbol(legendSymbol());
      }
      anItem->setSelected(isSelected());
      anItem->updateHighlit();
    }
  }
}

/*!
  Redefined method, which creates and returns legend item of the curve
*/
QWidget* Plot2d_QwtPlotCurve::legendItem() const
{
  return new Plot2d_QwtLegendItem;
}

/*!
  Redefined method, which draw a set of points of a curve.
*/
void Plot2d_QwtPlotCurve::draw(QPainter *painter,
                               const QwtScaleMap &xMap, const QwtScaleMap &yMap, 
                               int from, int to) const
{
  if (to < 0)
    to = dataSize() - 1;
  QwtPlotCurve::draw(painter, xMap, yMap, from, to);

  //draw deviation data
  if(hasDeviationData()) {    
    painter->save();
    int lineW = deviationMarkerLineWidth();
    int tickSz = deviationMarkerTickSize() + qRound(lineW/2);
    double min, max, xi, yi;
    int xp, ytop, ybtm, tickl, tickr;
    QColor c = isSelected() ? Plot2d_Object::selectionColor() : deviationMarkerColor(); 
    QPen p = QPen(c, lineW, Qt::SolidLine);
    painter->setPen(p);
    for (int i = from; i <= to; i++) {
      if(!myDeviationData->values(i,min,max)) continue;
      xi = x(i);
      yi = y(i);
      xp = xMap.transform(xi);
      ytop = yMap.transform(yi + max);
      ybtm = yMap.transform(yi - min);
      tickl = xp - tickSz;
      tickr = xp + tickSz;
      painter->drawLine(tickl,ytop,tickr,ytop);
      painter->drawLine(xp,ytop,xp,ybtm);
      painter->drawLine(tickl,ybtm,tickr,ybtm);
    }
	  painter->restore();
  }
}

/*!
 * Return color of the deviation marker.
 */
QColor Plot2d_QwtPlotCurve::deviationMarkerColor() const {
  QColor c(0, 0, 127);
  if(plot()) {
    QVariant var = plot()->property(PLOT2D_DEVIATION_COLOR);
    if(var.isValid())
      c = var.value<QColor>();
  }
  return c;
}
/*!
 * Return line width of the deviation marker.
 */
int Plot2d_QwtPlotCurve::deviationMarkerLineWidth() const {
  int lw = 1;
  if(plot()) {
    QVariant var = plot()->property(PLOT2D_DEVIATION_LW);
    if(var.isValid())
      lw = var.toInt();
  }
  return lw;
}

/*!
 * Return tick size of the deviation marker.
 */
int Plot2d_QwtPlotCurve::deviationMarkerTickSize() const {
  int ts = 2;
  if(plot()) {
    QVariant var = plot()->property(PLOT2D_DEVIATION_TS);
    if(var.isValid())
      ts = var.toInt();
  }
  return ts;
}

/*!
 * Sets deviation data for the plot item.
 */
void Plot2d_QwtPlotCurve::setDeviationData(const double* min, const double* max,const QList<int> &idx) {
  clearDeviationData();
  myDeviationData = new Plot2d_DeviationData(min,max,idx);
}

/*!
 * Return true if deviation is assigned to the plot item,
   false otherwise.
 */
bool Plot2d_QwtPlotCurve::hasDeviationData() const {
  return myDeviationData != 0;
}

/*!
 * Remove deviation data from the plot item.
 */
void Plot2d_QwtPlotCurve::clearDeviationData() 
{
  if(myDeviationData)
    delete myDeviationData;
  myDeviationData = 0;
}



/*!
  Constructor.
*/
Plot2d_SelectableItem::Plot2d_SelectableItem():
  myIsSelected(false)
{ 
}

/*!
  Destructor.
*/
Plot2d_SelectableItem::~Plot2d_SelectableItem()
{ 
}

/*!
  Sets selected property.
*/
void Plot2d_SelectableItem::setSelected( const bool on) {
  myIsSelected = on;
}

/*!
  Return selected property.
*/
bool Plot2d_SelectableItem::isSelected() const {
  return myIsSelected;
}

/*!
  Sets legend pen property.
*/  
void Plot2d_SelectableItem::setLegendPen( const QPen & p) {
  myLegendPen = p;
}

/*!
  Return legend pen property.
*/  
QPen Plot2d_SelectableItem::legendPen() const {
  return myLegendPen;
}

/*!
  Sets legend symbol property.
*/  
void Plot2d_SelectableItem::setLegendSymbol(const QwtSymbol& s) {
  myLegendSymbol = s;
}

/*!
  Sets legend symbol property.
*/  
QwtSymbol Plot2d_SelectableItem::legendSymbol() const {
  return myLegendSymbol;
}

/*!
  Constructor
*/
Plot2d_HistogramQwtItem::Plot2d_HistogramQwtItem( const QwtText& theTitle )
: QwtPlotItem( theTitle )
{
  init();
}

/*!
  Constructor
*/
Plot2d_HistogramQwtItem::Plot2d_HistogramQwtItem( const QString& theTitle )
: QwtPlotItem( QwtText( theTitle ) )
{
  init();
}

/*!
  Destructor
*/
Plot2d_HistogramQwtItem::~Plot2d_HistogramQwtItem()
{
}

/*!
  Initialization of object
*/
void Plot2d_HistogramQwtItem::init()
{
  myReference = 0.0;
  myAttributes = Plot2d_HistogramQwtItem::Auto;

  setItemAttribute( QwtPlotItem::AutoScale, true );
  setItemAttribute( QwtPlotItem::Legend,    true );

  setZ( 20.0 );
}

/*!
  Sets base line to object
  @param theRef
*/
void Plot2d_HistogramQwtItem::setBaseline( double theRef )
{
  if ( myReference != theRef ) {
    myReference = theRef;
    itemChanged();
  }
}

/*!
  Returns base line of object
*/
double Plot2d_HistogramQwtItem::baseline() const
{
  return myReference;
}

/*!
  Sets data to object
*/
void Plot2d_HistogramQwtItem::setData( const QwtIntervalData& theData )
{
  myData = theData;
  itemChanged();
}

/*!
  Returns data from object
*/
const QwtIntervalData& Plot2d_HistogramQwtItem::data() const
{
  return myData;
}

/*!
  Sets color to object
*/
void Plot2d_HistogramQwtItem::setColor( const QColor& theColor )
{
  if ( myColor != theColor ) {
    myColor = theColor;
    itemChanged();
  }
}

/*!
  Returns color from object
*/
QColor Plot2d_HistogramQwtItem::color() const
{
  return myColor;
}

/*!
  Returns bounding rect of object
*/
QwtDoubleRect Plot2d_HistogramQwtItem::boundingRect() const
{
  QwtDoubleRect aRect = myData.boundingRect();
  if ( !aRect.isValid() ) 
      return aRect;

  if ( myAttributes & Xfy ) {
    aRect = QwtDoubleRect( aRect.y(), aRect.x(),
                           aRect.height(), aRect.width() );
    if ( aRect.left() > myReference ) 
      aRect.setLeft( myReference );
    else if ( aRect.right() < myReference ) 
      aRect.setRight( myReference );
  } 
  else {
    if ( aRect.bottom() < myReference ) 
      aRect.setBottom( myReference );
    else if ( aRect.top() > myReference ) 
      aRect.setTop( myReference );
  }
  return aRect;
}

/*!
  Returns type of plot object
*/
int Plot2d_HistogramQwtItem::rtti() const
{
  return QwtPlotItem::Rtti_PlotHistogram;
}

/*!
  Sets histogram attributes
*/
void Plot2d_HistogramQwtItem::setHistogramAttribute( HistogramAttribute theAttr,
						     bool isOn )
{
  if ( testHistogramAttribute( theAttr ) != isOn ) {
    if ( isOn )
      myAttributes |= theAttr;
    else
      myAttributes &= ~theAttr;
    
    itemChanged();
  }
}

/*!
  Tests histogram attributes
*/
bool Plot2d_HistogramQwtItem::testHistogramAttribute( HistogramAttribute theAttr ) const
{
  return myAttributes & theAttr;
}

/*!
  Draws histogram object
*/
void Plot2d_HistogramQwtItem::draw( QPainter* thePainter,
				    const QwtScaleMap& theXMap, 
				    const QwtScaleMap& theYMap,
				    const QRect& ) const
{
  thePainter->setPen( QPen( myColor ) );

  const int x0 = theXMap.transform( baseline() );
  const int y0 = theYMap.transform( baseline() );

  for ( int i = 0; i < (int)myData.size(); i++ ) {
    if ( myAttributes & Plot2d_HistogramQwtItem::Xfy ) {
      const int x2 = theXMap.transform( myData.value( i ) );
      if ( x2 == x0 )
        continue;
      int y1 = theYMap.transform( myData.interval( i ).minValue() );
      int y2 = theYMap.transform( myData.interval( i ).maxValue() );
      if ( y1 > y2 )
        qSwap( y1, y2 );

      if ( i < (int)myData.size() - 2 ) {
        const int yy1 = theYMap.transform( myData.interval(i+1).minValue() );
        const int yy2 = theYMap.transform( myData.interval(i+1).maxValue() );
        if ( y2 == qwtMin( yy1, yy2 ) ) {
          const int xx2 = theXMap.transform( myData.interval(i+1).minValue() );
          if ( xx2 != x0 && ( ( xx2 < x0 && x2 < x0 ) ||
                              ( xx2 > x0 && x2 > x0 ) ) ) {
            // One pixel distance between neighboured bars
            y2++;
          }
        }
      }
      drawBar( thePainter, Qt::Horizontal, QRect( x0, y1, x2 - x0, y2 - y1 ) );
    }
    else {
      const int y2 = theYMap.transform( myData.value( i ) );
      if ( y2 == y0 )
        continue;
      int x1 = theXMap.transform( myData.interval( i ).minValue() );
      int x2 = theXMap.transform( myData.interval( i ).maxValue() );
      if ( x1 > x2 )
        qSwap( x1, x2 );

      if ( i < (int)myData.size() - 2 ) {
        const int xx1 = theXMap.transform( myData.interval(i+1).minValue() );
        const int xx2 = theXMap.transform( myData.interval(i+1).maxValue() );
        if ( x2 == qwtMin( xx1, xx2 ) ) {
          const int yy2 = theYMap.transform( myData.value(i+1) );
          if ( yy2 != y0 && ( ( yy2 < y0 && y2 < y0 ) ||
                              ( yy2 > y0 && y2 > y0 ) ) ) {
            // One pixel distance between neighboured bars
            x2--;
          }
        }
      }
      drawBar( thePainter, Qt::Vertical, QRect( x1, y0, x2 - x1, y2 - y0 ) );
    }
  }
}

/*!
  Draws single bar of histogram
*/
void Plot2d_HistogramQwtItem::drawBar( QPainter* thePainter,
				       Qt::Orientation,
				       const QRect& theRect ) const
{
  thePainter->save();

  const QColor color( thePainter->pen().color() );
  QRect r = theRect.normalized();

  const int factor = 125;
  const QColor light( color.light( factor ) );
  const QColor dark( color.dark( factor ) );

  thePainter->setBrush( color );
  thePainter->setPen( Qt::NoPen );
  QwtPainter::drawRect( thePainter, r.x() + 1, r.y() + 1,
                        r.width() - 2, r.height() - 2 );
  thePainter->setBrush( Qt::NoBrush );

  thePainter->setPen( QPen( light, 2 ) );
  QwtPainter::drawLine( thePainter, r.left() + 1, r.top() + 2,
                        r.right() + 1, r.top() + 2 );

  thePainter->setPen( QPen( dark, 2 ) );
  QwtPainter::drawLine( thePainter, r.left() + 1, r.bottom(),
                        r.right() + 1, r.bottom() );
  thePainter->setPen( QPen( light, 1 ) );

  QwtPainter::drawLine( thePainter, r.left(), r.top() + 1,
                        r.left(), r.bottom() );
  QwtPainter::drawLine( thePainter, r.left() + 1, r.top() + 2,
                         r.left() + 1, r.bottom() - 1 );
  thePainter->setPen( QPen( dark, 1 ) );

  QwtPainter::drawLine( thePainter, r.right() + 1, r.top() + 1,
                        r.right() + 1, r.bottom() );
  QwtPainter::drawLine(thePainter, r.right(), r.top() + 2,
                        r.right(), r.bottom() - 1 );
  thePainter->restore();
}

/*!
  Constructor
*/
Plot2d_HistogramItem::Plot2d_HistogramItem( const QwtText& theTitle )
: Plot2d_HistogramQwtItem( theTitle ),
  Plot2d_SelectableItem(), 
  myCrossed( true )
{
}

/*!
  Constructor
*/
Plot2d_HistogramItem::Plot2d_HistogramItem( const QString& theTitle )
: Plot2d_HistogramQwtItem( theTitle ),
  myCrossed( true )
{
}

/*!
  Destructor
*/
Plot2d_HistogramItem::~Plot2d_HistogramItem()
{
}

/*!
  Get histogram bar items
*/
QList<QRect> Plot2d_HistogramItem::getBars() const
{
  return myBarItems;
}

/*!
  Set to legend item symbol with color of item
*/
void Plot2d_HistogramItem::updateLegend( QwtLegend* theLegend ) const
{
  if ( !theLegend )
    return;
 
  Plot2d_HistogramQwtItem::updateLegend( theLegend );

  QWidget* theWidget = theLegend->find( this );
  if ( !theWidget || !theWidget->inherits( "QwtLegendItem" ) )
    return;

  Plot2d_QwtLegendItem* anItem = ( Plot2d_QwtLegendItem* )theWidget;
  QFontMetrics aFMetrics( anItem->font() );
  int aSize = aFMetrics.height();
  QwtSymbol aSymbol( QwtSymbol::Rect, QBrush( legendPen().color() ),
                     QPen( legendPen().color() ), QSize( aSize, aSize ) );
  anItem->setSymbol( aSymbol );
  anItem->setIdentifierMode( theLegend->identifierMode()
			     | QwtLegendItem::ShowSymbol ); 
  anItem->setSelected(isSelected());
  anItem->updateHighlit();
  anItem->update();
}

/*!
  Draws histogram object
*/
void Plot2d_HistogramItem::draw( QPainter* thePainter,
				 const QwtScaleMap& theXMap, 
				 const QwtScaleMap& theYMap,
				 const QRect& ) const
{
  // nds: clear list of bar items
  Plot2d_HistogramItem* anItem = (Plot2d_HistogramItem*)this;
  anItem->myBarItems.clear();

  thePainter->setPen( QPen( color() ) );
  const int x0 = theXMap.transform( baseline() );
  const int y0 = theYMap.transform( baseline() );

  const QwtIntervalData& iData = data();
  
  for ( int i = 0; i < (int)iData.size(); i++ ) {
    if ( testHistogramAttribute( Plot2d_HistogramItem::Xfy ) ) {
      const int x2 = theXMap.transform( iData.value( i ) );
      if ( x2 == x0 )
        continue;
      int y1 = theYMap.transform( iData.interval( i ).minValue() );
      int y2 = theYMap.transform( iData.interval( i ).maxValue() );
      if ( y1 > y2 )
        qSwap( y1, y2 );

      if ( i < (int)iData.size() - 2 ) {
        const int yy1 = theYMap.transform( iData.interval(i+1).minValue() );
        const int yy2 = theYMap.transform( iData.interval(i+1).maxValue() );
        if ( y2 == qwtMin( yy1, yy2 ) ) {
          const int xx2 = theXMap.transform( iData.interval(i+1).minValue() );
          if ( xx2 != x0 && ( ( xx2 < x0 && x2 < x0 ) ||
                              ( xx2 > x0 && x2 > x0 ) ) ) {
            // One pixel distance between neighboured bars
            y2++;
          }
        }
      }
      // nds: draw rect with the other lower rects
      QRect aRect( x0, y1, x2 - x0, y2 - y1 );
      drawRectAndLowers( thePainter, Qt::Horizontal, aRect );
      anItem->myBarItems.append( aRect );
    }
    else {
      const int y2 = theYMap.transform( iData.value( i ) );
      if ( y2 == y0 )
        continue;
      int x1 = theXMap.transform( iData.interval( i ).minValue() );
      int x2 = theXMap.transform( iData.interval( i ).maxValue() );
      if ( x1 > x2 )
        qSwap( x1, x2 );

      if ( i < (int)iData.size() - 2 ) {
        const int xx1 = theXMap.transform( iData.interval(i+1).minValue() );
        const int xx2 = theXMap.transform( iData.interval(i+1).maxValue() );
        if ( x2 == qwtMin( xx1, xx2 ) ) {
          const int yy2 = theYMap.transform( iData.value(i+1) );
          if ( yy2 != y0 && ( ( yy2 < y0 && y2 < y0 ) ||
                              ( yy2 > y0 && y2 > y0 ) ) ) {
            // One pixel distance between neighboured bars
            x2--;
          }
        }
      }
      // nds: draw rect with the other lower rects
      QRect aRect(x1, y0, x2 - x1, y2 - y0 );
      drawRectAndLowers( thePainter, Qt::Vertical, aRect );
      anItem->myBarItems.append( aRect );
    }
  }
}

/*!
  Set/clear "cross items" option
*/
void Plot2d_HistogramItem::setCrossItems( bool theCross )
{
  myCrossed = theCross;
}

/*!
  Get "cross items" option
*/
bool Plot2d_HistogramItem::isCrossItems() const
{
  return myCrossed;
}

/*!
  Redefined method, which creates and returns legend item of the curve
*/
QWidget* Plot2d_HistogramItem::legendItem() const
{
  return new Plot2d_QwtLegendItem;
}


/*!
  Draws bar of histogram and on it bars of histograms with lower height.
*/
void Plot2d_HistogramItem::drawRectAndLowers( QPainter* thePainter,
					      Qt::Orientation theOr,
					      const QRect& theRect ) const
{
  QRect aRect = theRect;
  // theRect has inversed coordinates on Y axis.
  // The top of the rect is bottom in standard QRect coordinates, 
  // and it bottom is the top.
  if ( myCrossed )//&& theOr == Qt::Horizontal )
    aRect.setTop( getCrossedTop( theRect ) );

  drawBar( thePainter, Qt::Horizontal, aRect );
}

/*!
  Returns top value of the given rect in the context of other bars.

  It's necessary to remember, that \a theRect has inverted coordinate Y.
*/
int Plot2d_HistogramItem::getCrossedTop( const QRect& theRect ) const
{
  int aRes = theRect.top();
  QwtPlot* aPlot = plot();
  // int aHeight = theRect.height();
  if ( aPlot ) {
    QwtPlotItemList anItems = aPlot->itemList();
    QwtPlotItemIterator anIt = anItems.begin(), aLast = anItems.end();
    Plot2d_HistogramItem* anItem;
    QList<QRect> aRects;
    for ( ; anIt != aLast; anIt++ ) {
      if ( !(*anIt)->rtti() == QwtPlotItem::Rtti_PlotHistogram )
        continue;
      anItem = dynamic_cast<Plot2d_HistogramItem*>( *anIt );
      if( !anItem || anItem == this )
        continue;
      aRects.clear();
      aRects = anItem->getBars();
      for ( int i = 0, aSize = aRects.size(); i < aSize; i++ ) {
        if ( qMax( theRect.x(), aRects[i].x() ) <=
             qMin( theRect.left(), aRects[i].left() ) ) {
          if ( theRect.bottom() < aRects[i].bottom() )
            if ( aRects[i].bottom() < aRes )
	      aRes = aRects[i].bottom();
        }
      }
    }
  }
  return aRes;
}
