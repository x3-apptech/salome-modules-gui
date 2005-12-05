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
// See http://www.salome-platform.org/
//
// File:      QtxParser.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_PARSER_HEADER__
#define __QTX_PARSER_HEADER__

#include "Qtx.h"
#include <qvaluestack.h>
#include <qvariant.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


class QtxOperations;

typedef QVariant QtxValue;


//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxParser
{
public:
    typedef enum
    {
        OK, OperandsNotMatch, InvalidResult, InvalidOperation,
        OperationsNull, InvalidToken, CloseExpected, ExcessClose,
        BracketsNotMatch, StackUnderflow, ExcessData

    } Error;

public:
    QtxParser( QtxOperations*, const QString& = QString::null );
    virtual ~QtxParser();

    QtxValue   calculate();
    QtxValue   calculate( const QString& );
    bool       setExpr( const QString& );

    virtual void     clear();
    virtual bool     has   ( const QString& name ) const;
    virtual void     set   ( const QString& name, const QtxValue& value );
    virtual bool     remove( const QString& name );
    virtual QtxValue value ( const QString& name ) const;

    bool       firstInvalid( QString& ) const;
    void       removeInvalids();
    QString    dump() const;
    Error      lastError() const;
    void       paramsList( QStringList& );

    static QString toString( const QValueList< QtxValue >& );

protected:
    typedef enum { Value, Param, Open, Close, Pre, Post, Binary } PostfixItemType;

    typedef struct
    {
        QtxValue          myValue;
        PostfixItemType   myType;

    } PostfixItem;

    typedef QValueList< PostfixItem > Postfix;
    typedef Postfix::const_iterator PostfixIterator;

protected:
            QString  dump( const Postfix& ) const;
    virtual bool     prepare( const QString&, Postfix& );
    virtual bool     setOperationTypes( Postfix& );
    virtual bool     sort( const Postfix&, Postfix&, 
                           const QStringList&, const QStringList&, 
                           int f=-1, int l=-1 );

    virtual bool     parse( const QString& );
    virtual void     setLastError( const Error );

            bool     calculate( const QString&, QtxValue&, QtxValue& );

    static int       search        ( const QStringList&, const QString&, int offset,
                                     int& matchLen, int& listind );
    static QString   note          ( const QString& str, int pos, int len );
    static int       globalBrackets( const Postfix&, int, int );

private:
    typedef QValueStack < QtxValue >  QtxValueStack;

private:
    QtxOperations*              myOperations;
    QMap< QString, QtxValue >   myParameters;
    Error                       myLastError;
    Postfix                     myPost;
};

#endif
