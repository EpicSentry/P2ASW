#ifndef C_BASEPROJECTOR_H
#define C_BASEPROJECTOR_H

#include "cbase.h"
#include "prop_portal_shared.h"

#define CBaseProjector C_BaseProjector

class C_BaseProjectedEntity;

class C_BaseProjector : public C_BaseAnimating
{
public:
	
	DECLARE_CLASS( C_BaseProjector, C_BaseAnimating );
	DECLARE_CLIENTCLASS();
	
	C_BaseProjector();
	~C_BaseProjector();
	
	C_BaseProjectedEntity *GetFirstProjectedEntity() { return m_hFirstChild.Get(); }
	
protected:
	
	CHandle<C_BaseProjectedEntity> m_hFirstChild;
	bool m_bEnabled;
};

#endif // C_BASEPROJECTOR_H