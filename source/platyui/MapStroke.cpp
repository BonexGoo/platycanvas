#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.MapStroke", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.int4("ThickR") = AtoI(option);
	m--.int4("RGB") = 0;
	m--.int4("Opacity") = *((int*) &param);
	m--.int4("CenterX") = 0;
	m--.int4("CenterY") = 0;

	m--.img("TransMap").Load("skin/transmap.png", BxImage::PNG, BxImage::PAD5);
	m--.img("RoundMap").Load("skin/roundmap.png", BxImage::PNG, BxImage::PAD7);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetThickR") == same)
        m--.int4("ThickR") = *((int*) &param);
	else if(StrCmp(message, "SetRGB") == same)
        m--.int4("RGB") = *((color_x888*) &param);
	else if(StrCmp(message, "SetOpacity") == same)
        m--.int4("Opacity") = *((int*) &param);
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	const int DistX = m--.int4("CenterX") - x;
	const int DistY = m--.int4("CenterY") - y;
	m--.int4("ThickR") = MinMax(3, Sqrt(DistX * DistX + DistY * DistY), 60);

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
		m--.UpdateEvent(BxString("<>:ThickR:<A>", BxARG(m--.int4("ThickR"))));
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	const int ThickR = m--.int4("ThickR");
	const int Opacity = m--.int4("Opacity");
	const color_x888 BackColor = RGB32(64, 64, 64);
	const color_x888 HoleColor = RGB32(224, 224, 224);
	const color_x888 StrokeColor = (color_x888) m--.int4("RGB");

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(BackColor));

		BxTRY(Draw, CLIP(XYWH((Draw.Width() - 120) / 2, (Draw.Height() - 120) / 2, 120, 120)))
		{
			const point Center = Draw.CurrentCenter();
			Draw.Area(Center.x, Center.y, FORM(&m--.img("TransMap")));
			Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(StrokeColor) >> OPACITY(Opacity));
			// 홀구현
			BxTRY(Draw, COLOR(HoleColor))
			{
				const rect HoleRect = XYXY(
					Center.x - ThickR, Center.y - ThickR,
					Center.x + ThickR, Center.y + ThickR);
				Draw.Rectangle(FILL, HoleRect, FORM(&m--.img("RoundMap")));
				Draw.Rectangle(FILL, XYXY(0, 0, Draw.Width(), HoleRect.t));
				Draw.Rectangle(FILL, XYXY(0, HoleRect.t, HoleRect.l, HoleRect.b));
				Draw.Rectangle(FILL, XYXY(HoleRect.r, HoleRect.t, Draw.Width(), HoleRect.b));
				Draw.Rectangle(FILL, XYXY(0, HoleRect.b, Draw.Width(), Draw.Height()));
			}
			// 배경연결
			Draw.Area(0, 0, FORM(&m--.img("RoundMap")) >> COLOR(BackColor));
		}
		m--.int4("CenterX") = Draw.CurrentCenter().x;
		m--.int4("CenterY") = Draw.CurrentCenter().y;
	}
}
