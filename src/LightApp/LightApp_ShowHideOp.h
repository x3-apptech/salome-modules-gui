
#ifndef LIGHTAPP_SHOW_HIDE_OPERATION_HEADER
#define LIGHTAPP_SHOW_HIDE_OPERATION_HEADER

#include "LightApp_Operation.h"

class LightApp_Displayer;
class LIGHTAPP_EXPORT LightApp_ShowHideOp : public LightApp_Operation
{
  Q_OBJECT

public:
    typedef enum { DISPLAY, ERASE, DISPLAY_ONLY, ERASE_ALL } ActionType;

public:
  LightApp_ShowHideOp( ActionType );
  ~LightApp_ShowHideOp();
  
protected:
  virtual void startOperation();
  virtual LightApp_Displayer* displayer( const QString& ) const;

private:
  ActionType   myActionType;
};

#endif

