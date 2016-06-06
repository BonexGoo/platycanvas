#include <BxPanel.hpp>

class ToolItemStroke : public BxPanel
{
	public: byte SavedThick;
	public: color_x888 SavedColor;
	public: byte SavedOpacity;
	public: byte SavedAqua;
	private: BxString EventString;
	public: ToolItemStroke(bool isRef, void* varL) : BxPanel(isRef, varL)
	{
		SavedThick = 0;
		SavedColor = 0;
		SavedOpacity = 0;
		SavedAqua = 0;
	}
	public: virtual ~ToolItemStroke()
	{
	}
	public: BxString& GetEventString()
	{
		EventString.Format("<>:Stroke:|<A>|<A>|<A>|",
			BxARG(SavedThick, SavedOpacity, SavedAqua));
		return EventString;
	}
};

FRAMEWORK_PANEL_CLASS("platycanvas.ToolItem.Stroke", ToolItemStroke, XYXY)

local_func void OnUpdated_Round(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<ToolItemStroke>& M = *((BxPanel::ServiceM<ToolItemStroke>*) data);
	if(StrSameCount(message, "Thick:") == 6)
	{
		M--.SavedThick = (byte) AtoI(message + 6);
		M--.UpdateEvent(M--.GetEventString());
	}
}

local_func void OnUpdated_Opacity(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<ToolItemStroke>& M = *((BxPanel::ServiceM<ToolItemStroke>*) data);
	if(StrSameCount(message, "Changed:") == 8)
	{
		M--.SavedOpacity = AtoI(message + 8);
		M--.UpdateEvent(M--.GetEventString());
	}
	else if(StrSameCount(message, "Slide:") == 6)
		M--.SavedOpacity = AtoI(message + 6);
}

local_func void OnUpdated_Aqua(BxPanel* panel, string message, void* data)
{
	BxPanel::ServiceM<ToolItemStroke>& M = *((BxPanel::ServiceM<ToolItemStroke>*) data);
	if(StrSameCount(message, "Changed:") == 8)
	{
		M--.SavedAqua = AtoI(message + 8);
		M--.UpdateEvent(M--.GetEventString());
	}
	else if(StrSameCount(message, "Slide:") == 6)
		M--.SavedAqua = AtoI(message + 6);
}

void OnCreate(string option, unknown param, const bool first)
{
	m--("Round").Bind("platycanvas.ToolRound");
	m--("Round").Updated += OnUpdated_Round, &m;
	m--("Opacity").Bind("platycanvas.ToolSlide", "skin/toolbar/icon_opacity.png");
	m--("Opacity").Updated += OnUpdated_Opacity, &m;
	m--("Aqua").Bind("platycanvas.ToolSlide", "skin/toolbar/icon_aqua.png");
	m--("Aqua").Updated += OnUpdated_Aqua, &m;

	m--.img("TransMap").Load("skin/transmap.png", BxImage::PNG, BxImage::PAD5);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetThick") == same)
	{
        m--.SavedThick = *((byte*) &param);
		m--("Round").Notify("SetThick", param);
	}
	else if(StrCmp(message, "SetColor") == same)
	{
		m--.SavedColor = *((color_x888*) &param);
		m--("Round").Notify("SetColor", param);
	}
	else if(StrCmp(message, "SetOpacity") == same)
	{
        m--.SavedOpacity = *((byte*) &param);
		m--("Opacity").Notify("SetSlide", param);
	}
	else if(StrCmp(message, "SetAqua") == same)
	{
        m--.SavedAqua = *((byte*) &param);
		m--("Aqua").Notify("SetSlide", param);
	}
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
		m--("Round").Paint(BxPanel::prmXYXY, BxARG(0, 0, 140, Draw.Height()));

		const int OpacityEnd = (Draw.Width() + 140) / 2;
		BxTRY(Draw, CLIP(XYXY(140, 10, OpacityEnd - 12, Draw.Height() - 10)))
		{
			Draw.Area(Draw.Width() / 2, Draw.Height() / 2, FORM(&m--.img("TransMap")));
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(m--.SavedColor) >> OPACITY(m--.SavedOpacity));
			m--("Opacity").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), Draw.Height()));
		}
		BxTRY(Draw, CLIP(XYXY(OpacityEnd, 10, Draw.Width() - 12, Draw.Height() - 10)))
		{
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(91, 151, 235));
			m--("Aqua").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), Draw.Height()));
		}
	}
}
