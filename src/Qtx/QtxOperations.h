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
// File:      QtxOperations.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_OPERATIONS_HEADER__
#define __QTX_OPERATIONS_HEADER__

#include "Qtx.h"
#include "QtxParser.h"

class QTX_EXPORT QtxOperations
{
public:
    QtxOperations();
    virtual ~QtxOperations();

    virtual void  opersList( QStringList& ) const = 0;
    //list of possible operations

    virtual void  bracketsList( QStringList&, bool open ) const = 0;
    //list of open/close brackets

    virtual bool  createValue( const QString&, QtxValue& ) const;
    //by default, the String value will be set, it corresponds to parameter
    //base method returns false (always parameter)
    //successor's method returns true if it has created custom value
    //or call base if it hasn't

    virtual int   prior( const QString&, bool isBin ) const = 0;
    //returns prioritet of operation;
    //if operation is impossible, it must return 0 or less

    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const = 0;
    //return OK if this parameter types is valid for operation
    //return OperandsNotMatch or InvalidOperation otherwise

    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const = 0;
    //process binary operation with values
    //for unary operation the second QtxValue will be passed as invalid
};

#endif
