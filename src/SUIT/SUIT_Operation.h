#ifndef SUIT_OPERATION_H
#define SUIT_OPERATION_H

#include <qobject.h>
#include <qstring.h>
#include <qiconset.h>
#include <qkeysequence.h>

#include "SUIT.h"

class SUIT_Study;
class SUIT_Application;

class SUIT_EXPORT SUIT_Operation : public QObject  
{
	Q_OBJECT

public:
	enum OperationState { Waiting, Running, Suspended };

public:
	SUIT_Operation( SUIT_Application* );
	virtual ~SUIT_Operation();

	OperationState    state() const;
	SUIT_Study*       study() const;
	SUIT_Application* application() const;

	bool              setSlot( const QObject* theReceiver, const char* theSlot );

public slots:
	void              start();
	void              abort();
	void              commit();
	void              resume();
	void              suspend();

signals:
	void              started( SUIT_Operation* );
	void              aborted( SUIT_Operation* );
	void              resumed( SUIT_Operation* );
	void              commited( SUIT_Operation* );
	void              suspended( SUIT_Operation* );
	void              callSlot();

protected:
	virtual bool      isReadyToStart();
	virtual void      startOperation();
	virtual void      abortOperation();
	virtual void      commitOperation();
	virtual void      resumeOperation();
	virtual void      suspendOperation();

  // Returns TRUE if the given operator is valid for (can be started "above") the current operator
	virtual bool      isValid( SUIT_Operation* ) const;

	/*!
		Returns TRUE if current operation must not be checked for 
		ActiveOperation->IsValid(this).  Default implementation returns FALSE, 
		so it is being checked for IsValid, but some operations may overload IsGranted()
		In this case they will always start, no matter what operation is running.
	*/
	virtual bool      isGranted() const;

  virtual void      setStudy( SUIT_Study* );
  virtual void      setApplication( SUIT_Application* );

private:
	SUIT_Application* myApp;
	SUIT_Study*		    myStudy;
	OperationState	  myState;

	friend class SUIT_Study;
};

#endif
