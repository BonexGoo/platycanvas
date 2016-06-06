#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.ToolBar.Layer", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--("LayerLabel").Bind("platycanvas.ToolLabel", "skin/toolbar/text_layer.png", 90);

	m--("Close").Bind("platycanvas.ToolClose", option, param);
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
		// 배경색
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(81, 81, 81));

		// Layer
		m--("LayerLabel").Paint(BxPanel::prmXYXY, BxARG(Draw.Width() - 41, 0, Draw.Width(), Draw.Height()));
		//BxTRY(Draw, CLIP(XYXY(0, 40, Draw.Width(), Draw.Height())))
		//	m--("Stroke").Paint(BxPanel::prmXYXY, BxARG(0, 1, Draw.Width(), Draw.Height()));

		// Close
		m--("Close").Paint(BxPanel::prmXY, BxARG(Draw.Width() - 40 / 2, 60));

		// 마감선
		Draw.Rectangle(FILL, XYWH(0, 0, 1, Draw.Height()), COLOR(36, 36, 36));
	}
}
