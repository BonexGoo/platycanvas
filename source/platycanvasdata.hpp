#include <BxScene.hpp>
#include <BxPanel.hpp>
#include "adapter.hpp"
#include "command.hpp"
#include "export.hpp"
#include "input.hpp"

class platycanvasdata
{
public:
	int ViewOx;
	int ViewOy;
	bool WacomDown;
	bool ScreenMode;
	bool IsZoomEnabled;
	float CursorX;
	float CursorY;
	float CursorD;
	BxImage CursorImage;
	bool IsZoomScaling;
	Point ZoomBegin;
	float OldCursorX;
	float OldCursorY;
	int CreateMin;
	int CreateRate;
	bool CreateCanvasMode;
	point CreateCanvasBegin;
	point CreateCanvasEnd;
	BxImage HandImage;
	BxImage SpoidImage;
	int HandAnimation;
	int SpoidAnimation;

	id_font TitleFont;
	// Pen
	bool IsOpenedPenTool;
	int PenToolPosPercent;
	BxPanel PenDoor;
	BxPanel PenTool;
	StrokeBy SavedStroke;
	// Layer
	bool IsOpenedLayerTool;
	int LayerToolPosPercent;
	BxPanel LayerDoor;
	BxPanel LayerTool;

	platycanvasdata();
	~platycanvasdata();

	void UpdateStroke(StrokeBy stroke);
	void AutoSaveAll(bool mustsave);
	void SetColor(byte r, byte g, byte b);
	void Resized();

	global_func void OnClicked_PenDoor(BxPanel* panel, string message, void* data);
	global_func void OnClicked_PenClose(BxPanel* panel, string message, void* data);
	global_func void OnClicked_LayerDoor(BxPanel* panel, string message, void* data);
	global_func void OnClicked_LayerClose(BxPanel* panel, string message, void* data);
	global_func void OnUpdated_Stroke(BxPanel* panel, string message, void* data);
    global_func void OnUpdated_Color(BxPanel* panel, string message, void* data);
	global_func void OnClicked_PenButton(BxPanel* panel, string message, void* data);
	global_func void OnClicked_EraserButton(BxPanel* panel, string message, void* data);
	global_func void OnClicked_HandButton(BxPanel* panel, string message, void* data);
	global_func void OnClicked_SpoidButton(BxPanel* panel, string message, void* data);
	global_func void OnFile(bool isdirname, string _tmp_ name, void* data1, void* data2);
};
