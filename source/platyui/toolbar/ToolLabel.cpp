#include <BxPanel.hpp>

class ToolLabel : public BxPanel
{
	public: int Angle;
	public: ToolLabel(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		Angle = 0;
	}
	public: virtual ~ToolLabel()
	{
	}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolLabel", ToolLabel, XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.Angle = *((int*) &param);

	m--.img("Text").Load(option, BxImage::PNG, BxImage::PAD5);
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
		Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width(), Draw.Height()), COLOR(60, 60, 60));
		switch(m--.Angle / 90 * 90)
		{
		case 0:
			Draw.Rectangle(FILL, XYWH(0, Draw.Height() - 3, Draw.Width(), 1), COLOR(53, 53, 53));
			Draw.Rectangle(FILL, XYWH(0, Draw.Height() - 2, Draw.Width(), 1), COLOR(48, 48, 48));
			Draw.Rectangle(FILL, XYWH(0, Draw.Height() - 1, Draw.Width(), 1), COLOR(68, 68, 68));
			break;
		case 90:
			Draw.Rectangle(FILL, XYWH(2, 0, 1, Draw.Height()), COLOR(53, 53, 53));
			Draw.Rectangle(FILL, XYWH(1, 0, 1, Draw.Height()), COLOR(48, 48, 48));
			Draw.Rectangle(FILL, XYWH(0, 0, 1, Draw.Height()), COLOR(68, 68, 68));
			break;
		case 180:
			Draw.Rectangle(FILL, XYWH(0, 2, Draw.Width(), 1), COLOR(53, 53, 53));
			Draw.Rectangle(FILL, XYWH(0, 1, Draw.Width(), 1), COLOR(48, 48, 48));
			Draw.Rectangle(FILL, XYWH(0, 0, Draw.Width(), 1), COLOR(68, 68, 68));
			break;
		case 270:
			Draw.Rectangle(FILL, XYWH(Draw.Width() - 3, 0, 1, Draw.Height()), COLOR(53, 53, 53));
			Draw.Rectangle(FILL, XYWH(Draw.Width() - 2, 0, 1, Draw.Height()), COLOR(48, 48, 48));
			Draw.Rectangle(FILL, XYWH(Draw.Width() - 1, 0, 1, Draw.Height()), COLOR(68, 68, 68));
			break;
		}
		Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&m--.img("Text")));
	}
}
