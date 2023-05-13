/**************************************************************************/
/*  display_server_widget.cpp                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "display_server_widget_windows.h"
#include "gdi_window.h"
#include "main/main.h"
#include "key_mapping_windows.h"
#if defined(GLES3_ENABLED)
#include "drivers/gles3/rasterizer_gles3.h"
#endif
bool has_change = false;
int has_change_width = 1;
int has_change_height = 1;
void DisplayServerWidgetWindows::resize(int p_width, int p_height)
{
	has_change = true;
	has_change_width = p_width;
	has_change_height = p_height;
//  _THREAD_SAFE_METHOD_
//#if defined(VULKAN_ENABLED)
//	if (context_vulkan) {
//		// Note: Trigger resize event to update swapchains when window is minimized/restored, even if size is not changed.
//		context_vulkan->window_resize(0, has_change_width, has_change_height);
//	}
//#endif

}

bool DisplayServerWidgetWindows::has_feature(Feature p_feature) const {
	switch (p_feature) {
	case FEATURE_SUBWINDOWS:
	case FEATURE_TOUCHSCREEN:
	case FEATURE_MOUSE:
	case FEATURE_MOUSE_WARP:
	case FEATURE_CLIPBOARD:
	case FEATURE_CURSOR_SHAPE:
	case FEATURE_CUSTOM_CURSOR_SHAPE:
	case FEATURE_IME:
	case FEATURE_WINDOW_TRANSPARENCY:
	case FEATURE_HIDPI:
	case FEATURE_ICON:
	case FEATURE_NATIVE_ICON:
	case FEATURE_SWAP_BUFFERS:
	case FEATURE_KEEP_SCREEN_ON:
	case FEATURE_TEXT_TO_SPEECH:
		return true;
	default:
		return false;
	}
}

String DisplayServerWidgetWindows::get_name() const {
	return "Widget";
}

void DisplayServerWidgetWindows::mouse_set_mode(MouseMode p_mode) {
	QtEventServer::get_singleton()->mouse_set_mode(p_mode);
}

Point2i DisplayServerWidgetWindows::mouse_get_position() const {
	return QtEventServer::get_singleton()->mouse_get_position();
}

BitField<MouseButtonMask> DisplayServerWidgetWindows::mouse_get_button_state() const {
	return QtEventServer::get_singleton()->mouse_get_button_state();
}

// AAAA
int DisplayServerWidgetWindows::get_screen_count() const {
	return 1;
}

int DisplayServerWidgetWindows::get_primary_screen() const {
	return 0;
}

Point2i DisplayServerWidgetWindows::screen_get_position(int p_screen) const {
	return Point2i();
	//return Point2i(geometry().x(), geometry().y());
}

Size2i DisplayServerWidgetWindows::screen_get_size(int p_screen) const {
	return Size2i();
	//return Size2i(geometry().width(), geometry().height());
}

Rect2i DisplayServerWidgetWindows::screen_get_usable_rect(int p_screen) const {
	return Rect2i();
}

// AAA
int DisplayServerWidgetWindows::screen_get_dpi(int p_screen) const {
	return 99;
}

// AAA
float DisplayServerWidgetWindows::screen_get_refresh_rate(int p_screen) const {
	return 60.0f;
}

Vector<DisplayServer::WindowID> DisplayServerWidgetWindows::get_window_list() const {
	Vector<DisplayServer::WindowID> ret;
	// AAA
	return ret;
}

DisplayServer::WindowID DisplayServerWidgetWindows::create_sub_window(WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Rect2i& p_rect)
{
	return -1;
}

void DisplayServerWidgetWindows::show_window(WindowID p_window) {
	QtEventServer::get_singleton()->get_window(MAIN_WINDOW_ID)->show_window();
}

void DisplayServerWidgetWindows::window_attach_instance_id(ObjectID p_instance, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	//_windows[p_window].instance_id = p_instance;
}

ObjectID DisplayServerWidgetWindows::window_get_attached_instance_id(WindowID p_window) const {
	// AAA
	ObjectID instance_id;
	return instance_id;
}

void DisplayServerWidgetWindows::window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->set_rect_changed_callback(p_callable);
}

void DisplayServerWidgetWindows::window_set_window_event_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->set_window_event_callback(p_callable);
}

void DisplayServerWidgetWindows::window_set_input_event_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->set_input_event_callback(p_callable);
}

void DisplayServerWidgetWindows::window_set_input_text_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->set_input_text_callback(p_callable);
}

void DisplayServerWidgetWindows::window_set_drop_files_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->set_drop_files_callback(p_callable);
}

void DisplayServerWidgetWindows::window_set_title(const String &p_title, WindowID p_window) {
}

int DisplayServerWidgetWindows::window_get_current_screen(WindowID p_window) const {
	return 0;
}

void DisplayServerWidgetWindows::window_set_current_screen(int p_screen, WindowID p_window) {
}

Point2i DisplayServerWidgetWindows::window_get_position(WindowID p_window) const {
	return Point2i();
}

Point2i DisplayServerWidgetWindows::window_get_position_with_decorations(WindowID p_window) const {
	return Point2i();
}

void DisplayServerWidgetWindows::window_set_position(const Point2i &p_position, WindowID p_window) {
}

DisplayServer::WindowID DisplayServerWidgetWindows::get_window_at_screen_position(const Point2i &p_position) const {
	WindowID id = 0;
	return id;
}

void DisplayServerWidgetWindows::window_set_transient(WindowID p_window, WindowID p_parent) {
}

void DisplayServerWidgetWindows::window_set_max_size(const Size2i p_size, WindowID p_window) {
}

Size2i DisplayServerWidgetWindows::window_get_max_size(WindowID p_window) const {
	Size2i max_size;
	return max_size;
}

void DisplayServerWidgetWindows::window_set_min_size(const Size2i p_size, WindowID p_window) {
}

Size2i DisplayServerWidgetWindows::window_get_min_size(WindowID p_window) const {
	Size2i min_size;
	return min_size;
}


void DisplayServerWidgetWindows::_send_window_event(const WindowData& wd, WindowEvent p_event) {
	if (!wd.event_callback.is_null()) {
		Variant event = int(p_event);
		Variant *eventp = &event;
		Variant ret;
		Callable::CallError ce;
		wd.event_callback.callp((const Variant **)&eventp, 1, ret, ce);
	}
}

void DisplayServerWidgetWindows::_set_mouse_mode_impl(MouseMode p_mode)
{
}

void DisplayServerWidgetWindows::_dispatch_input_events(const Ref<InputEvent> &p_event) {
	static_cast<DisplayServerWidgetWindows *>(get_singleton())->_dispatch_input_event(p_event);
}

void DisplayServerWidgetWindows::_dispatch_input_event(const Ref<InputEvent> &p_event) {

	QtEventServer::get_singleton()->dispatch_input_event(p_event);
}

void DisplayServerWidgetWindows::window_set_size(const Size2i p_size, WindowID p_window) {
	// AAA
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->set_size(p_size);
	int w = p_size.width;
	int h = p_size.height;

#if defined(VULKAN_ENABLED)
	if (context_vulkan) {
		context_vulkan->window_resize(p_window, w, h);
	}
#endif
#if defined(GLES3_ENABLED)
	if (gl_manager) {
		gl_manager->window_resize(p_window, w, h);
	}
#endif
}

Size2i DisplayServerWidgetWindows::window_get_size(WindowID p_window) const {
	ERR_FAIL_COND_V(!QtEventServer::get_singleton()->get_window(p_window), Size2());
	return QtEventServer::get_singleton()->get_window(p_window)->get_size();
}

Size2i DisplayServerWidgetWindows::window_get_size_with_decorations(WindowID p_window) const {
	return Size2();
}

void DisplayServerWidgetWindows::window_set_mode(WindowMode p_mode, WindowID p_window) {
	ERR_FAIL_COND(!QtEventServer::get_singleton()->get_window(p_window));
	QtEventServer::get_singleton()->get_window(p_window)->show_window();
}

DisplayServer::WindowMode DisplayServerWidgetWindows::window_get_mode(WindowID p_window) const {
	return WINDOW_MODE_WINDOWED;
}

bool DisplayServerWidgetWindows::window_is_maximize_allowed(WindowID p_window) const {
	// FIXME: Implement this, or confirm that it should always be true.
	return true;
}

void DisplayServerWidgetWindows::window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window) {
	int a = 0;
	printf("::%d\n", a);
}

bool DisplayServerWidgetWindows::window_get_flag(WindowFlags p_flag, WindowID p_window) const {
	return false;
}

void DisplayServerWidgetWindows::window_request_attention(WindowID p_window) {
}

void DisplayServerWidgetWindows::window_move_to_foreground(WindowID p_window) {
}

bool DisplayServerWidgetWindows::window_can_draw(WindowID p_window) const {
	return true;
}

bool DisplayServerWidgetWindows::can_any_window_draw() const {
	return true;
}

void DisplayServerWidgetWindows::cursor_set_shape(CursorShape p_shape)
{
	QtEventServer::get_singleton()->cursor_set_shape(p_shape);
}

DisplayServer::CursorShape DisplayServerWidgetWindows::cursor_get_shape() const
{
	return QtEventServer::get_singleton()->cursor_get_shape();
}

void DisplayServerWidgetWindows::window_set_vsync_mode(DisplayServer::VSyncMode p_vsync_mode, WindowID p_window)
{
#if defined(VULKAN_ENABLED)
		if (context_vulkan) {
			context_vulkan->set_vsync_mode(p_window, p_vsync_mode);
		}
#endif
#if defined(GLES3_ENABLED)
	if (gl_manager) {
		gl_manager->set_use_vsync(p_window, p_vsync_mode != DisplayServer::VSYNC_DISABLED);
	}
#endif
}

DisplayServer::VSyncMode DisplayServerWidgetWindows::window_get_vsync_mode(WindowID p_window) const
{
#if defined(VULKAN_ENABLED)
		if (context_vulkan) {
			return context_vulkan->get_vsync_mode(p_window);
		}
#endif
#if defined(GLES3_ENABLED)
	if (gl_manager) {
		return gl_manager->is_using_vsync(p_window) ? DisplayServer::VSYNC_ENABLED : DisplayServer::VSYNC_DISABLED;
	}
#endif
	return DisplayServer::VSYNC_ENABLED;
}

bool DisplayServerWidgetWindows::get_swap_cancel_ok() {
	return true;
}

void DisplayServerWidgetWindows::process_events() {

	if (!main_loop_valid) {
		main_loop_valid = true;
	}
	if (has_change) {

#if defined(VULKAN_ENABLED)
		if (context_vulkan) {
			// Note: Trigger resize event to update swapchains when window is minimized/restored, even if size is not changed.
			context_vulkan->window_resize(0, has_change_width, has_change_height);
		}
#endif
		has_change = false;
	}

	QtEventServer::get_singleton()->process_key_events();
	Input::get_singleton()->flush_buffered_events();
}


void DisplayServerWidgetWindows::process_events_end() {
	if (!has_change) {
		QtSyncGengine::get_singleton()->wake();
	}
}


Vector<String> DisplayServerWidgetWindows::get_rendering_drivers_func() {
	Vector<String> drivers;
#ifdef VULKAN_ENABLED
	drivers.push_back("vulkan");
#endif
#ifdef GLES3_ENABLED
	drivers.push_back("opengl3");
#endif
	return drivers;
}

DisplayServer *DisplayServerWidgetWindows::create_func(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error) {
	DisplayServer *ds = new DisplayServerWidgetWindows(p_rendering_driver, p_mode, p_vsync_mode, p_flags, p_position, p_resolution, p_screen, r_error);
	if (r_error != OK) {
		if (p_rendering_driver == "vulkan") {
			String executable_name = OS::get_singleton()->get_executable_path().get_file();
			OS::get_singleton()->alert(
					vformat("Your video card drivers seem not to support the required Vulkan version.\n\n"
							"If possible, consider updating your video card drivers or using the OpenGL 3 driver.\n\n"
							"You can enable the OpenGL 3 driver by starting the engine from the\n"
							"command line with the command:\n\n    \"%s\" --rendering-driver opengl3\n\n"
							"If you have recently updated your video card drivers, try rebooting.",
							executable_name),
					"Unable to initialize Vulkan video driver");
		} else {
			OS::get_singleton()->alert(
					"Your video card drivers seem not to support the required OpenGL 3.3 version.\n\n"
					"If possible, consider updating your video card drivers.\n\n"
					"If you have recently updated your video card drivers, try rebooting.",
					"Unable to initialize OpenGL video driver");
		}
	}
	return ds;
}

void DisplayServerWidgetWindows::register_widget_driver() {
	register_create_function("widget", create_func, get_rendering_drivers_func);
}

typedef enum _SHC_PROCESS_DPI_AWARENESS {
	SHC_PROCESS_DPI_UNAWARE = 0,
	SHC_PROCESS_SYSTEM_DPI_AWARE = 1,
	SHC_PROCESS_PER_MONITOR_DPI_AWARE = 2
} SHC_PROCESS_DPI_AWARENESS;

DisplayServerWidgetWindows::DisplayServerWidgetWindows(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error)
{
	GdiWindow* main_win = QtEventServer::get_singleton()->get_window(MAIN_WINDOW_ID);
	HWND winId = (HWND)main_win->winId();
	int width = main_win->width();
	int height = main_win->height();
	main_win->connect("resize", callable_mp(this, &DisplayServerWidgetWindows::resize));

	rendering_driver = p_rendering_driver;
	if (OS::get_singleton()->is_hidpi_allowed()) {
		HMODULE Shcore = LoadLibraryW(L"Shcore.dll");
		if (Shcore != nullptr) {
			typedef HRESULT(WINAPI * SetProcessDpiAwareness_t)(SHC_PROCESS_DPI_AWARENESS);
			SetProcessDpiAwareness_t SetProcessDpiAwareness = (SetProcessDpiAwareness_t)GetProcAddress(Shcore, "SetProcessDpiAwareness");
			if (SetProcessDpiAwareness) {
				SetProcessDpiAwareness(SHC_PROCESS_SYSTEM_DPI_AWARE);
			}
		}
	}

	WindowData wd;
	if (p_mode == WINDOW_MODE_FULLSCREEN || p_mode == WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
		wd.fullscreen = true;
		if (p_mode == WINDOW_MODE_FULLSCREEN) {
			wd.multiwindow_fs = true;
		}
	}
	if (p_mode != WINDOW_MODE_FULLSCREEN && p_mode != WINDOW_MODE_EXCLUSIVE_FULLSCREEN) {
		wd.pre_fs_valid = true;
	}
	if (p_mode == WINDOW_MODE_MAXIMIZED) {
		wd.maximized = true;
		wd.minimized = false;
	}
	if (p_mode == WINDOW_MODE_MINIMIZED) {
		wd.maximized = false;
		wd.minimized = true;
	}
	wd.last_pressure = 0;
	wd.last_pressure_update = 0;
	wd.last_tilt = Vector2();
	//wd.last_pos = p_rect.position;
	wd.width = width;
	wd.height = height;

#if defined(VULKAN_ENABLED)
	if (rendering_driver == "vulkan") {
		context_vulkan = memnew(VulkanContextWindows);
		//if (context_vulkan->initialize() != OK) {
			memdelete(context_vulkan);
			context_vulkan = nullptr;
			r_error = ERR_UNAVAILABLE;
			return;
		}
	}
#endif
	// Init context and rendering device
#if defined(GLES3_ENABLED)
	if (rendering_driver == "opengl3") {
		GLManager_Windows::ContextType opengl_api_type = GLManager_Windows::GLES_3_0_COMPATIBLE;
		gl_manager = memnew(GLManager_Windows(opengl_api_type));
		if (gl_manager->initialize() != OK) {
			memdelete(gl_manager);
			gl_manager = nullptr;
			r_error = ERR_UNAVAILABLE;
			return;
		}
		RasterizerGLES3::make_current();
	}
#endif
#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		if (context_vulkan->window_create(0, DisplayServer::VSYNC_ENABLED, winId, 0, width, height) != OK) {
			memdelete(context_vulkan);
			context_vulkan = nullptr;
			ERR_PRINT("Failed to create Vulkan Window.");
		}
		wd.context_created = true;
		context_vulkan->set_vsync_mode(0, DisplayServer::VSYNC_ENABLED);
	}
#endif
#ifdef GLES3_ENABLED
	if (gl_manager) {
		if (gl_manager->window_create(0, winId, 0, width, height) != OK) {
			memdelete(gl_manager);
			gl_manager = nullptr;
			ERR_PRINT("Failed to create an OpenGL window.");
		}
		window_set_vsync_mode(p_vsync_mode, 0);
	}
#endif
#if defined(VULKAN_ENABLED)
	if (rendering_driver == "vulkan") {
		rendering_device_vulkan = memnew(RenderingDeviceVulkan);
		rendering_device_vulkan->initialize(context_vulkan);
		RendererCompositorRD::make_current();
	}
#endif
	r_error = OK;
	
	Input::get_singleton()->set_event_dispatch_function(_dispatch_input_events);
}

DisplayServerWidgetWindows::~DisplayServerWidgetWindows() {
#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		context_vulkan->window_destroy(MAIN_WINDOW_ID);
	}
#endif
#if defined(VULKAN_ENABLED)
	if (rendering_device_vulkan) {
		rendering_device_vulkan->finalize();
		memdelete(rendering_device_vulkan);
		rendering_device_vulkan = nullptr;
	}
	if (context_vulkan) {
		memdelete(context_vulkan);
		context_vulkan = nullptr;
	}
#endif
#ifdef GLES3_ENABLED
	if (gl_manager) {
		memdelete(gl_manager);
		gl_manager = nullptr;
	}
#endif
}

