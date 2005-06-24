#include "SUIT_ViewManager.h"

#include "SUIT_Desktop.h"
#include "SUIT_ViewModel.h"
#include "SUIT_Study.h"

#include <qcursor.h>
#include <qmessagebox.h>

#ifdef WNT
#include <windows.h>
#endif

//***************************************************************
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

//***************************************************************
SUIT_ViewManager::~SUIT_ViewManager()
{
  if (myViewModel) {
    myViewModel->setViewManager(0);
    delete myViewModel;
  }
}

//***************************************************************
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

//***************************************************************
void SUIT_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
  int aPos = myViews.find(theView);
  theView->setCaption(myTitle + QString(":%1").arg(aPos+1));
}

//***************************************************************
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

//***************************************************************
void SUIT_ViewManager::createView()
{
  createViewWindow();
}

//***************************************************************
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


//***************************************************************
void SUIT_ViewManager::onDeleteView(SUIT_ViewWindow* theView)
{
  emit deleteView(theView);
  removeView(theView);
}

//***************************************************************
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

//***************************************************************
void SUIT_ViewManager::onMousePressed(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  emit mousePress(theView, theEvent);
}

//***************************************************************
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

//***************************************************************
void SUIT_ViewManager::closeAllViews()
{
  unsigned int aSize = myViews.size();
  for (uint i = 0; i < aSize; i++) {
    if (myViews[i])
      myViews[i]->close();
  }
}


//***************************************************************
QString  SUIT_ViewManager::getType() const 
{ 
  return (!myViewModel)? "": myViewModel->getType(); 
}

//***************************************************************
SUIT_Study* SUIT_ViewManager::study() const
{
    return myStudy;
}

//***************************************************************
void SUIT_ViewManager::onDeleteStudy()
{
    myStudy = NULL;
}

//***************************************************************
void SUIT_ViewManager::onContextMenuRequested( QContextMenuEvent* e )
{
  // invoke method of SUIT_PopupClient, which notifies about popup
  contextMenuRequest( e );
}

//***************************************************************
void SUIT_ViewManager::contextMenuPopup( QPopupMenu* popup )
{
  SUIT_ViewModel* vm = getViewModel();
  if ( vm )
    vm->contextMenuPopup( popup );
}
