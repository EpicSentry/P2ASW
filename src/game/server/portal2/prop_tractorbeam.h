#include "baseprojector.h"

#define NO_TRACTORBEAM_CLIENTCLASS

class ProjectedEntityAmbientSoundProxy : public CBaseEntity
{
public:
	
	DECLARE_CLASS( ProjectedEntityAmbientSoundProxy, CBaseEntity );
#ifndef NO_TRACTORBEAM_CLIENTCLASS
	DECLARE_SERVERCLASS();
#endif
	ProjectedEntityAmbientSoundProxy();
	~ProjectedEntityAmbientSoundProxy();
	
	static ProjectedEntityAmbientSoundProxy *Create( CBaseEntity *pAttachTo );
    
	int UpdateTransmitState();
	void AttachToEntity( CBaseEntity *pAttachTo );
};

class CPropTractorBeamProjector : public CBaseProjector
{
public:

	DECLARE_CLASS( CPropTractorBeamProjector, CBaseProjector );
	DECLARE_DATADESC();
#ifndef NO_TRACTORBEAM_CLIENTCLASS
	DECLARE_SERVERCLASS();
#endif
	CPropTractorBeamProjector();
    ~CPropTractorBeamProjector();
    void Spawn();
    void Precache();
    void Project();
	
    float GetLinearForce();
    
	bool IsReversed();
	
protected:
	
    //virtual CBaseProjectedEntity *CreateNewProjectedEntity();
    
	void InputSetLinearForce( inputdata_t &inputdata );
    
	void Shutdown();
		
	CNetworkVar( float, m_flLinearForce );
	
	CNetworkVector( m_vEndPos );
	
	CNetworkVar( bool, m_bNoEmitterParticles );
	CNetworkVar( bool, bDisableAutoReprojection );
	
    bool m_bUse128Model;
	
private:
	
    CSoundPatch *m_sndMechanical;
    CSoundPatch *m_sndAmbientMusic;
    CSoundPatch *m_sndAmbientSound;
    
	CHandle<ProjectedEntityAmbientSoundProxy> m_hAmbientSoundProxy;
	
    char s_AmbientBeamMusic[64];
};