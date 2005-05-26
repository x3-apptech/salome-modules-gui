#include "SalomeApp_GLSelector.h"

#include "SalomeApp_DataOwner.h"

#include <SALOME_GLOwner.h>

#include <GLViewer_Context.h>

SalomeApp_GLSelector::SalomeApp_GLSelector( GLViewer_Viewer2d* viewer, SUIT_SelectionMgr* mgr )
: QObject( 0 ),
SUIT_Selector( mgr ),
myViewer( viewer )
{
  if ( myViewer )
    connect( myViewer, SIGNAL( selectionChanged() ), this, SLOT( onSelectionChanged() ) );
}

SalomeApp_GLSelector::~SalomeApp_GLSelector()
{
}

GLViewer_Viewer2d* SalomeApp_GLSelector::viewer() const
{
  return myViewer;
}

void SalomeApp_GLSelector::onSelectionChanged()
{
  selectionChanged();
}

void SalomeApp_GLSelector::getSelection( SUIT_DataOwnerPtrList& aList ) const
{
  if ( !myViewer )
    return;

  GLViewer_Context* cont = myViewer->getGLContext();
  if ( !cont )
    return;

  for ( cont->InitSelected(); cont->MoreSelected(); cont->NextSelected() )
  {
    GLViewer_Object* obj = cont->SelectedObject();
    if ( obj )
    {
      SALOME_GLOwner* owner = dynamic_cast< SALOME_GLOwner* >( obj->owner() );
      if( owner )
        aList.append( SUIT_DataOwnerPtr( new SalomeApp_DataOwner( owner->entry() ) ) );
    }
  }
}

void SalomeApp_GLSelector::setSelection( const SUIT_DataOwnerPtrList& aList )
{
  if ( !myViewer )
    return;

  GLViewer_Context* cont = myViewer->getGLContext();
  if ( !cont )
    return;

  QMap<QString, GLViewer_Object*> aDisplayed;
  const ObjectMap& displayed = cont->getObjects();
  for ( ObjectMap::const_iterator it = displayed.begin(); it != displayed.end(); ++it )
  {
    if ( it.key()->getVisible() )
    {
      GLViewer_Object* obj = it.key();
      if ( obj )
      {
        SALOME_GLOwner* owner = dynamic_cast< SALOME_GLOwner* >( obj->owner() );
  	    if ( owner )
          aDisplayed.insert( owner->entry(), obj );
      }
    }
  }

  int Nb = 0;
  cont->clearSelected( false );
  for ( SUIT_DataOwnerPtrList::const_iterator itr = aList.begin(); itr != aList.end(); ++itr )
  {
    const SalomeApp_DataOwner* owner = dynamic_cast<const SalomeApp_DataOwner*>( (*itr).operator->() );

    if ( !owner )
      continue;

    if ( aDisplayed.contains( owner->entry() ) )
    {
      cont->setSelected( aDisplayed[owner->entry()], false );
      Nb++;
    }
  }

  if ( Nb > 0 )
    myViewer->updateAll();
}
