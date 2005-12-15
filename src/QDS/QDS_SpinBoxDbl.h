#ifndef QDS_PINBOXDBL_H
#define QDS_PINBOXDBL_H

#include "QDS_Datum.h"

class QtxDblSpinBox;

class QDS_EXPORT QDS_SpinBoxDbl : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_SpinBoxDbl( const QString&, QWidget* = 0, const int = All, const QString& = QString::null );
  virtual ~QDS_SpinBoxDbl();

  double           step() const;
  void             setStep( const double );

private slots:
  void             onValueChanged( double );

protected:
  QtxDblSpinBox*   spinBox() const;
  virtual QWidget* createControl( QWidget* );

  virtual QString  getString() const;
  virtual void     setString( const QString& );

  virtual void     unitSystemChanged( const QString& );
};

#endif 
