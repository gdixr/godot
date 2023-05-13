
#ifndef GDI_WINDOW_H
#define GDI_WINDOW_H
#include "gdi_common.h"
#include "servers/display_server.h"

#if defined(VULKAN_ENABLED)
#include "drivers/vulkan/rendering_device_vulkan.h"
#include "platform/windows/vulkan_context_win.h"
#endif

#if defined(GLES3_ENABLED)
#include "gl_manager_windows.h"
#endif


#include <QWidget>

class GdiWindow;

class QtEventServer : public QObject {
private:

#if defined(GLES3_ENABLED)
		GLManager_Windows* gl_manager = nullptr;
#endif

#if defined(VULKAN_ENABLED)
	VulkanContextWindows* context_vulkan = nullptr;
	RenderingDeviceVulkan* rendering_device_vulkan = nullptr;
#endif


	struct KeyEvent {
		int window_id;
		bool alt, shift, control, meta;
		bool pressed;
		int unicode;
		int keycode;
		bool extended_key = false;
	};

	KeyEvent _key_event_buffer[512];
	int _key_event_pos = 0;

	bool _in_dispatch_input_event = false;
	DisplayServer::MouseMode _mouse_mode = DisplayServer::MOUSE_MODE_VISIBLE;
	DisplayServer::CursorShape _cursor_shape = DisplayServer::CursorShape::CURSOR_ARROW;

	bool _old_invalid;
	int _old_x, _old_y;

	bool _alt_mem = false;
	bool _gr_mem = false;
	bool _shift_mem = false;
	bool _control_mem = false;
	bool _meta_mem = false;
	bool _drop_events = false;

	BitField<MouseButtonMask> _last_button_state;

	DisplayServer::WindowID _window_id_counter = DisplayServer::MAIN_WINDOW_ID;
	RBMap<DisplayServer::WindowID, GdiWindow *> _windows;
	List<DisplayServer::WindowID> _popup_list;
	DisplayServer::WindowID _last_focused_window = DisplayServer::INVALID_WINDOW_ID;
	DisplayServer::WindowID _window_mouseover_id = DisplayServer::INVALID_WINDOW_ID;
	static QtEventServer* _singleton;

private:

	DisplayServer::WindowID _get_focused_window_or_popup() const;

	void _mouse_button_event(GdiWindow*p_widget, QMouseEvent *p_event, bool p_mouse_down, bool p_double_click = false);
	void _key_event(GdiWindow*p_widget, QKeyEvent *p_event, bool p_key_down);

protected:
	bool eventFilter(QObject *p_watched, QEvent *p_event) override;

	void leave_event(GdiWindow* p_widget, QEvent* p_event);
	void mouse_move_event(GdiWindow* p_widget, QMouseEvent* p_event);
	void mouse_press_event(GdiWindow* p_widget, QMouseEvent* p_event);
	void mouse_release_event(GdiWindow* p_widget, QMouseEvent* p_event);
	void mouse_doubleclick_event(GdiWindow* p_widget, QMouseEvent* p_event);
	void key_press_event(GdiWindow* p_widget, QKeyEvent* p_event);
	void key_release_event(GdiWindow* p_widget, QKeyEvent* p_event);
	void drag_enter_event(GdiWindow* p_widget, QDragEnterEvent* p_event);
	void drop_event(GdiWindow* p_widget, QDropEvent* p_event);
	void resize_event(GdiWindow* p_widget, QResizeEvent* p_event);
	void wheel_event(GdiWindow* p_widget, QWheelEvent* p_event);
	void close_event(GdiWindow* p_widget, QCloseEvent* p_event);
	QPaintEngine* paintEngine(GdiWindow* p_widget) const;

public:

	static QtEventServer* get_singleton();

	void process_key_events();
	void dispatch_input_event(const Ref<InputEvent>& p_event);

	void cursor_set_shape(DisplayServer::CursorShape p_shape);
	DisplayServer::CursorShape cursor_get_shape() const;
	void mouse_set_mode(DisplayServer::MouseMode p_mode);
	BitField<MouseButtonMask> mouse_get_button_state() const;
	Point2i mouse_get_position() const;


	GdiWindow* get_window(DisplayServer::WindowID p_window_id);

	void create_and_show_window();

	QtEventServer(QObject *p_parent = nullptr);
	~QtEventServer();
};

class GdiWindow : public QWidget {
	Q_OBJECT

	friend QtEventServer;

private:
	Callable _rect_changed_callback;
	Callable _event_callback;
	Callable _input_event_callback;
	Callable _input_text_callback;
	Callable _drop_files_callback;
	Callable _resize_callback;
	int _width;
	int _height;
	QString _name;
	QtEventServer *_event_server;
	int _winid;

	static GdiWindow *_singleton;

	QtCallGengine *_signal;

public:
	static GdiWindow *get_singleton();

	void set_rect_changed_callback(const Callable &p_callable);
	void set_window_event_callback(const Callable &p_callable);
	void set_input_event_callback(const Callable &p_callable);
	void set_input_text_callback(const Callable &p_callable);
	void set_drop_files_callback(const Callable &p_callable);
	const Callable get_rect_changed_callback() { return _rect_changed_callback; };
	const Callable get_window_event_callback() { return _event_callback; };
	const Callable get_input_event_callback() { return _input_event_callback; };
	const Callable get_input_text_callback() { return _input_text_callback; };
	const Callable get_drop_files_callback() { return _drop_files_callback; };

	QPaintEngine *paintEngine() const override;

	void set_size(const Size2i p_size);
	Size2i get_size();

	void connect(const StringName &p_signal, const Callable &p_callable, uint32_t p_flags = 0);

	void show_window();
	GdiWindow(QtEventServer *p_event_server = nullptr, QWidget *p_parent = nullptr);
	GdiWindow(QWidget *p_parent = nullptr);
	~GdiWindow();
};

#endif //GDI_WINDOW_H
