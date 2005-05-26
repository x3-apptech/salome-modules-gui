#ifndef CAF_APPLICATION_H
#define CAF_APPLICATION_H

#include "CAF.h"

#include "STD_Application.h"

#include <qmap.h>
#include <qptrlist.h>

class QtxAction;
class CAF_Study;

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

class CAF_EXPORT CAF_Application : public STD_Application
{
  Q_OBJECT

public:
  CAF_Application();
  virtual ~CAF_Application();

  virtual QString applicationName() const;

protected slots:
  virtual bool    onUndo( int );
  virtual bool    onRedo( int );

protected:
  enum {  EditUndoId = STD_Application::UserID, EditRedoId, UserID };

protected:
  virtual void    createActions();
  virtual void    updateCommandsStatus();

  bool				    undo( CAF_Study* doc );
  bool				    redo( CAF_Study* doc );
};

#if defined WIN32
#pragma warning ( default: 4251 )
#endif

#endif
