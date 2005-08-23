#ifndef SUIT_STUDY_H
#define SUIT_STUDY_H

#include "SUIT.h"

#include "SUIT_Operation.h"

#include <qobject.h>
#include <qptrlist.h>

class SUIT_DataObject;
class SUIT_Application;
class QDialog;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class SUIT_EXPORT SUIT_Study : public QObject
{
  Q_OBJECT
  
public:
  SUIT_Study( SUIT_Application* );
  virtual ~SUIT_Study();

  virtual int       id() const;

  SUIT_DataObject*  root() const;
  QString           studyName() const;
  SUIT_Application* application() const;

  virtual bool      isSaved()  const;
  virtual bool      isModified() const;

  virtual void      closeDocument(bool permanently = true);
  virtual void      createDocument();
  virtual bool      openDocument( const QString& );

  bool              saveDocument();
  virtual bool      saveDocumentAs( const QString& );

  virtual void      update();

  virtual void      sendChangesNotification();

  // Operation management
  SUIT_Operation*   activeOperation() const;
  virtual void      abortAllOperations();
  const QPtrList<SUIT_Operation>& operations() const;
  
  virtual SUIT_Operation* blockingOperation( SUIT_Operation* ) const;

  bool              start( SUIT_Operation*, const bool check = true );
  bool              abort( SUIT_Operation* );
  bool              commit( SUIT_Operation* );
  bool              suspend( SUIT_Operation* );
  bool              resume( SUIT_Operation* );

signals:
  void              studyModified( SUIT_Study* );

protected:
  virtual void      setIsSaved( const bool );
  virtual void      setIsModified( const bool );
  virtual void      setRoot( SUIT_DataObject* );
  virtual void      setStudyName( const QString& );

private:
  typedef QPtrList<SUIT_Operation> Operations;
  void              stop( SUIT_Operation* );

private:
  int               myId;
  SUIT_Application* myApp;
  SUIT_DataObject*  myRoot;
  QString           myName;
  bool              myIsSaved;
  bool              myIsModified;
  Operations        myOperations;
  bool              myBlockChangeState;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
