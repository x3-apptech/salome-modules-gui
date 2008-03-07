
#ifndef OBJECT_BROWSER_FIND_DIALOG_HEADER
#define OBJECT_BROWSER_FIND_DIALOG_HEADER

#include <qgroupbox.h>

class OB_ObjSearch;
class QLineEdit;
class QPushButton;
class QCheckBox;

class OB_FindDlg : public QGroupBox
{
  Q_OBJECT

public:
  OB_FindDlg( QWidget* = 0 );
  virtual ~OB_FindDlg();

  OB_ObjSearch* getSearch() const;
  void setSearch( OB_ObjSearch* );

private slots:
  void onFind();
  void onClose();

private:
  QLineEdit* myData;
  QPushButton *myToFirst, *myToLast, *myNext, *myPrev, *myClose;
  QCheckBox *myIsCaseSens, *myIsRegExp;
  OB_ObjSearch* mySearch;
};

#endif
