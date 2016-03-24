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

// SUIT_ViewWindow.cxx: implementation of the SUIT_ViewWindow class.
//
#include "SUIT_ViewWindow.h"

#include "SUIT_Tools.h"
#include "SUIT_Session.h"
#include "SUIT_Study.h"
#include "SUIT_Desktop.h"
#include "SUIT_MessageBox.h"
#include "SUIT_Application.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ResourceMgr.h"
#include "QtxActionToolMgr.h"
#include "QtxMultiAction.h"

#include <QEvent>
#include <QIcon>
#include <QMenu>
#include <QApplication>
#include <QContextMenuEvent>

/*!\class SUIT_ViewWindow
 * Class provide view window.
 */

/*! Dump view custom event*/
const int DUMP_EVENT = QEvent::User + 123;

/*! Constructor.*/
SUIT_ViewWindow::SUIT_ViewWindow( SUIT_Desktop* theDesktop )
  : QMainWindow( theDesktop ), myManager( 0 ), myIsDropDown( true ), mySyncAction( 0 )
{
  myDesktop = theDesktop;

  setWindowIcon( myDesktop ? myDesktop->windowIcon() : QApplication::windowIcon() );

  setAttribute( Qt::WA_DeleteOnClose );

  myToolMgr = new QtxActionToolMgr( this );

  setProperty( "VectorsMode", false );
}

/*! Destructor.*/
SUIT_ViewWindow::~SUIT_ViewWindow()
{
}

/*!
  Sets new view manager for window
  \param theManager - new view manager
*/
void SUIT_ViewWindow::setViewManager( SUIT_ViewManager* theManager )
{
  myManager = theManager;
}

/*!
  \return view manager of window
*/
SUIT_ViewManager* SUIT_ViewWindow::getViewManager() const
{
  return myManager;
}

/*!
  \return QImage, containing all scene rendering in window
*/
QImage SUIT_ViewWindow::dumpView()
{
  return QImage();
}

/*!
  Saves image to file according to the format
  \param image - image
  \param fileName - name of file
  \param format - string contains name of format (for example, "BMP"(default) or "JPEG", "JPG")
*/
bool SUIT_ViewWindow::dumpViewToFormat( const QImage& img, const QString& fileName, const QString& format )
{
  if( img.isNull() )
    return false;

  QString fmt = format;
  if( fmt.isEmpty() )
    fmt = QString( "BMP" ); // default format
  else if( fmt == "JPG" )
    fmt = "JPEG";

  QApplication::setOverrideCursor( Qt::WaitCursor );
  bool res = img.save( fileName, fmt.toLatin1() );
  QApplication::restoreOverrideCursor();
  return res;
}

/*!
  Saves scene rendering in window to file
  \param fileName - name of file
  \param format - string contains name of format (for example, "BMP"(default) or "JPEG", "JPG")
*/
bool SUIT_ViewWindow::dumpViewToFormat( const QString& fileName, const QString& format )
{
  Qtx::Localizer loc;
  return dumpViewToFormat( dumpView(), fileName, format );
}

/*!
  Set or clear flag Qt::WDestructiveClose
*/
void SUIT_ViewWindow::setDestructiveClose( const bool on )
{
  setAttribute( Qt::WA_DeleteOnClose, on );
}

/*! Close event \a theEvent.
*/
void SUIT_ViewWindow::closeEvent( QCloseEvent* e )
{
  e->ignore();
  emit tryClosing( this );
  if ( closable() ) emit closing( this );
}

/*! Context menu requested for event \a e.
*/
void SUIT_ViewWindow::contextMenuEvent( QContextMenuEvent* e )
{
  e->ignore();

  QMainWindow::contextMenuEvent( e );

  if ( e->isAccepted() )
    return;

  if ( e->reason() != QContextMenuEvent::Mouse )
    emit contextMenuRequested( e );
}

/*! Post events on dump view.
*/
void SUIT_ViewWindow::onDumpView()
{
  // VSV (TRIPOLI dev): next line commented: causes error messages
  //QApplication::postEvent( this, new QPaintEvent( QRect( 0, 0, width(), height() ) ) );
  QApplication::postEvent( this, new QEvent( (QEvent::Type)DUMP_EVENT ) );
}

/*!
  \return filters for image files
*/
QString SUIT_ViewWindow::filter() const
{
  return tr( "TLT_IMAGE_FILES" );
}

/*! Reaction view window on event \a e.
*/
bool SUIT_ViewWindow::event( QEvent* e )
{
  if ( e->type() == DUMP_EVENT )
  {
    bool bOk = false;
    if ( myManager && myManager->study() && myManager->study()->application() )
    {
      // get file name
      SUIT_Application* app = myManager->study()->application();
      QString fileName = app->getFileName( false, QString(), filter(), tr( "TLT_DUMP_VIEW" ), 0 );
      if ( !fileName.isEmpty() )
      {
        QImage im = dumpView();
	QString fmt = SUIT_Tools::extension( fileName ).toUpper();
	Qtx::Localizer loc;
	bOk = dumpViewToFormat( im, fileName, fmt );
      }
      else
	bOk = true; // cancelled
    }
    if ( !bOk )
      SUIT_MessageBox::critical( this, tr( "ERROR" ), tr( "ERR_CANT_DUMP_VIEW" ) );

    return true;
  }
  return QMainWindow::event( e );
}

/*! Called by SUIT_Accel::onActivated() when a key accelerator was activated and this window was active
*/
bool SUIT_ViewWindow::onAccelAction( int _action )
{
  return action( _action );
}

/*! action  handle standard action (zoom, pan) or custom action.  to be redefined in successors.
*/
bool SUIT_ViewWindow::action( const int  )
{
  return true;
}

/*! Returns \c true if view window can be closed by the user
*/
bool SUIT_ViewWindow::closable() const
{
  QVariant val = property( "closable" );
  return !val.isValid() || val.toBool();
}

/*! Set / reset "closable" option of the view window
*/
bool SUIT_ViewWindow::setClosable( const bool on )
{
  bool prev = closable();
  setProperty( "closable", on );
  return prev;
}

/*!
  \return string containing visual parameters of window
*/
QString SUIT_ViewWindow::getVisualParameters()
{
  return "empty";
}

/*!
  Sets visual parameters of window by its string representation
  \param parameters - string with visual parameters
*/
void SUIT_ViewWindow::setVisualParameters( const QString& /*parameters*/ )
{
}

/*!
  \return associated tool bar manager
*/
QtxActionToolMgr* SUIT_ViewWindow::toolMgr() const
{
  return myToolMgr;
}

/*!
  \brief Set buttons mode to drop-down (\a on = \c true) or ligned (\a on = \c false)
  \param on new buttons mode
  \sa dropDownButtons()
*/
void SUIT_ViewWindow::setDropDownButtons( bool on )
{
  if ( myIsDropDown != on ) {
    myIsDropDown = on;
    if ( myIsDropDown ) {
      ActionsMap::const_iterator it;
      for( it = myMultiActions.constBegin(); it != myMultiActions.constEnd(); ++it )
      {
	int tid = it.key();
	const QList<QtxMultiAction*>& mlist = it.value();
	QList<QtxMultiAction*>::const_iterator mit;
	for ( mit = mlist.constBegin(); mit != mlist.constEnd(); ++mit )
	{
	  QtxMultiAction* ma = *mit;
	  const QList<QAction*> alist = ma->actions();
	  if ( alist.isEmpty() ) continue;
	  int idx = toolMgr()->index( toolMgr()->actionId( alist[0] ), tid );
	  if ( idx == -1 ) continue;
	  foreach ( QAction* a, alist ) toolMgr()->remove( toolMgr()->actionId( a ), tid );
	  toolMgr()->insert( ma, tid, idx );
	}
      }
      myMultiActions.clear();
    }
    else {
      QIntList tblist = toolMgr()->toolBarsIds();
      QIntList alist  = toolMgr()->idList();
      foreach( int aid, alist )
      {
	QtxMultiAction* ma = qobject_cast<QtxMultiAction*>( toolMgr()->action( aid ) );
	if ( !ma ) continue;
	foreach( int tid, tblist )
	{
	  int idx = toolMgr()->index( aid, tid );
	  if ( idx >= 0 )
	  {
	    myMultiActions[ tid ].append( ma );
	    toolMgr()->remove( aid, tid );
	    foreach( QAction* a, ma->actions() ) toolMgr()->insert( a, tid, idx++ );
	  }
	}
      }
    }
  }
}

/*!
  \brief Get current buttons mode
  \return current buttons mode
  \sa setDropDownButtons()
*/
bool SUIT_ViewWindow::dropDownButtons() const
{
  return myIsDropDown;
}

/*!
  \return window unique identifier
*/
int SUIT_ViewWindow::getId() const
{
  return int(long(this));
}

/*!
  Get camera properties for the view window.
  \return shared pointer on camera properties. Base implementation
          returns null properties.
*/
SUIT_CameraProperties SUIT_ViewWindow::cameraProperties()
{
  return SUIT_CameraProperties();
}

/*!
  Synchronize this view window's camera properties with specified
  view window.

  This method is a part of general views synchronization mechanism.
  It should be redefined in successors. Base imlementation does nothing.

  \param otherWindow other view window
*/
void SUIT_ViewWindow::synchronize( SUIT_ViewWindow* /*otherWindow*/ )
{
  // base implementation does nothing
}

/*!
  Get action for views syncronization.

  This method is a part of general views synchronization mechanism.
  It creates an action that can be inserted, for instance, to the toolbar.

  \return action for views synchronization
*/
QAction* SUIT_ViewWindow::synchronizeAction()
{
  if ( !mySyncAction ) {
    SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
    mySyncAction = new QtxAction( tr( "MNU_SYNCHRONIZE_VIEW" ),
				  resMgr->loadPixmap( "SUIT", tr( "ICON_VIEW_SYNC" ) ),
				  tr( "MNU_SYNCHRONIZE_VIEW" ), 0, this );
    mySyncAction->setStatusTip( tr( "DSC_SYNCHRONIZE_VIEW" ) );
    mySyncAction->setMenu( new QMenu( this ) );
    mySyncAction->setCheckable( true );
    connect( mySyncAction->menu(), SIGNAL( aboutToShow() ),     this, SLOT( updateSyncViews() ) );
    connect( mySyncAction,         SIGNAL( triggered( bool ) ), this, SLOT( onSynchronizeView( bool ) ) );
  }
  return mySyncAction;
}

/*!
  Emit notification signal that the view is transformed.
  Other views can use the signal for synchronization.
*/
void SUIT_ViewWindow::emitViewModified()
{
  emit viewModified( this );
}

/*!
  Update list of available view for the "Synchronize View" action
*/
void SUIT_ViewWindow::updateSyncViews()
{
  SUIT_CameraProperties props = cameraProperties();
  if ( !props.isValid() )
    return;

  QAction* anAction = synchronizeAction();
  if ( anAction && anAction->menu() ) {
    int currentId = anAction->data().toInt();
    anAction->menu()->clear();
    SUIT_Application* app = SUIT_Session::session()->activeApplication();
    if ( app ) {
      SUIT_Desktop* d = app->desktop();
      QList<SUIT_ViewWindow*> allViews = d->findChildren<SUIT_ViewWindow*>();
      foreach( SUIT_ViewWindow* vw, allViews ) {
	if ( !vw || vw == this ) continue; // skip invalid views and this one
	SUIT_CameraProperties otherProps = vw->cameraProperties();
	if ( otherProps.isCompatible( props ) ) {
	  QAction* a = anAction->menu()->addAction( vw->windowTitle() );
	  if ( vw->getId() == currentId ) {
	    QFont f = a->font();
	    f.setBold( true );
	    a->setFont( f );
	  }
	  a->setData( vw->getId() );
	  connect( a, SIGNAL( triggered( bool ) ), this, SLOT( onSynchronizeView( bool ) ) );
	}
	else if ( vw->getId() == currentId ) {
	  // other view, this one is being currently synchronized to, seems has become incompatible
	  // we have to break synchronization
	  vw->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), this, SLOT( synchronize( SUIT_ViewWindow* ) ) );
	  this->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), vw, SLOT( synchronize( SUIT_ViewWindow* ) ) );
	  // 
	  bool blocked = anAction->blockSignals( true );
	  anAction->setChecked( false );
	  anAction->blockSignals( blocked );
	  anAction->setData( 0 );
	  //
	  QAction* a = vw->synchronizeAction();
	  if ( a ) {
	    blocked = a->blockSignals( true );
	    a->setChecked( false );
	    a->blockSignals( blocked );
	  }
	}
      }
    }
    if ( anAction->menu()->actions().isEmpty() ) {
      anAction->setData( 0 );
      anAction->menu()->addAction( tr( "MNU_SYNC_NO_VIEW" ) );
    }
  }
}

/*!
  "Synchronize View" action slot.
*/
void SUIT_ViewWindow::onSynchronizeView( bool checked )
{
  QAction* a = qobject_cast<QAction*>( sender() );
  if ( a ) {
    synchronizeView( this, a->data().toInt() );
  }
}

/*!
  Synchronize camera properties of view \a viewWindow with
  camera properties of view specified via \a id
*/
void SUIT_ViewWindow::synchronizeView( SUIT_ViewWindow* viewWindow, int id )
{
  SUIT_ViewWindow* sourceView = 0;
  QList<SUIT_ViewWindow*> otherViews;

  bool isSync = viewWindow->synchronizeAction() && viewWindow->synchronizeAction()->isChecked();

  int vwid = viewWindow->getId();

  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( !app ) return;
  SUIT_Desktop* d = app->desktop();
  if ( !d ) return;

  QList<SUIT_ViewWindow*> allViews = d->findChildren<SUIT_ViewWindow*>();
  foreach( SUIT_ViewWindow* vw, allViews ) {
    if ( !vw->cameraProperties().isValid() )
      continue;                    // omit views not supporting camera properties
    if ( vw->getId() == id )
      sourceView = vw;             // remember source view
    else if ( vw != viewWindow )
      otherViews.append( vw );     // collect all remaining views
  }

  if ( isSync && id ) {
    // remove all possible disconnections
    foreach( SUIT_ViewWindow* vw, otherViews ) {
      // disconnect target view
      vw->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), viewWindow, SLOT( synchronize( SUIT_ViewWindow* ) ) );
      viewWindow->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), vw, SLOT( synchronize( SUIT_ViewWindow* ) ) );
      if ( sourceView ) {
	// disconnect source view
	vw->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), sourceView, SLOT( synchronize( SUIT_ViewWindow* ) ) );
	sourceView->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), vw, SLOT( synchronize( SUIT_ViewWindow* ) ) );
      }
      QAction* a = vw->synchronizeAction();
      if ( a ) {
	int anid = a->data().toInt();
	if ( a->isChecked() && ( anid == id || anid == vwid ) ) {
	  bool blocked = a->blockSignals( true );
	  a->setChecked( false );
	  a->blockSignals( blocked );
	}
      }
    }
    if ( sourceView ) {
      // reconnect source and target views
      sourceView->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), viewWindow, SLOT( synchronize( SUIT_ViewWindow* ) ) );
      viewWindow->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), sourceView, SLOT( synchronize( SUIT_ViewWindow* ) ) );
      sourceView->connect( viewWindow, SIGNAL( viewModified( SUIT_ViewWindow* ) ), SLOT( synchronize( SUIT_ViewWindow* ) ) );
      viewWindow->connect( sourceView, SIGNAL( viewModified( SUIT_ViewWindow* ) ), SLOT( synchronize( SUIT_ViewWindow* ) ) );
      // synchronize target view with source view
      viewWindow->synchronize( sourceView );
      if ( viewWindow->synchronizeAction() )
	viewWindow->synchronizeAction()->setData( sourceView->getId() );
      QAction* sourceAction = sourceView->synchronizeAction();
      if ( sourceAction ) {
        sourceAction->setData( viewWindow->getId() );
        if ( !sourceAction->isChecked() ) {
	  bool blocked = sourceAction->blockSignals( true );
	  sourceAction->setChecked( true );
	  sourceAction->blockSignals( blocked );
        }
      }
    }
  }
  else if ( sourceView ) {
    // reconnect source and target view
    sourceView->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), viewWindow, SLOT( synchronize( SUIT_ViewWindow* ) ) );
    viewWindow->disconnect( SIGNAL( viewModified( SUIT_ViewWindow* ) ), sourceView, SLOT( synchronize( SUIT_ViewWindow* ) ) );
    viewWindow->synchronize( sourceView );
    if ( viewWindow->synchronizeAction() )
      viewWindow->synchronizeAction() ->setData( sourceView->getId() );
    QAction* sourceAction = sourceView->synchronizeAction();
    if ( sourceAction ) {
      if ( sourceAction->data().toInt() == viewWindow->getId() && sourceAction->isChecked() ) {
        bool blocked = sourceAction->blockSignals( true );
        sourceAction->setChecked( false );
        sourceAction->blockSignals( blocked );
      }
    }
  }
}
