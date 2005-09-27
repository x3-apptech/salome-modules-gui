
#ifndef SALOMEAPP_SHOW_HIDE_OPERATION_HEADER
#define SALOMEAPP_SHOW_HIDE_OPERATION_HEADER

#include "SalomeApp_Operation.h"

class SalomeApp_ShowHideOp : public SalomeApp_Operation
{
  Q_OBJECT

public:
    typedef enum { DISPLAY, ERASE, DISPLAY_ONLY } ActionType;

public:
  SalomeApp_ShowHideOp( ActionType );
  ~SalomeApp_ShowHideOp();

protected:
  virtual void startOperation();

private:
  ActionType   myActionType;
};

#endif

