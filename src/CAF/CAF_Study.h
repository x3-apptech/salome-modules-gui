#ifndef CAF_STUDY_H
#define CAF_STUDY_H

#include "CAF.h"

#include "SUIT_Study.h"

#include <qobject.h>
#include <qptrstack.h>

#include <TDocStd_Document.hxx>

#if defined WNT
#pragma warning ( disable: 4251 )
#endif

class CAF_EXPORT CAF_Study : public SUIT_Study
{
  Q_OBJECT

public:
	CAF_Study( SUIT_Application* theApp );
	CAF_Study( SUIT_Application* theApp, Handle (TDocStd_Document)& aStdDoc );
	virtual ~CAF_Study();

	virtual bool startOperation();
	virtual void abortOperation();
	virtual void commitOperation();

	bool         isSaved() const;
	bool         isModified() const;
	void         doModified( bool undoable = true);
	void         undoModified();
	void         clearModified();

  bool         undo();
	bool         redo();
	bool         canUndo() const;
	bool         canRedo() const;
	QStringList  undoNames() const;
	QStringList  redoNames() const;

  void         setStdDocument( Handle(TDocStd_Document)& aStdDoc ) { myStdDoc = aStdDoc; }
  Handle(TDocStd_Document) getStdDocument() { return myStdDoc; }

protected:
	Handle(TDocStd_Document) myStdDoc;
	int                      myModifiedCnt;

  friend class CAF_Operation;
};

#if defined WNT
#pragma warning ( default: 4251 )
#endif

#endif
