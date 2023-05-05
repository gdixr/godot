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

#include "main/main.h"

#if defined(GLES3_ENABLED)
#include "drivers/gles3/rasterizer_gles3.h"
#endif

#include <QResizeEvent>
#include <QMouseEvent>

bool DisplayServerWidgetWindows::has_feature(Feature p_feature) const {
	switch (p_feature) {
		case FEATURE_MOUSE:
			return true;
		default:
			return false;
	}
}

String DisplayServerWidgetWindows::get_name() const {
	return "Widget";
}

void DisplayServerWidgetWindows::mouse_set_mode(MouseMode p_mode) {
	// not called with small test
	if (p_mode == MOUSE_MODE_CAPTURED || p_mode == MOUSE_MODE_CONFINED || p_mode == MOUSE_MODE_CONFINED_HIDDEN) {
	
	}
}

Point2i DisplayServerWidgetWindows::mouse_get_position() const {

	QPoint p = mapFromGlobal(QCursor::pos());
	return Point2i(p.x(), p.y());
}

BitField<MouseButtonMask> DisplayServerWidgetWindows::mouse_get_button_state() const {
	return last_button_state;
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

void DisplayServerWidgetWindows::show_window(WindowID p_window) {
	// QWidget related show function
	if (isVisible()) {
		show();
	}
}

void DisplayServerWidgetWindows::window_attach_instance_id(ObjectID p_instance, WindowID p_window) {
	// AAA
}

ObjectID DisplayServerWidgetWindows::window_get_attached_instance_id(WindowID p_window) const {
	// AAA
	ObjectID instance_id;
	return instance_id;
}

void DisplayServerWidgetWindows::window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window) {

	rect_changed_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_window_event_callback(const Callable &p_callable, WindowID p_window) {

	event_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_input_event_callback(const Callable &p_callable, WindowID p_window) {

	input_event_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_input_text_callback(const Callable &p_callable, WindowID p_window) {
}

void DisplayServerWidgetWindows::window_set_drop_files_callback(const Callable &p_callable, WindowID p_window) {
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

bool initialized = false;

void DisplayServerWidgetWindows::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);

	// change the rendering content size
	window_set_size(Size2i(event->size().width(), event->size().height()), 0);

	// call Main::iteration function to call the engine update
	if (main_loop_valid && !Main::is_iterating()) {
		Main::iteration();
	}
}

void DisplayServerWidgetWindows::mouseMoveEvent(QMouseEvent *event) {
	Ref<InputEventMouseMotion> mm;
	mm.instantiate();

	mm->set_window_id(0);
	mm->set_button_mask(last_button_state);

	QPoint p = mapFromGlobal(QCursor::pos());
	Point2i c(p.x(), p.y());

	mm->set_position(c);
	mm->set_global_position(c);
	mm->set_velocity(Vector2(0, 0));

	Input::get_singleton()->parse_input_event(mm);
}

void DisplayServerWidgetWindows::mousePressEvent(QMouseEvent *event) {
	Ref<InputEventMouseButton> mb;
	mb.instantiate();
	mb->set_window_id(0);
	mb->set_pressed(true);
	mb->set_button_index(MouseButton::LEFT);

	if (mb->is_pressed()) {
		last_button_state.set_flag(mouse_button_to_mask(mb->get_button_index()));
	} else {
		last_button_state.clear_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	mb->set_button_mask(last_button_state);

	QPoint p = mapFromGlobal(QCursor::pos());
	mb->set_position(Vector2(p.x(), p.y()));

	Input::get_singleton()->parse_input_event(mb);
}

void DisplayServerWidgetWindows::mouseReleaseEvent(QMouseEvent *event) {
	Ref<InputEventMouseButton> mb;
	mb.instantiate();
	mb->set_window_id(0);
	mb->set_pressed(false);
	mb->set_button_index(MouseButton::LEFT);

	if (mb->is_pressed()) {
		last_button_state.set_flag(mouse_button_to_mask(mb->get_button_index()));
	} else {
		last_button_state.clear_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	mb->set_button_mask(last_button_state);

	Input::get_singleton()->parse_input_event(mb);
	Input::get_singleton()->release_pressed_events();
}

void DisplayServerWidgetWindows::closeEvent(QCloseEvent* event) {

	// notify the engine to exit
	_send_window_event(event_callback, WINDOW_EVENT_CLOSE_REQUEST);
}

/* Override default system paint engine to prevent errors. */
QPaintEngine *DisplayServerWidgetWindows::paintEngine() const {
	return reinterpret_cast<QPaintEngine *>(0);
}

void DisplayServerWidgetWindows::_send_window_event(const Callable& event_callback, WindowEvent p_event) {
	if (!event_callback.is_null()) {
		Variant event = int(p_event);
		Variant *eventp = &event;
		Variant ret;
		Callable::CallError ce;
		event_callback.callp((const Variant **)&eventp, 1, ret, ce);
	}
}

void DisplayServerWidgetWindows::_dispatch_input_events(const Ref<InputEvent> &p_event) {
	static_cast<DisplayServerWidgetWindows *>(get_singleton())->_dispatch_input_event(p_event);
}

void DisplayServerWidgetWindows::_dispatch_input_event(const Ref<InputEvent> &p_event) {

	if (in_dispatch_input_event) {
		return;
	}

	in_dispatch_input_event = true;
	Variant ev = p_event;
	Variant *evp = &ev;
	Variant ret;
	Callable::CallError ce;
	{
		if (input_event_callback.is_valid()) {
			input_event_callback.callp((const Variant **)&evp, 1, ret, ce);
		}
	}
	in_dispatch_input_event = false;

}

void DisplayServerWidgetWindows::window_set_size(const Size2i p_size, WindowID p_window) {
	// AAA

	if (p_size.width != size().width() || p_size.height != size().height()) {
		resize(p_size.width, p_size.height);
		return;
	}

	
#if defined(VULKAN_ENABLED)
	if (context_vulkan) {
		context_vulkan->window_resize(p_window, p_size.width, p_size.height);
	}
#endif
#if defined(GLES3_ENABLED)
	if (gl_manager) {
		gl_manager->window_resize(p_window, size().width(),size().height());
	}
#endif

	if (!rect_changed_callback.is_null()) {
		Variant size = Rect2i(geometry().x(), geometry().y(), geometry().width(), geometry().height());
		const Variant *args[] = { &size };
		Variant ret;
		Callable::CallError ce;
		rect_changed_callback.callp(args, 1, ret, ce);
	}

}

Size2i DisplayServerWidgetWindows::window_get_size(WindowID p_window) const {
	return Size2(size().width(), size().height());
}

Size2i DisplayServerWidgetWindows::window_get_size_with_decorations(WindowID p_window) const {
	return Size2();
}

void DisplayServerWidgetWindows::window_set_mode(WindowMode p_mode, WindowID p_window) {
	int a = 0;
	show();
	printf("::%d\n", a);
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
	return isVisible();
}

bool DisplayServerWidgetWindows::can_any_window_draw() const {
	return isVisible();
}

bool DisplayServerWidgetWindows::get_swap_cancel_ok() {
	return isVisible();
}

void DisplayServerWidgetWindows::process_events() {

	if (!main_loop_valid) {
		main_loop_valid = true;
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

DisplayServerWidgetWindows::DisplayServerWidgetWindows(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error, QWidget *parent) //:
	: QWidget(parent) {

	// These widget attributes are important. This lets Godot take over what's
	// drawn onto the widget's screen space. 
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setFocusPolicy(Qt::StrongFocus);
	// HOWEVER, this important attribute stops the "paintEvent" slot from being called,
	// thus we'll need to write our own method that paints to the screen every frame.
	setAttribute(Qt::WA_PaintOnScreen);

	// re-size the widget size to the default godot window size
	resize(1152, 648);

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

#if defined(VULKAN_ENABLED)
	if (rendering_driver == "vulkan") {
		context_vulkan = memnew(VulkanContextWindows);
		if (context_vulkan->initialize() != OK) {
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

	int width = size().width();
	int height = size().height();

#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		if (context_vulkan->window_create(0, DisplayServer::VSYNC_ENABLED, (HWND)winId(), 0, width, height) != OK) {
			memdelete(context_vulkan);
			context_vulkan = nullptr;
			//windows.erase(0);
			//ERR_FAIL_V_MSG(INVALID_WINDOW_ID, "Failed to create Vulkan Window.");
		}
		//wd.context_created = true;
		context_vulkan->set_vsync_mode(0, DisplayServer::VSYNC_ENABLED);
	}
#endif

#ifdef GLES3_ENABLED
	if (gl_manager) {
		if (gl_manager->window_create(0, (HWND)winId(), 0, size().width(), size().height()) != OK) {
			memdelete(gl_manager);
			gl_manager = nullptr;
			//windows.erase(0);
			//ERR_FAIL_V_MSG(INVALID_WINDOW_ID, "Failed to create an OpenGL window.");
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

	QPoint mouse_pos = mapFromGlobal(QCursor::pos());
	Input::get_singleton()->set_mouse_position(Point2i(mouse_pos.x(), mouse_pos.y()));
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
