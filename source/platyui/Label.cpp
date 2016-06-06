#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.Label", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	if(first)
	{
        s--.fnt("Font").Load("skin/cartoonstory.ttf", 30);
	}
	m--.str("Name") = option;
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
		Draw.Text(s--.fnt("Font"), m--.str("Name"),
			Draw.CurrentCenter(), textalign_center_middle,
			COLOR(128, 128, 128));
	}
}
