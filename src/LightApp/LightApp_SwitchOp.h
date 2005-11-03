/**
*  LIGHT LightApp
*
*  Copyright (C) 2005  CEA/DEN, EDF R&D
*
*
*
*  File   : LightApp_SwitchOp.h
*  Author : Sergey LITONIN
*  Module : LIGHT
*/



#ifndef LightApp_SwitchOp_H
#define LightApp_SwitchOp_H

#include "LightApp.h"
#include <qobject.h>

class LightApp_Module;
class LightApp_Operation;
class QEvent;
class SUIT_Study;

/*!
 * \brief This class is intended for controling switching between operation
 *
 * Several operation may be launched simultaneously. This class is intended for
 * controlling switching between such operations. This class works with operations having
 * dialogs (activation of other operations is performed by SUIT_Study). When several
 * operations is launched simultaneously corresponding dialogs are shown on the screen.
 * Only one operation from the launched ones can be active (active operation). Other
 * operations are suspended. As result only one dialog from shown ones can be active too.
 * Other dialogs are disabled. This class installs event filter on application. When mouse
 * cursor is moved above disabled dialog corresponding event is catched by this class.
 * It finds corresponding operation and verify whether operation can be resumed (see
 * SUIT_Study::isDenied( SUIT_Operation* ) method). If yes then current active
 * operation is suspended and new operation activated. Module contains this class as a
 * field. Then module is created instance of this class created too.
 */
class LIGHTAPP_EXPORT LightApp_SwitchOp : public QObject
{
  Q_OBJECT

public:

  LightApp_SwitchOp( LightApp_Module* );
  virtual ~LightApp_SwitchOp();

  // Redefined from base class
  bool                     eventFilter( QObject*, QEvent* );

private:

  LightApp_Module*         module() const;
  LightApp_Operation*      operation( QWidget* ) const;
  SUIT_Study*              study() const;
  
private:
  
  LightApp_Module*         myModule;  

};

#endif






