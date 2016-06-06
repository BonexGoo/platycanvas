#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.ToolButton", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
    m--.flag("State") = 0;
	m--.int4("Lightning") = 0;
	m--.img("Button").Load(option, BxImage::PNG, BxImage::PAD5);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetState") == same)
        m--.flag("State") = *((bool*) &param);
	else if(StrCmp(message, "SetLightning") == same)
		m--.int4("Lightning") = *((int*) &param);
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	const int Color = Min(m--.int4("Lightning") * m--.int4("Lightning"), 255);
	if(0 < m--.int4("Lightning")) m--.int4("Lightning")--;

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
        switch(m--.flag("State"))
		{
        case false:
			Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&m--.img("Button"))
				>> COLOR(255, 255 - Color, 255 - Color));
			break;
        case true:
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(255, 255, 255));
			Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&m--.img("Button"))
				>> COLOR(Color, 0, 0));
			break;
		}
	}
}
