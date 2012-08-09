#include "GenericDialog.hxx"

GenericDialog::GenericDialog(QDialog *parent) : QDialog(parent)
{
  ui.setupUi(this); // A faire en premier
  
  /*
    Personnalisez vos widgets ici si nécessaire
    Réalisez des connexions supplémentaires entre signaux et slots
  */
  
  // The slots accept() and reject() are already connected to the
  // buttonbox (inherited features) 
}

QFrame * GenericDialog::getPanel() {
  return ui.centralPanel;
}

QDialogButtonBox * GenericDialog::getButtonBox() {
  return ui.buttonBox;
}


#include <QDebug>
void GenericDialog::accept() {
  qDebug() << "accept() is not implemented yet";
  QDialog::accept();
}
