#include <BxPanel.hpp>

class ToolItemColor : public BxPanel
{
	public: bool NeedUpdate;
	public: byte SavedHue;
	public: byte SavedLightness;
	public: byte SavedSaturation;
	private: BxString EventString;
	public: ToolItemColor(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		NeedUpdate = true;
		SavedHue = 0;
		SavedLightness = 0;
		SavedSaturation = 0;
	}
	public: virtual ~ToolItemColor()
	{
	}
	public: BxString& GetEventString()
	{
		const color_x888 RGB = ColorHLSToRGB(
			SavedHue, SavedLightness, SavedSaturation);
		EventString.Format("<>:Color:<A:06>", BxARG((ARG_HEX_UPPER) RGB));
		return EventString;
	}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolItem.Color", ToolItemColor, XYXY)

local_func void OnUpdated_Palette(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<ToolItemColor>& M = *((BxPanel::ServiceM<ToolItemColor>*) data);

	if(StrSameCount(message, "Hue:") == 4)
	{
		M--.NeedUpdate = true;
		M--.SavedHue = AtoI(message + 4);
		M--.UpdateEvent(M--.GetEventString());
	}
	else if(StrSameCount(message, "Lightness:") == 10)
	{
		M--.NeedUpdate = true;
		M--.SavedLightness = AtoI(message + 10);
		M--.UpdateEvent(M--.GetEventString());
	}
}

local_func void OnUpdated_Saturation(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<ToolItemColor>& M = *((BxPanel::ServiceM<ToolItemColor>*) data);
	if(StrSameCount(message, "Changed:") == 8)
	{
		M--.SavedSaturation = AtoI(message + 8);
		M--.UpdateEvent(M--.GetEventString());
	}
	else if(StrSameCount(message, "Slide:") == 6)
	{
		M--.SavedSaturation = AtoI(message + 6);
		M--("Palette").Notify("SetSaturation", M--.SavedSaturation);
	}
}

void OnCreate(string option, unknown param, const bool first)
{
	m--("Palette").Bind("platycanvas.ToolPalette", "skin/toolbar/icon_spoid.png");
	m--("Palette").Updated += OnUpdated_Palette, &m;
	m--("Saturation").Bind("platycanvas.ToolSlide", "skin/toolbar/icon_saturation.png");
	m--("Saturation").Updated += OnUpdated_Saturation, &m;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetHue") == same)
	{
		const byte NewHue = *((byte*) &param);
		if(m--.SavedHue != NewHue)
		{
			m--.NeedUpdate = true;
			m--.SavedHue = NewHue;
		}
		m--("Palette").Notify("SetHue", param);
	}
	else if(StrCmp(message, "SetLightness") == same)
	{
		const byte NewLightness = *((byte*) &param);
		if(m--.SavedLightness != NewLightness)
		{
			m--.NeedUpdate = true;
			m--.SavedLightness = NewLightness;
		}
		m--("Palette").Notify("SetLightness", param);
	}
    else if(StrCmp(message, "SetSaturation") == same)
    {
		m--.SavedSaturation = *((byte*) &param);
        m--("Palette").Notify("SetSaturation", param);
        m--("Saturation").Notify("SetSlide", param);
    }
	else if(StrCmp(message, "NeedUpdate") == same)
		m--("Palette").Notify(message, param);
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
		const int PaletteEnd = Draw.Width() - 80;
		BxTRY(Draw, CLIP(XYXY(10, 10, PaletteEnd - 12, Draw.Height() - 10)))
			m--("Palette").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), Draw.Height()));
		BxTRY(Draw, CLIP(XYXY(PaletteEnd, 10, Draw.Width() - 12, Draw.Height() - 10)))
		{
			if(m--.NeedUpdate)
			{
				m--.NeedUpdate = false;
				const int Width = Draw.Width() - 6 - 6;
				const int Height = Draw.Height() - 18 - 6;
				const byte* Bitmap = BxCore::AddOn::CreateBMP(Width, Height);
				bitmappixel* Bits = BxCore::AddOn::GetBMPBits(Bitmap);
				for(int i = 0, iend = Width * Height; i < iend; ++i)
				{
					const int y = i / Width;
					const color_x888 RGB = ColorHLSToRGB(
						m--.SavedHue,
						m--.SavedLightness,
						255 * y / (Height - 1));
					Bits[i].a = 255;
					Bits[i].r = ColorToRed(RGB);
					Bits[i].g = ColorToGreen(RGB);
					Bits[i].b = ColorToBlue(RGB);
				}
				m--.img("SaturationMap").Load(Bitmap, 0, BxImage::BMP, BxImage::PAD7);
				BxCore::AddOn::Release(Bitmap);
			}
			Draw.Area(6, 18, FORM(&m--.img("SaturationMap")));
			m--("Saturation").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), Draw.Height()));
		}
	}
}
