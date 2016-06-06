#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.VSlider", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--.int4("Slide") = *((int*) &param);
	m--.int4("Height") = 0;
}

unknown OnNotify(string message, unknown param)
{
	if(StrCmp(message, "SetSlide") == same)
		m--.int4("Slide") = *((int*) &param);
	return nullptr;
}

string OnTouch(BxPanel::Touch type, int x, int y)
{
	if(type == BxPanel::tchDrag || type == BxPanel::tchDrop)
		return "";

	const int Height = m--.int4("Height") - 30;
	const int NewSlide = MinMax(0, 255 * (Height - y + 15) / Height, 255);
	if(m--.int4("Slide") != NewSlide)
	{
		m--.int4("Slide") = NewSlide;
		m--.UpdateEvent(BxString("<>:Slide:<A>", BxARG(NewSlide)));
	}

	if(type == BxPanel::tchUpIn || type == BxPanel::tchUpOut)
		m--.UpdateEvent(BxString("<>:Changed:<A>", BxARG(m--.int4("Slide"))));
	return "";
}

void OnPaint(rect& r, int x1, int y1, int x2, int y2)
{
	BxDraw& Draw = m--.GetDraw();
	BxTRY(Draw, CLIP(r = XYXY(x1, y1, x2, y2)))
	{
		m--.UpdateRect();
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(255, 255, 255));
		const point Center = Draw.CurrentCenter();
		const int SliderSize = (Draw.Height() - 20) / 2;
		const int SliderPos = (SliderSize - 5) * (255 - m--.int4("Slide") * 2) / 255;
		Draw.Rectangle(FILL, XYRR(Center.x, Center.y, 5, SliderSize), COLOR(192, 192, 192));
		Draw.Rectangle(FILL, XYRR(Center.x, Center.y + SliderPos, 10, 5), COLOR(192, 0, 0) >> OPACITY(128));
		m--.int4("Height") = Draw.Height();
	}
}
