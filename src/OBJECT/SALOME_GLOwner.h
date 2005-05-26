#ifndef SALOME_GLOWNER_H
#define SALOME_GLOWNER_H

#include <string>
#include <GLViewer_Object.h>

class Standard_EXPORT SALOME_GLOwner : public GLViewer_Owner
{
public:
  SALOME_GLOwner( const char* );
  ~SALOME_GLOwner();

  const char*       entry() const;
  void              setEntry( const char* );

private:
  std::string       myEntry;
};

#endif
