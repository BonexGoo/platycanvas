#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.ToolStroke", XYXY)

local_func void OnUpdatedMapStroke(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<BxPanel>& M = *((BxPanel::ServiceM<BxPanel>*) data);
	if(StrSameCount(message, "ThickR:") == 7)
	{
		M--.int4("ThickR") = AtoI(message + 7);
		M--.UpdateEvent(BxString("<>:Stroke:|<A>|<A>|", BxARG(M--.int4("ThickR"), M--.int4("Opacity"))));
	}
}

local_func void OnUpdatedVSlider(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<BxPanel>& M = *((BxPanel::ServiceM<BxPanel>*) data);
	if(StrSameCount(message, "Changed:") == 8)
	{
		M--.int4("Opacity") = AtoI(message + 8);
		M--.UpdateEvent(BxString("<>:Stroke:|<A>|<A>|", BxARG(M--.int4("ThickR"), M--.int4("Opacity"))));
	}
	else if(StrSameCount(message, "Slide:") == 6)
		M--("MapStroke").Notify("SetOpacity", AtoI(message + 6));
}

void OnCreate(string option, unknown param, const bool first)
{
	const int ThickR = 3;
	const int Opacity = 0;
	m--.int4("ThickR") = ThickR;
	m--.int4("Opacity") = Opacity;

	m--("Label").Bind("platycanvas.Label", "STROKE");
	m--("MapStroke").Bind("platycanvas.MapStroke", ItoA(ThickR), Opacity);
	m--("MapStroke").Updated += OnUpdatedMapStroke, &m;
	m--("VSlider").Bind("platycanvas.VSlider", "", Opacity);
	m--("VSlider").Updated += OnUpdatedVSlider, &m;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetThickR") == same)
	{
        m--.int4("ThickR") = *((int*) &param);
		m--("MapStroke").Notify("SetThickR", param);
	}
	else if(StrCmp(message, "SetRGB") == same)
		m--("MapStroke").Notify("SetRGB", param);
	else if(StrCmp(message, "SetOpacity") == same)
	{
        m--.int4("Opacity") = *((byte*) &param);
		m--("MapStroke").Notify("SetOpacity", param);
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
		m--("MapStroke").Paint(BxPanel::prmXYXY, BxARG(0, 0, Width - 40, Height - 40));
		m--("VSlider").Paint(BxPanel::prmXYXY, BxARG(Width - 40, 5, Width, Height - 45));
	}
}
