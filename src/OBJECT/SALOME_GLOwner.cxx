
#include <SALOME_GLOwner.h>

SALOME_GLOwner::SALOME_GLOwner( const char* entry )
: GLViewer_Owner()
{
  setEntry( entry );
}

SALOME_GLOwner::~SALOME_GLOwner()
{
}

const char* SALOME_GLOwner::entry() const
{
  return myEntry.c_str();
}

void SALOME_GLOwner::setEntry( const char* entry )
{
  myEntry = entry;
}
