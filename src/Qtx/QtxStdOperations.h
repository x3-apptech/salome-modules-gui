// File:      QtxStdOperations.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_STD_OPERATIONS_HEADER__
#define __QTX_STD_OPERATIONS_HEADER__

#include "Qtx.h"
#include "QtxOperations.h"

//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxStdOperations : public QtxOperations
{
public:
    QtxStdOperations();
    virtual ~QtxStdOperations();

    virtual void opersList( QStringList& ) const;
    virtual void bracketsList( QStringList&, bool open ) const;

    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const;
protected: 
    typedef QValueList< QVariant::Type > ListOfTypes;

    void addOperations( const QStringList& );
    void addTypes( const ListOfTypes& );

private:
    QStringList myOpers;
    ListOfTypes myTypes;
};

//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxArithmetics : public QtxStdOperations
{
public:
    QtxArithmetics();
    virtual ~QtxArithmetics();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxLogic : public QtxStdOperations
{
public:
    QtxLogic();
    virtual ~QtxLogic();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxFunctions : public QtxStdOperations
{
public:
    QtxFunctions();
    virtual ~QtxFunctions();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxStrings : public QtxStdOperations
{
public:
    QtxStrings();
    virtual ~QtxStrings();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

//================================================================
// Class    : 
// Purpose  : 
//================================================================
class QTX_EXPORT QtxSets : public QtxStdOperations
{
public:
    typedef QValueList< QtxValue > ValueSet;

public:
    QtxSets();
    virtual ~QtxSets();

    virtual void bracketsList( QStringList&, bool open ) const;
    virtual bool createValue( const QString&, QtxValue& ) const;
    virtual int  prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;

    static void add( ValueSet&, const QtxValue& );
    static void add( ValueSet&, const ValueSet& );
    static void remove( ValueSet&, const QtxValue& );
    static void remove( ValueSet&, const ValueSet& );
};


#endif
