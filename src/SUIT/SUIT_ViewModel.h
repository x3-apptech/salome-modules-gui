#ifndef SUIT_VIEWMODEL_H
#define SUIT_VIEWMODEL_H

#include "SUIT.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"

#include <qobject.h>
#include <qcursor.h>

/*!
	This object manages the definition and behaviour of a View Window. 
	In case of definition of this object in an external lib that lib must 
	have an exported function "createViewModel" which returns newly created 
	instance of SUIT_ViewModel.
*/

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_EXPORT SUIT_ViewModel : public QObject
{
	Q_OBJECT
public:
  enum HotOperation { PAN, ZOOM, ROTATE, FIT_AREA };

  typedef QMap<HotOperation, Qt::ButtonState> StatesMap;
  typedef QMap<HotOperation, Qt::ButtonState> ButtonsMap;
  
	SUIT_ViewModel();
	virtual ~SUIT_ViewModel();

	virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);

	virtual void      setViewManager(SUIT_ViewManager* theViewManager) { myViewManager = theViewManager; }
	SUIT_ViewManager* getViewManager() const { return myViewManager; }

  virtual QString   getType() const { return "SUIT_ViewModel"; }

  virtual void      contextMenuPopup(QPopupMenu*) {}

  static void       setHotButton(HotOperation theOper, Qt::ButtonState theState,
                                                       Qt::ButtonState theButton);
  static void       getHotButton(HotOperation theOper, Qt::ButtonState& theState,
                                                       Qt::ButtonState& theButton);

protected:
	SUIT_ViewManager* myViewManager;

public:
  static StatesMap  myStateMap;
  static ButtonsMap myButtonMap;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

extern "C"
{
  typedef SUIT_ViewModel* (*VIEWMODEL_CREATE_FUNC)();
}

#define VIEWMODEL_CREATE_NAME   "createViewModel"

#endif
