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
#include "SUIT_ViewManager.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewModel.h"
#include "SUIT_Study.h"

#include <qcursor.h>
#include <qmessagebox.h>

#ifdef WNT
#include <windows.h>
#endif

/*!\class SUIT_ViewManager.
 * Class provide manipulation with view windows.
 */

/*!Constructor.*/
SUIT_ViewManager::SUIT_ViewManager( SUIT_Study* theStudy,
                                    SUIT_Desktop* theDesktop,
                                    SUIT_ViewModel* theViewModel )
: QObject( 0 ),
myDesktop( theDesktop ),
myTitle( "Default viewer" ),
myStudy( NULL )
{
  myViewModel = 0;
  myActiveView = 0;
  setViewModel(theViewModel);
  connect(theDesktop, SIGNAL(windowActivated(SUIT_ViewWindow*)), 
          this,       SLOT(onWindowActivated(SUIT_ViewWindow*)));

  myStudy = theStudy;
  if( myStudy )
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
void SUIT_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption(myTitle + QString(":%1").arg(aPos+1));
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
  //myDesktop->addViewWindow( aView );
  //it is done automatically during creation of view

  aView->setViewManager(this);

  emit viewCreated(aView);

  // Special treatment for the case when <aView> is the first one in this view manager
  // -> call onWindowActivated() directly, because somebody may always want
  // to use getActiveView()
  if ( !myActiveView )
    onWindowActivated( aView );

  return aView;
}

/*!Create view window.*/
void SUIT_ViewManager::createView()
{
  createViewWindow();
}

/*!Insert view window to view manager.
 *\retval false - if something wrong, else true.
 */
bool SUIT_ViewManager::insertView(SUIT_ViewWindow* theView)
{
  unsigned int aSize = myViews.size();
  unsigned int aNbItems = myViews.count()+1;
  if (aNbItems > aSize) {
    if (!myViews.resize(aNbItems)) {
      QMessageBox::critical(myDesktop, tr("Critical error"), tr("There is no memory for the new view!!!"));
      return false;
    }
    aSize = myViews.size();
  }
  
  connect(theView, SIGNAL(closing(SUIT_ViewWindow*)),
          this,    SLOT(onDeleteView(SUIT_ViewWindow*)));

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

  for (uint i = 0; i < aSize; i++) {
    if (myViews[i]==0) {
      myViews.insert(i, theView);
      return true;
    }
  }
  return false;
}

/*!Emit delete view. Remove view window \a theView from view manager.
*/
void SUIT_ViewManager::onDeleteView(SUIT_ViewWindow* theView)
{
  emit deleteView(theView);
  removeView(theView);
}

/*!Remove view window \a theView from view manager.
 *And close the last view, if it has \a theView.
*/
void SUIT_ViewManager::removeView(SUIT_ViewWindow* theView) 
{
  theView->disconnect(this);
  myViews.remove(myViews.find(theView));
  if (myActiveView == theView)
    myActiveView = 0;
  int aNumItems = myViews.count();
  if (aNumItems == 0)
    emit lastViewClosed(this);
}

/*!
  Show or hide all views (view windows)
*/
void SUIT_ViewManager::setShown( const bool on )
{
  for ( uint i = 0; i < myViews.count(); i++ )
    myViews.at( i )->setShown( on );
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
  unsigned int aSize = myViews.size();
  for (uint i = 0; i < aSize; i++) {
    if (myViews[i])
      myViews[i]->close();
  }
}

/*!
 *\retval QString - type of view model.
 */
QString  SUIT_ViewManager::getType() const 
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
void SUIT_ViewManager::contextMenuPopup( QPopupMenu* popup )
{
  SUIT_ViewModel* vm = getViewModel();
  if ( vm )
    vm->contextMenuPopup( popup );
}
