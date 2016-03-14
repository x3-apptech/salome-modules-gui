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

#include "SUIT_ViewManager.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewModel.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_Study.h"
#include "SUIT_Session.h"

#include <QMap>
#include <QRegExp>
#include <QIcon>

#ifdef WIN32
#include <windows.h>
#endif

QMap<QString, int> SUIT_ViewManager::_ViewMgrId;

/*!\class SUIT_ViewManager.
 * Class provide manipulation with view windows.
 */

/*!Constructor.*/
SUIT_ViewManager::SUIT_ViewManager( SUIT_Study* theStudy,
                                    SUIT_Desktop* theDesktop,
                                    SUIT_ViewModel* theViewModel )
: QObject( 0 ),
  myDesktop( theDesktop ),
  myTitle( "Default: %M - viewer %V" ),
  myStudy( NULL )
{
  myViewModel = 0;
  myActiveView = 0;
  setViewModel( theViewModel );

  myId = useNewId( getType() );

  connect( theDesktop, SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
           this,       SLOT( onWindowActivated( SUIT_ViewWindow* ) ) );

  myStudy = theStudy;
  if ( myStudy )
    connect( myStudy, SIGNAL( destroyed() ), this, SLOT( onDeleteStudy() ) );
}

/*!Destructor.*/
SUIT_ViewManager::~SUIT_ViewManager()
{
  if ( myViewModel )
  {
    myViewModel->setViewManager( 0 );
    delete myViewModel;
  }
}

int SUIT_ViewManager::useNewId( const QString& type )
{
  if ( !_ViewMgrId.contains( type ) )
    _ViewMgrId.insert( type, 0 );

  int id = _ViewMgrId[type];
  _ViewMgrId[type]++;
  return id;
}


int SUIT_ViewManager::getGlobalId() const {
  int id = -1;
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if(app) {
    id = app->viewManagerId(this);
  }
  return id;
}

void SUIT_ViewManager::setTitle( const QString& theTitle )
{
  if ( myTitle == theTitle )
    return;

  myTitle = theTitle;
  for ( int i = 0; i < myViews.count(); i++ )
    setViewName( myViews[i] );
}

void SUIT_ViewManager::setIcon( const QPixmap& theIcon )
{
  myIcon = theIcon;
  for ( int i = 0; i < myViews.count(); i++ )
    myViews[i]->setWindowIcon( QIcon( myIcon ) );
}

/*!Sets view model \a theViewModel to view manager.*/
void SUIT_ViewManager::setViewModel(SUIT_ViewModel* theViewModel)
{
  if (myViewModel && myViewModel != theViewModel) {
    myViewModel->setViewManager(0);
    delete myViewModel;
  }
  myViewModel = theViewModel;
  if (myViewModel)
    myViewModel->setViewManager(this);
}

/*!Sets view name for view window \a theView.*/
void SUIT_ViewManager::setViewName( SUIT_ViewWindow* theView )
{
  QString title = prepareTitle( getTitle(), myId + 1, myViews.indexOf( theView ) + 1 );
  theView->setWindowTitle( title );
}

QString SUIT_ViewManager::prepareTitle( const QString& title, const int mId, const int vId )
{
  QString res = title;
  QRegExp re( "%[%MV]" );
  int i = 0;
  while ( ( i = re.indexIn( res, i ) ) != -1 )
  {
    QString rplc;
    QString str = res.mid( i, re.matchedLength() );
    if ( str == QString( "%%" ) )
      rplc = QString( "%" );
    else if ( str == QString( "%M" ) )
      rplc = QString::number( mId );
    else if ( str == QString( "%V" ) )
      rplc = QString::number( vId );
    res.replace( i, re.matchedLength(), rplc );
    i += rplc.length();
  }
  return res;
}

/*! Creates View, adds it into list of views and returns just created view window*/
SUIT_ViewWindow* SUIT_ViewManager::createViewWindow()
{
  SUIT_ViewWindow* aView = myViewModel->createView(myDesktop);

  if ( !insertView( aView ) ){
    delete aView;
    return 0;
  }

  setViewName( aView );
  aView->setWindowIcon( QIcon( myIcon ) );

  //myDesktop->addViewWindow( aView );
  //it is done automatically during creation of view

  aView->setViewManager( this );
  emit viewCreated( aView );

  // Special treatment for the case when <aView> is the first one in this view manager
  // -> call onWindowActivated() directly, because somebody may always want
  // to use getActiveView()
  if ( !myActiveView )
    onWindowActivated( aView );

  return aView;
}

/*! Get identifier */
int SUIT_ViewManager::getId() const
{
  return myId;
}

/*!Create view window.*/
void SUIT_ViewManager::createView()
{
  createViewWindow();
}

QVector<SUIT_ViewWindow*> SUIT_ViewManager::getViews() const
{
  QVector<SUIT_ViewWindow*> res;
  for ( int i = 0; i < myViews.count(); i++ )
  {
    if ( myViews[i] )
      res.append( myViews[i] );
  }

  return res;
}

/*!Insert view window to view manager.
 *\retval false - if something wrong, else true.
 */
bool SUIT_ViewManager::insertView(SUIT_ViewWindow* theView)
{
  unsigned int aSize = myViews.size();
  unsigned int aNbItems = myViews.count() + 1;
  if ( aNbItems > aSize )
  {
    myViews.resize( aNbItems );
    aSize = myViews.size();
  }

  connect(theView, SIGNAL(closing(SUIT_ViewWindow*)),
          this,    SLOT(onClosingView(SUIT_ViewWindow*)));

  connect(theView, SIGNAL(tryClosing(SUIT_ViewWindow*)),
          this,    SIGNAL(tryCloseView(SUIT_ViewWindow*)));

  connect(theView, SIGNAL(mousePressed(SUIT_ViewWindow*, QMouseEvent*)),
          this,    SLOT(onMousePressed(SUIT_ViewWindow*, QMouseEvent*)));

  connect(theView, SIGNAL(mouseReleased(SUIT_ViewWindow*, QMouseEvent*)),
          this,    SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)));

  connect(theView, SIGNAL(mouseDoubleClicked(SUIT_ViewWindow*, QMouseEvent*)),
          this,    SIGNAL(mouseDoubleClick(SUIT_ViewWindow*, QMouseEvent*)));

  connect(theView, SIGNAL(mouseMoving(SUIT_ViewWindow*, QMouseEvent*)),
          this,    SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)));

  connect(theView, SIGNAL(wheeling(SUIT_ViewWindow*, QWheelEvent*)),
          this,    SIGNAL(wheel(SUIT_ViewWindow*, QWheelEvent*)));

  connect(theView, SIGNAL(keyPressed(SUIT_ViewWindow*, QKeyEvent*)),
          this,    SIGNAL(keyPress(SUIT_ViewWindow*, QKeyEvent*)));

  connect(theView, SIGNAL(keyReleased(SUIT_ViewWindow*, QKeyEvent*)),
          this,    SIGNAL(keyRelease(SUIT_ViewWindow*, QKeyEvent*)));

  connect(theView, SIGNAL(contextMenuRequested( QContextMenuEvent * )),
          this,    SLOT  (onContextMenuRequested( QContextMenuEvent * )));

  for ( uint i = 0; i < aSize; i++ )
  {
    if ( myViews[i] == 0 )
    {
      myViews[i] = theView;
      return true;
    }
  }
  return false;
}

/*!Emit delete view. Remove view window \a theView from view manager.
*/
void SUIT_ViewManager::onClosingView( SUIT_ViewWindow* theView )
{
  closeView( theView );
}

/*!
  Remove the view window \a theView from view manager and destroy it.
*/
void SUIT_ViewManager::closeView( SUIT_ViewWindow* theView )
{
  if ( !theView )
    return;

  QPointer<SUIT_ViewWindow> view( theView );

  view->setClosable( false );
  view->hide();

  if ( !view->testAttribute( Qt::WA_DeleteOnClose ) )
    return;

  emit deleteView( view );
  removeView( view );

  if ( view )
    delete view;
}

/*!Remove view window \a theView from view manager.
 *And close the last view, if it has \a theView.
*/
void SUIT_ViewManager::removeView( SUIT_ViewWindow* theView )
{
  theView->disconnect( this );
  myViews.remove( myViews.indexOf( theView ) );
  if ( myActiveView == theView )
    myActiveView = 0;
  if ( !myViews.count() )
    emit lastViewClosed( this );
}

/*!
  Set or clear flag Qt::WDestructiveClose for all views
*/
void SUIT_ViewManager::setDestructiveClose( const bool on )
{
  for ( int i = 0; i < myViews.count(); i++ )
    myViews[i]->setDestructiveClose( on );
}

/*!
  Returns 'true' if any of views (view windows) is visible.
*/
bool SUIT_ViewManager::isVisible() const
{
  bool res = false;
  for ( int i = 0; i < myViews.count() && !res; i++ )
    res = myViews[i]->isVisibleTo( myViews[i]->parentWidget() );
  return res;
}

/*!
  Show or hide all views (view windows)
*/
void SUIT_ViewManager::setShown( const bool on )
{
  for ( int i = 0; i < myViews.count(); i++ )
    myViews.at( i )->setVisible( on );
}

/*!Emit on \a theEvent mouse pressed in \a theView.*/
void SUIT_ViewManager::onMousePressed(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  emit mousePress(theView, theEvent);
}

/*!Emit activated for view \a view.
*/
void SUIT_ViewManager::onWindowActivated(SUIT_ViewWindow* view)
{
  if (view) {
    unsigned int aSize = myViews.size();
    for (uint i = 0; i < aSize; i++) {
      if (myViews[i] && myViews[i] == view) {
        myActiveView = view;
        emit activated( this );
        return;
      }
    }
  }
}

/*!Close all views.
*/
void SUIT_ViewManager::closeAllViews()
{
  for ( int i = 0; i < myViews.size(); i++ )
    delete myViews[i];
  myViews.clear();
}

/*!
 *\retval QString - type of view model.
 */
QString SUIT_ViewManager::getType() const
{
  return (!myViewModel)? "": myViewModel->getType();
}

/*!
 *\retval SUIT_Study* - current study.
 */
SUIT_Study* SUIT_ViewManager::study() const
{
    return myStudy;
}

/*!
 * Sets stydy to NULL.
 */
void SUIT_ViewManager::onDeleteStudy()
{
    myStudy = NULL;
}

/*! invoke method of SUIT_PopupClient, which notifies about popup*/
void SUIT_ViewManager::onContextMenuRequested( QContextMenuEvent* e )
{
  contextMenuRequest( e );
}

/*!Context menu popup for \a popup.*/
void SUIT_ViewManager::contextMenuPopup( QMenu* popup )
{
  SUIT_ViewModel* vm = getViewModel();
  if ( vm )
    vm->contextMenuPopup( popup );
}
