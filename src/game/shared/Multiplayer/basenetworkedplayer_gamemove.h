#include "cbase.h"
#include "gamemovement.h"

class CNetworkedPlayerMovement : public CGameMovement
{
	DECLARE_CLASS(CNetworkedPlayerMovement,CGameMovement);

public:
	virtual bool CheckJumpButton();
};