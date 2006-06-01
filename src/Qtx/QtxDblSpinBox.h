// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxDblSpinBox.h
// Author:    Sergey TELKOV

#ifndef QTXDBLSPINBOX_H
#define QTXDBLSPINBOX_H

#include "Qtx.h"

#include <qspinbox.h>

/*!
  \class  QtxDblSpinBox
  Spin box for real numbers.
*/
class QTX_EXPORT QtxDblSpinBox : public QSpinBox
{
  Q_OBJECT

  class Validator;

public:
	typedef enum QSpinBox::ButtonSymbols ButtonSymbols;

public:
  QtxDblSpinBox( QWidget* = 0, const char* = 0 );
  QtxDblSpinBox( double, double, double = 1, QWidget* = 0, const char* = 0 );
  virtual ~QtxDblSpinBox();

  double	           minValue() const;
  double	           maxValue() const;
  void               setMinValue( int );
  void               setMaxValue( int );
  void               setMinValue( double );
  void               setMaxValue( double );
  void	             setRange( int, int );
  virtual void       setRange( double, double );

  double	           lineStep() const;
  void               setLineStep( int );
  virtual void       setLineStep( double );

  double             value() const;

  int                precision() const;
  virtual void       setPrecision( const int );
  
  bool               isCleared() const;
  virtual void       setCleared( const bool );
  
  virtual bool       eventFilter( QObject*, QEvent* );
    
signals:
  void               valueChanged( double );
  void		           valueChanged( const QString& );
    
public slots:
  virtual void	     stepUp();
  virtual void	     stepDown();
  virtual void       selectAll();
  virtual void	     setValue( int );
  virtual void	     setValue( double );
  
protected slots:
  virtual void       onTextChanged( const QString& );

protected:
  virtual void	     updateDisplay();
  virtual void	     interpretText();
  
  virtual void	     valueChange();
  virtual void	     rangeChange();
  
  QString            currentValueText();
  virtual QString    mapValueToText( int );
  virtual QString    mapValueToText( double );
  virtual double     mapTextToDoubleValue( bool* );
  
  virtual void       leaveEvent( QEvent* );
  virtual void       wheelEvent( QWheelEvent* );
  
  double             bound( double );

  QString            removeTrailingZeroes( const QString& ) const;

private:
  double             myMin;
  double             myMax;
  double             myStep;
  double             myValue;
  bool               myCleared;
  bool               myBlocked;
  int                myPrecision;
};

#endif
