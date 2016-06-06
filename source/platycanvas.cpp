#include <BxScene.hpp>
#include <BxPanel.hpp>
#include "adapter.hpp"
#include "command.hpp"
#include "export.hpp"
#include "input.hpp"
#include "platycanvasdata.hpp"

FRAMEWORK_SCENE(platycanvasdata, "platycanvas")

syseventresult OnEvent(platycanvasdata& This, const sysevent& Event)
{
	if(Event.type == syseventtype_init)
	{
		This.ViewOx = Event.init.w / 2;
		This.ViewOy = Event.init.h / 2;
		Command::SetViewRadius(Math::Sqrt(This.ViewOx * This.ViewOx + This.ViewOy * This.ViewOy));
		// 스피디포스 설정
		#ifdef ANDROID
			Input::SetDeviceSpeedyForceOn(DeviceByTouch, 100);
		#else
			Input::SetDeviceSpeedyForceOn(DeviceByTouch, 50);
		#endif
	}
	else if(Event.type == syseventtype_resize)
	{
		This.ViewOx = Event.resize.w / 2;
		This.ViewOy = Event.resize.h / 2;
		Command::SetViewRadius(Math::Sqrt(This.ViewOx * This.ViewOx + This.ViewOy * This.ViewOy), true);
		This.Resized();
	}
	else if(Event.type == syseventtype_touch)
	{
		const float X = Event.touch.fx - This.ViewOx;
		const float Y = Event.touch.fy - This.ViewOy;
		if(Event.touch.type == systouchtype_down
			&& Command::Canvas::GetFocus() == -1
			&& Command::Canvas::GetDocumentBy(Point(X, Y)) == -1)
			This.CreateCanvasMode = true;

		if(This.CreateCanvasMode)
		{
			if(0 < Event.touch.id)
				This.CreateCanvasMode = false;
			else switch(Event.touch.type)
			{
			case systouchtype_down:
				This.CreateCanvasBegin = XY(Event.touch.x, Event.touch.y);
			case systouchtype_move:
				This.CreateCanvasEnd = XY(Event.touch.x, Event.touch.y);
				break;
			default:
				{
					This.CreateCanvasMode = false;
					const int L = Min(This.CreateCanvasBegin.x, This.CreateCanvasEnd.x);
					const int T = Min(This.CreateCanvasBegin.y, This.CreateCanvasEnd.y);
					const int R = Max(This.CreateCanvasBegin.x, This.CreateCanvasEnd.x);
					const int B = Max(This.CreateCanvasBegin.y, This.CreateCanvasEnd.y);
					if(L + This.CreateMin <= R && T + This.CreateMin <= B)
					{
						sint32 doc = Command::Canvas::Create(true);
						Size size = Size((R - L) * This.CreateRate, (B - T) * This.CreateRate);
						Command::Canvas::SetArea(doc, Rect(Point(-size.w / 2, -size.h / 2), size));
						Command::Canvas::SetZoomInRect(doc, Rect(L - This.ViewOx, T - This.ViewOy, R - This.ViewOx, B - This.ViewOy));
						Command::Canvas::SetFocus(doc);

						for(int i = 0; i < 10240; ++i)
						{
							BxString NewDocName("<>:untitled-<A>", BxARG(i));
							BxString NewFileName(NewDocName + ".0.bmp");
							if(!BxCore::File::IsExist(NewFileName))
							{
								Command::Canvas::SetName(doc, (string) NewDocName);
								Export::Save(doc, NewFileName, 0);
								break;
							}
						}
					}
				}
				break;
			}
		}

		Command::SetCurrentTime(BxCore::System::GetTimeMilliSecond());
		DeviceBy Kind = (Event.touch.kind == inputkind_wacom_pen)?
			DeviceByTablet : DeviceByTouch;
		switch(Event.touch.type)
		{
		case systouchtype_down:
			Input::ScreenDown(Event.touch.id, X, Y, Event.touch.force, Kind);
			if(This.SavedStroke == StrokeBySpoid)
			{
				Color OldColor = Command::GetColor();
				This.SetColor(OldColor.r, OldColor.g, OldColor.b);
			}
			break;
		case systouchtype_move:
			Input::ScreenDrag(Event.touch.id, X, Y, Event.touch.force, Kind);
			if(This.SavedStroke == StrokeBySpoid)
			{
				Color OldColor = Command::GetColor();
				This.SetColor(OldColor.r, OldColor.g, OldColor.b);
			}
			break;
		default:
			Input::ScreenUp(Event.touch.id, Kind);
			if(This.SavedStroke == StrokeBySpoid)
			{
				This.UpdateStroke(StrokeByAqua);
				Command::SetStrokeBy("aqua");
			}
			break;
		}
		return syseventresult_done;
	}
    else if(Event.type == syseventtype_button && Event.button.type == sysbuttontype_up)
    {
        if(StrCmp(Event.button.name, "RemoveBar") == same)
        {
            const int doc = Command::Canvas::GetFocus();
            if(doc != -1)
            {
                String DocName = Command::Canvas::GetName(doc);
                for(int i = 0; i < 10240; ++i)
                {
                    BxString CurFileName("<>:<A>.<A>.bmp", BxARG((string) DocName, i));
                    if(BxCore::File::IsExist(CurFileName))
                        BxCore::File::RemoveFile(CurFileName);
                    else break;
                }
                Command::Canvas::Remove(doc);
            }
        }
    }
	return syseventresult_pass;
}

sysupdateresult OnUpdate(platycanvasdata& This)
{
	float IsZooming = false;
	while(BxCore::Wacom::TryNextPacket())
	{
		const uint ButtonID = BxCore::Wacom::GetPacketButtons();
		const bool HasDrawPressed = ((ButtonID & 0x1) != 0);
		const bool HasZoomPressed = ((ButtonID & 0x2) != 0) || This.IsZoomScaling;
		const bool HasSpoidPressed = ((ButtonID & 0x4) != 0);
		const float Force = BxCore::Wacom::GetPacketNormalPressure() / 1023.0f;

		const size ScreenSize = BxCore::Simulator::GetScreenSize();
		const point WindowPos = BxCore::Simulator::GetWindowPos();
		const float SX = ScreenSize.w * BxCore::Wacom::GetPacketX()
			/ (float) BxCore::Wacom::GetDeviceExtX() - WindowPos.x;
		const float SY = ScreenSize.h - ScreenSize.h * BxCore::Wacom::GetPacketY()
			/ (float) BxCore::Wacom::GetDeviceExtY() - WindowPos.y;

		// 드로잉
		const sint32 doc = Command::Canvas::GetFocus();
		float CX = 0, CY = 0;
		if(This.ScreenMode || doc == -1)
		{
			// 가상터치
			BxCore::Simulator::SetCursorState(inputkind_wacom_pen,
				SX, SY, Force, HasDrawPressed && !HasZoomPressed);
			// 커서계산
			CX = SX - BxCore::Main::GetGUIMarginL();
			CY = SY - BxCore::Main::GetGUIMarginT();
		}
		else
		{
			const Rect DocArea = Command::Canvas::GetArea(doc);
			const float Rate = Math::MaxF(
				DocArea.Width() / BxCore::Wacom::GetDeviceExtX(),
				DocArea.Height() / BxCore::Wacom::GetDeviceExtY());
			const float Width = BxCore::Wacom::GetDeviceExtX() * Rate;
			const float Height = BxCore::Wacom::GetDeviceExtY() * Rate;
			const float X = BxCore::Wacom::GetPacketX() * Rate - Width / 2;
			const float Y = Height / 2 - BxCore::Wacom::GetPacketY() * Rate;

			// 펜드로잉
			if(HasDrawPressed && !HasZoomPressed)
			{
				if(This.WacomDown) Input::CanvasDrag(X, Y, Force);
				else Input::CanvasDown(X, Y, Force);
				This.WacomDown = true;
			}
			else if(This.WacomDown)
			{
				This.WacomDown = false;
				Input::CanvasUp();
			}
			// 커서계산
			Point Cursor = Command::Canvas::GetZoom(doc).ToView(X, Y);
			CX = Cursor.x + This.ViewOx;
			CY = Cursor.y + This.ViewOy;
		}

		// 줌처리
		if(HasZoomPressed)
		{
			IsZooming = true;
			This.IsZoomEnabled = true;
			int doc = Command::Canvas::GetFocus();
			if(doc != -1)
			{
				Zoom zoom = Command::Canvas::GetZoom(doc);
				if(0.25f < Force)
				{
					if(!This.IsZoomScaling)
					{
						This.IsZoomScaling = true;
						This.ZoomBegin = Point(CX - This.ViewOx, CY - This.ViewOy);
					}
					const float VectorY = Math::MinF(Math::MaxF(-1, (SY - This.OldCursorY) / 10), 1);
					zoom.UpdateBy(This.ZoomBegin, This.ZoomBegin, 1 + (Force - 0.25f) * 0.1f * VectorY);
				}
				else if(!HasDrawPressed)
				{
					This.IsZoomScaling = false;
					zoom.UpdateBy(Point(This.OldCursorX, This.OldCursorY), Point(SX, SY), 1);
				}
				Command::Canvas::SetZoom(doc, zoom);

				if(0 < This.PenToolPosPercent)
					This.PenTool("HandButton").Notify("SetLightning", 20);
				This.HandAnimation = 50;
				This.SpoidAnimation = 0;
			}
		}
		This.OldCursorX = SX;
		This.OldCursorY = SY;

		// 스포이드
		if(HasSpoidPressed)
		{
			int doc = Command::Canvas::GetFocus();
			if(doc != -1)
			{
				const Zoom SpoidZoom = Command::Canvas::GetZoom(doc);
				const Point SpoidPoint = SpoidZoom.ToDocument(CX - This.ViewOx, CY - This.ViewOy);
				const Color BackColor = Color(255, 255, 255, 255, 0);
				const Color SpoidColor = Command::GetColorBy(SpoidPoint, BackColor);
				Color OldColor = Command::GetColor();
				OldColor.r = SpoidColor.r;
				OldColor.g = SpoidColor.g;
				OldColor.b = SpoidColor.b;
				Command::SetColor(OldColor);
				This.SetColor(OldColor.r, OldColor.g, OldColor.b);

				if(0 < This.PenToolPosPercent)
					This.PenTool("SpoidButton").Notify("SetLightning", 20);
				This.SpoidAnimation = 50;
				This.HandAnimation = 0;
			}
		}

		// 커서업데이트
		const float Distance = BxCore::Wacom::GetPacketZ()
			/ (float) BxCore::Wacom::GetDeviceExtZ();
		This.CursorX = CX;
		This.CursorY = CY;
		This.CursorD = Distance * 2;
	}

	// 줌처리후 업데이트
	if(!IsZooming && This.IsZoomEnabled)
	{
		This.IsZoomEnabled = false;
		int doc = Command::Canvas::GetFocus();
		Command::Canvas::Update(doc);
	}

	// 자동 파일저장
	Command::SetCurrentTime(BxCore::System::GetTimeMilliSecond());
	This.AutoSaveAll(false);

	// 툴바 애니메이션
	if(This.IsOpenedPenTool)
		This.PenToolPosPercent = ((This.PenToolPosPercent + 1) * 3 + 100 * 1) / (3 + 1);
	else This.PenToolPosPercent = ((This.PenToolPosPercent + 0) * 3 + 0 * 1) / (3 + 1);
	if(This.IsOpenedLayerTool)
		This.LayerToolPosPercent = ((This.LayerToolPosPercent + 1) * 3 + 100 * 1) / (3 + 1);
	else This.LayerToolPosPercent = ((This.LayerToolPosPercent + 0) * 3 + 0 * 1) / (3 + 1);

	// 애니메이션
	if(0 < This.HandAnimation)
		This.HandAnimation--;
	if(0 < This.SpoidAnimation)
		This.SpoidAnimation--;

	return sysupdateresult_do_render;
}

void OnRender(platycanvasdata& This, BxDraw& Draw)
{
    Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(128, 128, 128));

	BxTRY(Draw, HOTSPOT(This.ViewOx, This.ViewOy))
	for(int i = 0, iend = Command::Canvas::GetDocumentCount(); i < iend; ++i)
	{
		const int doc = Command::Canvas::GetDocument(i);
		linkstep1 doclink = Adapter::GetDocumentLink(doc);
		if(!doclink) continue;

		const Zoom DocZoom = Command::Canvas::GetZoom(doc);
		const Rect DocArea = Command::Canvas::GetArea(doc);
		const Rect ViewRect = DocArea * DocZoom;
		const int L = Max(-This.ViewOx, (int) ViewRect.l);
		const int T = Max(-This.ViewOy, (int) ViewRect.t);
		const int R = Min((int) ViewRect.r, This.ViewOx);
		const int B = Min((int) ViewRect.b, This.ViewOy);
		const color_x888 BackColor = (Command::Canvas::GetFocus() == doc)?
			RGB32(255, 255, 255) : RGB32(192, 192, 192);

		// 배경화면
		Draw.Rectangle(FILL, XYXY(L, T, R, B), COLOR(BackColor));

		// 타이틀
		if((int) ViewRect.l < This.ViewOx && -This.ViewOx < (int) ViewRect.r
			&& (int) ViewRect.t < This.ViewOy && -This.ViewOy < (int) ViewRect.b)
		{
			String DocName = Command::Canvas::GetName(doc);
			sint64 DocTime = Command::Canvas::GetTimeOfLastDrawing(doc);
			Draw.Text(This.TitleFont, BxString("<>:<A><A>, <A> x <A>",
				BxARG((string) DocName, (DocTime != -1)? "(*)" : "",
				(int) DocArea.Width(), (int) DocArea.Height())),
				XY((int) ViewRect.r, (int) ViewRect.t), textalign_right_bottom, COLOR(BackColor));
		}

		// 도형들
		BxTRY(Draw, CLIP(XYXY(L, T, R, B), L, T))
		for(int l = 0, lend = Adapter::GetLayerCount(doclink); l < lend; ++l)
		{
			linkstep2 layerlink = Adapter::GetLayerLink(doclink, l);
			if(!layerlink || !Adapter::IsLayerVisibled(layerlink))
				continue;
			for(int dyn = 0; dyn < 2; ++dyn)
			for(int s = 0, send = Adapter::GetShapeCount(layerlink, dyn != 0); s < send; ++s)
			{
				linkstep3 shapelink = Adapter::GetShapeLink(layerlink, dyn != 0, s);
				if(!shapelink) continue;
				if(CONST_STRING("PlatyVGElement::MeshAqua") == Adapter::GetShapeMeshType(shapelink))
				{
					color_x888 ShapeColor = RGB32(
						Adapter::GetShapeColorRed(shapelink),
						Adapter::GetShapeColorGreen(shapelink),
						Adapter::GetShapeColorBlue(shapelink));
					BxCore::OpenGL2D::RenderStripDirectly(
						Adapter::GetShapeMeshCount(shapelink),
						(const void*) Adapter::GetShapeMeshArray(shapelink),
						Adapter::GetShapeColorAlpha(shapelink),
						Adapter::GetShapeColorAqua(shapelink), ShapeColor,
						DocZoom.offset.x, DocZoom.offset.y, DocZoom.scale,
						Adapter::GetShapeMatrixM11(shapelink),
						Adapter::GetShapeMatrixM12(shapelink),
						Adapter::GetShapeMatrixM21(shapelink),
						Adapter::GetShapeMatrixM22(shapelink),
						Adapter::GetShapeMatrixDx(shapelink),
						Adapter::GetShapeMatrixDy(shapelink));
				}
			}
		}
	}

	// Create Canvas
	if(This.CreateCanvasMode)
	{
		#ifdef ANDROID
			const int Gap = 40;
		#else
			const int Gap = 20;
		#endif
		const int L = Min(This.CreateCanvasBegin.x, This.CreateCanvasEnd.x);
		const int T = Min(This.CreateCanvasBegin.y, This.CreateCanvasEnd.y);
		const int R = Max(This.CreateCanvasBegin.x, This.CreateCanvasEnd.x);
		const int B = Max(This.CreateCanvasBegin.y, This.CreateCanvasEnd.y);
		BxTRY(Draw, COLOR(255, 255, 255) >> OPACITY(64))
		{
			Draw.Rectangle(FILL, XYXY(L - Gap, T - Gap, R + Gap, T));
			Draw.Rectangle(FILL, XYXY(L - Gap, T, L, B));
			Draw.Rectangle(FILL, XYXY(R, T, R + Gap, B));
			Draw.Rectangle(FILL, XYXY(L - Gap, B, R + Gap, B + Gap));
		}
		BxTRY(Draw, CLIP(XYXY(L - Gap, T - Gap, R, T)))
		{
			const bool IsPossibled = (L + This.CreateMin <= R && T + This.CreateMin <= B);
			const color_x888 TextColor = (IsPossibled)? RGB32(0, 0, 0) : RGB32(128, 0, 0);
			Draw.Text(This.TitleFont, BxString("<>:<A> x <A>",
				BxARG((R - L) * This.CreateRate, (B - T) * This.CreateRate)),
				XY(Draw.Width(), Draw.Height()), textalign_right_bottom, COLOR(TextColor));
		}
	}

	const int PenToolHeight = 180;
	const int PenToolPos = PenToolHeight * This.PenToolPosPercent / 100;
	const int LayerToolWidth = 240;
	const int LayerToolPos = LayerToolWidth * This.LayerToolPosPercent / 100;

	// Pen
	if(0 < This.PenToolPosPercent)
		This.PenTool.Paint(BxPanel::prmXYXY, BxARG(0, PenToolPos - PenToolHeight, Draw.Width(), PenToolPos));
	if(This.PenToolPosPercent < 100)
	{
		const int PenDoorHeight = 33;
		const int PenDoorPos = PenDoorHeight * This.PenToolPosPercent / 100;
		This.PenDoor.Paint(BxPanel::prmXY, BxARG(60, 0 - PenDoorPos));
	}

	// Layer
	if(0 < This.LayerToolPosPercent)
		This.LayerTool.Paint(BxPanel::prmXYXY, BxARG(Draw.Width() - LayerToolPos,
			PenToolPos, Draw.Width() - LayerToolPos + LayerToolWidth, Draw.Height()));
	if(This.LayerToolPosPercent < 100)
	{
		const int LayerDoorWidth = 33;
		const int LayerDoorPos = LayerDoorWidth * This.LayerToolPosPercent / 100;
		This.LayerDoor.Paint(BxPanel::prmXY, BxARG(Draw.Width() + LayerDoorPos, PenToolPos + 60));
	}

	// QuickHand
	if(0 < This.HandAnimation && This.PenToolPosPercent == 0)
	BxTRY(Draw, OPACITY(Min(This.HandAnimation * This.HandAnimation, 255)))
	{
		Draw.Area(Draw.Width() - Max(80, 50 + LayerToolPos), 50, FORM(&This.CursorImage)
			>> COLOR(255, 0, 255) >> OPACITY(128));
		Draw.Area(Draw.Width() - Max(80, 50 + LayerToolPos), 50, FORM(&This.HandImage));
	}

	// QuickSpoid
	if(0 < This.SpoidAnimation && This.PenToolPosPercent == 0)
	BxTRY(Draw, OPACITY(Min(This.SpoidAnimation * This.SpoidAnimation, 255)))
	{
		const Color SpoidColor = Command::GetColor();
		Draw.Area(Draw.Width() - Max(80, 50 + LayerToolPos), 50, FORM(&This.CursorImage)
			>> COLOR(SpoidColor.r, SpoidColor.g, SpoidColor.b));
		Draw.Area(Draw.Width() - Max(80, 50 + LayerToolPos), 50, FORM(&This.SpoidImage));
	}

	// Cursor
	if(0 < This.CursorD)
	{
		Point Cursor = Point(This.CursorX, This.CursorY);
		BxTRY(Draw, (This.IsZoomEnabled)? COLOR(255, 0, 255) : COLOR(0, 0, 0))
		{
			const byte CursorOpacity = (byte) MinMax(0, (int) (128 - 128 * This.CursorD), 128);
			Draw.Rectangle(FILL, XYR((int) Cursor.x, (int) Cursor.y, (int) (50 * This.CursorD)),
				FORM(&This.CursorImage) >> OPACITY(CursorOpacity));
			if(32 <= CursorOpacity)
				Draw.Rectangle(FILL, XYR((int) Cursor.x, (int) Cursor.y, 1));
		}
	}

    // Remove Bar
    if(Command::Canvas::GetFocus() != -1)
	#ifdef ANDROID
		BxTRY(Draw, CLIP(XYWH(0, Draw.Height() - 80, 400, 80)), "RemoveBar")
	#else
		BxTRY(Draw, CLIP(XYWH(0, Draw.Height() - 40, 200, 40)), "RemoveBar")
	#endif
    {
        Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(255, 0, 0) >> OPACITY(128));
        Draw.Text(This.TitleFont, BxString("REMOVE THIS"),
            Draw.CurrentCenter(), textalign_center_middle, COLOR(255, 0, 0));
    }
}
