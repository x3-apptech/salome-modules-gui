// SUIT_Accel.cxx: implementation of the SUIT_Accel class.
//
//////////////////////////////////////////////////////////////////////

#include "SUIT_Accel.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewModel.h"

#include <qaccel.h>

/*!\class SUIT_Accel
 * Class handles keyboard accelerator bindings.
 */

/*! Constructor.*/
SUIT_Accel::SUIT_Accel(SUIT_Desktop* theDesktop)
  : QObject( theDesktop, "SUIT_Accel" ),
    myDesktop( theDesktop )
{
  myAccel = new QAccel( theDesktop, "SUIT_Accel_interal_qaccel" );
  connect( myAccel, SIGNAL( activated( int ) ), this, SLOT( onActivated( int ) ) );
}

/*! Destructor.*/
SUIT_Accel::~SUIT_Accel()
{
}

/*! setActionKey  assign a ceratain action for a key accelerator */
void SUIT_Accel::setActionKey( const int action, const int key, const QString& type )
{    
  if ( myKeyActionMap.contains( key ) )
    myAccel->removeItem( action );

  myKeyActionMap[key] = action;
  QStringList vTypes;
  if ( myActionViewerTypesMap.contains( action ) )
    vTypes = myActionViewerTypesMap[action];
  if ( !vTypes.contains( type ) )
    vTypes.append( type );
  myActionViewerTypesMap[action] = vTypes;

  myAccel->insertItem( key, action );
}

/*! onActivated  slot called when a registered key accelerator was activated */
void SUIT_Accel::onActivated( int action )
{
  if ( myDesktop ) {
    if ( SUIT_ViewWindow* vw = myDesktop->activeWindow() ) {
      QString type = vw->getViewManager()->getViewModel()->getType();
      if (  myActionViewerTypesMap.contains( action ) ) {
	QStringList vTypes = myActionViewerTypesMap[action];
	if ( vTypes.contains( type ) ) {
	  vw->onAccelAction( action );
	}
      }
    }
  }
}

