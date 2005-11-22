#ifndef CAF_STUDY_H
#define CAF_STUDY_H

#include "CAF.h"

#include "SUIT_Study.h"

#include <qobject.h>

#include <TDocStd_Document.hxx>
#include <TDocStd_Application.hxx>

class CAF_Application;

#if defined WNT
#pragma warning ( disable: 4251 )
#endif

class CAF_EXPORT CAF_Study : public SUIT_Study
{
  Q_OBJECT

public:
	CAF_Study( SUIT_Application* theApp );
	CAF_Study( SUIT_Application* theApp, Handle(TDocStd_Document)& aStdDoc );
	virtual ~CAF_Study();

  virtual void                createDocument();
  virtual void                closeDocument( bool = true );
  virtual bool                openDocument( const QString& );

  virtual bool                saveDocumentAs( const QString& );

  bool                        isSaved() const;
	bool                        isModified() const;
	void                        doModified( bool = true );
	void                        undoModified();
	void                        clearModified();

  bool                        undo();
	bool                        redo();
	bool                        canUndo() const;
	bool                        canRedo() const;
	QStringList                 undoNames() const;
	QStringList                 redoNames() const;

  Handle(TDocStd_Document)    stdDoc() const;

protected:
  Handle(TDocStd_Application) stdApp() const;
  CAF_Application*            cafApplication() const;

  virtual bool                openTransaction();
  virtual bool                abortTransaction();
  virtual bool                hasTransaction() const;
  virtual bool                commitTransaction( const QString& = QString::null );

  virtual void                setStdDoc( Handle(TDocStd_Document)& );

private:
	Handle(TDocStd_Document)    myStdDoc;
	int                         myModifiedCnt;

  friend class CAF_Operation;
};

#if defined WNT
#pragma warning ( default: 4251 )
#endif

#endif
