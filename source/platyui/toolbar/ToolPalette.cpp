#include <BxPanel.hpp>

class ToolPalette : public BxPanel
{
	public: bool NeedUpdate;
	public: byte Hue;
	public: byte Lightness;
	public: byte SavedSaturation;
	public: int SavedXBegin;
	public: int SavedXEnd;
	public: int SavedYBegin;
	public: int SavedYEnd;
	public: ToolPalette(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		NeedUpdate = true;
		Hue = 0;
		Lightness = 0;
		SavedSaturation = 0;
		SavedXBegin = 0;
		SavedXEnd = 0;
		SavedYBegin = 0;
		SavedYEnd = 0;
	}
	public: virtual ~ToolPalette()
	{
	}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolPalette", ToolPalette, XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	if(first)
	{
        s--.fnt("Font").Load("skin/cartoonstory.ttf", 13);
		s--.img("Rect").Load("skin/toolbar/map_rect.png", BxImage::PNG, BxImage::PAD7);
	}

	m--.img("Icon").Load(option, BxImage::PNG, BxImage::PAD5);
	m--.img("ColorIcon").Load("skin/coloricon.png", BxImage::PNG, BxImage::PAD5);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetHue") == same)
		m--.Hue = *((byte*) &param);
	else if(StrCmp(message, "SetLightness") == same)
		m--.Lightness = *((byte*) &param);
    else if(StrCmp(message, "SetSaturation") == same)
    {
		const byte NewSaturation = *((byte*) &param);
		if(m--.SavedSaturation != NewSaturation)
		{
			m--.NeedUpdate = true;
			m--.SavedSaturation = NewSaturation;
		}
    }
	else if(StrCmp(message, "NeedUpdate") == same)
		m--.NeedUpdate = true;
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	const int Width = m--.SavedXEnd - m--.SavedXBegin;
	const int Height = m--.SavedYEnd - m--.SavedYBegin;
	m--.Hue = (byte) MinMax(0, 255 * (x - m--.SavedXBegin) / Width, 255);
	m--.Lightness = (byte) MinMax(0, 255 * (y - m--.SavedYBegin) / Height, 255);

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
	{
		m--.UpdateEvent(BxString("<>:Hue:<A>", BxARG(m--.Hue)));
		m--.UpdateEvent(BxString("<>:Lightness:<A>", BxARG(m--.Lightness)));
	}
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	const color_x888 OuterColor = RGB32(81, 81, 81);
	const color_x888 InnerColor = RGB32(64, 64, 64);

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();

		// 색상맵
		BxTRY(Draw, CLIP(XYXY(6, 18, Draw.Width() - 6, Draw.Height() - 6)))
		{
			if(m--.NeedUpdate)
			{
				m--.NeedUpdate = false;
				const int Width = Draw.Width();
				const int Height = Draw.Height();
				const byte* Bitmap = BxCore::AddOn::CreateBMP(Width, Height);
				bitmappixel* Bits = BxCore::AddOn::GetBMPBits(Bitmap);
				for(int i = 0, iend = Width * Height; i < iend; ++i)
				{
					const int x = i % Width;
					const int y = i / Width;
					const color_x888 RGB = ColorHLSToRGB(
						255 * x / (Width - 1),
						255 * (Height - 1 - y) / (Height - 1),
						m--.SavedSaturation);
					Bits[i].a = 255;
					Bits[i].r = ColorToRed(RGB);
					Bits[i].g = ColorToGreen(RGB);
					Bits[i].b = ColorToBlue(RGB);
				}
				m--.img("ColorMap").Load(Bitmap, 0, BxImage::BMP, BxImage::PAD7);
				BxCore::AddOn::Release(Bitmap);
			}
			Draw.Area(0, 0, FORM(&m--.img("ColorMap")));
		}

		// 컬러아이콘
		const color_x888 RGB = ColorHLSToRGB(m--.Hue, m--.Lightness, m--.SavedSaturation);
		const int X = 6 + (Draw.Width() - 6 - 6) * m--.Hue / 255;
		const int Y = 18 + (Draw.Height() - 6 - 18) * m--.Lightness / 255;
		Draw.Area(X, Y, FORM(&m--.img("ColorIcon")) >> COLOR(RGB));

		// 안쪽채색
		BxTRY(Draw, COLOR(InnerColor))
		{
			// 주변
			Draw.Rectangle(FILL, XYXY(0, 0, Draw.Width(), 18));
			Draw.Rectangle(FILL, XYXY(0, 18, 6, Draw.Height() - 6));
			Draw.Rectangle(FILL, XYXY(Draw.Width() - 6, 18, Draw.Width(), Draw.Height() - 6));
			Draw.Rectangle(FILL, XYXY(0, Draw.Height() - 6, Draw.Width(), Draw.Height()));
		}

		// 아이콘
		Draw.Area(6 + 4, 18 - 8, FORM(&m--.img("Icon")));
		// 숫자
		Draw.Text(s--.fnt("Font"), BxString("<>:R.<A> G.<A> B.<A>",
			BxARG(ColorToRed(RGB), ColorToGreen(RGB), ColorToBlue(RGB))),
			XY(6 + 4 + 6, 18 + 1), textalign_left_bottom, COLOR(255, 255, 255));

		// 배경연결
		Draw.Rectangle9P(&s--.img("Rect"), Draw.CurrentRect(), COLOR(OuterColor));

		m--.SavedXBegin = 6;
		m--.SavedXEnd = Draw.Width() - 6;
		m--.SavedYBegin = 18;
		m--.SavedYEnd = Draw.Height() - 6;
	}
}
