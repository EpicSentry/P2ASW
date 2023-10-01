#include "cbase.h"
#include "hudelement.h"
#include "vgui_controls/panel.h"
#include "vgui_controls/editablepanel.h"
#include "vgui_controls/MessageMap.h"


class CHudIndicator : public vgui::EditablePanel, CHudElement
{
public:
	
	DECLARE_PANELMESSAGEMAP( CHudIndicator );
	
	CHudIndicator( Panel *parent, const char *panelName );
    ~CHudIndicator( void );

    void MsgFunc_IndicatorFlash( bf_read &msg );
	
protected:

	void ApplySchemeSettings( vgui::IScheme *scheme );
    void Paint( void );
    void Init( void );
    void Reset( void );

    bool ShouldDraw( void );

private:

    int m_nTexture[2];
    int m_nIndicatorType;

    float m_flDisplayTime;
    float m_flStartTime;
    float m_flFadeInTime;
    float m_flFadeOutTime;	
};

CHudIndicator::CHudIndicator( Panel *parent, const char *panelName )
{

}