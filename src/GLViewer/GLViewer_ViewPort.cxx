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

//  Author : OPEN CASCADE
// File:      GLViewer_ViewPort.cxx
// Created:   November, 2004
//#include <GLViewerAfx.h>
//
#include "GLViewer_ViewPort.h"

#include "SUIT_ResourceMgr.h"
#include "SUIT_Session.h"

#include <QRect>
#include <QCursor>
#include <QPainter>
#include <QMultiHash>
//#include <QMenu>
#include <QColorDialog>
#include <QColormap>
#include <QMouseEvent>
#include <QCoreApplication>

#include <stdlib.h>

#if !defined WIN32
#include <QX11Info>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xmu/StdCmap.h>
#undef QT_CLEAN_NAMESPACE
#include <Xw_Window.hxx>

/*!
  \struct CMapEntry
  Map of indexed colors
  For internal purposes
*/
struct CMapEntry
{
    CMapEntry();
    ~CMapEntry();
    Colormap          cmap;
    bool              alloc;
    XStandardColormap scmap;
};

/*!
  Constructor
*/
CMapEntry::CMapEntry()
{
    cmap = 0;
    alloc = false;
    scmap.colormap = 0;
}

/*!
  Destructor
*/
CMapEntry::~CMapEntry()
{
    if ( alloc )
        XFreeColormap( QX11Info::display(), cmap );
}

static QMultiHash<int,CMapEntry> *cmap_dict = 0;
static bool mesa_gl = false;

static void cleanup_cmaps()
{
    if ( !cmap_dict )
        return;
    //while (!cmap_dict->isEmpty())
    //  cmap_dict->erase(cmap_dict->begin());
    cmap_dict->clear();
    delete cmap_dict;
    cmap_dict = 0;
}

static Colormap choose_cmap( Display *dpy, XVisualInfo *vi )
{
    if ( !cmap_dict )
    {
        cmap_dict = new QMultiHash<int,CMapEntry>;
        const char *v = glXQueryServerString( dpy, vi->screen, GLX_VERSION );
        mesa_gl = strstr( v,"Mesa" ) != 0;
        qAddPostRoutine( cleanup_cmaps );
    }

    QHash<int,CMapEntry>::iterator itH = cmap_dict->find( (long)vi->visualid );
    if ( itH != cmap_dict->end() )  // found colormap for visual
        return itH.value().cmap;

    CMapEntry x;

    XStandardColormap *c;
    int n, i;

#ifdef DEBUG
    cout << "Choosing cmap for vID = " << vi->visualid << endl;
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if ( vi->visualid == XVisualIDFromVisual( (Visual*)QX11Info::appVisual() ) )
    {
#ifdef DEBUG
        cout << "Using x11AppColormap" << endl;
#endif
        return QX11Info::appColormap();
    }
#else
    if ( vi->visualid == XVisualIDFromVisual( XDefaultVisual( QX11Info::display(), -1 ) ) )
    {
#ifdef DEBUG
        cout << "Using XDefaultColormap" << endl;
#endif
        return XDefaultColormap( QX11Info::display(), -1 );
    }
#endif
    if ( mesa_gl )
    {
        Atom hp_cmaps = XInternAtom( dpy, "_HP_RGB_SMOOTH_MAP_LIST", true );
        if ( hp_cmaps && vi->visual->c_class == TrueColor && vi->depth == 8 )
        {
            if ( XGetRGBColormaps( dpy, RootWindow( dpy, vi->screen ), &c, &n, hp_cmaps ) )
            {
                i = 0;
                while ( i < n && x.cmap == 0 )
                {
                    if ( c[i].visualid == vi->visual->visualid )
                    {
                        x.cmap = c[i].colormap;
                        x.scmap = c[i];
                    }
                    i++;
                }
                XFree( (char*)c );
            }
        }
    }
#if !defined( _OS_SOLARIS_ )
    if ( !x.cmap )
    {
        if ( XmuLookupStandardColormap( dpy, vi->screen, vi->visualid, vi->depth, XA_RGB_DEFAULT_MAP, false, true ) )
        {
            if ( XGetRGBColormaps( dpy, RootWindow( dpy, vi->screen ), &c, &n, XA_RGB_DEFAULT_MAP ) )
            {
                i = 0;
                while ( i < n && x.cmap == 0 )
                {
                    if ( c[i].visualid == vi->visualid )
                    {
                        x.cmap = c[i].colormap;
                        x.scmap = c[i];
                    }
                    i++;
                }
                XFree( (char *)c );
            }
        }
    }
#endif
    if ( !x.cmap )
    {
        // no shared cmap found
        x.cmap = XCreateColormap( dpy, RootWindow( dpy, vi->screen ), vi->visual, AllocNone );
        x.alloc = true;
    }

    cmap_dict->insert( (long)vi->visualid, x ); // associate cmap with visualid
    return x.cmap;
}
#endif

int GLViewer_ViewPort::nCounter = 0;
QCursor* GLViewer_ViewPort::defCursor = 0;
QCursor* GLViewer_ViewPort::panglCursor = 0;
QCursor* GLViewer_ViewPort::handCursor = 0;
QCursor* GLViewer_ViewPort::panCursor = 0;
QCursor* GLViewer_ViewPort::zoomCursor = 0;
QCursor* GLViewer_ViewPort::rotCursor = 0;
QCursor* GLViewer_ViewPort::sketchCursor = 0;

/*!
    Creates the necessary viewport cursors. [ static ]
*/
void GLViewer_ViewPort::createCursors ()
{
    defCursor   = new QCursor( Qt::ArrowCursor );
    panglCursor = new QCursor( Qt::CrossCursor );
    handCursor  = new QCursor( Qt::PointingHandCursor );
    panCursor   = new QCursor( Qt::SizeAllCursor );

    SUIT_ResourceMgr* rmgr = SUIT_Session::session()->resourceMgr();
    zoomCursor   = new QCursor( rmgr->loadPixmap( "GLViewer", tr( "ICON_GL_CURSOR_ZOOM" ) ) );
    rotCursor    = new QCursor( rmgr->loadPixmap( "GLViewer", tr( "ICON_GL_CURSOR_ROTATE" ) ) );
    sketchCursor = new QCursor( rmgr->loadPixmap( "GLViewer", tr( "ICON_GL_CURSOR_SKETCH" ) ) );
}

/*!
    Destroys the viewport cursors. [ static ]
*/
void GLViewer_ViewPort::destroyCursors()
{
    delete defCursor;   defCursor   = 0;
    delete panglCursor; panglCursor = 0;
    delete handCursor;  handCursor  = 0;
    delete panCursor;   panCursor   = 0;
    delete zoomCursor;  zoomCursor  = 0;
    delete rotCursor;   rotCursor   = 0;
    delete sketchCursor; sketchCursor = 0;
}

/*!
    Sets new default cursor. [ static ]
*/
void GLViewer_ViewPort::setDefaultCursor( const QCursor& newCursor )
{
    if ( !defCursor )
        defCursor = new QCursor();
    *defCursor = newCursor;
}

/*!
    Sets new cursor for drawing rectangle in the viewport. [ static ]
*/
void GLViewer_ViewPort::setHandCursor( const QCursor& newCursor )
{
    if ( !handCursor )
        handCursor = new QCursor();
    *handCursor = newCursor;
}

/*!
    Sets new cursor for panning. [ static ]
*/
void GLViewer_ViewPort::setPanCursor( const QCursor& newCursor )
{
    if ( !panCursor )
        panCursor = new QCursor();
    *panCursor = newCursor;
}

/*!
    Sets new cursor for global panning. [ static ]
*/
void GLViewer_ViewPort::setPanglCursor( const QCursor& newCursor )
{
    if ( !panglCursor )
        panglCursor = new QCursor();
    *panglCursor = newCursor;
}

/*!
    Sets new cursor for zooming. [ static ]
*/
void GLViewer_ViewPort::setZoomCursor( const QCursor& newCursor )
{
    if ( !zoomCursor )
        zoomCursor = new QCursor();
    *zoomCursor = newCursor;
}

/*!
    Sets new cursor for rotating. [ static ]
*/
void GLViewer_ViewPort::setRotCursor( const QCursor& newCursor )
{
    if ( !rotCursor )
        rotCursor = new QCursor();
    *rotCursor = newCursor;
}

/*!
    Sets new cursor for rotating. [ static ]
*/
void GLViewer_ViewPort::setSketchCursor( const QCursor& newCursor )
{
    if ( !rotCursor )
        sketchCursor = new QCursor();
    *sketchCursor = newCursor;
}

/*!
    Constructor
*/
GLViewer_ViewPort::GLViewer_ViewPort( QWidget* parent )
: QWidget( parent, 0 )
{
    initialize();
}

/*!
    Destructor
*/
GLViewer_ViewPort::~GLViewer_ViewPort()
{
    cleanup();
}

/*!
    Initializes viewport. [ private ]
*/
void GLViewer_ViewPort::initialize()
{
    if ( nCounter++ == 0 )
        createCursors();

    //myPopupActions.setAutoDelete( true );
    myPaintersRedrawing = false;
    myEnableSketching = false;
    myEnableTransform = true;

    setMouseTracking( true );
    setBackgroundRole( QPalette::NoRole );//NoBackground );

    setFocusPolicy( Qt::StrongFocus );
}

/*!
    Cleans up the viewport. [ private ]
*/
void GLViewer_ViewPort::cleanup()
{
    if ( --nCounter == 0 )
        destroyCursors();
}

/*!
    Selects visual ID for OpenGL window ( X11 specific ). [ protected ]
*/
void GLViewer_ViewPort::selectVisualId( ViewType type )
{
#if !defined WIN32
    XVisualInfo* pVisualInfo;
    if ( QX11Info::display() )
    {
        /* Initialization with the default VisualID */
        //Visual *v = DefaultVisual( x11Display(), DefaultScreen( x11Display() ) );
        // int visualID = XVisualIDFromVisual( v ); unused

        /*  Here we use the settings from Optimizer_ViewInfo::TxglCreateWindow() */
        int visualAttr[] = { GLX_RGBA, GLX_DEPTH_SIZE, 1, GLX_RED_SIZE, 1, GLX_GREEN_SIZE, 1,
                             GLX_BLUE_SIZE, 1, GLX_DOUBLEBUFFER, None };

        pVisualInfo = ::glXChooseVisual( QX11Info::display(), DefaultScreen( QX11Info::display() ), visualAttr );

        if ( isVisible() )
            hide();

        XSetWindowAttributes a;

        a.colormap = choose_cmap( QX11Info::display(), pVisualInfo );      /* find best colormap */
        a.background_pixel = QColormap::instance().pixel( backgroundColor() );
        a.border_pixel = QColormap::instance().pixel( Qt::black );
        Window p = RootWindow( QX11Info::display(), DefaultScreen( QX11Info::display() ) );
        if ( parentWidget() )
            p = parentWidget()->winId();

        Window w;
        if ( type == Type2D )  // creating simple X window for 2d
        {
            unsigned long xbackground =
                BlackPixel( QX11Info::display(), DefaultScreen( QX11Info::display() ) );
            unsigned long xforeground =
                WhitePixel( QX11Info::display(), DefaultScreen( QX11Info::display() ) );

            w = XCreateSimpleWindow ( QX11Info::display(), p, x(), y(), width(),
                                      height(), 0, xforeground, xbackground );
        }
        else if ( type == Type3D )
        {
            w = XCreateWindow( QX11Info::display(), p,  x(), y(), width(), height(),
                               0, pVisualInfo->depth, InputOutput, pVisualInfo->visual,
                               CWBackPixel | CWBorderPixel | CWColormap, &a );
        }
        else
            return;

        Window *cmw;
        Window *cmwret;
        int count;
        if ( XGetWMColormapWindows( QX11Info::display(), topLevelWidget()->winId(), &cmwret, &count ) )
        {
            cmw = new Window[count+1];
            memcpy( (char*)cmw, (char*)cmwret, sizeof(Window) * count );
            XFree( (char*)cmwret );
            int i;

            for ( i = 0; i < count; i++ )
            {
                if ( cmw[i] == winId() ) /* replace old window */
                {
                    cmw[i] = w;
                    break;
                }
            }

            if ( i >= count )            /* append new window */
                cmw[count++] = w;
        }
        else
        {
            count = 1;
            cmw = new Window[count];
            cmw[0] = w;
        }

        /* Creating new window (with good VisualID) for this widget */
        create(w);
        XSetWMColormapWindows( QX11Info::display(), topLevelWidget()->winId(), cmw, count );
        delete[] cmw;

        if ( isVisible() )
            show();

        if ( pVisualInfo )
        {
            XFree( (char *)pVisualInfo );
        }
        XFlush( QX11Info::display() );
    }
#endif
}

/*!
    Sets the background 'color'. [ virtual ]
*/
void GLViewer_ViewPort::setBackgroundColor( const QColor& color )
{
    QPalette pal = palette();
    pal.setColor( QPalette::Background, color );
    setPalette( pal );
    repaint();
}

/*!
    Returns the background color. [ virtual ]
*/
QColor GLViewer_ViewPort::backgroundColor() const
{
    return palette().color( QPalette::Active, QPalette::Background );
}

/*!
    Returns 'true' if sketching is enabled in  this viewport. [ public ]
*/
bool GLViewer_ViewPort::isSketchingEnabled() const
{
    return myEnableSketching;
}

/*!
    Enables / disables sketching  [ public ]
*/
void GLViewer_ViewPort::setSketchingEnabled( bool enable )
{
    myEnableSketching = enable;
}

/*!
    Returns 'true' if transformations ( rotation, zoom etc. )
    are enabled in this viewport. [ public ]
*/
bool GLViewer_ViewPort::isTransformEnabled() const
{
    return myEnableTransform;
}

/*!
    Enables / disables transformations. [ public ]
*/
void GLViewer_ViewPort::setTransformEnabled( bool enable )
{
    myEnableTransform = enable;
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::mousePressEvent( QMouseEvent *e )
{
    emit vpMouseEvent( e );
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::mouseMoveEvent( QMouseEvent* e )
{
    emit vpMouseEvent( e );
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::mouseReleaseEvent( QMouseEvent *e )
{
    emit vpMouseEvent( e );

    /* show popup menu */
    if ( e->button() == Qt::RightButton )
    {
        //QPopupMenu* popup = createPopup();
        //if ( popup && popup->count() )
        //    popup->exec( QCursor::pos() );
        //destroyPopup( /*popup*/ );
    }
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::mouseDoubleClickEvent( QMouseEvent *e )
{
    emit vpMouseEvent( e );
}

/*!
    Emits 'keyEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::keyPressEvent( QKeyEvent *e )
{
    emit vpKeyEvent( e );
}

/*!
    Emits 'keyEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::keyReleaseEvent( QKeyEvent *e )
{
    emit vpKeyEvent( e );
}

/*!
    Emits 'mouseEvent' signal. [ virtual protected ]
*/
void GLViewer_ViewPort::wheelEvent( QWheelEvent *e )
{
    emit vpWheelEvent( e );
}

/*!
    Repaints the viewport. [ virtual protected ]
*/
void GLViewer_ViewPort::paintEvent( QPaintEvent* )
{
    if ( myPaintersRedrawing )
    {
        QPainter p( this );
        emit vpDrawExternal( &p );
        myPaintersRedrawing = false;
    }
}

/*!
    Forces to redraw the viewport by an external painter. [ public ]
*/
void GLViewer_ViewPort::redrawPainters()
{
    myPaintersRedrawing = true;
    repaint();
}

/*!
    Updates this view. Does nothing by default. [ virtual public ]
*/
void GLViewer_ViewPort::onUpdate()
{
}

/*!
    Sets the background color with color selection dialog. [ virtual protected slot ]
*/
void GLViewer_ViewPort::onChangeBgColor()
{
    QColor selColor = QColorDialog::getColor ( backgroundColor(), this );
    if ( selColor.isValid() )
        setBackgroundColor( selColor );
}

/*!
  Custom context menu event handler
*/
void GLViewer_ViewPort::contextMenuEvent( QContextMenuEvent* e )
{
  //if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e );
}
