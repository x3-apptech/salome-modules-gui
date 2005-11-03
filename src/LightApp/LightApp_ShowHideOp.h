
#ifndef LIGHTAPP_SHOW_HIDE_OPERATION_HEADER
#define LIGHTAPP_SHOW_HIDE_OPERATION_HEADER

#include "LightApp_Operation.h"

class LightApp_ShowHideOp : public LightApp_Operation
{
  Q_OBJECT

public:
    typedef enum { DISPLAY, ERASE, DISPLAY_ONLY } ActionType;

public:
  LightApp_ShowHideOp( ActionType );
  ~LightApp_ShowHideOp();

protected:
  virtual void startOperation();

private:
  ActionType   myActionType;
};

#endif

