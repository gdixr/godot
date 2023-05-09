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
#include "core/os/thread_safe.h"
#include "main/main.h"
#include "key_mapping_windows.h"
#if defined(GLES3_ENABLED)
#include "drivers/gles3/rasterizer_gles3.h"
#endif
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDropEvent>
#include <QMimeData>

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
	if (_mouse_mode == p_mode) {
		return;
	}
	_mouse_mode = p_mode;
	_set_mouse_mode_impl(p_mode);
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

DisplayServer::WindowID DisplayServerWidgetWindows::create_sub_window(WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Rect2i& p_rect)
{
	return -1;
}

void DisplayServerWidgetWindows::show_window(WindowID p_window) {
	// QWidget related show function
	if (isVisible()) {
		show();
	}
}

void DisplayServerWidgetWindows::window_attach_instance_id(ObjectID p_instance, WindowID p_window) {
	ERR_FAIL_COND(!_windows.has(p_window));
	_windows[p_window].instance_id = p_instance;
}

ObjectID DisplayServerWidgetWindows::window_get_attached_instance_id(WindowID p_window) const {
	// AAA
	ObjectID instance_id;
	return instance_id;
}

void DisplayServerWidgetWindows::window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!_windows.has(p_window));
	_windows[p_window].rect_changed_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_window_event_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!_windows.has(p_window));
	_windows[p_window].event_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_input_event_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!_windows.has(p_window));
	_windows[p_window].input_event_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_input_text_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!_windows.has(p_window));
	_windows[p_window].input_text_callback = p_callable;
}

void DisplayServerWidgetWindows::window_set_drop_files_callback(const Callable &p_callable, WindowID p_window) {
	ERR_FAIL_COND(!_windows.has(p_window));
	_windows[p_window].drop_files_callback = p_callable;
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

void DisplayServerWidgetWindows::_process_key_events()
{
	for (int i = 0; i < _key_event_pos; i++) {
		KeyEvent& ke = _key_event_buffer[i];
		Ref<InputEventKey> k;
		k.instantiate();
		k->set_window_id(ke.window_id);
		k->set_shift_pressed(ke.shift);
		k->set_alt_pressed(ke.alt);
		k->set_ctrl_pressed(ke.control);
		k->set_meta_pressed(ke.meta);
		k->set_pressed(ke.pressed);
		k->set_keycode((Key)KeyMappingWindows::get_keysym(ke.keycode));
		k->set_physical_keycode((Key)KeyMappingWindows::get_keysym(ke.keycode));
		k->set_unicode(ke.unicode);	
		if (k->get_unicode() && _gr_mem) {
			k->set_alt_pressed(false);
			k->set_ctrl_pressed(false);
		}
		if (k->get_unicode() < 32) {
			k->set_unicode(0);
		}
		k->set_echo(ke.pressed && ke.extended_key);
		Input::get_singleton()->parse_input_event(k);
	} 
	_key_event_pos = 0;
}

void DisplayServerWidgetWindows::_mouse_button_event(QMouseEvent *p_event, bool p_mouse_down, bool p_double_click)
{
	Ref<InputEventMouseButton> mb;
	mb.instantiate();
	if (p_event->button() == Qt::LeftButton) {
		mb->set_button_index(MouseButton::LEFT);
	}
	else if (p_event->button() == Qt::RightButton) {
		mb->set_button_index(MouseButton::RIGHT);
	}
	else if (p_event->button() == Qt::MiddleButton) {
		mb->set_button_index(MouseButton::MIDDLE);
	}
	else {
		return;
	}
	mb->set_window_id(0);
	mb->set_pressed(p_mouse_down);
	mb->set_double_click(p_double_click);
	mb->set_ctrl_pressed(_control_mem);
	mb->set_shift_pressed(_shift_mem);
	mb->set_alt_pressed(_alt_mem);
	if (mb->is_pressed()) {
		last_button_state.set_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	else {
		last_button_state.clear_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	mb->set_button_mask(last_button_state);
	QPoint p = mapFromGlobal(QCursor::pos());
	mb->set_position(Vector2(p.x(), p.y()));
	Input::get_singleton()->parse_input_event(mb);
}

void DisplayServerWidgetWindows::_key_event(QKeyEvent *p_event, bool p_key_down)
{
	int key = p_event->key();
	int modifiers = p_event->modifiers();
	bool auto_repeat = p_event->isAutoRepeat();
	if (key == Qt::Key_Shift) {
		_shift_mem = p_key_down;
	}
	if (key == Qt::Key_Control) {
		_control_mem = p_key_down;
	}
	if (key == Qt::Key_Alt) {
		_alt_mem = p_key_down;
		if (auto_repeat) {// 这是一个重复按键事件
			_gr_mem = _alt_mem;
			return;
		}
	}
	int virtual_key = p_event->nativeVirtualKey();
	WPARAM wParam = virtual_key;
	LPARAM lParam = p_event->nativeScanCode() << 16;
	KeyEvent ke;
	ke.pressed = p_key_down;
	ke.window_id = 0;
	ke.shift = (key != Qt::Key_Shift) ? _shift_mem : false;
	ke.alt = (!(key == Qt::Key_Alt && p_key_down)) ? _alt_mem : false;
	ke.control = (key != Qt::Key_Control) ? _control_mem : false;
	ke.meta = _meta_mem;
	ke.keycode = virtual_key;
	int scan_code = p_event->nativeScanCode();
	ke.unicode = scan_code;
	if (!auto_repeat && (scan_code == Qt::Key_unknown || scan_code >= 0xE000)) {// 处理扩展键事件
		ke.extended_key = true;
	}
	_key_event_buffer[_key_event_pos++] = ke;
}

void DisplayServerWidgetWindows::activateEvent(QEvent* p_event)
{
	if (p_event->type() == QEvent::WindowActivate) {
	}
}

void DisplayServerWidgetWindows::enterEvent(QEvent *p_event)
{
}

void DisplayServerWidgetWindows::leaveEvent(QEvent *p_event)
{
	_old_invalid = true;
}

void DisplayServerWidgetWindows::mouseMoveEvent(QMouseEvent *p_event) {
	Ref<InputEventMouseMotion> mm;
	mm.instantiate();
	mm->set_window_id(0);
	mm->set_ctrl_pressed(_control_mem);
	mm->set_shift_pressed(_shift_mem);
	mm->set_alt_pressed(_alt_mem);
	mm->set_button_mask(last_button_state);
	QPoint p = mapFromGlobal(QCursor::pos());
	Point2i c(p.x(), p.y());
	mm->set_position(c);
	mm->set_global_position(c);
	mm->set_velocity(Input::get_singleton()->get_last_mouse_velocity());
	if (_old_invalid) {
		_old_x = mm->get_position().x;
		_old_y = mm->get_position().y;
		_old_invalid = false;
	}
	mm->set_relative(Vector2(mm->get_position() - Vector2(_old_x, _old_y)));
	_old_x = mm->get_position().x;
	_old_y = mm->get_position().y;
	Input::get_singleton()->parse_input_event(mm);
}

void DisplayServerWidgetWindows::mousePressEvent(QMouseEvent *p_event) {
	_mouse_button_event(p_event,true);
}

void DisplayServerWidgetWindows::mouseReleaseEvent(QMouseEvent *p_event) {
	_mouse_button_event(p_event, false);
}

void DisplayServerWidgetWindows::mouseDoubleClickEvent(QMouseEvent* p_event) {
	_mouse_button_event(p_event, true,true);
}

void DisplayServerWidgetWindows::wheelEvent(QWheelEvent *p_event) {
	Ref<InputEventMouseButton> mb;
	mb.instantiate();
	mb->set_window_id(MAIN_WINDOW_ID);
	mb->set_pressed(true);
	
	int motion = p_event->angleDelta().y();
	if (motion > 0) {
		mb->set_button_index(MouseButton::WHEEL_UP);
	}
	else {
		mb->set_button_index(MouseButton::WHEEL_DOWN);
	}
	mb->set_factor(fabs((double)motion / (double)WHEEL_DELTA));
	if (mb->is_pressed()) {
		last_button_state.set_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	else {
		last_button_state.clear_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	mb->set_button_mask(last_button_state);
	QPoint p = mapFromGlobal(QCursor::pos());
	mb->set_position(Vector2(p.x(), p.y()));
	Input::get_singleton()->parse_input_event(mb);

	if (mb->is_pressed() && mb->get_button_index() >= MouseButton::WHEEL_UP && mb->get_button_index() <= MouseButton::WHEEL_RIGHT) {
		// Send release for mouse wheel.
		Ref<InputEventMouseButton> mbd = mb->duplicate();
		mbd->set_window_id(0);
		last_button_state.clear_flag(mouse_button_to_mask(mbd->get_button_index()));
		mbd->set_button_mask(last_button_state);
		mbd->set_pressed(false);
		Input::get_singleton()->parse_input_event(mbd);
	}
}

void DisplayServerWidgetWindows::keyPressEvent(QKeyEvent *p_event)
{
	_key_event(p_event, true);
}

void DisplayServerWidgetWindows::keyReleaseEvent(QKeyEvent *p_event)
{
	_key_event(p_event, false);
}

void DisplayServerWidgetWindows::dragEnterEvent(QDragEnterEvent* p_event)
{
	if (p_event->mimeData()->hasUrls()) {
		p_event->acceptProposedAction();
	}
}

void DisplayServerWidgetWindows::dropEvent(QDropEvent* p_event)
{
	const QMimeData* mime_data = p_event->mimeData();
	if (mime_data->hasUrls()) {
		QList<QUrl> url_list = mime_data->urls();

		Vector<String> files;
		foreach(QUrl url, url_list) {
			QString file_path = url.toLocalFile();
			files.push_back(file_path.toStdWString().c_str());
		}
		WindowID window_id = MAIN_WINDOW_ID;

		WindowData& wd = _windows[window_id];
		if (files.size() && !wd.drop_files_callback.is_null()) {
			Variant v = files;
			Variant* vp = &v;
			Variant ret;
			Callable::CallError ce;
			wd.drop_files_callback.callp((const Variant**)&vp, 1, ret, ce);
		}
	}
}

void DisplayServerWidgetWindows::resizeEvent(QResizeEvent* p_event)
{
	WindowID window_id = MAIN_WINDOW_ID;
	WindowData& wd = _windows[window_id];

	int w = p_event->size().width();
	int h = p_event->size().height();

	if (w == wd.width && h == wd.height) {
		return;
	}
	wd.width = w;
	wd.height = h;
	wd.maximized = false;
	wd.minimized = false;
	wd.fullscreen = false;
	if (windowState() & Qt::WindowMaximized) {
		wd.maximized = true;
	}
	if (windowState() & Qt::WindowMinimized) {
		wd.minimized = true;
	}
	if (windowState() & Qt::WindowFullScreen) {
		wd.fullscreen = true;
	}
	bool rect_changed = true;
#if defined(VULKAN_ENABLED)
	if (context_vulkan && wd.context_created) {
		// Note: Trigger resize event to update swapchains when window is minimized/restored, even if size is not changed.
		context_vulkan->window_resize(window_id, w, h);
	}
#endif
	if (rect_changed) {
		if (!wd.rect_changed_callback.is_null()) {
			Variant size = Rect2i(wd.last_pos.x, wd.last_pos.y, wd.width, wd.height);
			const Variant* args[] = { &size };
			Variant ret;
			Callable::CallError ce;
			wd.rect_changed_callback.callp(args, 1, ret, ce);
		}
	}
}

void DisplayServerWidgetWindows::closeEvent(QCloseEvent *p_event) {

	WindowID window_id = MAIN_WINDOW_ID;
	WindowData& wd = _windows[window_id];
	_send_window_event(wd, WINDOW_EVENT_CLOSE_REQUEST);
}

/* Override default system paint engine to prevent errors. */
QPaintEngine *DisplayServerWidgetWindows::paintEngine() const {
	return reinterpret_cast<QPaintEngine *>(0);
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
	if (in_dispatch_input_event) {
		return;
	}
	in_dispatch_input_event = true;
	Variant ev = p_event;
	Variant *evp = &ev;
	Variant ret;
	Callable::CallError ce;
	{
		List<WindowID>::Element* E = _popup_list.back();
		if (E && Object::cast_to<InputEventKey>(*p_event)) {
			// Redirect keyboard input to active popup.
			if (_windows.has(E->get())) {
				Callable callable = _windows[E->get()].input_event_callback;
				if (callable.is_valid()) {
					callable.callp((const Variant**)&evp, 1, ret, ce);
				}
			}
			in_dispatch_input_event = false;
			return;
		}
	}
	Ref<InputEventFromWindow> event_from_window = p_event;
	if (event_from_window.is_valid() && event_from_window->get_window_id() != INVALID_WINDOW_ID) {
		// Send to a single window.
		if (_windows.has(event_from_window->get_window_id())) {
			Callable callable = _windows[event_from_window->get_window_id()].input_event_callback;
			if (callable.is_valid()) {
				callable.callp((const Variant**)&evp, 1, ret, ce);
			}
		}
	}
	else {
		// Send to all windows.
		for (const KeyValue<WindowID, WindowData>& E : _windows) {
			const Callable callable = E.value.input_event_callback;
			if (callable.is_valid()) {
				callable.callp((const Variant**)&evp, 1, ret, ce);
			}
		}
	}
	in_dispatch_input_event = false;
}

void DisplayServerWidgetWindows::window_set_size(const Size2i p_size, WindowID p_window) {
	// AAA
	ERR_FAIL_COND(!_windows.has(p_window));
	WindowData& wd = _windows[p_window];
	if (wd.fullscreen || wd.maximized) {
		return;
	}
	int w = p_size.width;
	int h = p_size.height;
	if (w == wd.width && h == wd.height) {
		return;
	}
	wd.width = w;
	wd.height = h;
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
	resize(w, h);
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

void DisplayServerWidgetWindows::cursor_set_shape(CursorShape p_shape)
{
	ERR_FAIL_INDEX(p_shape, CURSOR_MAX);
	if (_cursor_shape == p_shape) {
		return;
	}
	if (_mouse_mode != MOUSE_MODE_VISIBLE && _mouse_mode != MOUSE_MODE_CONFINED) {
		_cursor_shape = p_shape;
		return;
	}
	static const QCursor win_cursors[CURSOR_MAX] = {
		Qt::ArrowCursor,
		Qt::IBeamCursor,
		Qt::PointingHandCursor,
		Qt::CrossCursor,
		Qt::WaitCursor,
		Qt::BusyCursor,
		Qt::SizeAllCursor,
		Qt::ArrowCursor,
		Qt::ForbiddenCursor,
		Qt::SizeVerCursor,
		Qt::SizeHorCursor,
		Qt::SizeBDiagCursor,
		Qt::SizeFDiagCursor,
		Qt::SizeVerCursor,
		Qt::SizeVerCursor,
		Qt::SizeHorCursor,
		Qt::WhatsThisCursor
	};
	QCursor cursor(win_cursors[p_shape]);
	setCursor(cursor);
	_cursor_shape = p_shape;
}

DisplayServer::CursorShape DisplayServerWidgetWindows::cursor_get_shape() const
{
	return _cursor_shape;
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
	return isVisible();
}

void DisplayServerWidgetWindows::process_events() {

	if (!main_loop_valid) {
		main_loop_valid = true;
	}
	_process_key_events();
	Input::get_singleton()->flush_buffered_events();
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
	KeyMappingWindows::initialize();
	_old_invalid = true;
	_old_x = 0;
	_old_y = 0;
	// These widget attributes are important. This lets Godot take over what's
	// drawn onto the widget's screen space. 
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setFocusPolicy(Qt::StrongFocus);
	// HOWEVER, this important attribute stops the "paintEvent" slot from being called,
	// thus we'll need to write our own method that paints to the screen every frame.
	setAttribute(Qt::WA_PaintOnScreen);
	setMouseTracking(true);
	setAcceptDrops(true);
	// re-size the widget size to the default godot window size
	resize(1152, 648);
	int width = size().width();
	int height = size().height();
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
	WindowID id = _window_id_counter;
	WindowData& wd = _windows[id];
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
	_window_id_counter++;
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
#ifdef VULKAN_ENABLED
	if (context_vulkan) {
		if (context_vulkan->window_create(0, DisplayServer::VSYNC_ENABLED, (HWND)winId(), 0, width, height) != OK) {
			memdelete(context_vulkan);
			context_vulkan = nullptr;
			_windows.erase(0);
			ERR_PRINT("Failed to create Vulkan Window.");
		}
		wd.context_created = true;
		context_vulkan->set_vsync_mode(0, DisplayServer::VSYNC_ENABLED);
	}
#endif
#ifdef GLES3_ENABLED
	if (gl_manager) {
		if (gl_manager->window_create(0, (HWND)winId(), 0, size().width(), size().height()) != OK) {
			memdelete(gl_manager);
			gl_manager = nullptr;
			_windows.erase(0);
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

