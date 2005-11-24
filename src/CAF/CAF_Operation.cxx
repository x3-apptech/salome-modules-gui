#include "CAF_Operation.h"

#include "CAF_Study.h"
#include "CAF_Application.h"

#include <TDocStd_Document.hxx>

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

Handle(TDocStd_Document) CAF_Operation::stdDoc() const
{
  Handle(TDocStd_Document) doc;
  CAF_Study* s = ::qt_cast<CAF_Study*>( study() );
  if ( s )
    doc = s->stdDoc();
  return doc;
}
