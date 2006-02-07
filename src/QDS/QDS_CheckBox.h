#ifndef QDS_CHECKBOX_H
#define QDS_CHECKBOX_H

#include "QDS_Datum.h"

class QCheckBox;

class QDS_EXPORT QDS_CheckBox : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_CheckBox( const QString&, QWidget* = 0, const int = All, const QString& = QString::null );
  virtual ~QDS_CheckBox();

  bool                 isChecked() const;
  void                 setChecked( const bool );

  virtual void         clear();

signals:
  void                 toggled( bool );

private slots:
  void                 onParamChanged();
  void                 onStateChanged( int );

protected:
  QCheckBox*           checkBox() const;
  virtual QWidget*     createControl( QWidget* );

  virtual QString      getString() const;
  virtual void         setString( const QString& );
};

#endif
