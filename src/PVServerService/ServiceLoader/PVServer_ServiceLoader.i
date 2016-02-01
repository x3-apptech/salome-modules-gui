%module PVServer_ServiceLoader

%{
  #include "PVServer_ServiceLoader.h"
%}

%include <std_string.i>

class PVServer_ServiceLoader
{
public:
  //! Get the IOR of the CORBA service handling the PVServer
  std::string findOrLoadService(const char * containerName);
};
