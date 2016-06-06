#include <BxPanel.hpp>

class ToolSlide : public BxPanel
{
	private: byte Slide;
	public: int SavedYBegin;
	public: int SavedYEnd;
	public: ToolSlide(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		SetSlide(0);
		SavedYBegin = 0;
		SavedYEnd = 0;
	}
	public: virtual ~ToolSlide()
	{
	}
	public: void SetSlide(int slide) {Slide = (byte) MinMax(0, slide, 255);}
	public: byte GetSlide() {return Slide;}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolSlide", ToolSlide, XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	if(first)
	{
		s--.fnt("Font").Load("skin/cartoonstory.ttf", 13);
		s--.img("Rect").Load("skin/toolbar/map_rect.png", BxImage::PNG, BxImage::PAD7);
	}
	m--.img("Icon").Load(option, BxImage::PNG, BxImage::PAD5);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetSlide") == same)
		m--.SetSlide(*((byte*) &param));
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	const int Height = m--.SavedYEnd - m--.SavedYBegin;
	const byte OldSlide = m--.GetSlide();
	m--.SetSlide(255 - 255 * (y - m--.SavedYBegin) / Height);
	if(m--.GetSlide() != OldSlide)
		m--.UpdateEvent(BxString("<>:Slide:<A>", BxARG(m--.GetSlide())));

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
		m--.UpdateEvent(BxString("<>:Changed:<A>", BxARG(m--.GetSlide())));
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	const int Border = 2;
	const int Height = y2 - y1 - 6 - 18 - Border;
	const int Pos = Height - Height * m--.GetSlide() / 255;
	const color_x888 OuterColor = RGB32(81, 81, 81);
	const color_x888 InnerColor = RGB32(64, 64, 64);

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();

		// 안쪽채색
		BxTRY(Draw, COLOR(InnerColor))
		{
			// 주변
			Draw.Rectangle(FILL, XYXY(0, 0, Draw.Width(), 18));
			Draw.Rectangle(FILL, XYXY(0, 18, 6, Draw.Height() - 6));
			Draw.Rectangle(FILL, XYXY(Draw.Width() - 6, 18, Draw.Width(), Draw.Height() - 6));
			Draw.Rectangle(FILL, XYXY(0, Draw.Height() - 6, Draw.Width(), Draw.Height()));
			// 포인터
			Draw.Rectangle(FILL, XYXY(6, 18, Draw.Width() - 6 - 18, 18 + Pos));
			Draw.Rectangle(FILL, XYXY(6, 18 + Pos + Border, Draw.Width() - 6 - 18, Draw.Height() - 6));
		}
		// 아이콘
		Draw.Area(6 + 4, 18 + Pos - 8, FORM(&m--.img("Icon")));
		// 숫자
		Draw.Text(s--.fnt("Font"), BxString("<>:<A:.2>", BxARG(100.0f * m--.GetSlide() / 255)),
			XY(6 + 4 + 6, 18 + Pos + 1), textalign_left_bottom, COLOR(255, 255, 255));
		// 배경연결
		Draw.Rectangle9P(&s--.img("Rect"), Draw.CurrentRect(), COLOR(OuterColor));

		m--.SavedYBegin = 18;
		m--.SavedYEnd = Draw.Height() - 6 - Border;
	}
}
