// CAF_Operation.cxx: implementation of the CAF_Operation class.
//
//////////////////////////////////////////////////////////////////////

#include "CAF_Operation.h"
#include "CAF_Application.h"
#include "CAF_Study.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAF_Operation::CAF_Operation(SUIT_Application* theApp)
:SUIT_Operation(theApp)
{
}

CAF_Operation::~CAF_Operation()
{
}

/*!
    Sets the name of this operation.
    This name is displayed in 'Undo'/'Redo' list. [ public ]
*/
void CAF_Operation::setName( const QString& name )
{
	myName = name;
}

/*!
    Returns the name of this operation.
    This name is displayed in 'Undo'/'Redo' list. [ public ]
*/
const QString& CAF_Operation::getName() const
{
	return myName;
}

void CAF_Operation::startOperation()
{
  myIsDataChanged = true;
  CAF_Study* cafStudy = dynamic_cast<CAF_Study*>( study() );
	if ( cafStudy )
	  cafStudy->startOperation();
}

void CAF_Operation::abortOperation()
{
  CAF_Study* cafStudy = dynamic_cast<CAF_Study*>( study() );
	if ( cafStudy )
	  cafStudy->abortOperation();
}

void CAF_Operation::commitOperation()
{
  CAF_Study* cafStudy = dynamic_cast<CAF_Study*>( study() );
  if ( cafStudy )
  {
	  cafStudy->commitOperation();
    if ( myIsDataChanged )
      cafStudy->doModified();
	}
}
