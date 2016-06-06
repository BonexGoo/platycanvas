#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.PenStatusBar", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--("ToolStroke").Bind("platycanvas.ToolStroke");
	m--("ToolColor").Bind("platycanvas.ToolColor");
	m--("ToolAqua").Bind("platycanvas.ToolAqua");

	m--("PenButton").Bind("platycanvas.Button");
	m--("EraserButton").Bind("platycanvas.Button");
}

unknown OnNotify(string message, unknown param)
{
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(64, 64, 64));

		m--("ToolStroke").Paint(BxPanel::prmXYXY,
			BxARG(0, 10, 190, 180));

		m--("ToolColor").Paint(BxPanel::prmXYXY,
			BxARG(200, 10, Draw.Width() - 270, 180));
		Draw.Rectangle(FILL, XYWH(200 + 1, 10, 4, 170), COLOR(48, 48, 48));

		m--("ToolAqua").Paint(BxPanel::prmXYXY,
			BxARG(Draw.Width() - 260, 10, Draw.Width() - 100, 180));
		Draw.Rectangle(FILL, XYWH(Draw.Width() - 260 + 1, 10, 4, 170), COLOR(48, 48, 48));

		m--("PenButton").Paint(BxPanel::prmXYXY,
			BxARG(Draw.Width() - 90, 10, Draw.Width() - 10, 90));
		m--("EraserButton").Paint(BxPanel::prmXYXY,
			BxARG(Draw.Width() - 90, 100, Draw.Width() - 10, 180));
		Draw.Rectangle(FILL, XYWH(Draw.Width() - 100 - 4 + 1, 10, 4, 170), COLOR(48, 48, 48));
	}
}
