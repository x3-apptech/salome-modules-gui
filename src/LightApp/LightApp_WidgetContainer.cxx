#include "LightApp_WidgetContainer.h"

#include <qobjectlist.h>
#include <qwidgetstack.h>

/*!
  Constructor.
*/
LightApp_WidgetContainer::LightApp_WidgetContainer( const int type, QWidget* parent )
: QDockWindow( QDockWindow::InDock, parent ),
myType( type )
{
  setWidget( myStack = new QWidgetStack( this ) );
  myStack->show();
}

/*!
  Destructor.
*/
LightApp_WidgetContainer::~LightApp_WidgetContainer()
{
}

/*!
  Checks: is widget container is empty?
*/
bool LightApp_WidgetContainer::isEmpty() const
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

/*!
  Gets type of widget container.
*/
int LightApp_WidgetContainer::type() const
{
  return myType;
}

/*!
  Checks: is container contains widget with id \a id.
*/
bool LightApp_WidgetContainer::contains( const int id ) const
{
  return myStack->widget( id ) != 0;
}

/*!
 * Insert widget(\a wid with id \a id) to container.And return id of widget in stack.
 *\warning remove widget with id = \a id , if it was in container.
 */
int LightApp_WidgetContainer::insert( const int id, QWidget* wid )
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

/*!
  Remove widget(\a wid) from stack.
*/
void LightApp_WidgetContainer::remove( const int id )
{
  remove( myStack->widget( id ) );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

/*!
  Remove widget(\a wid) from stack.
*/
void LightApp_WidgetContainer::remove( QWidget* wid )
{
  myStack->removeWidget( wid );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

/*!
  Raise widget with id = \a id.
*/
void LightApp_WidgetContainer::activate( const int id )
{
  myStack->raiseWidget( id );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

/*!
  Raise widget (\a wid).
*/
void LightApp_WidgetContainer::activate( QWidget* wid )
{
  myStack->raiseWidget( wid );

  setCaption( myStack->visibleWidget() ? myStack->visibleWidget()->caption() : QString::null );
}

/*!
  Gets widget from container list(stack) by id = \a id.
*/
QWidget* LightApp_WidgetContainer::widget( const int id ) const
{
  return myStack->widget( id );
}

/*!
  Gets visible widget.
*/
QWidget* LightApp_WidgetContainer::active() const
{
  return myStack->visibleWidget();
}
