#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.Button", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.int4("Color") = RGB32(255, 255, 0);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetColor") == same)
        m--.int4("Color") = *((int*) &param);
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
		Draw.Rectangle(FILL, Draw.CurrentRect(),
			COLOR((color_x888) m--.int4("Color")));
	}
}
