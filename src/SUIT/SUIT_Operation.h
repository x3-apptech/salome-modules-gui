// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef SUIT_OPERATION_H
#define SUIT_OPERATION_H

#include "SUIT.h"

#include <QObject>
#include <QPointer>

class SUIT_Study;
class SUIT_Application;

/*!
   \class SUIT_Operation
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
#ifdef WIN32
#pragma warning( disable:4251 )
#endif

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

  /*!
  * Enum describes setting of the operation.
  */
  enum Flags
  {
    None        = 0x00, //!< None options
    Transaction = 0x01  //!< Automatically open (commit/abort) transaction during start (commit/abort).
  };

public:
  SUIT_Operation( SUIT_Application* );
  virtual ~SUIT_Operation();

  OperationState    state() const;
  bool              isActive() const;
  bool              isRunning() const;

  SUIT_Study*       study() const;
  virtual void      setStudy( SUIT_Study* theStudy );
  
  SUIT_Application* application() const;
  virtual void      setApplication( SUIT_Application* theApp );

  virtual bool      isValid( SUIT_Operation* theOtherOp ) const;
  virtual bool      isGranted() const;

  bool              setSlot( const QObject* theReceiver, const char* theSlot );

  void              setFlags( const int );
  void              clearFlags( const int );
  bool              testFlags( const int ) const;

  virtual QString   operationName() const;

  int               execStatus() const;

signals:
  void              started( SUIT_Operation* );
  void              aborted( SUIT_Operation* );
  void              committed( SUIT_Operation* );

  void              stopped( SUIT_Operation* );
  void              resumed( SUIT_Operation* );
  void              suspended( SUIT_Operation* );

  void              callSlot();

public slots:
  void              start();
  void              abort();
  void              commit();
  void              resume();
  void              suspend();

protected:
  virtual bool      isReadyToStart() const;

  virtual void      stopOperation();
  virtual void      startOperation();
  virtual void      abortOperation();
  virtual void      commitOperation();
  virtual void      resumeOperation();
  virtual void      suspendOperation();

  virtual bool      openTransaction();
  virtual bool      abortTransaction();
  virtual bool      hasTransaction() const;
  virtual bool      commitTransaction( const QString& = QString() );

  void              setExecStatus( const int );

  void              setState( const OperationState );

  void              start( SUIT_Operation*, const bool = false );

private:
  typedef QPointer<SUIT_Study> StudyPtr;

private:
  SUIT_Application* myApp;        //!< application for this operation
  int               myFlags;      //!< operation flags
  StudyPtr          myStudy;      //!< study for this operation
  OperationState    myState;      //!< Operation state
  ExecStatus        myExecStatus; //!< Execution status

  friend class SUIT_Study;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
