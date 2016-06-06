#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.ToolColor", XYXY)

local_func void OnUpdatedMapColor(BxPanel* panel, string message, void* data)
{
    BxPanel::ServiceM<BxPanel>& M = *((BxPanel::ServiceM<BxPanel>*) data);
	if(StrSameCount(message, "RGB:") == 4)
		M--.UpdateEvent(BxString("<>:Color:<A>", BxARG(message + 4)));
}

local_func void OnUpdatedVSlider(BxPanel* panel, string message, void* data)
{
    BxPanel::ServiceM<BxPanel>& M = *((BxPanel::ServiceM<BxPanel>*) data);
    if(StrSameCount(message, "Changed:") == 8)
	{
		color_x888 RGB = M--("MapColor").Notify<color_x888>("GetRGB");
		m--.UpdateEvent(BxString("<>:Color:<A:06>", BxARG((ARG_HEX_UPPER) RGB)));
	}
    else if(StrSameCount(message, "Slide:") == 6)
        M--("MapColor").Notify("SetSaturation", AtoI(message + 6));
}

void OnCreate(string option, unknown param, const bool first)
{
    m--("Label").Bind("platycanvas.Label", "COLOR");
    m--("MapColor").Bind("platycanvas.MapColor", "", 0);
    m--("MapColor").Updated += OnUpdatedMapColor, &m;
	m--("VSlider").Bind("platycanvas.VSlider");
    m--("VSlider").Updated += OnUpdatedVSlider, &m;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetHue") == same)
		m--("MapColor").Notify("SetHue", param);
	else if(StrCmp(message, "SetLightness") == same)
		m--("MapColor").Notify("SetLightness", param);
    else if(StrCmp(message, "SetSaturation") == same)
    {
        m--("MapColor").Notify("SetSaturation", param);
        m--("VSlider").Notify("SetSlide", param);
    }
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, HOTSPOT(x1, y1))
	{
		const int Width = x2 - x1;
		const int Height = y2 - y1;
		m--("Label").Paint(BxPanel::prmXYXY, BxARG(0, Height - 30, Width, Height));
		m--("MapColor").Paint(BxPanel::prmXYXY, BxARG(0, 0, Width - 40, Height - 40));
		m--("VSlider").Paint(BxPanel::prmXYXY, BxARG(Width - 40, 5, Width, Height - 45));
	}
}
