#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.MapAqua", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.int4("AquaR") = *((int*) &param);
	m--.int4("CenterX") = 0;
	m--.int4("CenterY") = 0;

	m--.img("Water").Load("skin/water.png", BxImage::PNG, BxImage::PAD7);
	m--.img("Round").Load("skin/round.png", BxImage::PNG, BxImage::PAD5);
	m--.img("RoundMap").Load("skin/roundmap.png", BxImage::PNG, BxImage::PAD7);
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetAquaR") == same)
        m--.int4("AquaR") = *((int*) &param);
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	const int DistX = m--.int4("CenterX") - x;
	const int DistY = m--.int4("CenterY") - y;
	m--.int4("AquaR") = MinMax(0, Sqrt(DistX * DistX + DistY * DistY), 60);

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
		m--.UpdateEvent(BxString("<>:AquaR:<A>", BxARG(m--.int4("AquaR"))));
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	const int AquaR = m--.int4("AquaR");
	const color_x888 BackColor = RGB32(64, 64, 64);

	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(BackColor));

		BxTRY(Draw, CLIP(XYWH((Draw.Width() - 120) / 2, (Draw.Height() - 120) / 2, 120, 120)))
		{
			const point Center = Draw.CurrentCenter();
			Draw.Area(0, 0, FORM(&m--.img("Water")));
			// 홀구현
			BxTRY(Draw, COLOR(BackColor))
			{
				const rect HoleRect = XYXY(
					Center.x - AquaR, Center.y - AquaR,
					Center.x + AquaR, Center.y + AquaR);
				Draw.Rectangle(FILL, HoleRect, FORM(&m--.img("Round")));
			}
			// 배경연결
			Draw.Area(0, 0, FORM(&m--.img("RoundMap")) >> COLOR(BackColor));
		}
		m--.int4("CenterX") = Draw.CurrentCenter().x;
		m--.int4("CenterY") = Draw.CurrentCenter().y;
	}
}
