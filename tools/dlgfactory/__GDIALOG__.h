#ifndef ___CLASSNAME___HXX
#define ___CLASSNAME___HXX

#include "ui___CLASSNAME__.h"
#include "GenericDialog.h"

class __CLASSNAME__ : public GenericDialog
{
  Q_OBJECT

public:
  __CLASSNAME__(QDialog* parent = 0);

private:
  Ui___CLASSNAME__ ui; // instance of the class defined in ui___CLASSNAME__.hxx
};


#endif // ___CLASSNAME___HXX
