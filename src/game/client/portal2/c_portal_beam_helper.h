#ifndef PORTAL_BEAM_HELPER_H
#define PORTAL_BEAM_HELPER_H

#include "beam_shared.h"

class C_PortalBeamHelper
{
public:

    //void C_PortalBeamHelper(class C_PortalBeamHelper & );
    C_PortalBeamHelper();
    ~C_PortalBeamHelper();
    void Init( class C_Beam * );
    void TurnOn();
    void TurnOff();
    int BeamCount();
    C_Beam *GetBeam( int i );
    C_Beam *GetFirstBeam();
    C_Beam *GetLastBeam();
    void UpdatePoints( Vector &vStartPoint, Vector &vEndPoint, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t &tr );
    void UpdatePointDirection( Vector &vStartPoint, Vector &vDirection, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t &tr );
    //void * __vecDelDtor(unsigned int );

private:
    C_Beam *m_pBeamTemplate;
    C_Beam *CreateBeam();
    CUtlVector<C_Beam *,CUtlMemory<C_Beam *,int> > m_beams;
	//CUtlVector<C_Beam*> m_beams;
};

#endif // PORTAL_BEAM_HELPER_H