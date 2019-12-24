#include <stdio.h>
#include <stdlib.h>

#include <xcb/xcb.h>

int main() {
    // 屏幕下標
    int screen_index;

    // 連接 X server
    xcb_connection_t *conn = xcb_connect(nullptr, &screen_index);

    // 根據屏幕下標獲取屏幕
    const xcb_setup_t *setup = xcb_get_setup(conn);
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(setup);

    for (int i = 0; i < screen_index; i++) {
        xcb_screen_next(&screen_iter);
    }

    xcb_screen_t *screen = screen_iter.data;

    // 參數
    uint32_t mask;
    uint32_t values[10];

    // 創建窗口
    xcb_window_t window = xcb_generate_id(conn);

    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE;

    xcb_create_window(conn,
                      XCB_COPY_FROM_PARENT,
                      window,
                      screen->root,
                      0,   // x
                      0,   // y
                      200, // 窗口寬度
                      200, // 窗口高度
                      10,  // 邊框寬度
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      screen->root_visual,
                      mask,
                      values);

    // 將窗口 map 到 X server
    xcb_map_window(conn, window);

    // 創建黑色前景色上下文
    xcb_gcontext_t foreground = xcb_generate_id(conn);

    mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    values[0] = screen->black_pixel;
    values[1] = 0;

    xcb_create_gc(conn, foreground, screen->root, mask, values);

    // 沖洗數據
    xcb_flush(conn);

    // 繪圖數據
    xcb_point_t points[] = {{10, 10}, {10, 20}, {20, 10}, {20, 20}};

    xcb_point_t polyline[] = {{50, 10}, {5, 20}, {25, -20}, {10, 10}};

    xcb_segment_t segments[] = {{100, 10, 140, 30}, {110, 25, 130, 60}};

    xcb_rectangle_t rectangles[] = {{10, 50, 40, 20}, {80, 50, 10, 40}};

    xcb_arc_t arcs[] = {{10, 100, 60, 40, 0, 90 << 6},
                        {90, 100, 55, 40, 0, 270 << 6}};

    // 繪圖原語
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(conn))) {
        switch (event->response_type & ~0x80) {
        case XCB_EXPOSE:
            // 繪製點
            xcb_poly_point(conn,
                           XCB_COORD_MODE_ORIGIN,
                           window,
                           foreground,
                           4,
                           points);
            xcb_flush(conn);

            // 繪製線
            xcb_poly_line(conn,
                          XCB_COORD_MODE_PREVIOUS,
                          window,
                          foreground,
                          4,
                          polyline);
            xcb_flush(conn);

            // 繪製線段
            xcb_poly_segment(conn, window, foreground, 2, segments);
            xcb_flush(conn);

            // 繪製方形
            xcb_poly_rectangle(conn, window, foreground, 2, rectangles);
            xcb_flush(conn);

            // 繪製圓形
            xcb_poly_arc(conn, window, foreground, 2, arcs);
            xcb_flush(conn);

            break;
        default:
            break;
        }

        free(event);
    }

    return 0;
}
