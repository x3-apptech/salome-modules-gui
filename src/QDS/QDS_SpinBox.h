#ifndef QDS_SPINBOX_H
#define QDS_SPINBOX_H

#include "QDS_Datum.h"

class QtxIntSpinBox;

class QDS_EXPORT QDS_SpinBox : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_SpinBox( const QString&, QWidget* = 0, const int = All, const QString& = QString::null );
  virtual ~QDS_SpinBox();

  int              step() const;
  void             setStep( const int );

private slots:
  void             onValueChanged( int );

protected:
  QtxIntSpinBox*   spinBox() const;

  virtual QWidget* createControl( QWidget* );

  virtual QString  getString() const;
  virtual void     setString( const QString& );

  virtual void     unitSystemChanged( const QString& );
};

#endif 
