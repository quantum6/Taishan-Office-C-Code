
#ifndef __NPPLUGIN_SDK_PLATFORM_H__
#define __NPPLUGIN_SDK_PLATFORM_H__

#include "npplugin_sdk.h"

/**
 * Returns true if the plugin supports windowed mode
 */
bool    pluginSupportsWindowMode();

/**
 * Returns true if the plugin supports windowless mode. At least one of
 * "pluginSupportsWindowMode" and "pluginSupportsWindowlessMode" must
 * return true.
 */
bool    pluginSupportsWindowlessMode();

/**
 * Initialize the plugin instance. Returning an error here will cause the
 * plugin instantiation to fail.
 */
NPError pluginInstanceInit(InstanceData* instanceData);

/**
 * Shutdown the plugin instance.
 */
void    pluginInstanceShutdown(InstanceData* instanceData);

/**
 * Set the instanceData's window to newWindow.
 */
void    pluginDoSetWindow(InstanceData* instanceData, NPWindow* newWindow);

/**
 * Initialize the window for a windowed plugin. oldWindow is the old
 * native window value. This will never be called for windowless plugins.
 */
void    pluginWidgetInit(InstanceData* instanceData, void* oldWindow);

/**
 * Handle an event for a windowless plugin. (Windowed plugins are
 * responsible for listening for their own events.)
 */
int16_t pluginHandleEvent(InstanceData* instanceData, void* event);

enum RectEdge {
  EDGE_LEFT = 0,
  EDGE_TOP = 1,
  EDGE_RIGHT = 2,
  EDGE_BOTTOM = 3
};

enum {
  NPTEST_INT32_ERROR = 0x7FFFFFFF
};

/**
 * Return the coordinate of the given edge of the plugin's area, relative
 * to the top-left corner of the toplevel window containing the plugin,
 * including window decorations. Only works for window-mode plugins
 * and Mac plugins.
 * Returns NPTEST_ERROR on error.
 */
int32_t pluginGetEdge(InstanceData* instanceData, RectEdge edge);

/**
 * Return the number of rectangles in the plugin's clip region. Only
 * works for window-mode plugins and Mac plugins.
 * Returns NPTEST_ERROR on error.
 */
int32_t pluginGetClipRegionRectCount(InstanceData* instanceData);

/**
 * Return the coordinate of the given edge of a rectangle in the plugin's
 * clip region, relative to the top-left corner of the toplevel window
 * containing the plugin, including window decorations. Only works for
 * window-mode plugins and Mac plugins.
 * Returns NPTEST_ERROR on error.
 */
int32_t pluginGetClipRegionRectEdge(InstanceData* instanceData, 
    int32_t rectIndex, RectEdge edge);

/**
 * Check that the platform-specific plugin state is internally consistent.
 * Just return if everything is OK, otherwise append error messages
 * to 'error' separated by \n.
 */
void pluginDoInternalConsistencyCheck(InstanceData* instanceData, std::string& error);

/**
 * Get the current clipboard item as text.  If the clipboard item
 * isn't text, the returned value is undefined.
 */
std::string pluginGetClipboardText(InstanceData* instanceData);

/**
 * Crash while in a nested event loop.  The goal is to catch the
 * browser processing the XPCOM event generated from the plugin's
 * crash while other plugin code is still on the stack. 
 * See https://bugzilla.mozilla.org/show_bug.cgi?id=550026.
 */
bool pluginCrashInNestedLoop(InstanceData* instanceData);

/**
 * Destroy gfx things that might be shared with the parent process
 * when we're run out-of-process.  It's not expected that this
 * function will be called when the test plugin is loaded in-process,
 * and bad things will happen if it is called.
 *
 * This call leaves the plugin subprocess in an undefined state.  It
 * must not be used after this call or weird things will happen.
 */
bool pluginDestroySharedGfxStuff(InstanceData* instanceData);

#endif // __NPPLUGIN_SDK_PLATFORM_H__
