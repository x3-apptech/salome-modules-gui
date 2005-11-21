#ifndef CAF_APPLICATION_H
#define CAF_APPLICATION_H

#include "CAF.h"

#include "STD_Application.h"

#include <qmap.h>
#include <qptrlist.h>

#include <TDocStd_Application.hxx>

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
  CAF_Application( const Handle(TDocStd_Application)& );
  virtual ~CAF_Application();

  virtual QString             applicationName() const;

  Handle(TDocStd_Application) stdApp() const;

  virtual QString             getFileFilter() const;

public slots:  
  virtual void                onHelpAbout();

protected slots:
  virtual bool                onUndo( int );
  virtual bool                onRedo( int );
  
protected:
  enum {  EditUndoId = STD_Application::UserID, EditRedoId, UserID };

protected:
  virtual void                createActions();
  virtual void                updateCommandsStatus();

  virtual SUIT_Study*         createNewStudy();

  bool				                undo( CAF_Study* doc );
  bool				                redo( CAF_Study* doc );

  virtual void                setStdApp( const Handle(TDocStd_Application)& );

private:
  Handle(TDocStd_Application) myStdApp;
};

#if defined WIN32
#pragma warning ( default: 4251 )
#endif

#endif
