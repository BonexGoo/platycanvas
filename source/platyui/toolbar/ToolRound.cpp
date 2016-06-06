#include <BxPanel.hpp>

class ToolRound : public BxPanel
{
	private: byte Thick;
	public: color_x888 SavedColor;
	public: int SavedCenterX;
	public: int SavedCenterY;
	public: ToolRound(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		SetThick(0);
		SavedColor = 0;
		SavedCenterX = 0;
		SavedCenterY = 0;
	}
	public: virtual ~ToolRound()
	{
	}
	public: void SetThick(int thick) {Thick = (byte) MinMax(16, thick, 255);}
	public: byte GetThick() {return Thick;}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolRound", ToolRound, XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.img("Round").Load("skin/toolbar/map_round.png", BxImage::PNG, BxImage::PAD7);
	m--.img("Hole").Load("skin/toolbar/map_hole.png", BxImage::PNG, BxImage::PAD7);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetThick") == same)
        m--.SetThick(*((byte*) &param));
	else if(StrCmp(message, "SetColor") == same)
        m--.SavedColor = *((color_x888*) &param);
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	const int RW = m--.img("Round").Width();
	const int DistX = m--.SavedCenterX - x;
	const int DistY = m--.SavedCenterY - y;
	m--.SetThick(255 * 2 * Sqrt(DistX * DistX + DistY * DistY) / RW);

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
		m--.UpdateEvent(BxString("<>:Thick:<A>", BxARG(m--.GetThick())));
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	const int RW = m--.img("Round").Width();
	const int RH = m--.img("Round").Height();
	const int ThickLT = RW / 2 * m--.GetThick() / 255;
	const int ThickRB = (RW + 1) / 2 * m--.GetThick() / 255;
	const color_x888 OuterColor = RGB32(81, 81, 81);
	const color_x888 InnerColor = RGB32(64, 64, 64);

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(OuterColor));

		BxTRY(Draw, CLIP(XYWH((Draw.Width() - RW) / 2, (Draw.Height() - RH) / 2, RW, RH)))
		{
			const point Center = Draw.CurrentCenter();
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(m--.SavedColor));
			// 홀구현
			BxTRY(Draw, COLOR(InnerColor))
			{
				const rect HoleRect = XYXY(
					Center.x - ThickLT, Center.y - ThickLT,
					Center.x + ThickRB, Center.y + ThickRB);
				Draw.Rectangle(FILL, HoleRect, FORM(&m--.img("Hole")));
				Draw.Rectangle(FILL, XYXY(0, 0, Draw.Width(), HoleRect.t));
				Draw.Rectangle(FILL, XYXY(0, HoleRect.t, HoleRect.l, HoleRect.b));
				Draw.Rectangle(FILL, XYXY(HoleRect.r, HoleRect.t, Draw.Width(), HoleRect.b));
				Draw.Rectangle(FILL, XYXY(0, HoleRect.b, Draw.Width(), Draw.Height()));
			}
			// 배경연결
			Draw.Area(0, 0, FORM(&m--.img("Round")) >> COLOR(OuterColor));
		}
		m--.SavedCenterX = Draw.CurrentCenter().x;
		m--.SavedCenterY = Draw.CurrentCenter().y;
	}
}
