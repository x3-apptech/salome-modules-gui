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
#include "SalomeStyle.h"

#include <qimage.h>
#include <qcursor.h>
#include <qtabbar.h>
#include <qpalette.h>
#include <qtoolbar.h>
#include <qspinbox.h>
#include <qpainter.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <qdrawutil.h>
#include <qpopupmenu.h>
#include <qpushbutton.h>
#include <qobjectlist.h>
#include <qtoolbutton.h>
#include <qapplication.h>

static const char* const hole_xpm[] = {
"3 3 3 1",
". c None",
"a c #999999",
"b c #FFFFFF",
"aa.",
"aab",
".bb"
};

static const char* const cross_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
"............",
"............",
".aaaa..aaaa.",
"..abbaabba..",
"...abbbba...",
"....abba....",
"...abbbba...",
"..abbaabba..",
".aaaa..aaaa.",
"............",
"............"
};

static const char* const maximize_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
".aaaaaaaaaa.",
".acccccccca.",
".acccccccca.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".abbbbbbbba.",
".aaaaaaaaaa.",
"............"
};

static const char* const normal_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
"...aaaaaaaa.",
"...acccccca.",
"...abbbbbba.",
"...aaaaaaba.",
".aaaaaaaaba.",
".accccccaba.",
".abbbbbbaaa.",
".abbbbbba...",
".abbbbbba...",
".aaaaaaaa...",
"............"
};

static const char* const minimize_xpm[] = {
"12 12 4 1",
". c None",
"a c #000000",
"b c #FFFFFF",
"c c #666666",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"............",
"aaaaaaaaaaaa",
"abbbbbbbbbba",
"aaaaaaaaaaaa",
"............"
};

/*!
    Class: SalomeStyle [Public]
    Descr: Style for SALOME platform
*/

/*!
  Constructor
*/
SalomeStyle::SalomeStyle()
: myTitleParent( 0 )
{
  qApp->installEventFilter( this ); 
}

/*!
  Destructor
*/
SalomeStyle::~SalomeStyle()
{
}

/*!
  Delayed initialization of style
*/
void SalomeStyle::polish( QWidget* w )
{
  if ( !w )
    return;
/*
  if ( w->inherits( "QLineEdit" ) )
  {
    QLineEdit* fr = (QLineEdit*)w;
    if ( w->parentWidget() && w->parentWidget()->inherits( "QSpinWidget" ) )
      fr->setFrameStyle( QFrame::Plain );
    else
    {
      fr->setFrameStyle( QFrame::Plain | QFrame::Box );
      fr->setLineWidth( 1 );
      fr->setMidLineWidth( 0 );
    }
  }
  else
*/
  if ( w->inherits( "QToolBar" ) )
  {
    QToolBar* tb = (QToolBar*)w;
    tb->setFrameStyle( QFrame::ToolBarPanel | QFrame::Plain );
    tb->setLineWidth( 1 );
    tb->setMidLineWidth( 1 );
  }
  else
  if ( w->inherits( "QDockWindow" ) )
  {
    QDockWindow* dw = (QDockWindow*)w;
    dw->setFrameStyle( QFrame::ToolBarPanel | QFrame::Raised );
  }
/*
  else if ( w->inherits( "QListView" ) )
  {
    QListView* lv = (QListView*)w;
    lv->setFrameStyle( QFrame::Box | QFrame::Plain );
  }
*/
  else
    PARENT_STYLE::polish( w );
}

/*!
  Custom event filter
*/
bool SalomeStyle::eventFilter( QObject* o, QEvent* e )
{
  if ( e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut )
  {
    QDockWindow* wnd = NULL;
    while( o && !wnd )
      if( o->inherits( "QDockWindow" ) )
        wnd = ( QDockWindow* )o;
      else
        o = o->parent();

    if ( wnd )
    {
      const QObjectList* objList = o->children();
      for( QObjectListIterator anIt( *objList ); anIt.current(); ++anIt )
        if( anIt.current()->inherits( "QDockWindowHandle" ) )
        {
          QWidget* ww = ( QWidget* )anIt.current();
          ww->repaint( ww->rect() );
          const QObjectList* wc = ww->children();
          for( QObjectListIterator anIt2( *wc ); anIt2.current(); ++anIt2 )
            if( anIt2.current()->inherits( "QToolButton" ) )
            {
              QWidget* tb = ( QWidget* )anIt2.current();
              tb->repaint( tb->rect() );
            }
        }      
    }
  }

  if ( o && o->inherits( "QPushButton" ) && ( e->type() == QEvent::Enter || e->type() == QEvent::Leave ) )
    ((QWidget*)o)->update();

  return false;
}

double linear( double x )
{
  return x;
}

/*!
  Mixes two colors, part of first is 1-t, part of second is t
  \param t - part parameter
  \param c1, c2 - colors
  \param res - result color
*/
void SalomeStyle::mix( const double t, const QColor& c1, const QColor& c2, QColor& res )
{
  if( t<0.0 || t>1.0 )
    return;

  int r = int( c1.red() * (1-t) + c2.red() * t ),
      g = int( c1.green() * (1-t) + c2.green() * t ),
      b = int( c1.blue() * (1-t) + c2.blue() * t );
  res.setRgb( r, g, b );
}

/*!
  Mixes two colors, part of first is 1-t, part of second is t
  \param t - part parameter
  \param rgb1, rgb2 - colors (result is stored in rgb1)
*/
void SalomeStyle::mix( const double t, QRgb& rgb1, const QRgb& rgb2 )
{
  if( t<0.0 || t>1.0 )
    return;

  int c[2][4] = { qRed( rgb1 ), qGreen( rgb1 ), qBlue( rgb1 ), qAlpha( rgb1 ),
                  qRed( rgb2 ), qGreen( rgb2 ), qBlue( rgb2 ), qAlpha( rgb2 ) };
  for( int i=0; i<4; i++ )
    c[0][i] = (int)( c[0][i] * (1-t) + c[1][i] * t );

  rgb1 = qRgba( c[0][0], c[0][1], c[0][2], qAlpha( rgb1 ) );
}

/*!
  Mixes colors of pixmap points with other color
  \param t - part parameter
  \param pix - pixmap to be processed
  \param col - other color
*/
void SalomeStyle::mix( const double t, QPixmap& pix, const QColor& col )
{
  if( t<0.0 || t>1.0 )
    return;

  QImage anImage = pix.convertToImage();
  if( anImage.isNull() )
    return;

  if( anImage.depth()!=32 )
    anImage = anImage.convertDepth( 32 );

  int w = anImage.width(),
      h = anImage.height();

  QRgb colrgb = col.rgb(), cur;
  for( int y=0; y<h; y++ )
  {
    uchar* line = anImage.scanLine( y );
    QRgb*  colorline = ( QRgb* ) line;
    for( int x=0; x<w; x++ )
    {
      cur = colorline[ x ];
      mix( t, cur, colrgb );
      colorline[ x ] = cur;
    }
  }
  pix = anImage;
}

/*!
  Converts pixmap to grayscale
  \param pix - pixmap to be processed
  \param k - factor (gray value after conversion will be multiplied on it and truncated by 255.0)
*/
void SalomeStyle::toGrayscale( QPixmap& pix, double k )
{
  QImage anImage = pix.convertToImage();
  if( anImage.isNull() )
    return;

  if( anImage.depth()!=32 )
    anImage = anImage.convertDepth( 32 );

  int w = anImage.width(),
      h = anImage.height();

  for( int y=0; y<h; y++ )
  {
    uchar* line = anImage.scanLine( y );
    QRgb*  colorline = ( QRgb* ) line;
    for( int x=0; x<w; x++ )
    {
      int gray = (int)( k * qGray( colorline[ x ] ) );
      if ( gray>255 )
        gray = 255;
      else if( gray<0 )
        gray = 0;
      colorline[ x ] = qRgba( gray, gray, gray, qAlpha( colorline[x] ) );
    }
  }
  pix = anImage;
}

/*!
  Draws gradient filling
  \param p - painter
  \param r - rect
  \param c1, c2 - two colors of gradient
  \param d - direction of gradient
  \param f - gradient function (maps co-ordinate to part parameter)
*/
void SalomeStyle::drawGradient( QPainter* p, const QRect& r,
                                    const QColor& c1, const QColor& c2,
                                    const Direction d, gradient_func f ) const 
{
  p->save();
  p->translate( r.x(), r.y() );

  QColor mid;
  if( d==LeftToRight || d==RightToLeft )
    for( int xmax = r.width(), x = d==LeftToRight ? 0 : xmax;
         d==LeftToRight ? x<=xmax : x>=0;
         d==LeftToRight ? x++ : x-- )
    {
      double t = double( x ) / double( xmax );
      mix( d==LeftToRight ? f(t) : 1-f(t), c1, c2, mid );
      p->setPen( mid );
      p->drawLine( x, 0, x, r.height() );
    }
  else
    for( int ymax = r.height(), y = d==UpToDown ? 0 : ymax;
         d==UpToDown ? y<=ymax : y>=0;
         d==UpToDown ? y++ : y-- )
    {
      double t = double( y ) / double( ymax );
      mix( d==UpToDown ? f(t) : 1-f(t), c1, c2, mid );
      p->setPen( mid );
      p->drawLine( 0, y, r.width(), y );
    }

  p->restore();
}

/*!
  Draws primitive element
  \param pe - type of primitive element
  \param p - painter
  \param r - rect
  \param cg - group of colors
  \param flags - is used to control how the PrimitiveElement is drawn
  \param opt - can be used to control how various PrimitiveElements are drawn
*/
void SalomeStyle::drawPrimitive( PrimitiveElement pe, QPainter* p, const QRect& r,
				 const QColorGroup& cg, SFlags flags, const QStyleOption& opt ) const
{
  switch ( pe )
  {
  case PE_ButtonTool:
    if ( !myTitleParent )
      PARENT_STYLE::drawPrimitive( pe, p, r, cg, flags, opt );
    break;

  case PE_DockWindowResizeHandle:
    p->fillRect( r.x(), r.y(), r.width(), r.height(), cg.dark() );
    if ( flags & Style_Horizontal )
      p->fillRect( r.x(), r.y() + 1, r.width(), r.height() - 2, cg.midlight() );
    else
      p->fillRect( r.x() + 1, r.y(), r.width() - 2, r.height(), cg.midlight() );
    break;

  case PE_DockWindowHandle:
    {
      QPaintDevice* pd = p->device();
      if ( !pd )
        break;

      QWidget* w = (QWidget*)pd;

      if ( !w || !w->inherits( "QDockWindowHandle" ) )
        break;

      QDockWindow* wnd = ( QDockWindow* ) w->parent();
      if ( !wnd )
        return;

      QObject* activeWidget = qApp->focusWidget();
      QDockWindow* activeWnd = 0;
      while ( !activeWnd && activeWidget )
      {
        if ( activeWidget->inherits( "QDockWindow" ) )
          activeWnd = (QDockWindow*)activeWidget;
        else
          activeWidget = activeWidget->parent();
      }

      bool act = wnd == activeWnd, horiz = flags & Style_Horizontal;

      QPixmap hole( (const char**)hole_xpm );

      if ( wnd->inherits( "QToolBar" ) )
      {
        drawGradient( p, r, cg.light(), cg.background(), horiz ? UpToDown : LeftToRight, linear );

        int c = 4, i; double d = ( horiz ? r.height() : r.width() ) / ( c + 1 );
        QBrush fill = cg.brush( QColorGroup::Dark );
        p->setPen( Qt::red );
        for ( i = 0; i < c; i++ )
        {
          if ( horiz )
            p->drawPixmap( r.x() + r.width() / 2 - 1, (int)( r.y() + ( i + 1 ) * d - 1 ), hole );
          else
            p->drawPixmap( (int) ( r.x() + ( i + 1 ) * d - 1 ), r.y() + r.height() / 2 - 1, hole );
        }

        int dd = (int(d)/2) + (int(d)%2);
        for ( i = 0; i < c - 1; i++ )
        {
          if ( horiz )
            p->drawPixmap( r.x() + r.width() / 2 + 2, (int)( r.y() + dd + ( i + 1 ) * d - 1 ), hole );
          else
            p->drawPixmap( (int)( r.x() + dd + ( i + 1 ) * d - 1 ), r.y() + r.height() / 2 + 2, hole );
        }
      }
      else
      {
        int d = 1;
        QRect rr( r.x()+d, r.y()+d, r.width()-2*d, r.height()-2*d );

        QColor col; 
        if ( act )
          mix( 0.5, cg.highlight(), Qt::white, col );
        else
          col = cg.background();

        drawGradient( p, rr, act ? cg.highlight() : cg.dark(), col,
                      horiz ? LeftToRight : UpToDown, linear );

		    QRect rt = rr;
        if ( flags & Style_Horizontal )
          rt.addCoords( 0, 20, 0, 0 );
        else
          rt.addCoords( 0, 0, -20, 0 );

        int textW = flags & Style_Horizontal ? rt.height() : rt.width();
        int textH = flags & Style_Horizontal ? rt.width() : rt.height();

        QString title = titleText( wnd->caption(), textW, p->fontMetrics() );

		    if ( wnd )
        {
		      QColorGroup cgroup = wnd->isActiveWindow() ? wnd->palette().active() : wnd->palette().inactive();
		      p->setPen( cgroup.highlightedText() );

		      if ( flags & Style_Horizontal )
          {
		        p->rotate( 270.0 );
		        p->translate( -(rt.height()+rt.y()), (rt.width()-rt.x()) );
		        p->drawText( 0, 0, title );
		      }
          else
		        p->drawText( 2, 2, textW, textH, AlignLeft, title );
	      }
      }
      break;
    }

  case PE_DockWindowSeparator:
    {
      bool horiz = ( flags & Style_Horizontal );
      QWidget* w = ( QWidget* )p->device();
      if( w->inherits( "QToolBarSeparator" ) )
      {
        drawGradient( p, r, cg.light(), cg.background(), horiz ? UpToDown : LeftToRight, linear );
        int w = r.width(), h = r.height();
        int part = 8;

        p->setPen( cg.dark() );
        if( horiz )
          p->drawLine( r.x() + w/2, r.y() + h/part, r.x() + w/2, r.y() + (part-1)*h/part );
        else
          p->drawLine( r.x() + w/part, r.y() + h/2, r.x() + (part-1)*w/part, r.y() + h/2 );

        p->setPen( cg.light() );
        if( horiz )
          p->drawLine( r.x() + w/2 + 1, r.y() + h/part + 1, r.x() + w/2 + 1, r.y() + (part-1)*h/part + 1 );
        else
          p->drawLine( r.x() + w/part + 1, r.y() + h/2 + 1, r.x() + (part-1)*w/part + 1, r.y() + h/2 + 1 );
      }
      else
        PARENT_STYLE::drawPrimitive( pe, p, r, cg, flags, opt );
      break;
    }
    
  case PE_PanelDockWindow:
    {
      QWidget* w = ( QWidget* )p->device();

      int offset = pixelMetric( PM_DockWindowHandleExtent, w );
      if ( w->inherits( "QToolBar" ) )
      {
        bool vert = ( ( QToolBar* )w )->orientation() == Qt::Vertical;
        if ( vert )
          drawGradient( p, QRect( r.x(), r.y()+offset, r.x()+r.width(), r.y()+offset ),
                        cg.light(), cg.background(), LeftToRight, linear );
        else
          drawGradient( p, QRect( r.x()+offset, r.y(), r.x()+offset, r.y()+r.height() ),
                        cg.light(), cg.background(), UpToDown, linear );

        p->setPen( QPen( cg.light(), 1 ) );
        p->drawRect( r );
      }
      else
      {
        p->setPen( QPen( cg.dark(), 1 ) );
        p->setBrush( cg.light() );
        p->drawRect( r );
      }

      break;
    }

  case PE_PanelPopup:
    p->setPen( QPen( cg.dark(), 1 ) );
    p->setBrush( cg.light() );
    p->drawRect( r );
    break;

  case PE_HeaderSection:
    {
      QColor col; mix( 0.2, cg.light(), Qt::black, col );
      drawGradient( p, r, cg.light(), col, DownToUp, linear );
      drawGradient( p, QRect( r.x(), r.y(), 1, r.height() ), cg.light(), cg.dark(), DownToUp, linear );
      p->setPen( cg.dark() );
      p->drawLine( r.x(), r.y()+r.height()-1, r.x()+r.width(), r.y()+r.height()-1 );
      break;
    }

  case PE_Splitter:
    p->fillRect( r.x() + 1, r.y() + 1, r.width() - 2, r.height() - 2, cg.dark() );
    p->fillRect( r.x() + 2, r.y() + 2, r.width() - 4, r.height() - 4, cg.midlight() );
	  break;

  case PE_ButtonBevel:
    PARENT_STYLE::drawPrimitive( PE_ButtonDropDown, p, r, cg, flags, opt );
    break;

  case PE_ButtonCommand:
    {
      QColorGroup cGrp( cg );
      if ( flags & Style_MouseOver )
        cGrp.setColor( QColorGroup::Button, cGrp.midlight() );

      if ( flags & Style_MouseOver )
        PARENT_STYLE::drawPrimitive( PE_ButtonDropDown, p, r, cGrp, flags, opt );
      else
        PARENT_STYLE::drawPrimitive( flags & Style_Raised ? PE_ButtonDropDown : pe, p, r, cGrp, flags, opt );
    }
    break;

  default:
    PARENT_STYLE::drawPrimitive( pe, p, r, cg, flags, opt );
    break;
  }
}

/*!
  Draws control element
  \param element - type of control element
  \param p - painter
  \param widget - widget
  \param r - rect
  \param cg - group of colors
  \param flags - is used to control how the element is drawn
  \param opt - can be used to control how various elements are drawn
*/
void SalomeStyle::drawControl( ControlElement element, QPainter* p, const QWidget* widget, const QRect& r,
                               const QColorGroup& cg, SFlags flags, const QStyleOption& opt ) const 
{
  switch( element )
  {
    case CE_PopupMenuItem:
    {
	  if(! widget || opt.isDefault())
	    break;

      const QPopupMenu *popupmenu = (const QPopupMenu *) widget;
      QMenuItem *mi = opt.menuItem();
      if( !mi )
	    break;

	  int tab = opt.tabWidth();
	  int maxpmw = opt.maxIconWidth();
	  bool dis = !( flags & Style_Enabled );
	  bool checkable = popupmenu->isCheckable();
	  bool act = flags & Style_Active;
      bool use2000style = true;
	  int x, y, w, h;

      int windowsItemFrame = 2,
          windowsItemHMargin = 5,
          windowsItemVMargin = 1,
          windowsRightBorder = 2,
          windowsArrowHMargin = 2;

	  r.rect(&x, &y, &w, &h);

	  if ( checkable )
      {
	    // space for the checkmarks
        if (use2000style)
		  maxpmw = QMAX( maxpmw, 20 );
		else
		  maxpmw = QMAX( maxpmw, 12 );
	  }

	  int checkcol = maxpmw;

	  if ( mi && mi->isSeparator() )
      {
		p->setPen( cg.dark() );
		p->drawLine( x+3*checkcol/2, y, x+w, y );
		//p->setPen( cg.light() );
		//p->drawLine( x, y+1, x+w, y+1 );
		return;
	  }

      QBrush fill = ( act && !dis ?
	         cg.brush( QColorGroup::Highlight ) :
			 cg.brush( QColorGroup::Light ) );
	  p->fillRect( x, y, w, h, fill );

	  if( !mi )
		return;

	  int xpos = x;
	  QRect vrect = visualRect( QRect( xpos, y, checkcol, h ), r );
	  int xvis = vrect.x();

	  if( mi->isChecked() )
      {
	    if( act && !dis )
		  qDrawShadePanel( p, xvis, y, checkcol, h,
                           cg, TRUE, 1, &cg.brush( QColorGroup::Button ) );
		else
        {
		  QBrush fill( cg.light(), Dense4Pattern );
		  // set the brush origin for the hash pattern to the x/y coordinate
		  // of the menu item's checkmark... this way, the check marks have
		  // a consistent look
		  QPoint origin = p->brushOrigin();
		  p->setBrushOrigin( xvis, y );
		  qDrawShadePanel( p, xvis, y, checkcol, h, cg, TRUE, 1, &fill );
          // restore the previous brush origin
		  p->setBrushOrigin( origin );
		}
	  }
      else if( !act )
		p->fillRect(x+checkcol, y, w-checkcol , h, cg.brush( QColorGroup::Light ));

      if( !( act && !dis ) && !mi->isChecked() )
      {
        QColor col; mix( 0.5, cg.mid(), Qt::white, col );
        drawGradient( p, QRect( xvis, y, checkcol, h+1 ), cg.light(), col, LeftToRight, linear );
      }

      if( mi->iconSet() )
      { // draw iconset
		QIconSet::Mode mode = dis ? QIconSet::Active : QIconSet::Normal;
		if(act && !dis )
		  mode = QIconSet::Active;
		QPixmap pixmap;
		if( checkable && mi->isChecked() )
		  pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode, QIconSet::On );
		else
		  pixmap = mi->iconSet()->pixmap( QIconSet::Small, mode );
		int pixw = pixmap.width();
		int pixh = pixmap.height();

        if( dis )
          toGrayscale( pixmap, 1.1 );

		//if( act && !dis && !mi->isChecked() )
		//  qDrawShadePanel( p, xvis, y, checkcol, h, cg, FALSE, 1, &cg.brush( QColorGroup::Button ) );

		QRect pmr( 0, 0, pixw, pixh );
		pmr.moveCenter( vrect.center() );
		p->setPen( cg.text() );
		p->drawPixmap( pmr.topLeft(), pixmap );

		QBrush fill = ( act ? cg.brush( QColorGroup::Highlight ) : cg.brush( QColorGroup::Light ));
		int xp = xpos + checkcol + 1;
        if( !dis )
		  p->fillRect( visualRect( QRect( xp, y, w - checkcol - 1, h ), r ), fill);
	  }
      else if( checkable )
      {  // just "checking"...
		if( mi->isChecked() )
        {
		  int xp = xpos + windowsItemFrame;

		  SFlags cflags = Style_Default;
		  if( !dis )
		    cflags |= Style_Enabled;
		  if( act )
			cflags |= Style_On;

		  drawPrimitive( PE_CheckMark, p,
				         visualRect( QRect( xp, y + windowsItemFrame,
					     checkcol - 2*windowsItemFrame,
					     h - 2*windowsItemFrame), r ), cg, cflags, opt );
		}
	  }

	  p->setPen( act ? cg.highlightedText() : cg.buttonText() );

	  QColor discol;
	  if( dis )
      {
		discol = cg.text();
		p->setPen( discol );
	  }

	  int xm = windowsItemFrame + checkcol + windowsItemHMargin;
	  xpos += xm;

	  vrect = visualRect( QRect( xpos, y+windowsItemVMargin, w-xm-tab+1, h-2*windowsItemVMargin ), r );
	  xvis = vrect.x();
	  if( mi->custom() )
      {
		p->save();
		if ( dis && !act )
        {
		  p->setPen( cg.light() );
		  mi->custom()->paint( p, cg, act, !dis,
					           xvis+1, y+windowsItemVMargin+1,
                               w-xm-tab+1, h-2*windowsItemVMargin );
		  p->setPen( discol );
		}
		mi->custom()->paint( p, cg, act, !dis, xvis, y+windowsItemVMargin,
                             w-xm-tab+1, h-2*windowsItemVMargin );
		p->restore();
	  }
	  QString s = mi->text();
	  if( !s.isNull() )
      { // draw text
		int t = s.find( '\t' );
		int text_flags = AlignVCenter | ShowPrefix | DontClip | SingleLine;
		if( !styleHint( SH_UnderlineAccelerator, widget ) )
		  text_flags |= NoAccel;
		text_flags |= (QApplication::reverseLayout() ? AlignRight : AlignLeft );
		if( t >= 0 )
        { // draw tab text
		  int xp = x + w - tab - windowsItemHMargin - windowsItemFrame + 1;
		  if( use2000style )
		    xp -= 20;
		  else
			xp -= windowsRightBorder;
		  int xoff = visualRect( QRect( xp, y+windowsItemVMargin, tab, h-2*windowsItemVMargin ), r ).x();
		  if( dis && !act )
          {
			p->setPen( cg.light() );
			p->drawText( xoff+1, y+windowsItemVMargin+1, tab, h-2*windowsItemVMargin, text_flags, s.mid( t+1 ));
			p->setPen( discol );
		  }
		  p->drawText( xoff, y+windowsItemVMargin, tab, h-2*windowsItemVMargin, text_flags, s.mid( t+1 ) );
		  s = s.left( t );
		}
		if( dis && !act )
        {
		  p->setPen( cg.light() );
		  p->drawText( xvis+1, y+windowsItemVMargin+1, w-xm-tab+1, h-2*windowsItemVMargin, text_flags, s, t );
		  p->setPen( discol );
		}
		p->drawText( xvis, y+windowsItemVMargin, w-xm-tab+1, h-2*windowsItemVMargin, text_flags, s, t );
	  }
      else if( mi->pixmap() )
      { // draw pixmap
		QPixmap *pixmap = mi->pixmap();
		if( pixmap->depth() == 1 )
		  p->setBackgroundMode( OpaqueMode );
		p->drawPixmap( xvis, y+windowsItemFrame, *pixmap );
		if( pixmap->depth() == 1 )
		  p->setBackgroundMode( TransparentMode );
	  }
	  if( mi->popup() )
      { // draw sub menu arrow
		int dim = (h-2*windowsItemFrame) / 2;
		PrimitiveElement arrow;
		arrow = ( QApplication::reverseLayout() ? PE_ArrowLeft : PE_ArrowRight );
		xpos = x+w - windowsArrowHMargin - windowsItemFrame - dim;
		vrect = visualRect( QRect(xpos, y + h / 2 - dim / 2, dim, dim), r );
		if( act )
        {
		  QColorGroup g2 = cg;
		  g2.setColor( QColorGroup::ButtonText, g2.highlightedText() );
		  drawPrimitive( arrow, p, vrect, g2, dis ? Style_Default : Style_Enabled, opt );
		}
        else
        {
		  drawPrimitive( arrow, p, vrect, cg, dis ? Style_Default : Style_Enabled, opt );
		}
	  }

      if( act && !dis )
      {
        p->setPen( cg.dark() );
        p->drawRect( r );
      }
	  break;
    }

    case CE_MenuBarItem:
	{
      bool active = flags & Style_Active;
	  bool down = flags & Style_Down;
	  QRect pr = r;

      p->save();
      if( active && down )
      {
        //QColor col; mix( 0.5, cg.mid(), Qt::white, col );
        //drawGradient( p, r, col, cg.light(), UpToDown, linear );
        p->setPen( cg.light() );
        p->setBrush( cg.light() );
        p->drawRect( r );
        
        p->setPen( cg.dark() );
        int w = r.width()-1, h = r.height();
        p->drawLine( r.x(), r.y(), r.x()+w, r.y() );
        p->drawLine( r.x(), r.y(), r.x(), r.y()+h );
        p->drawLine( r.x()+w, r.y(), r.x()+w, r.y()+h );

        p->translate( 1, 1 );
      }
      else
        drawGradient( p, r, cg.light(), cg.background(), active ? DownToUp : UpToDown, linear );

	  QCommonStyle::drawControl(element, p, widget, pr, cg, flags, opt);
      p->restore();
	  break;
	}

    case CE_MenuBarEmptyArea:
    {
      drawGradient( p, r, cg.light(), cg.background(), UpToDown, linear );
	  break;
    }

    case CE_ToolButtonLabel:
	{
      const QToolButton *toolbutton = (const QToolButton *) widget;
      QObject* parent = toolbutton->parent();

      QToolBar* tb = NULL;
      while( parent )
        if( parent->inherits( "QToolBar" ) )
        {
          tb = ( QToolBar* )parent;
          break;
        }
        else
          parent = parent->parent();
      parent = toolbutton->parent();

      int horiz = -1;
      if( tb )
      {
        if( tb->orientation()==Qt::Horizontal )
          horiz = 1;
        else
          horiz = 0;
        drawGradient( p, r, cg.light(), cg.background(), horiz ? UpToDown : LeftToRight, linear );
      }
      else if( parent->inherits( "QDockWindowHandle" ) )
      {
        QWidget* w = ( QWidget* ) parent;
        QDockWindow* wnd = ( QDockWindow* )w->parent();
        if( !wnd->orientation()==Qt::Horizontal )
          horiz=1;
        else
          horiz=0;

        QPoint pp( r.x(), r.y() ),
               qq = toolbutton->mapToParent( pp );
        
        bitBlt( p->device(), r.x(), r.y(), w, horiz ? qq.x()-r.width() : qq.x(),
                horiz ? qq.y() : qq.y()+r.height(),
                r.width(), r.height(), Qt::CopyROP, true );
      }

      bool highlight = ( flags & Style_MouseOver ),
           down = ( flags & ( Style_Down | Style_On ) ),
           en = toolbutton->isEnabled();

      if( down )
        if( horiz>=0 )
        {
          drawGradient( p, r, cg.light(), cg.background(), horiz==1 ? DownToUp : RightToLeft, linear );
          p->setPen( cg.background() );
          p->drawRect( r );
        }
        else
        {
          p->setPen( cg.dark() );
          p->setBrush( cg.light() );
          p->drawRect( r );
        }
      if( highlight )
      {
        p->setPen( cg.dark() );
        QColor col; mix( 0.05, cg.background(), Qt::black, col );
        p->setBrush( col );
        p->drawRect( r );
      }


	  QRect rect = r;
	  Qt::ArrowType arrowType = opt.isDefault() ? Qt::DownArrow : opt.arrowType();

	  int shiftX = 0, shiftY = 0;
	  if( flags & ( Style_Down | Style_On ) )
      {
		shiftX = pixelMetric(PM_ButtonShiftHorizontal, widget);
		shiftY = pixelMetric(PM_ButtonShiftVertical, widget);
	  }

	  if (!opt.isDefault())
      {
		PrimitiveElement pe;
		switch (arrowType)
        {
		  case Qt::LeftArrow:  pe = PE_ArrowLeft;  break;
		  case Qt::RightArrow: pe = PE_ArrowRight; break;
		  case Qt::UpArrow:    pe = PE_ArrowUp;    break;
		  default:
		  case Qt::DownArrow:  pe = PE_ArrowDown;  break;
        }

        rect.moveBy(shiftX, shiftY);
	    drawPrimitive(pe, p, rect, cg, flags, opt);
	  }
      else
      {
		QColor btext = toolbutton->paletteForegroundColor();

		if (toolbutton->iconSet().isNull() &&
		    ! toolbutton->text().isNull() &&
		    ! toolbutton->usesTextLabel())
        {
		    int alignment = AlignCenter | ShowPrefix;
		    if (!styleHint(SH_UnderlineAccelerator, widget, QStyleOption::Default, 0))
			  alignment |= NoAccel;

		    rect.moveBy(shiftX, shiftY);
		    drawItem( p, rect, alignment, cg, flags & Style_Enabled, 0, toolbutton->text(),
			          toolbutton->text().length(), &btext);
        }
        else
        {
	      QPixmap pm;
		  QIconSet::Size size =
		  toolbutton->usesBigPixmap() ? QIconSet::Large : QIconSet::Small;
		  QIconSet::State state = toolbutton->isOn() ? QIconSet::On : QIconSet::Off;
		  QIconSet::Mode mode;
		  if( !toolbutton->isEnabled() )
	        mode = QIconSet::Active; //QIconSet::Disabled;
		  else if( flags & (Style_Down | Style_On ) || ( flags & Style_Raised ) && (flags & Style_AutoRaise))
 		    mode = QIconSet::Active;
		  else
		    mode = QIconSet::Normal;
		  pm = toolbutton->iconSet().pixmap( size, mode, state );

		  if( toolbutton->usesTextLabel() )
          {
		    p->setFont( toolbutton->font() );
		    QRect pr = rect, tr = rect;
		    int alignment = ShowPrefix;
		    if (!styleHint(SH_UnderlineAccelerator, widget, QStyleOption::Default, 0))
			  alignment |= NoAccel;

		    if( toolbutton->textPosition() == QToolButton::Under )
            {
			  int fh = p->fontMetrics().height();
			  pr.addCoords( 0, 1, 0, -fh-3 );
			  tr.addCoords( 0, pr.bottom(), 0, -3 );
			  pr.moveBy(shiftX, shiftY);
			  drawItem( p, pr, AlignCenter, cg, TRUE, &pm, QString::null );
			  alignment |= AlignCenter;
            }
            else
            {
  			  pr.setWidth( pm.width() + 8 );
			  tr.addCoords( pr.right(), 0, 0, 0 );
			  pr.moveBy(shiftX, shiftY);
 
			  drawItem( p, pr, AlignCenter, cg, TRUE, &pm, QString::null );
			  alignment |= AlignLeft | AlignVCenter;
            }

	        tr.moveBy(shiftX, shiftY);
		    drawItem( p, tr, alignment, cg, flags & Style_Enabled, 0, toolbutton->textLabel(),
				      toolbutton->textLabel().length(), &btext);
          }
          else
          {
            /*if( en && !highlight )
              mix( 0.6, pm, cg.background() );
            else*/ if( !en )
            {
              toGrayscale( pm, 1.1 );
              mix( 0.8, pm, cg.background() );
            }

		    rect.moveBy(shiftX, shiftY);
		    drawItem( p, rect, AlignCenter, cg, true, &pm, QString::null );
          }
        }
      }
    }
    break;

    case CE_TabBarTab:
	  {
      QPen oldPen = p->pen();

	    QRect r2( r );
	    bool selected = flags & Style_Selected;

      p->setPen( cg.midlight() );
      p->drawLine( r2.left(), r2.bottom(), r2.right(), r2.bottom() );

      if ( !selected )
        r2.setRect( r2.left(), r2.top() + 1, r2.width(), r2.height() - 1 );
      else
        r2.setRect( r2.left(), r2.top(), r2.width() - 2, r2.height() - 1 );

      p->setPen( cg.foreground() );
      drawGradient( p, QRect( r2.left(), r2.top(), r2.width(), r2.height() - 1 ),
                    selected ? cg.highlight() : cg.dark(), cg.background(), UpToDown, linear );

//      p->fillRect( r2.left(), r2.top(), r2.width(), r2.height() - 1, selected ? cg.highlight() : cg.background() );
      p->drawRect( r2.left(), r2.top(), r2.width(), r2.height() - 1 );

      if ( selected )
      {
        p->setPen( cg.shadow() );
        p->drawLine( r2.left() + 1, r2.bottom(), r2.right() + 1, r2.bottom() );
        p->drawLine( r2.right() + 1, r2.top() + 1, r2.right() + 1, r2.bottom() );
      }

	    break;
    }

    case CE_TabBarLabel:
      {
        QColorGroup tmp( cg );
        if ( flags & Style_Selected )
          tmp.setColor( QColorGroup::Foreground, tmp.highlightedText() );
        PARENT_STYLE::drawControl( element, p, widget, r, tmp, flags, opt );
      }
      break;

    case CE_PushButton:
	  {
      if ( flags & Style_Enabled && !( flags & Style_Down || flags & Style_On ) )
      {
	      const QPushButton* button = (const QPushButton*)widget;
        QRect br = QRect( button->mapToGlobal( button->rect().topLeft() ), button->rect().size() );
        if ( br.contains( QCursor::pos() )  )
          flags |= Style_MouseOver;
      }

      PARENT_STYLE::drawControl( element, p, widget, r, cg, flags, opt );
	    break;
	  }

    default:
      PARENT_STYLE::drawControl( element, p, widget, r, cg, flags, opt );
      break;
  }
}

/*!
  \return the sub-area for the widget in logical coordinates
  \param subrect - type of sub area
  \param w - widget
*/
QRect SalomeStyle::subRect( SubRect subrect, const QWidget* w ) const
{
  QRect r = PARENT_STYLE::subRect( subrect, w );
  if ( subrect==SR_DockWindowHandleRect && w->inherits( "QDockWindowHandle" ) )
  {
    QDockWindow* wnd = ( QDockWindow* ) w->parent();
    if( wnd->orientation()==Qt::Vertical )
      r.setWidth( w->width() );
    else
    {
      r.setY( r.y()+r.height()-w->height() );
      r.setHeight( w->height() );
    }
  }

  return r;
}

/*!
  Draws the ComplexControl
  \param control - type of complex control element
  \param p - painter
  \param widget - widget
  \param r - rect
  \param cg - group of colors
  \param flags, controls, active - is used to control how the element is drawn
  \param opt - can be used to control how various elements are drawn
*/
void SalomeStyle::drawComplexControl( ComplexControl control, QPainter* p, const QWidget* widget,
				                              const QRect& r, const QColorGroup& cg, SFlags flags,
				                              SCFlags controls, SCFlags active, const QStyleOption& opt ) const
{




  ((SalomeStyle*)this)->myTitleParent = 0;

  switch ( control )
  {
  case CC_ComboBox:
	  if ( controls & SC_ComboBoxArrow )
    {
	    SFlags flags = Style_Default;

      qDrawShadeRect( p, r, cg, true, pixelMetric( PM_DefaultFrameWidth, widget ),
                      0, widget->isEnabled() ? &cg.brush( QColorGroup::Base ) : &cg.brush( QColorGroup::Background ) );

	    QRect ar = QStyle::visualRect( querySubControlMetrics( CC_ComboBox, widget, SC_ComboBoxArrow ), widget );
	    if ( active == SC_ComboBoxArrow )
      {
		    p->setPen( cg.dark() );
		    p->setBrush( cg.brush( QColorGroup::Button ) );
		    p->drawRect( ar );
	    }
      else
		    qDrawWinPanel( p, ar, cg, FALSE, &cg.brush( QColorGroup::Button ) );

	    ar.addCoords( 2, 2, -2, -2 );
	    if ( widget->isEnabled() )
		    flags |= Style_Enabled;

	    if ( active == SC_ComboBoxArrow )
		    flags |= Style_Sunken;
	    drawPrimitive( PE_ArrowDown, p, ar, cg, flags );
	  }
    PARENT_STYLE::drawComplexControl( control, p, widget, r, cg, flags, controls & ~SC_ComboBoxArrow, active, opt );
    break;

  case CC_TitleBar:
    if( widget->inherits( "QTitleBar" ) )
      ( ( SalomeStyle* )this )->myTitleParent = ( QWidget* )widget->parent();

  default:
    PARENT_STYLE::drawComplexControl( control, p, widget, r, cg, flags, controls, active, opt );
    break;
  }
}

/*!
  \return pixmap by type
  \param st - type of pixmap
  \param w - widget
  \param opt - style option flags
*/
QPixmap SalomeStyle::stylePixmap( StylePixmap st, const QWidget* w, const QStyleOption& opt ) const
{
  switch ( st )
  {
  case SP_DockWindowCloseButton:
  case SP_TitleBarCloseButton:
    return QPixmap( cross_xpm );
  case SP_TitleBarMaxButton:
    return QPixmap( maximize_xpm );
  case SP_TitleBarNormalButton:
    return QPixmap( normal_xpm );
  case SP_TitleBarMinButton:
    return QPixmap( minimize_xpm );
  default:
    return PARENT_STYLE::stylePixmap( st, w, opt );
  }
}

/*!
  \return the pixel size
  \param pm - type of pixel metrics
  \param widget - widget
*/
int SalomeStyle::pixelMetric( PixelMetric pm, const QWidget* widget ) const
{
  int ret = 0;

  switch ( pm )
  {
  case PM_ButtonMargin:
    ret = 5;
    break;
  case PM_DockWindowFrameWidth:
    ret = 0;
    break;
  case PM_TabBarTabOverlap:
	  ret = 2;
	  break;
  case PM_TabBarTabHSpace:
	  ret = 20;
	  break;
  case PM_TabBarTabShiftHorizontal:
  case PM_TabBarTabShiftVertical:
	  ret = 0;
	  break;
  case PM_TabBarTabVSpace:
		ret = 5;
    break;
  case PM_DefaultFrameWidth:
    ret = 1;
    break;
  case PM_SpinBoxFrameWidth:
    ret = 1;
    break;
  case PM_ProgressBarChunkWidth:
    ret = 1;
    break;
  default:
    ret = PARENT_STYLE::pixelMetric( pm, widget );
    break;
  }
  return ret;
}

/*!
  \return corrected title text
  \param txt - title text
  \param w - possible width
  \param fm - font metrics
*/
QString SalomeStyle::titleText( const QString& txt, const int W, const QFontMetrics& fm ) const
{
  QString res = txt.stripWhiteSpace();

  if ( fm.width( res ) > W )
  {
    QString end( "..." );
    while ( !res.isEmpty() && fm.width( res + end ) > W )
      res.remove( res.length() - 1, 1 );

    if ( !res.isEmpty() )
      res += end;
  }

  return res;
}

/*!
    Class: SalomeStylePlugin [Internal]
    Constructor
*/
SalomeStylePlugin::SalomeStylePlugin()
{
}

/*!
  Destructor
*/
SalomeStylePlugin::~SalomeStylePlugin()
{
}

/*!
  \return keys of styles of plugin
*/
QStringList SalomeStylePlugin::keys() const
{
  return QStringList() << "salome";
}

/*!
  \return just created style 
  \param str - style key
*/
QStyle* SalomeStylePlugin::create( const QString& str )
{
  if ( str == "salome" )
    return new SalomeStyle();
  else
    return 0;
}

Q_EXPORT_PLUGIN( SalomeStylePlugin )
