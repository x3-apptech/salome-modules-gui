#ifndef _GenericDialog_HXX
#define _GenericDialog_HXX

#include <QtGui>
#include "ui_GenericDialog.hxx"

class GenericDialog : public QDialog
{
  Q_OBJECT
    
public:
  GenericDialog(QDialog *parent = 0);

 protected:
  QFrame * getPanel();
  QDialogButtonBox * getButtonBox();
  
 protected slots:    
    void accept();
  //void reject();
  
 private:
  Ui_GenericDialog ui; // instance of the class defined in ui_GenericDialog.h
};


#endif // _GenericDialog_HXX
