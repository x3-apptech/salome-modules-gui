#ifndef ___CLASSNAME___HXX
#define ___CLASSNAME___HXX

#include <QtGui>
#include "ui___CLASSNAME__.hxx"

class __CLASSNAME__ : public QDialog
{
  Q_OBJECT
    
 public:
  __CLASSNAME__(QDialog *parent = 0);

 protected slots:    
    void accept();
  //void reject();
  
 private:
  Ui___CLASSNAME__ ui; // instance of the class defined in ui___CLASSNAME__.hxx
};


#endif // ___CLASSNAME___HXX
