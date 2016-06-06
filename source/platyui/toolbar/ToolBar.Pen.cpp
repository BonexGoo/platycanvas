#include <BxPanel.hpp>

FRAMEWORK_PANEL("platycanvas.ToolBar.Pen", XYXY)

void OnCreate(string option, unknown param, const bool first)
{
	m--("Stroke").Bind("platycanvas.ToolItem.Stroke");
	m--("Color").Bind("platycanvas.ToolItem.Color");

	m--("StrokeLabel").Bind("platycanvas.ToolLabel", "skin/toolbar/text_stroke.png", 0);
	m--("ColorLabel").Bind("platycanvas.ToolLabel", "skin/toolbar/text_color.png", 0);
	m--("ToolLabel").Bind("platycanvas.ToolLabel", "skin/toolbar/text_tool.png", 0);

	m--("PenButton").Bind("platycanvas.ToolButton", "skin/toolbar/button_pen.png");
	m--("EraserButton").Bind("platycanvas.ToolButton", "skin/toolbar/button_eraser.png");
	m--("HandButton").Bind("platycanvas.ToolButton", "skin/toolbar/button_hand.png");
	m--("SpoidButton").Bind("platycanvas.ToolButton", "skin/toolbar/button_spoid.png");

	m--("Close").Bind("platycanvas.ToolClose", option, param);
}

unknown OnNotify(string message, unknown param)
{
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
		m--.UpdateRect();
		// 배경색
		Draw.Rectangle(FILL, Draw.CurrentRect(), COLOR(81, 81, 81));

		// Stroke
		BxTRY(Draw, CLIP(XYXY(0, 0, 300, Draw.Height())))
		{
			m--("StrokeLabel").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), 41));
			BxTRY(Draw, CLIP(XYXY(0, 40, Draw.Width(), Draw.Height())))
			{
				m--("Stroke").Paint(BxPanel::prmXYXY, BxARG(0, 1, Draw.Width(), Draw.Height()));
				Draw.Rectangle(FILL, XYWH(Draw.Width() - 2, 0, 2, Draw.Height()), COLOR(64, 64, 64));
			}
		}

		// Color
		BxTRY(Draw, CLIP(XYXY(300, 0, Draw.Width() - 130, Draw.Height())))
		{
			m--("ColorLabel").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), 41));
			BxTRY(Draw, CLIP(XYXY(0, 40, Draw.Width(), Draw.Height())))
			{
				m--("Color").Paint(BxPanel::prmXYXY, BxARG(0, 1, Draw.Width(), Draw.Height()));
				Draw.Rectangle(FILL, XYWH(Draw.Width() - 2, 0, 2, Draw.Height()), COLOR(64, 64, 64));
			}
		}

		// Tool
		BxTRY(Draw, CLIP(XYXY(Draw.Width() - 130, 0, Draw.Width(), Draw.Height())))
		{
			m--("ToolLabel").Paint(BxPanel::prmXYXY, BxARG(0, 0, Draw.Width(), 41));
			BxTRY(Draw, CLIP(XYXY(0, 40, Draw.Width(), Draw.Height())))
			{
				m--("PenButton").Paint(BxPanel::prmXYXY,
					BxARG(0, 1, Draw.Width() / 2, Draw.Height() / 2));
				m--("EraserButton").Paint(BxPanel::prmXYXY,
					BxARG(Draw.Width() / 2, 1, Draw.Width(), Draw.Height() / 2));
				m--("HandButton").Paint(BxPanel::prmXYXY,
					BxARG(0, Draw.Height() / 2, Draw.Width() / 2, Draw.Height()));
				m--("SpoidButton").Paint(BxPanel::prmXYXY,
					BxARG(Draw.Width() / 2, Draw.Height() / 2, Draw.Width(), Draw.Height()));
				Draw.Rectangle(FILL, XYWH(Draw.Width() / 2 - 1, 0, 2, Draw.Height()), COLOR(64, 64, 64));
				Draw.Rectangle(FILL, XYWH(0, Draw.Height() / 2 - 1, Draw.Width(), 2), COLOR(64, 64, 64));
			}
		}

		// Close
		m--("Close").Paint(BxPanel::prmXY, BxARG(60, 40 / 2));

		// 마감선
		Draw.Rectangle(FILL, XYWH(0, Draw.Height() - 1, Draw.Width(), 1), COLOR(36, 36, 36));
	}
}
