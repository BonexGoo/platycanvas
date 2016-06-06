#include <BxPanel.hpp>

enum {BorderX = 15, BorderY = 5};

class MapColor : public BxPanel
{
	public: byte Hue;
	public: byte Lightness;
	public: byte Saturation;
	public: MapColor(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		Hue = 0;
		Lightness = 0;
		Saturation = 0;
	}
	public: virtual ~MapColor()
	{
	}
	public: color_x888 GetRGB()
	{return ColorHLSToRGB(Hue, Lightness, Saturation);}
};

FRAMEWORK_PANEL_CLASS("platycanvas.MapColor", MapColor, XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.Hue = 128;
	m--.Lightness = 128;
	m--.Saturation = *((byte*) &param);
	m--.flag("NeedUpdate") = true;
	m--.int4("Width") = 0;
	m--.int4("Height") = 0;

	m--.img("ColorIcon").Load("skin/coloricon.png", BxImage::PNG, BxImage::PAD5);
	m--.img("RectMap").Load("skin/rectmap.png", BxImage::PNG, BxImage::PAD7);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetHue") == same)
		m--.Hue = *((byte*) &param);
	else if(StrCmp(message, "SetLightness") == same)
		m--.Lightness = *((byte*) &param);
    else if(StrCmp(message, "SetSaturation") == same)
    {
        m--.Saturation = *((byte*) &param);
        m--.flag("NeedUpdate") = true;
    }
	else if(StrCmp(message, "GetRGB") == same)
		return (unknown) m--.GetRGB();
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	m--.Hue = (byte) MinMax(0, 255 * (x - BorderX)
		/ (m--.int4("Width") - BorderX * 2 - 1), 255);
	m--.Lightness = (byte) MinMax(0, 255 * (y - BorderY)
		/ (m--.int4("Height") - BorderY * 2 - 1), 255);

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
		m--.UpdateEvent(BxString("<>:RGB:<A:06>", BxARG((ARG_HEX_UPPER) m--.GetRGB())));
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
    const color_x888 BackColor = RGB32(64, 64, 64);

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(BackColor));

		BxTRY(Draw, CLIP(XYXY(BorderX, BorderY, Draw.Width() - BorderX, Draw.Height() - BorderY)))
		{
			// 맵제작
			if(m--.flag("NeedUpdate"))
			{
				m--.flag("NeedUpdate") = false;
				const int Width = Draw.Width();
				const int Height = Draw.Height();
				const byte* Bitmap = BxCore::AddOn::CreateBMP(Width, Height);
				bitmappixel* Bits = BxCore::AddOn::GetBMPBits(Bitmap);
				for(int i = 0, iend = Width * Height; i < iend; ++i)
				{
					const int x = i % Width;
					const int y = Height - 1 - i / Width;
					const color_x888 RGB = ColorHLSToRGB(
                        255 * x / (Width - 1), 255 * y / (Height - 1), m--.Saturation);
					Bits[i].a = 255;
                    Bits[i].r = ColorToRed(RGB);
                    Bits[i].g = ColorToGreen(RGB);
                    Bits[i].b = ColorToBlue(RGB);
				}
				m--.img("Map").Load(Bitmap, 0, BxImage::BMP, BxImage::PAD7);
				BxCore::AddOn::Release(Bitmap);
			}
			Draw.Area(0, 0, FORM(&m--.img("Map")));
			Draw.Rectangle9P(&m--.img("RectMap"), Draw.CurrentRect(), COLOR(BackColor));
		}

		m--.int4("Width") = Draw.Width();
		m--.int4("Height") = Draw.Height();
	}

	// 컬러아이콘
	const int X = BorderX + (m--.int4("Width") - BorderX * 2) * m--.Hue / 255;
	const int Y = BorderY + (m--.int4("Height") - BorderY * 2) * m--.Lightness / 255;
	Draw.Area(x1 + X, y1 + Y, FORM(&m--.img("ColorIcon")) >> COLOR(m--.GetRGB()));
}
