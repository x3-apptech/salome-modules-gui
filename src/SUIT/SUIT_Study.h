#ifndef SUIT_STUDY_H
#define SUIT_STUDY_H

#include "SUIT.h"

#include "SUIT_Operation.h"

#include <qobject.h>
#include <qptrstack.h>

class SUIT_DataObject;
class SUIT_Application;

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
  SUIT_Operation*   activeOperation() const;

  virtual bool      isSaved()  const;
  virtual bool      isModified() const;

  virtual void      closeDocument();
  virtual void      createDocument();
  virtual bool      openDocument( const QString& );

  bool              saveDocument();
  virtual bool      saveDocumentAs( const QString& );

  virtual void      abortAllOperations();

  virtual void      update();

  virtual void      sendChangesNotification();

signals:
  void              studyModified( SUIT_Study* );

protected:
  virtual void      setIsSaved( const bool );
  virtual void      setIsModified( const bool );
  virtual void      setRoot( SUIT_DataObject* );
  virtual void      setStudyName( const QString& );

  void              stopOperation();
  bool              canStartOperation( SUIT_Operation* );

protected:
  typedef QPtrStack<SUIT_Operation> OperationsStack;

protected:
  OperationsStack   myOperations;

private:
  int               myId;
  SUIT_Application* myApp;
  SUIT_DataObject*  myRoot;
  QString           myName;
  bool              myIsSaved;
  bool              myIsModified;

  friend class SUIT_Operation;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
