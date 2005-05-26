// CAF_Operation.h: interface for the CAF_Operation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAF_OPERATION_H__87D24897_EA69_4A5A_B81F_39E25ABC254B__INCLUDED_)
#define AFX_CAF_OPERATION_H__87D24897_EA69_4A5A_B81F_39E25ABC254B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <qobject.h>
#include <qstring.h>
#include <qiconset.h>
#include <qkeysequence.h>

#include "CAF.h"

#include "SUIT_Operation.h"

class CAF_Study;

class CAF_EXPORT CAF_Operation : public SUIT_Operation
{
	Q_OBJECT
public:
	CAF_Operation(SUIT_Application* theApp);
	virtual ~CAF_Operation();

	void             setName( const QString& name );
    const QString&   getName() const;

protected: 
    bool myIsDataChanged;
    // All operations-successors must set this field to true in order
    // to indicates that data are changed or false if aren't changed.
    // By default, in startOperation method this field is set to true.

protected:
	virtual void startOperation();
	virtual void abortOperation();
	virtual void commitOperation();

private:
	QString  myName;

	friend class SUIT_Study;
};

#endif // !defined(AFX_SUIT_OPERATION_H__87D24897_EA69_4A5A_B81F_39E25ABC254B__INCLUDED_)
