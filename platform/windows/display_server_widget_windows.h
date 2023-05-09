#ifndef DISPLAY_SERVER_WIDGET_H
#define DISPLAY_SERVER_WIDGET_H

#include "servers/display_server.h"
#include "servers/rendering_server.h"

#include "servers/rendering/renderer_compositor.h"
#include "servers/rendering/renderer_rd/renderer_compositor_rd.h"

#if defined(VULKAN_ENABLED)
#include "drivers/vulkan/rendering_device_vulkan.h"
#include "platform/windows/vulkan_context_win.h"
#endif

#if defined(GLES3_ENABLED)
#include "gl_manager_windows.h"
#endif

#include <QWidget>

class DisplayServerWidgetWindows : public QWidget, DisplayServer {

	Q_OBJECT

#if defined(GLES3_ENABLED)
	GLManager_Windows *gl_manager = nullptr;
#endif

#if defined(VULKAN_ENABLED)
	VulkanContextWindows *context_vulkan = nullptr;
	RenderingDeviceVulkan *rendering_device_vulkan = nullptr;
#endif

	struct WindowData {
		bool pre_fs_valid = false;
		bool maximized = false;
		bool minimized = false;
		bool fullscreen = false;
		bool multiwindow_fs = false;
		bool borderless = false;
		bool resizable = true;
		bool window_focused = false;
		bool was_maximized = false;
		bool always_on_top = false;
		bool no_focus = false;
		bool window_has_focus = false;
		bool exclusive = false;
		bool context_created = false;

		// Timers.
		uint32_t move_timer_id = 0U;
		uint32_t focus_timer_id = 0U;

		int min_pressure;
		int max_pressure;
		bool tilt_supported;
		bool pen_inverted = false;
		bool block_mm = false;

		int last_pressure_update;
		float last_pressure;
		Vector2 last_tilt;
		bool last_pen_inverted = false;

		Size2 min_size;
		Size2 max_size;
		int width = 0, height = 0;

		Size2 window_rect;
		Point2 last_pos;

		ObjectID instance_id;

		bool layered_window = false;

		Callable rect_changed_callback;
		Callable event_callback;
		Callable input_event_callback;
		Callable input_text_callback;
		Callable drop_files_callback;

		WindowID transient_parent = INVALID_WINDOW_ID;
		HashSet<WindowID> transient_children;

		bool is_popup = false;
		Rect2i parent_safe_rect;
	};

	WindowID _window_id_counter = MAIN_WINDOW_ID;
	RBMap<WindowID, WindowData> _windows;

	String rendering_driver;
	BitField<MouseButtonMask> last_button_state;

	// update flag
	bool main_loop_valid = false;
	bool in_dispatch_input_event = false;

	List<WindowID> _popup_list;

	struct KeyEvent {
		WindowID window_id;
		bool alt, shift, control, meta;
		bool pressed;
		int unicode;
		int keycode;
		bool extended_key = false;
	};

	KeyEvent _key_event_buffer[512];
	int _key_event_pos = 0;

	bool _old_invalid;
	int _old_x, _old_y;

	MouseMode _mouse_mode = MOUSE_MODE_VISIBLE;
	bool _alt_mem = false;
	bool _gr_mem = false;
	bool _shift_mem = false;
	bool _control_mem = false;
	bool _meta_mem = false;
	bool _drop_events = false;

	CursorShape _cursor_shape = CursorShape::CURSOR_ARROW;
	RBMap<CursorShape, Vector<Variant>> cursors_cache;



private:

	void _set_mouse_mode_impl(MouseMode p_mode);

	void _process_key_events();
	void _mouse_button_event(QMouseEvent* p_event, bool p_mouse_down, bool p_double_click = false);
	void _key_event(QKeyEvent* p_event, bool p_key_down);


protected:
	virtual void activateEvent(QEvent *p_event);
	virtual void enterEvent(QEvent *p_event);
	virtual void leaveEvent(QEvent *p_event);
	virtual void mouseMoveEvent(QMouseEvent *p_event) override;
	virtual void mousePressEvent(QMouseEvent *p_event) override;
	virtual void mouseReleaseEvent(QMouseEvent *p_event) override;
	virtual void mouseDoubleClickEvent(QMouseEvent *p_event) override;
	virtual void wheelEvent(QWheelEvent *p_event) override;
	virtual void keyPressEvent(QKeyEvent *p_event) override;
	virtual void keyReleaseEvent(QKeyEvent *p_event) override;
	virtual void dragEnterEvent(QDragEnterEvent *p_event) override;
	virtual void dropEvent(QDropEvent *p_event) override;
	virtual void resizeEvent(QResizeEvent *p_event) override;
	virtual void closeEvent(QCloseEvent *p_event) override;

	/* Overriding this method prevents the
	 * "QWidget::paintEngine: Should no longer be called" error. */
	QPaintEngine *paintEngine() const override;

	void _send_window_event(const WindowData& wd, WindowEvent p_event);

	static void _dispatch_input_events(const Ref<InputEvent> &p_event);
	void _dispatch_input_event(const Ref<InputEvent> &p_event);

public:
	virtual bool has_feature(Feature p_feature) const override;
	virtual String get_name() const override;

	virtual void mouse_set_mode(MouseMode p_mode) override;

	virtual Point2i mouse_get_position() const override;
	virtual BitField<MouseButtonMask> mouse_get_button_state() const override;

	virtual int get_screen_count() const override;
	virtual int get_primary_screen() const override;
	virtual Point2i screen_get_position(int p_screen = SCREEN_OF_MAIN_WINDOW) const override;
	virtual Size2i screen_get_size(int p_screen = SCREEN_OF_MAIN_WINDOW) const override;
	virtual Rect2i screen_get_usable_rect(int p_screen = SCREEN_OF_MAIN_WINDOW) const override;
	virtual int screen_get_dpi(int p_screen = SCREEN_OF_MAIN_WINDOW) const override;
	virtual float screen_get_refresh_rate(int p_screen = SCREEN_OF_MAIN_WINDOW) const override;

	virtual Vector<DisplayServer::WindowID> get_window_list() const override;
	virtual WindowID create_sub_window(WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Rect2i& p_rect = Rect2i()) override;
	virtual void show_window(WindowID p_window) override;

	virtual void window_attach_instance_id(ObjectID p_instance, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual ObjectID window_get_attached_instance_id(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual void window_set_rect_changed_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;

	virtual void window_set_window_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_set_input_event_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_set_input_text_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;

	virtual void window_set_drop_files_callback(const Callable &p_callable, WindowID p_window = MAIN_WINDOW_ID) override;

	virtual void window_set_title(const String &p_title, WindowID p_window = MAIN_WINDOW_ID) override;

	virtual int window_get_current_screen(WindowID p_window = MAIN_WINDOW_ID) const override;
	virtual void window_set_current_screen(int p_screen, WindowID p_window = MAIN_WINDOW_ID) override;

	virtual Point2i window_get_position(WindowID p_window = MAIN_WINDOW_ID) const override;
	virtual Point2i window_get_position_with_decorations(WindowID p_window = MAIN_WINDOW_ID) const override;
	virtual void window_set_position(const Point2i &p_position, WindowID p_window = MAIN_WINDOW_ID) override;

	virtual void window_set_transient(WindowID p_window, WindowID p_parent) override;

	virtual WindowID get_window_at_screen_position(const Point2i &p_position) const override;

	virtual void window_set_max_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual Size2i window_get_max_size(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual void window_set_min_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual Size2i window_get_min_size(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual void window_set_size(const Size2i p_size, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual Size2i window_get_size(WindowID p_window = MAIN_WINDOW_ID) const override;
	virtual Size2i window_get_size_with_decorations(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual void window_set_mode(WindowMode p_mode, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual WindowMode window_get_mode(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual bool window_is_maximize_allowed(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual void window_set_flag(WindowFlags p_flag, bool p_enabled, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual bool window_get_flag(WindowFlags p_flag, WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual void window_request_attention(WindowID p_window = MAIN_WINDOW_ID) override;
	virtual void window_move_to_foreground(WindowID p_window = MAIN_WINDOW_ID) override;

	virtual bool window_can_draw(WindowID p_window = MAIN_WINDOW_ID) const override;

	virtual bool can_any_window_draw() const override;

	virtual void window_set_vsync_mode(DisplayServer::VSyncMode p_vsync_mode, WindowID p_window = MAIN_WINDOW_ID) override;
	virtual DisplayServer::VSyncMode window_get_vsync_mode(WindowID p_vsync_mode) const override;

	virtual void cursor_set_shape(CursorShape p_shape) override;
	virtual CursorShape cursor_get_shape() const override;

	virtual bool get_swap_cancel_ok() override;
	virtual void process_events() override;

	static DisplayServer *create_func(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error);
	static Vector<String> get_rendering_drivers_func();
	static void register_widget_driver();

	DisplayServerWidgetWindows(const String &p_rendering_driver, WindowMode p_mode, VSyncMode p_vsync_mode, uint32_t p_flags, const Vector2i *p_position, const Vector2i &p_resolution, int p_screen, Error &r_error, QWidget *parent = NULL);
	virtual ~DisplayServerWidgetWindows();
};

#endif
