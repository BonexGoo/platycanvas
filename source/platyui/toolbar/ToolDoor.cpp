#include <BxPanel.hpp>

class ToolDoor : public BxPanel
{
	public: color_x888 Color;
	public: int Angle;
	public: ToolDoor(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		Color = 0;
		Angle = 0;
	}
	public: virtual ~ToolDoor()
	{
	}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolDoor", ToolDoor, XY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.Color = HtoI(option);
	m--.Angle = *((int*) &param);

	m--.img("Door").Load("skin/toolbar/door_top.png", BxImage::PNG, BxImage::PAD5);
	m--.img("Arrow").Load("skin/toolbar/arrow_down.png", BxImage::PNG);
	m--.img("Arrow").SetHotspot(18, 12);
}

unknown OnNotify(string message, unknown param)
{
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	return "";
}

void OnPaint(rect& r, int x, int y, int, int)
{
	const int w = m--.img("Door").Width();
	const int h = m--.img("Door").Height();
	const int hx = m--.img("Door").GetArea()->hx;
	const int hy = m--.img("Door").GetArea()->hy;
	const int rx = w - hx, ry = h - hy;

	BxDraw& Draw = m--.GetDraw();
	switch(m--.Angle / 90 * 90)
	{
	case 0: Draw.AddStyle(CLIP(r = XYWH(x - hx, y, w, h), hx, hy)); break;
	case 90: Draw.AddStyle(CLIP(r = XYWH(x - h, y - hx, h, w), ry, hx)); break;
	case 180: Draw.AddStyle(CLIP(r = XYWH(x - rx, y - h, w, h), rx, ry)); break;
	case 270: Draw.AddStyle(CLIP(r = XYWH(x, y - rx, h, w), hy, rx)); break;
	}
	BxTRY(Draw, ROTATE(1024 * m--.Angle / 360))
	{
		m--.UpdateRect();
		Draw.Area(0, 0, FORM(&m--.img("Door")) >> COLOR(60, 60, 60));
		Draw.Area(0, 0, FORM(&m--.img("Arrow")) >> COLOR(m--.Color));
	}
	Draw.SubStyle();
}
