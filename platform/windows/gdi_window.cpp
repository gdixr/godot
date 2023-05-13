#include "gdi_window.h"
#include "key_mapping_windows.h"

#include "servers/rendering/renderer_compositor.h"
#include "servers/rendering/renderer_rd/renderer_compositor_rd.h"
#if defined(GLES3_ENABLED)
#include "drivers/gles3/rasterizer_gles3.h"
#endif

#include <QApplication>
#include <QCursor>
#include <QDropEvent>
#include <QGuiApplication>
#include <QMimeData>
#include <QMouseEvent>
#include <QResizeEvent>

QtEventServer *QtEventServer::_singleton = nullptr;

QtEventServer *QtEventServer::get_singleton() {
	return _singleton;
}

bool QtEventServer::eventFilter(QObject *p_watched, QEvent *p_event) {
	if (!Input::get_singleton()) {
		return false;
	}
	GdiWindow *widget = qobject_cast<GdiWindow *>(p_watched);

	switch (p_event->type()) {
		case QEvent::ApplicationActivate:
			break;
		case QEvent::FocusIn:
			_last_focused_window = widget->_winid;
			break;
		case QEvent::FocusOut:
			_last_focused_window = widget->_winid;
			break;
		case QEvent::Leave:
			leave_event(widget, p_event);
			break;
		case QEvent::MouseMove:
			mouse_move_event(widget, (QMouseEvent *)p_event);
			break;
		case QEvent::MouseButtonPress:
			mouse_press_event(widget, (QMouseEvent *)p_event);
			break;
		case QEvent::MouseButtonRelease:
			mouse_release_event(widget, (QMouseEvent *)p_event);
			break;
		case QEvent::MouseButtonDblClick:
			mouse_doubleclick_event(widget, (QMouseEvent *)p_event);
			break;
		case QEvent::KeyPress:
			key_press_event(widget, (QKeyEvent *)p_event);
			break;
		case QEvent::KeyRelease:
			key_release_event(widget, (QKeyEvent *)p_event);
			break;
		case QEvent::DragEnter:
			drag_enter_event(widget, (QDragEnterEvent *)p_event);
			break;
		case QEvent::Drop:
			drop_event(widget, (QDropEvent *)p_event);
			break;
		case QEvent::Resize:
			resize_event(widget, (QResizeEvent *)p_event);
			break;
		case QEvent::Wheel:
			wheel_event(widget, (QWheelEvent *)p_event);
			break;
		case QEvent::Close:
			close_event(widget, (QCloseEvent *)p_event);
			break;
		case QEvent::Paint:
			widget->paintEngine();
			break;
		default:
			break;
	}
	return QObject::eventFilter(p_watched, p_event);
	//return true;
}

DisplayServer::WindowID QtEventServer::_get_focused_window_or_popup() const {
	const List<DisplayServer::WindowID>::Element *E = _popup_list.back();
	if (E) {
		return E->get();
	}
	return _last_focused_window;
}

void QtEventServer::_mouse_button_event(GdiWindow *p_widget, QMouseEvent *p_event, bool p_mouse_down, bool p_double_click) {
	Ref<InputEventMouseButton> mb;
	mb.instantiate();
	if (p_event->button() == Qt::LeftButton) {
		mb->set_button_index(MouseButton::LEFT);
	} else if (p_event->button() == Qt::RightButton) {
		mb->set_button_index(MouseButton::RIGHT);
	} else if (p_event->button() == Qt::MiddleButton) {
		mb->set_button_index(MouseButton::MIDDLE);
	} else {
		return;
	}
	mb->set_window_id(0);
	mb->set_pressed(p_mouse_down);
	mb->set_double_click(p_double_click);
	mb->set_ctrl_pressed(_control_mem);
	mb->set_shift_pressed(_shift_mem);
	mb->set_alt_pressed(_alt_mem);
	if (mb->is_pressed()) {
		_last_button_state.set_flag(mouse_button_to_mask(mb->get_button_index()));
	} else {
		_last_button_state.clear_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	mb->set_button_mask(_last_button_state);
	QPoint p = p_widget->mapFromGlobal(QCursor::pos());
	mb->set_position(Vector2(p.x(), p.y()));
	Input::get_singleton()->parse_input_event(mb);
}

void QtEventServer::_key_event(GdiWindow *p_widget, QKeyEvent *p_event, bool p_key_down) {
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
		if (auto_repeat) { // 这是一个重复按键事件
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
	if (!auto_repeat && (scan_code == Qt::Key_unknown || scan_code >= 0xE000)) { // 处理扩展键事件
		ke.extended_key = true;
	}
	_key_event_buffer[_key_event_pos++] = ke;
}

void QtEventServer::leave_event(GdiWindow *p_widget, QEvent *p_event) {
	_old_invalid = true;
}

void QtEventServer::mouse_move_event(GdiWindow *p_widget, QMouseEvent *p_event) {
	Ref<InputEventMouseMotion> mm;
	mm.instantiate();
	mm->set_window_id(0);
	mm->set_ctrl_pressed(_control_mem);
	mm->set_shift_pressed(_shift_mem);
	mm->set_alt_pressed(_alt_mem);
	mm->set_button_mask(_last_button_state);
	QPoint p = p_widget->mapFromGlobal(QCursor::pos());
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

void QtEventServer::mouse_press_event(GdiWindow *p_widget, QMouseEvent *p_event) {
	_mouse_button_event(p_widget, p_event, true);
}

void QtEventServer::mouse_release_event(GdiWindow *p_widget, QMouseEvent *p_event) {
	_mouse_button_event(p_widget, p_event, false);
}

void QtEventServer::mouse_doubleclick_event(GdiWindow *p_widget, QMouseEvent *p_event) {
	_mouse_button_event(p_widget, p_event, true, true);
}

void QtEventServer::key_press_event(GdiWindow *p_widget, QKeyEvent *p_event) {
	_key_event(p_widget, p_event, true);
}

void QtEventServer::key_release_event(GdiWindow *p_widget, QKeyEvent *p_event) {
	_key_event(p_widget, p_event, false);
}

void QtEventServer::drag_enter_event(GdiWindow *p_widget, QDragEnterEvent *p_event) {
	if (p_event->mimeData()->hasUrls()) {
		p_event->acceptProposedAction();
	}
}

void QtEventServer::drop_event(GdiWindow *p_widget, QDropEvent *p_event) {
	const QMimeData *mime_data = p_event->mimeData();
	if (mime_data->hasUrls()) {
		QList<QUrl> url_list = mime_data->urls();

		Vector<String> files;
		foreach (QUrl url, url_list) {
			QString file_path = url.toLocalFile();
			files.push_back(file_path.toStdWString().c_str());
		}

		DisplayServer::WindowID window_id = p_widget->_winid;
		if (files.size() && _windows[window_id]->_drop_files_callback.is_valid()) {
			Variant v = files;
			Variant *vp = &v;
			Variant ret;
			Callable::CallError ce;
			_windows[window_id]->_drop_files_callback.callp((const Variant **)&vp, 1, ret, ce);
		}
	}
}

void QtEventServer::resize_event(GdiWindow *p_widget, QResizeEvent *p_event) {
	//p_widget->resizeEvent(p_event);

	DisplayServer::WindowID window_id = p_widget->_winid;

	int w = p_event->size().width();
	int h = p_event->size().height();

	GdiWindow *wd = _windows[window_id];
	if (wd->_width == w && h == wd->_height) {
		return;
	}
	wd->_width = w;
	wd->_height = h;
	if (wd->_rect_changed_callback.is_valid()) {
		Variant size = Rect2i(0, 0, w, h);
		const Variant *args[] = { &size };
		Variant ret;
		Callable::CallError ce;
		wd->_rect_changed_callback.callp(args, 1, ret, ce);
	}

	if (wd->_resize_callback.is_valid()) {
		Variant ww = w;
		Variant hh = h;
		const Variant *args2[] = { &ww, &hh };
		Variant ret2;
		Callable::CallError ce2;
		wd->_resize_callback.callp(args2, 2, ret2, ce2);

		QtSyncGengine::get_singleton()->wait();
	}
}

void QtEventServer::wheel_event(GdiWindow *p_widget, QWheelEvent *p_event) {
	Ref<InputEventMouseButton> mb;
	mb.instantiate();
	mb->set_window_id(0);
	mb->set_pressed(true);

	int motion = p_event->angleDelta().y();
	if (motion > 0) {
		mb->set_button_index(MouseButton::WHEEL_UP);
	} else {
		mb->set_button_index(MouseButton::WHEEL_DOWN);
	}
	mb->set_factor(fabs((double)motion / (double)WHEEL_DELTA));
	if (mb->is_pressed()) {
		_last_button_state.set_flag(mouse_button_to_mask(mb->get_button_index()));
	} else {
		_last_button_state.clear_flag(mouse_button_to_mask(mb->get_button_index()));
	}
	mb->set_button_mask(_last_button_state);
	QPoint p = p_widget->mapFromGlobal(QCursor::pos());
	mb->set_position(Vector2(p.x(), p.y()));
	Input::get_singleton()->parse_input_event(mb);

	if (mb->is_pressed() && mb->get_button_index() >= MouseButton::WHEEL_UP && mb->get_button_index() <= MouseButton::WHEEL_RIGHT) {
		// Send release for mouse wheel.
		Ref<InputEventMouseButton> mbd = mb->duplicate();
		mbd->set_window_id(0);
		_last_button_state.clear_flag(mouse_button_to_mask(mbd->get_button_index()));
		mbd->set_button_mask(_last_button_state);
		mbd->set_pressed(false);
		Input::get_singleton()->parse_input_event(mbd);
	}
}

void QtEventServer::close_event(GdiWindow *p_widget, QCloseEvent *p_event) {
	DisplayServer::WindowID window_id = p_widget->_winid;
	if (_windows[window_id]->_event_callback.is_valid()) {
		Variant event = int(DisplayServer::WINDOW_EVENT_CLOSE_REQUEST);
		Variant *eventp = &event;
		Variant ret;
		Callable::CallError ce;
		_windows[window_id]->_event_callback.callp((const Variant **)&eventp, 1, ret, ce);
	}
}

QPaintEngine *QtEventServer::paintEngine(GdiWindow *p_widget) const {
	return reinterpret_cast<QPaintEngine *>(0);
}

void QtEventServer::process_key_events() {
	for (int i = 0; i < _key_event_pos; i++) {
		KeyEvent &ke = _key_event_buffer[i];
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

void QtEventServer::dispatch_input_event(const Ref<InputEvent> &p_event) {
	if (_in_dispatch_input_event) {
		return;
	}
	_in_dispatch_input_event = true;
	Variant ev = p_event;
	Variant *evp = &ev;
	Variant ret;
	Callable::CallError ce;
	{
		//List<WindowID>::Element* E = _popup_list.back();
		//if (E && Object::cast_to<InputEventKey>(*p_event)) {
		//	// Redirect keyboard input to active popup.
		//	if (QtEventServer::get_singleton()->get_window(E->get())) {
		//		Callable callable = QtEventServer::get_singleton()->get_window(E->get())->get_input_event_callback();
		//		if (callable.is_valid()) {
		//			callable.callp((const Variant**)&evp, 1, ret, ce);
		//		}
		//	}
		//	_in_dispatch_input_event = false;
		//	return;
		//}
	}
	Ref<InputEventFromWindow> event_from_window = p_event;
	if (event_from_window.is_valid() && event_from_window->get_window_id() != DisplayServer::INVALID_WINDOW_ID) {
		// Send to a single window.
		if (_windows.has(event_from_window->get_window_id())) {
			Callable callable = _windows[event_from_window->get_window_id()]->get_input_event_callback();
			if (callable.is_valid()) {
				callable.callp((const Variant **)&evp, 1, ret, ce);
			}
		}
	} else {
		// Send to all windows.
		for (const KeyValue<DisplayServer::WindowID, GdiWindow *> &E : _windows) {
			const Callable callable = E.value->get_input_event_callback();
			if (callable.is_valid()) {
				callable.callp((const Variant **)&evp, 1, ret, ce);
			}
		}
	}
	_in_dispatch_input_event = false;
}

void QtEventServer::cursor_set_shape(DisplayServer::CursorShape p_shape) {
	ERR_FAIL_INDEX(p_shape, DisplayServer::CursorShape::CURSOR_MAX);

	if (_cursor_shape == p_shape || _mouse_mode != DisplayServer::MOUSE_MODE_VISIBLE && _mouse_mode != DisplayServer::MOUSE_MODE_CONFINED) {
		_cursor_shape = p_shape;
		if (p_shape == DisplayServer::CursorShape::CURSOR_ARROW) {
			QApplication::restoreOverrideCursor();
		}
		return;
	}
	_cursor_shape = p_shape;

	static Qt::CursorShape win_cursors[DisplayServer::CursorShape::CURSOR_MAX] = {
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

	QApplication::restoreOverrideCursor();
	QCursor cursor(win_cursors[p_shape]);
	QApplication::setOverrideCursor(cursor);
}

DisplayServer::CursorShape QtEventServer::cursor_get_shape() const {
	return _cursor_shape;
}

void QtEventServer::mouse_set_mode(DisplayServer::MouseMode p_mode) {
	if (_mouse_mode == p_mode) {
		return;
	}
	_mouse_mode = p_mode;

	if (_windows.has(DisplayServer::MAIN_WINDOW_ID) && (p_mode == DisplayServer::MOUSE_MODE_CAPTURED || p_mode == DisplayServer::MOUSE_MODE_CONFINED || p_mode == DisplayServer::MOUSE_MODE_CONFINED_HIDDEN)) {
		// Mouse is grabbed (captured or confined).

		DisplayServer::WindowID window_id = _get_focused_window_or_popup();
		if (!_windows.has(window_id)) {
			window_id = DisplayServer::MAIN_WINDOW_ID;
		}

		GdiWindow *wd = _windows[window_id];

		QRect rect = wd->geometry();
		//QCursor::setPos(mouseX, mouseY);

		if (p_mode == DisplayServer::MOUSE_MODE_CAPTURED) {
		}
	} else {
		// Mouse is free to move around (not captured or confined).
	}

	if (p_mode == DisplayServer::MOUSE_MODE_HIDDEN || p_mode == DisplayServer::MOUSE_MODE_CAPTURED || p_mode == DisplayServer::MOUSE_MODE_CONFINED_HIDDEN) {
		QCursor cursor(Qt::BlankCursor);
		cursor.setShape(Qt::BlankCursor);
		_windows[DisplayServer::MAIN_WINDOW_ID]->setCursor(cursor);
	} else {
		DisplayServer::CursorShape c = _cursor_shape;
		_cursor_shape = DisplayServer::CursorShape::CURSOR_MAX;
		cursor_set_shape(c);
	}
}

BitField<MouseButtonMask> QtEventServer::mouse_get_button_state() const {
	return _last_button_state;
}

Point2i QtEventServer::mouse_get_position() const {
	QRect screenRect = QGuiApplication::primaryScreen()->geometry();
	QPoint top_left = screenRect.topLeft();
	QPoint p = QCursor::pos();
	return Point2i(p.x() - top_left.x(), p.y() - top_left.y());
}

GdiWindow *QtEventServer::get_window(DisplayServer::WindowID p_window_id) {
	ERR_FAIL_COND_V(!_windows.has(p_window_id), nullptr);
	return _windows[p_window_id];
}

void QtEventServer::create_and_show_window() {
	GdiWindow *main_win = new GdiWindow(this);
	_windows[DisplayServer::MAIN_WINDOW_ID] = main_win;
	main_win->_winid = DisplayServer::MAIN_WINDOW_ID;
	_window_id_counter++;

	if (false) {
		String rendering_driver = "vulkan";
		HWND winId = (HWND)main_win->winId();
		int width = main_win->_width;
		int height = main_win->_height;
		Error r_error = OK;
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
			if (context_vulkan->window_create(DisplayServer::MAIN_WINDOW_ID, DisplayServer::VSYNC_ENABLED, winId, 0, width, height) != OK) {
				memdelete(context_vulkan);
				context_vulkan = nullptr;
				ERR_PRINT("Failed to create Vulkan Window.");
			}
			//wd.context_created = true;
			context_vulkan->set_vsync_mode(DisplayServer::MAIN_WINDOW_ID, DisplayServer::VSYNC_ENABLED);
		}
#endif
#ifdef GLES3_ENABLED
		if (gl_manager) {
			if (gl_manager->window_create(0, winId, 0, width, height) != OK) {
				memdelete(gl_manager);
				gl_manager = nullptr;
				ERR_PRINT("Failed to create an OpenGL window.");
			}
			//window_set_vsync_mode(p_vsync_mode, 0);
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
	}

	main_win->show();
}

QtEventServer::QtEventServer(QObject *p_parent) :
		QObject(p_parent) {
	KeyMappingWindows::initialize();
	_old_invalid = true;
	_old_x = 0;
	_old_y = 0;

	_singleton = this;
}

QtEventServer::~QtEventServer() {
	for (RBMap<DisplayServer::WindowID, GdiWindow *>::Element *E = _windows.front(); E; E = E->next()) {
		delete E->value();
	}
	_windows.clear();
}

//==================================================================================================

GdiWindow *GdiWindow::_singleton = nullptr;

GdiWindow *GdiWindow::get_singleton() {
	return _singleton;
}

void GdiWindow::set_rect_changed_callback(const Callable &p_callable) {
	_rect_changed_callback = p_callable;
}

void GdiWindow::set_window_event_callback(const Callable &p_callable) {
	_event_callback = p_callable;
}

void GdiWindow::set_input_event_callback(const Callable &p_callable) {
	_input_event_callback = p_callable;
}

void GdiWindow::set_input_text_callback(const Callable &p_callable) {
	_input_text_callback = p_callable;
}

void GdiWindow::set_drop_files_callback(const Callable &p_callable) {
	_drop_files_callback = p_callable;
}

/* Override default system paint engine to prevent errors. */
QPaintEngine *GdiWindow::paintEngine() const {
	return reinterpret_cast<QPaintEngine *>(0);
}

void GdiWindow::set_size(const Size2i p_size) {
	if (windowState() == Qt::WindowFullScreen || windowState() == Qt::WindowMaximized) {
		return;
	}
	int w = p_size.width;
	int h = p_size.height;
	if (w == _width && h == _height) {
		return;
	}
	_width = w;
	_height = h;
	resize(w, h);
}

Size2i GdiWindow::get_size() {
	return Size2(_width, _height);
}

void GdiWindow::show_window() {
	show();
}

void GdiWindow::connect(const StringName &p_signal, const Callable &p_callable, uint32_t p_flags) {
	_resize_callback = p_callable;
	//_signal->connect(p_signal, p_callable, p_flags);
}

GdiWindow::GdiWindow(QtEventServer *p_event_server, QWidget *p_parent) :
		QWidget(p_parent) {
	_event_server = p_event_server;
	if (_event_server) {
		installEventFilter(_event_server);
	}
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_PaintOnScreen);
	setMouseTracking(true);
	setAcceptDrops(true);

	_singleton = this;

	_width = 1152;
	_height = 648;
	resize(_width, _height);
}

GdiWindow::GdiWindow(QWidget *p_parent) :
		QWidget(p_parent) {
	_singleton = this;
	//_signal = memnew(QtCallGe);

	/*KeyMappingWindows::initialize();
	_old_invalid = true;
	_old_x = 0;
	_old_y = 0;*/

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

	_width = 1152;
	_height = 648;
	resize(_width, _height);
}

GdiWindow::~GdiWindow() {
	//memdelete(_signal);
}
