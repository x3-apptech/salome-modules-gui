#ifndef PLOT2D_FITDATADLG_H
#define PLOT2D_FITDATADLG_H

#include "Plot2d.h"
#include <qdialog.h>

class QButtonGroup;
class QRadioButton;
class QLineEdit;
class QPushButton;

class PLOT2D_EXPORT Plot2d_FitDataDlg : public QDialog
{
  Q_OBJECT

public:
// constuctor
  Plot2d_FitDataDlg( QWidget* parent, bool secondAxisY );

// sets range
  void setRange(const double xMin, 
                const double xMax,
                const double yMin,
                const double yMax,
                const double y2Min = 0,
                const double y2Max = 0);
// gets range, returns mode (see getMode())
  int getRange(double& xMin, 
               double& xMax,
               double& yMin,
               double& yMax,
               double& y2Min,
               double& y2Max);
// gets mode : 0 - Fit all; 1 - Fit horizontal, 2 - Fit vertical
  int getMode();

protected slots:
// called when range mode changed
  void onModeChanged(int);

private:
  QButtonGroup*           myRangeGrp;
  QRadioButton*           myModeAllRB;
  QRadioButton*           myModeHorRB;
  QRadioButton*           myModeVerRB;
  QLineEdit*              myXMinEdit;
  QLineEdit*              myYMinEdit;
  QLineEdit*              myY2MinEdit;
  QLineEdit*              myXMaxEdit;
  QLineEdit*              myYMaxEdit;
  QLineEdit*              myY2MaxEdit;
  QPushButton*            myOkBtn;
  QPushButton*            myCancelBtn;
  bool                    mySecondAxisY;
};

#endif
