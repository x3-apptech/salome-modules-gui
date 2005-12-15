#ifndef QDS_TEXTEEDIT_H
#define QDS_TEXTEEDIT_H

#include "QDS_Datum.h"

class QTextEdit;

class QDS_EXPORT QDS_TextEdit : public QDS_Datum
{
  Q_OBJECT

public:
  QDS_TextEdit( const QString&, QWidget* = 0, const int = All, const QString& = QString::null );
  virtual ~QDS_TextEdit();

signals:
  void                            returnPressed();

private slots:
  void                            onTextChanged();

protected:
  QTextEdit*                      textEdit() const;
  virtual QWidget*                createControl( QWidget* );

  virtual QString                 getString() const;
  virtual void                    setString( const QString& );
};

#endif 
