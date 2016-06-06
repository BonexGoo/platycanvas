#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.ToolAqua", XYXY)

local_func void OnUpdatedMapAqua(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<BxPanel>& M = *((BxPanel::ServiceM<BxPanel>*) data);
	if(StrSameCount(message, "AquaR:") == 6)
		M--.UpdateEvent(BxString("<>:Aqua:<A>", BxARG(AtoI(message + 6))));
}

void OnCreate(string option, unknown param, const bool first)
{
	m--("Label").Bind("platycanvas.Label", "AQUA");
	m--("MapAqua").Bind("platycanvas.MapAqua", "", 0);
	m--("MapAqua").Updated += OnUpdatedMapAqua, &m;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetAquaR") == same)
		m--("MapAqua").Notify("SetAquaR", param);
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
		m--("MapAqua").Paint(BxPanel::prmXYXY, BxARG(0, 0, Width, Height - 40));
	}
}
