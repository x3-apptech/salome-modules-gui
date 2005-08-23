/**
*  SALOME SalomeApp
*
*  Copyright (C) 2005  CEA/DEN, EDF R&D
*
*
*
*  File   : SUIT_Operation.h
*  Author : Unknown
*  Module : SALOME
*/

#ifndef SUIT_OPERATION_H
#define SUIT_OPERATION_H

#include <qobject.h>

#include "SUIT.h"

class SUIT_Study;
class SUIT_Application;

/*!
 * \brief Base class for all operations
 *
 *  Base class for all operations. If you perform an action it is reasonable to create
 *  operation intended for this. This is a base class for all operations which provides
 *  mechanism for correct starting operations, starting operations above already started
 *  ones, committing operations and so on. To create own operation it is reasonable to
 *  inherit it from this class and redefines virtual methods to provide own behavior
 *  Main virtual methods are
 *  - virtual bool      isReadyToStart();
 *  - virtual void      startOperation();
 *  - virtual void      abortOperation();
 *  - virtual void      commitOperation();
 *  - virtual void      resumeOperation();
 *  - virtual void      suspendOperation();
*/
class SUIT_EXPORT SUIT_Operation : public QObject
{
  Q_OBJECT

public:

  /*! Enum describes state of operation */
  enum OperationState
  {
    Waiting,  //!< Operation is not used (it is not run or suspended)
    Running,  //!< Operation is started
    Suspended //!< Operation is started but suspended (other operation is performed above it)
  };

  /*!
  * Enum describes execution status of operation. Execution status often used after
  * ending work of operation which was started from this one. In this case this
  * operation can ask previously started operation whether it finished successfully.
  */
  enum ExecStatus
  {
    Rejected, //!< Operation has not performed any action (modification of data model for example)
    Accepted  //!< Operation has performed an actions and must be stopped
  };

public:

  SUIT_Operation( SUIT_Application* );
  virtual ~SUIT_Operation();

  OperationState    state() const;
  bool              isActive() const;

  SUIT_Study*       study() const;
  virtual void      setStudy( SUIT_Study* theStudy );
  
  SUIT_Application* application() const;
  virtual void      setApplication( SUIT_Application* theApp );

  virtual bool      isValid( SUIT_Operation* theOtherOp ) const;
  virtual bool      isGranted() const;

  bool              setSlot( const QObject* theReceiver, const char* theSlot );

public slots:

  void              start();
  void              abort();
  void              commit();
  void              suspend();
  void              resume();

signals:

  void              started( SUIT_Operation* );
  void              aborted( SUIT_Operation* );
  void              resumed( SUIT_Operation* );
  void              committed( SUIT_Operation* );
  void              suspended( SUIT_Operation* );
  void              stopped( SUIT_Operation* );
  
  void              callSlot();

protected:

  virtual bool      isReadyToStart() const;
  
  virtual void      startOperation();
  virtual void      abortOperation();
  virtual void      commitOperation();
  virtual void      suspendOperation();
  virtual void      resumeOperation();

  void              setExecStatus( const int theStatus );
  int               execStatus() const;

  void              setState( const OperationState theState );

  void              start( SUIT_Operation* theOp );

private:

  SUIT_Application* myApp;        //!< application for this operation
  SUIT_Study*       myStudy;      //!< study for this operation
  OperationState    myState;      //!< Operation state
  ExecStatus        myExecStatus; //!< Execution status

  friend class SUIT_Study;
};

#endif
