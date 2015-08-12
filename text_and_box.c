#include <clutter/clutter.h>
#include <cairo.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

static gboolean on_clicked(ClutterClickAction *action, ClutterActor *actor) {
    clutter_actor_hide(actor);
    return TRUE;
}

static gboolean draw_circle(ClutterCanvas *canvas, cairo_t *cr, gint width, gint height, gpointer user_data) {
    cairo_set_line_width(cr, 4);
    cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
    cairo_translate(cr, width/2, height/2);
    cairo_arc(cr, 0, 0, 50, 0, 2 * M_PI);
    cairo_stroke_preserve(cr);
    cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
    cairo_fill(cr);
    return TRUE;
}

int main(int argc, char *argv[]) {
    if (clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS)
        return EXIT_FAILURE;

    ClutterActor *stage = clutter_stage_new();
    ClutterColor stage_color = {0, 0, 0, 255};
    clutter_actor_set_background_color(stage, &stage_color);
    clutter_actor_set_size(stage, 512, 512);
    clutter_actor_show(stage);

    ClutterActor *rect = clutter_actor_new();
    ClutterColor rect_color = {0, 0, 255, 255};
    clutter_actor_set_background_color(rect, &rect_color);
    clutter_actor_set_size(rect, 100, 100);
    clutter_actor_set_position(rect, 100, 100);
    clutter_actor_add_child(stage, rect);
    clutter_actor_show(rect);

    ClutterActor *txt = clutter_text_new_with_text("dejavu-sans 30", "Aloha");
    clutter_text_set_editable(CLUTTER_TEXT(txt), TRUE);
    clutter_text_set_color(CLUTTER_TEXT(txt), &rect_color);
    clutter_actor_add_child(stage, txt);
    clutter_actor_show(txt);

    ClutterAction *clicked = clutter_click_action_new();
    clutter_actor_add_action(rect, clicked);
    clutter_actor_set_reactive(rect, TRUE);
    g_signal_connect(clicked, "clicked", G_CALLBACK(on_clicked), NULL);

    ClutterContent *canvas = clutter_canvas_new();
    clutter_canvas_set_size(CLUTTER_CANVAS(canvas), 200, 200);
    g_signal_connect(canvas, "draw", G_CALLBACK(draw_circle), NULL);
    clutter_actor_set_content(rect, canvas);
    clutter_actor_set_content_scaling_filters(rect, CLUTTER_SCALING_FILTER_TRILINEAR, CLUTTER_SCALING_FILTER_LINEAR);
    g_object_unref(canvas);
    clutter_content_invalidate(canvas);

    clutter_main();

    return EXIT_SUCCESS;
}
