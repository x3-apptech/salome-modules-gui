#ifndef CAF_OPERATION_H
#define CAF_OPERATION_H

#include "CAF.h"

#include "SUIT_Operation.h"

#include <qobject.h>
#include <qstring.h>

class CAF_Study;

class CAF_EXPORT CAF_Operation : public SUIT_Operation
{
	Q_OBJECT

public:
	CAF_Operation( SUIT_Application* );
	virtual ~CAF_Operation();
};

#endif
