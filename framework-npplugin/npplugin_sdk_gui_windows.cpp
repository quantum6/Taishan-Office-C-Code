#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <process.h>

#include "fw_kit.h"
#include "fw_config.h"
#include "fw_log.h"

#include "npplugin_sdk_platform.h"
#include "npplugin_app.h"

static int g_iPluginCounter = 0;

void SetSubclass(HWND hWnd, InstanceData* instanceData);
void ClearSubclass(HWND hWnd);
LRESULT CALLBACK PluginWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

struct _PlatformData {
  HWND childWindow;
};

bool
pluginSupportsWindowMode()
{
  return true;
}

bool
pluginSupportsWindowlessMode()
{
  return false;
}

NPError
pluginInstanceInit(InstanceData* instanceData)
{
  instanceData->platformData = static_cast<PlatformData*>
    (NPN_MemAlloc(sizeof(PlatformData)));
  if (!instanceData->platformData)
    return NPERR_OUT_OF_MEMORY_ERROR;

  instanceData->platformData->childWindow = NULL;

    g_iPluginCounter ++;
    NPPluginApp* pApp = new NPPluginApp(g_iPluginCounter);

	instanceData->pNPPluginApp = pApp;
	pApp->start();

  return NPERR_NO_ERROR;
}

void
pluginInstanceShutdown(InstanceData* instanceData)
{
    if (!instanceData)
	{
        return;
	}
    NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
	if (!pApp || !(pApp->isValid()))
	{
		return;
	}
	delete instanceData->pNPPluginApp;
	instanceData->pNPPluginApp = NULL;

	NPN_MemFree(instanceData->platformData);
	instanceData->platformData = NULL;
}

void
pluginDoSetWindow(InstanceData* instanceData, NPWindow* newWindow)
{
  instanceData->window = *newWindow;
}

#define CHILD_WIDGET_SIZE 10

void
pluginWidgetInit(InstanceData* instanceData, void* oldWindow)
{
  if (!instanceData)
  {
    return;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return;
  }

  HWND hWnd = (HWND)instanceData->window.window;
  if (oldWindow) {
    // chrashtests/539897-1.html excercises this code
    HWND hWndOld = (HWND)oldWindow;
    ClearSubclass(hWndOld);
    if (instanceData->platformData->childWindow) {
      ::DestroyWindow(instanceData->platformData->childWindow);
    }
  }

  SetSubclass(hWnd, instanceData);

  instanceData->platformData->childWindow =
    ::CreateWindowW(L"SCROLLBAR", L"Dummy child window", 
                    WS_CHILD, 0, 0, CHILD_WIDGET_SIZE, CHILD_WIDGET_SIZE, hWnd, NULL,
                    NULL, NULL);
}

static void
drawToDC(InstanceData* instanceData, HDC dc,
         int x, int y, int width, int height)
{
  if (!instanceData)
  {
    return;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
		return;
  }

  pApp->window((int)((HWND)instanceData->window.window),
        x, y, width, height);
    //plugin_refresh(instanceData->pJNIEnv, instanceData->tioclass, instanceData->tsoffice);
}

void
pluginDraw(InstanceData* instanceData)
{
  if (!instanceData)
  {
    return;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return;
  }

  NPP npp = instanceData->npp;
  if (!npp)
    return;

  HDC hdc = NULL;
  PAINTSTRUCT ps;

  notifyDidPaint(instanceData);

  if (instanceData->hasWidget)
    hdc = ::BeginPaint((HWND)instanceData->window.window, &ps);
  else
    hdc = (HDC)instanceData->window.window;

  if (hdc == NULL)
    return;

  // Push the browser's hdc on the resource stack. If this test plugin is windowless,
  // we share the drawing surface with the rest of the browser.
  int savedDCID = SaveDC(hdc);

  // When we have a widget, window.x/y are meaningless since our widget
  // is always positioned correctly and we just draw into it at 0,0.
  int x = instanceData->hasWidget ? 0 : instanceData->window.x;
  int y = instanceData->hasWidget ? 0 : instanceData->window.y;
  int width = instanceData->window.width;
  int height = instanceData->window.height;
  drawToDC(instanceData, hdc, x, y, width, height);

  // Pop our hdc changes off the resource stack
  RestoreDC(hdc, savedDCID);

  if (instanceData->hasWidget)
    ::EndPaint((HWND)instanceData->window.window, &ps);
}

/* script interface */

int32_t
pluginGetEdge(InstanceData* instanceData, RectEdge edge)
{
  if (!instanceData)
  {
    return NPTEST_INT32_ERROR;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return NPTEST_INT32_ERROR;
  }

  // Get the plugin client rect in screen coordinates
  RECT rect = {0};
  if (!::GetClientRect((HWND)instanceData->window.window, &rect))
    return NPTEST_INT32_ERROR;
  ::MapWindowPoints((HWND)instanceData->window.window, NULL, (LPPOINT)&rect, 2);

  // Get the toplevel window frame rect in screen coordinates
  HWND rootWnd = ::GetAncestor((HWND)instanceData->window.window, GA_ROOT);
  if (!rootWnd)
    return NPTEST_INT32_ERROR;
  RECT rootRect;
  if (!::GetWindowRect(rootWnd, &rootRect))
    return NPTEST_INT32_ERROR;

  switch (edge) {
  case EDGE_LEFT:
    return rect.left - rootRect.left;
  case EDGE_TOP:
    return rect.top - rootRect.top;
  case EDGE_RIGHT:
    return rect.right - rootRect.left;
  case EDGE_BOTTOM:
    return rect.bottom - rootRect.top;
  }

  return NPTEST_INT32_ERROR;
}

static BOOL
getWindowRegion(HWND wnd, HRGN rgn)
{
  if (::GetWindowRgn(wnd, rgn) != ERROR)
    return TRUE;

  RECT clientRect;
  if (!::GetClientRect(wnd, &clientRect))
    return FALSE;
  return ::SetRectRgn(rgn, 0, 0, clientRect.right, clientRect.bottom);
}

static RGNDATA*
computeClipRegion(InstanceData* instanceData)
{
  if (!instanceData)
  {
    return NULL;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return NULL;
  }

  HWND wnd = (HWND)instanceData->window.window;
  HRGN rgn = ::CreateRectRgn(0, 0, 0, 0);
  if (!rgn)
    return NULL;
  HRGN ancestorRgn = ::CreateRectRgn(0, 0, 0, 0);
  if (!ancestorRgn) {
    ::DeleteObject(rgn);
    return NULL;
  }
  if (!getWindowRegion(wnd, rgn)) {
    ::DeleteObject(ancestorRgn);
    ::DeleteObject(rgn);
    return NULL;
  }

  HWND ancestor = wnd;
  for (;;) {
    ancestor = ::GetAncestor(ancestor, GA_PARENT);
    if (!ancestor || ancestor == ::GetDesktopWindow()) {
      ::DeleteObject(ancestorRgn);

      DWORD size = ::GetRegionData(rgn, 0, NULL);
      if (!size) {
        ::DeleteObject(rgn);
        return NULL;
      }

      HANDLE heap = ::GetProcessHeap();
      RGNDATA* data = static_cast<RGNDATA*>(::HeapAlloc(heap, 0, size));
      if (!data) {
        ::DeleteObject(rgn);
        return NULL;
      }
      DWORD result = ::GetRegionData(rgn, size, data);
      ::DeleteObject(rgn);
      if (!result) {
        ::HeapFree(heap, 0, data);
        return NULL;
      }

      return data;
    }

    if (!getWindowRegion(ancestor, ancestorRgn)) {
      ::DeleteObject(ancestorRgn);
      ::DeleteObject(rgn);
      return 0;
    }

    POINT pt = { 0, 0 };
    ::MapWindowPoints(ancestor, wnd, &pt, 1);
    if (::OffsetRgn(ancestorRgn, pt.x, pt.y) == ERROR ||
        ::CombineRgn(rgn, rgn, ancestorRgn, RGN_AND) == ERROR) {
      ::DeleteObject(ancestorRgn);
      ::DeleteObject(rgn);
      return 0;
    }
  }
}

int32_t
pluginGetClipRegionRectCount(InstanceData* instanceData)
{
  if (!instanceData)
  {
    return NPTEST_INT32_ERROR;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return NPTEST_INT32_ERROR;
  }

  RGNDATA* data = computeClipRegion(instanceData);
  if (!data)
    return NPTEST_INT32_ERROR;

  int32_t result = data->rdh.nCount;
  ::HeapFree(::GetProcessHeap(), 0, data);
  return result;
}

static int32_t
addOffset(LONG coord, int32_t offset)
{
  if (offset == NPTEST_INT32_ERROR)
    return NPTEST_INT32_ERROR;
  return coord + offset;
}

int32_t
pluginGetClipRegionRectEdge(InstanceData* instanceData, 
    int32_t rectIndex, RectEdge edge)
{
  if (!instanceData)
  {
    return NPTEST_INT32_ERROR;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return NPTEST_INT32_ERROR;
  }

  RGNDATA* data = computeClipRegion(instanceData);
  if (!data)
    return NPTEST_INT32_ERROR;

  HANDLE heap = ::GetProcessHeap();
  if (rectIndex >= int32_t(data->rdh.nCount)) {
    ::HeapFree(heap, 0, data);
    return NPTEST_INT32_ERROR;
  }

  RECT rect = reinterpret_cast<RECT*>(data->Buffer)[rectIndex];
  ::HeapFree(heap, 0, data);

  switch (edge) {
  case EDGE_LEFT:
    return addOffset(rect.left,   pluginGetEdge(instanceData, EDGE_LEFT));
  case EDGE_TOP:
    return addOffset(rect.top,    pluginGetEdge(instanceData, EDGE_TOP));
  case EDGE_RIGHT:
    return addOffset(rect.right,  pluginGetEdge(instanceData, EDGE_LEFT));
  case EDGE_BOTTOM:
    return addOffset(rect.bottom, pluginGetEdge(instanceData, EDGE_TOP));
  }

  return NPTEST_INT32_ERROR;
}

/* windowless plugin events */

static bool
handleEventInternal(InstanceData* instanceData, NPEvent* pe, LRESULT* result)
{
  if (!instanceData)
  {
    return false;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return false;
  }

  switch ((UINT)pe->event) {
    case WM_PAINT:
      pluginDraw(instanceData);
      return true;

    case WM_MOUSEACTIVATE:
      if (instanceData->hasWidget) {
        ::SetFocus((HWND)instanceData->window.window);
        *result = MA_ACTIVATEANDEAT;
        return true;
      }
      return false;

    case WM_MOUSEWHEEL:
      return true;

    case WM_WINDOWPOSCHANGED: {
      WINDOWPOS* pPos = (WINDOWPOS*)pe->lParam;
      instanceData->winX = instanceData->winY = 0;
      if (pPos) {
        instanceData->winX = pPos->x;
        instanceData->winY = pPos->y;
        return true;
      }
      return false;
    }

    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: {
      int x = instanceData->hasWidget ? 0 : instanceData->winX;
      int y = instanceData->hasWidget ? 0 : instanceData->winY;
      instanceData->lastMouseX = GET_X_LPARAM(pe->lParam) - x;
      instanceData->lastMouseY = GET_Y_LPARAM(pe->lParam) - y;
      return true;
    }

    default:
      return false;
  }
}

int16_t
pluginHandleEvent(InstanceData* instanceData, void* event)
{
  if (!instanceData)
  {
    return 0;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return 0;
  }

  NPEvent* pe = (NPEvent*)event;

  if (pe == NULL || instanceData == NULL ||
      instanceData->window.type != NPWindowTypeDrawable)
    return 0;   

  LRESULT result = 0;
  return handleEventInternal(instanceData, pe, &result);
}

/* windowed plugin events */

LRESULT CALLBACK PluginWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC wndProc = (WNDPROC)GetProp(hWnd, LPCWSTR("MozillaWndProc"));
  if (!wndProc)
    return 0;

  InstanceData* pInstance = (InstanceData*)GetProp(hWnd, LPCWSTR("InstanceData"));
  if (!pInstance)
  {
    return 0;
  }
  NPPluginApp* pApp = (NPPluginApp*)(pInstance->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return 0;
  }

  NPEvent event = { uMsg, wParam, lParam };

  LRESULT result = 0;
  if (handleEventInternal(pInstance, &event, &result))
    return result;

  if (uMsg == WM_CLOSE) {
    ClearSubclass((HWND)pInstance->window.window);
  }

  return CallWindowProc(wndProc, hWnd, uMsg, wParam, lParam);
}

void
ClearSubclass(HWND hWnd)
{
  if (GetProp(hWnd, LPCWSTR("MozillaWndProc"))) {
    ::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)GetProp(hWnd, LPCWSTR("MozillaWndProc")));
    RemoveProp(hWnd, LPCWSTR("MozillaWndProc"));
    RemoveProp(hWnd, LPCWSTR("InstanceData"));
  }
}

void
SetSubclass(HWND hWnd, InstanceData* instanceData)
{
  if (!instanceData)
  {
    return;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return;
  }

  // Subclass the plugin window so we can handle our own windows events.
  SetProp(hWnd, LPCWSTR("InstanceData"),   (HANDLE)instanceData);
  WNDPROC origProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)PluginWndProc);
  SetProp(hWnd, LPCWSTR("MozillaWndProc"), (HANDLE)origProc);
}

static void checkEquals(int a, int b, const char* msg, string& error)
{
  if (a == b) {
    return;
  }

  error.append(msg);
  char buf[100];
  sprintf(buf, " (got %d, expected %d)\n", a, b);
  error.append(buf);
}

void pluginDoInternalConsistencyCheck(InstanceData* instanceData, string& error)
{
  if (!instanceData)
  {
    return;
  }
  NPPluginApp* pApp = (NPPluginApp*)(instanceData->pNPPluginApp);
  if (!pApp || !(pApp->isValid()))
  {
    return;
  }

  if (instanceData->platformData->childWindow) {
    RECT childRect;
    ::GetWindowRect(instanceData->platformData->childWindow, &childRect);
    RECT ourRect;
    HWND hWnd = (HWND)instanceData->window.window;
    ::GetWindowRect(hWnd, &ourRect);
    checkEquals(childRect.left,   ourRect.left, "Child widget left", error);
    checkEquals(childRect.top,    ourRect.top,  "Child widget top",  error);
    checkEquals(childRect.right,  childRect.left + CHILD_WIDGET_SIZE, "Child widget width",  error);
    checkEquals(childRect.bottom, childRect.top  + CHILD_WIDGET_SIZE, "Child widget height", error);
  }
}
