#include "SalomeApp_WidgetContainer.h"

#include <qobjectlist.h>
#include <qwidgetstack.h>

SalomeApp_WidgetContainer::SalomeApp_WidgetContainer( const int type, QWidget* parent )
: QDockWindow( QDockWindow::InDock, parent ),
myType( type )
{
  setWidget( myStack = new QWidgetStack( this ) );
  myStack->show();
}

SalomeApp_WidgetContainer::~SalomeApp_WidgetContainer()
{
}

bool SalomeApp_WidgetContainer::isEmpty() const
{
  const QObjectList* lst = myStack->children();
  if ( !lst )
    return true;

  bool res = true;
  for ( QObjectListIt it( *lst ); it.current() && res; ++it )
  {
    if ( it.current()->isWidgetType() && myStack->id( (QWidget*)it.current() ) != -1 )
      res = false;
  }
  return res;
}

int SalomeApp_WidgetContainer::type() const
{
  return myType;
}

bool SalomeApp_WidgetContainer::contains( const int id ) const
{
  return myStack->widget( id ) != 0;
}

int SalomeApp_WidgetContainer::insert( const int id, QWidget* wid )
{
  if ( id == -1 || !wid )
    return -1;

  if ( contains( id ) )
    remove( id );

  int stackId = myStack->addWidget( wid, id );
  if ( !myStack->visibleWidget() )
    myStack->raiseWidget( wid );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );

  return stackId;
}

void SalomeApp_WidgetContainer::remove( const int id )
{
  remove( myStack->widget( id ) );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

void SalomeApp_WidgetContainer::remove( QWidget* wid )
{
  myStack->removeWidget( wid );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

void SalomeApp_WidgetContainer::activate( const int id )
{
  myStack->raiseWidget( id );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

void SalomeApp_WidgetContainer::activate( QWidget* wid )
{
  myStack->raiseWidget( wid );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

QWidget* SalomeApp_WidgetContainer::widget( const int id ) const
{
  return myStack->widget( id );
}

QWidget* SalomeApp_WidgetContainer::active() const
{
  return myStack->visibleWidget();
}
