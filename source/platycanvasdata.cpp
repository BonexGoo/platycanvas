#include <BxScene.hpp>
#include <BxPanel.hpp>
#include "adapter.hpp"
#include "command.hpp"
#include "export.hpp"
#include "input.hpp"
#include "platycanvasdata.hpp"

#define THICK_SIZE (60.0f)

platycanvasdata::platycanvasdata()
{
	ViewOx = 0;
	ViewOy = 0;
	WacomDown = false;
	ScreenMode = true;
	IsZoomEnabled = false;
	CursorX = 0;
	CursorY = 0;
	CursorD = 0;
	CursorImage.Load("skin/cursor.png", BxImage::PNG, BxImage::PAD5);
	IsZoomScaling = false;
	OldCursorX = 0;
	OldCursorY = 0;
	CreateMin = 50;
	CreateRate = 10;
	CreateCanvasMode = false;
	CreateCanvasBegin = XY(0, 0);
	CreateCanvasEnd = XY(0, 0);
	HandImage.Load("skin/toolbar/button_hand.png", BxImage::PNG, BxImage::PAD5);
	SpoidImage.Load("skin/toolbar/button_spoid.png", BxImage::PNG, BxImage::PAD5);
	HandAnimation = 0;
	SpoidAnimation = 0;

	if(!BxCore::Font::IsExistNick("skin/cartoonstory.ttf"))
		BxCore::Font::NickOpen("skin/cartoonstory.ttf", "skin/cartoonstory.ttf");
	#ifdef ANDROID
		TitleFont = BxCore::Font::Open("skin/cartoonstory.ttf", 30);
	#else
		TitleFont = BxCore::Font::Open("skin/cartoonstory.ttf", 15);
	#endif

    IsOpenedPenTool = true;
    PenToolPosPercent = 100;
	PenDoor.Bind("platycanvas.ToolDoor", "FF7F27", 0);
	PenDoor.Clicked += OnClicked_PenDoor, this;
	PenTool.Bind("platycanvas.ToolBar.Pen", "FF7F27", 0);
	PenTool("Close").Clicked += OnClicked_PenClose, this;

	IsOpenedLayerTool = false;
	LayerToolPosPercent = 0;
	LayerDoor.Bind("platycanvas.ToolDoor", "B5E61D", 90);
	LayerDoor.Clicked += OnClicked_LayerDoor, this;
	LayerTool.Bind("platycanvas.ToolBar.Layer", "B5E61D", 90);
	LayerTool("Close").Clicked += OnClicked_LayerClose, this;

	const byte Thick = 64;
    const byte Opacity = 255;
	const byte Aqua = 0;
	const byte Hue = 128;
	const byte Lightness = 128;
	const byte Saturation = 255;
	const color_x888 RGB = ColorHLSToRGB(Hue, Lightness, Saturation);

	UpdateStroke(StrokeByAqua);
	Command::SetStrokeBy("aqua");
	Command::SetThick(THICK_SIZE * Thick / 255);
	Command::SetColor(Color(ColorToRed(RGB), ColorToGreen(RGB), ColorToBlue(RGB), Opacity, Aqua));

	PenTool("Stroke").Notify("SetThick", Thick);
	PenTool("Stroke").Notify("SetColor", RGB);
	PenTool("Stroke").Notify("SetOpacity", Opacity);
	PenTool("Stroke").Notify("SetAqua", Aqua);
	PenTool("Stroke").Updated += OnUpdated_Stroke, this;

	PenTool("Color").Notify("SetHue", Hue);
	PenTool("Color").Notify("SetLightness", Lightness);
    PenTool("Color").Notify("SetSaturation", Saturation);
    PenTool("Color").Updated += OnUpdated_Color, this;

	PenTool("PenButton").Clicked += OnClicked_PenButton, this;
	PenTool("EraserButton").Clicked += OnClicked_EraserButton, this;
	PenTool("HandButton").Clicked += OnClicked_HandButton, this;
	PenTool("SpoidButton").Clicked += OnClicked_SpoidButton, this;

    BxCore::File::SearchFiles("", platycanvasdata::OnFile, this, nullptr);
}

platycanvasdata::~platycanvasdata()
{
	AutoSaveAll(true);
	BxCore::Font::Close(TitleFont);
}

void platycanvasdata::UpdateStroke(StrokeBy stroke)
{
	PenTool("PenButton").Notify("SetState", stroke == StrokeByAqua);
	PenTool("EraserButton").Notify("SetState", stroke == StrokeByErase);
	PenTool("HandButton").Notify("SetState", stroke == StrokeByHand);
	PenTool("SpoidButton").Notify("SetState", stroke == StrokeBySpoid);
	SavedStroke = stroke;
}

void platycanvasdata::AutoSaveAll(bool mustsave)
{
	for(int i = 0, iend = Command::Canvas::GetDocumentCount(); i < iend; ++i)
	{
		const sint32 doc = Command::Canvas::GetDocument(i);
		const sint64 time = Command::Canvas::GetTimeOfLastDrawing(doc);
		if(time != -1 && (time + 10000 < BxCore::System::GetTimeMilliSecond() || mustsave))
		{
			// 병합을 위한 스냅샷 저장
			const int BitmapWidth = 512;
			const int BitmapHeight = 512;
			const byte* Bitmap = BxCore::AddOn::CreateBMP(BitmapWidth, BitmapHeight);
			const int BitsLength = sizeof(bitmappixel) * BitmapWidth * BitmapHeight;
			bitmappixel* Bits = BxCore::AddOn::GetBMPBits(Bitmap);
			BxCore::Util::MemSet(Bits, 0xFF, BitsLength);
			Command::Canvas::Snapshot(doc, (uint32*) Bits, BitmapWidth, BitmapHeight, BoundByOuter);
			id_file SkinBmp = BxCore::File::Open("tempskin.bmp", "wb");
			BxCore::File::Write(SkinBmp, Bitmap + 2, 2 + sizeof(bitmapfile) + sizeof(bitmapinfo) + BitsLength);
			BxCore::File::Close(SkinBmp);
			BxCore::AddOn::Release(Bitmap);

			// 바이너리 저장
			String DocName = Command::Canvas::GetName(doc);
			int Begin = 0, Number = 0;
			do
			{
				BxString CurFileName("<>:<A>.<A>.bmp", BxARG((string) DocName, Number++));
				Begin = Export::Save(doc, CurFileName, Begin);
				Export::MergeSkin(CurFileName, "tempskin.bmp");
			}
			while(Begin);
		}
	}
}

void platycanvasdata::SetColor(byte r, byte g, byte b)
{
	const color_x888 RGB = RGB32(r, g, b);
	PenTool("Stroke").Notify("SetColor", RGB);

	const color_x888 HLS = ColorRGBToHLS(r, g, b);
	const byte Hue = BxDrawGlobal::ColorToRed(HLS);
	const byte Lightness = BxDrawGlobal::ColorToGreen(HLS);
	const byte Saturation = BxDrawGlobal::ColorToBlue(HLS);
	PenTool("Color").Notify("SetHue", Hue);
	PenTool("Color").Notify("SetLightness", Lightness);
    PenTool("Color").Notify("SetSaturation", Saturation);
}

void platycanvasdata::Resized()
{
	PenTool("Color").Notify("NeedUpdate");
}

void platycanvasdata::OnClicked_PenDoor(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.IsOpenedPenTool = true;
}

void platycanvasdata::OnClicked_PenClose(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.IsOpenedPenTool = false;
}

void platycanvasdata::OnClicked_LayerDoor(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.IsOpenedLayerTool = true;
}

void platycanvasdata::OnClicked_LayerClose(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.IsOpenedLayerTool = false;
}

void platycanvasdata::OnUpdated_Stroke(BxPanel* panel, string message, void* data)
{
	if(StrSameCount(message, "Stroke:") == 7)
	{
		BxVar<BxString::Parse> Elements;
		BxString::Parse::Division(Elements, message + 7);
		const byte Thick = (byte) AtoI(Elements[0]);
		const byte Opacity = (byte) AtoI(Elements[1]);
		const byte Aqua = (byte) AtoI(Elements[2]);
		Command::SetThick(THICK_SIZE * Thick / 255);
		Color CurColor = Command::GetColor();
		CurColor.a = Opacity;
		CurColor.aqua = Aqua;
		Command::SetColor(CurColor);
	}
}

void platycanvasdata::OnUpdated_Color(BxPanel* panel, string message, void* data)
{
    if(StrSameCount(message, "Color:") == 6)
    {
		const color_x888 RGB = HtoI(message + 6);
		Color CurColor = Command::GetColor();
		CurColor.r = ColorToRed(RGB);
		CurColor.g = ColorToGreen(RGB);
		CurColor.b = ColorToBlue(RGB);
		Command::SetColor(CurColor);

		platycanvasdata& This = *((platycanvasdata*) data);
		This.PenTool("Stroke").Notify("SetColor", RGB);
    }
}

void platycanvasdata::OnClicked_PenButton(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.UpdateStroke(StrokeByAqua);
	Command::SetStrokeBy("aqua");

	if(StrCmp(message, "LongTake") == same)
		This.ScreenMode = !This.ScreenMode;
}

void platycanvasdata::OnClicked_EraserButton(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.UpdateStroke(StrokeByErase);
	Command::SetStrokeBy("erase");
}

void platycanvasdata::OnClicked_HandButton(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.UpdateStroke(StrokeByHand);
	Command::SetStrokeBy("hand");
}

void platycanvasdata::OnClicked_SpoidButton(BxPanel* panel, string message, void* data)
{
	platycanvasdata& This = *((platycanvasdata*) data);
	This.UpdateStroke(StrokeBySpoid);
	Command::SetStrokeBy("spoid");
}

void platycanvasdata::OnFile(bool isdirname, string _tmp_ name, void* data1, void* data2)
{
	platycanvasdata& This = *((platycanvasdata*) data1);

	if(!isdirname)
	{
		BxString Name(name);
		if(Name.Right(6).CompareNoCase(".0.bmp") == same)
		{
			Name.DeleteLeft(1);
			Name.DeleteRight(6);
			sint32 doc = Command::Canvas::Create(true);
			Command::Canvas::SetFocus(doc);

			Command::Canvas::SetName(doc, (string) Name);
			for(int i = 0; i < 10240; ++i)
				if(!Export::Load(doc, BxString("<>:<A>.<A>.bmp", BxARG(Name, i)), true, true))
					break;
		}
	}
}
