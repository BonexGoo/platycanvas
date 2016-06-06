#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.Tab", XY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.flag("IsOpened") = false;

	m--.img("Tab").Load("skin/tab.png", BxImage::PNG, BxImage::PAD7);
	m--.img("OpenIcon").Load("skin/openicon.png", BxImage::PNG, BxImage::PAD7);
	m--.img("CloseIcon").Load("skin/closeicon.png", BxImage::PNG, BxImage::PAD7);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetOpen") == same)
		m--.flag("IsOpened") = *((bool*) &param);
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchUpIn)
		return (m--.flag("IsOpened") ^= true)? "Open" : "Close";
	return "";
}

void OnPaint(rect& r, int x, int y, int, int)
{
	const color_x888 BackColor = RGB32(64, 64, 64);
	const color_x888 IconColor = RGB32(192, 192, 128);

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x, y, x + 90, y + 60)))
	{
		m--.UpdateRect();
		Draw.Area(0, 0, FORM(&m--.img("Tab")) >> COLOR(BackColor));

		const point Center = Draw.CurrentCenter();
		if(m--.flag("IsOpened"))
			Draw.Area(5, 0, FORM(&m--.img("CloseIcon")) >> COLOR(IconColor));
		else Draw.Area(5, 0, FORM(&m--.img("OpenIcon")) >> COLOR(IconColor));
	}
}
