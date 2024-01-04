#ifndef COMBAT_WEAPON_CLONE_H
#define COMBAT_WEAPON_CLONE_H

#include "c_basecombatweapon.h"

class C_CombatWeaponClone : public C_BaseAnimating
{
	DECLARE_CLASS( C_CombatWeaponClone, C_BaseAnimating );
public:
    C_CombatWeaponClone( C_BaseCombatWeapon *pWeaponParent );
	~C_CombatWeaponClone();
	virtual IClientModelRenderable *GetClientModelRenderable();
	virtual int DrawModel( int flags, const RenderableInstance_t &instance );
	virtual bool ShouldSuppressForSplitScreenPlayer( int nSlot );
	virtual bool ShouldDraw();
	virtual bool SetupBones( matrix3x4a_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime );
	virtual bool ComputeStencilState( ShaderStencilState_t *pStencilState );
	virtual const Vector& GetRenderOrigin( void );
	virtual const QAngle& GetRenderAngles( void );
	C_BaseCombatWeapon *MyCombatWeaponPointer();
	void UpdateClone();
private:
	C_BaseCombatWeapon *m_pWeaponParent;
	int m_nLastUpdatedWorldModelClone;
	int m_nWeaponBoneIndex;
	Vector m_vWeaponBonePosition;
	QAngle m_qWeaponBoneAngle;
};

#endif // COMBAT_WEAPON_CLONE_H