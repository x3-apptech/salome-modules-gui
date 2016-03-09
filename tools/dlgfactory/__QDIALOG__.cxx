#include "__CLASSNAME__.h"
#include <QDebug>

__CLASSNAME__::__CLASSNAME__(QDialog* parent) : QDialog(parent)
{
  ui.setupUi(this); // A faire en premier
  
  /*
    Personnalisez vos widgets ici si nécessaire
    Réalisez des connexions supplémentaires entre signaux et slots
  */
  
  // The slots accept() and reject() are already connected to the
  // buttonbox (inherited features) 
}

void __CLASSNAME__::accept() {
  qDebug() << "accept() is not implemented yet";
  QDialog::accept();
}
