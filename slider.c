#include "slider.h"

G_DEFINE_TYPE(Slider, slider, CLUTTER_TYPE_ACTOR);

#define SLIDER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TYPE_SLIDER, SliderPrivate))

struct _SliderPrivate {
    ClutterActor *box;
    ClutterActor *handle;
    ClutterAction *track_clicked;
    ClutterAction *handle_dragged;
    gfloat        pos;
};

enum {
    PROP_0,
    PROP_POS
};

enum {
    SLID,
    LAST_SIGNAL
};

static guint slider_signals[LAST_SIGNAL] = {0,};

static void slider_finalize(GObject *gobject) {
    // Memory freeing goes here
    G_OBJECT_CLASS(slider_parent_class)->finalize(gobject);
}

static void slider_set_property(
        GObject *gobject, guint prop_id, const GValue *value,
        GParamSpec *pspec) {
    Slider *slider = SLIDER(gobject);

    switch (prop_id) {
        case PROP_POS:
            slider_set_pos(slider, g_value_get_float(value));
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);
            break;
    }
}

static void slider_get_property(
        GObject *gobject, guint prop_id, GValue *value, GParamSpec *pspec) {
    SliderPrivate *priv = SLIDER(gobject)->priv;

    switch (prop_id) {
        case PROP_POS:
            g_value_set_float(value, priv->pos);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, pspec);
            break;
    }
}

static void slider_destroy(ClutterActor *self) {
    SliderPrivate *priv = SLIDER(self)->priv;

    if (priv->box) {
        clutter_actor_destroy(priv->box);
        priv->box = NULL;
        clutter_actor_destroy(priv->handle);
        priv->handle = NULL;
    }

    if (CLUTTER_ACTOR_CLASS(slider_parent_class)->destroy)
        CLUTTER_ACTOR_CLASS(slider_parent_class)->destroy(self);
}

static void slider_get_preferred_height(
        ClutterActor *self, gfloat for_width,
        gfloat *min_height, gfloat *natural_height) {
    *min_height = 200.0;
    *natural_height = 200.0;
}

static void slider_get_preferred_width(
        ClutterActor *self, gfloat for_height,
        gfloat *min_width, gfloat *natural_width) {
    *min_width = 40.0;
    *natural_width = 40.0;
}

static void slider_allocate(
        ClutterActor *actor, const ClutterActorBox *box,
        ClutterAllocationFlags flags) {
    CLUTTER_ACTOR_CLASS(slider_parent_class)->allocate(actor, box, flags);
    SliderPrivate *priv = SLIDER(actor)->priv;
    const gfloat box_height = clutter_actor_box_get_height(box);
    ClutterActorBox child_box = {
        .x1 = 0.0, .y1 = 0.0,
        .x2=clutter_actor_box_get_width(box),
        .y2=box_height};
    clutter_actor_allocate(priv->box, &child_box, flags);
    child_box.y1 = box_height * (priv->pos - 0.05);
    child_box.y2 = box_height * (priv->pos + 0.05);
    clutter_actor_allocate(priv->handle, &child_box, flags);
}

static void slider_paint(ClutterActor *actor) {
    SliderPrivate *priv = SLIDER(actor)->priv;
    clutter_actor_paint(priv->box);
    clutter_actor_paint(priv->handle);
}

static void slider_clicked(
        ClutterClickAction *action, ClutterActor *actor, gpointer user_data) {
    float x, y;
    clutter_click_action_get_coords(action, &x, &y);
    clutter_actor_transform_stage_point(actor, x, y, &x, &y);
    Slider* slider = SLIDER(user_data);
    slider_set_pos(slider, y/200);
}

static void handle_dragged(
        ClutterDragAction *action, ClutterActor *actor, gfloat dx, gfloat dy,
        gpointer user_data) {
    Slider* slider = SLIDER(user_data);
    gfloat const new_pos = slider_get_pos(slider) + (dy/200);
    slider_set_pos(slider, new_pos);
}

static gboolean scrolled(
        ClutterActor *actor, ClutterEvent *event, gpointer user_data) {
    ClutterScrollDirection direction =
        clutter_event_get_scroll_direction(event);
    Slider* slider = SLIDER(user_data);
    switch (direction) {
        case CLUTTER_SCROLL_UP:
            slider_set_pos(slider, slider_get_pos(slider) - 0.1);
            break;
        case CLUTTER_SCROLL_DOWN:
            slider_set_pos(slider, slider_get_pos(slider) + 0.1);
            break;
        default:
            break;
    }
    return CLUTTER_EVENT_STOP;
}

static void slider_class_init(SliderClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);

    gobject_class->finalize = slider_finalize;
    gobject_class->set_property = slider_set_property;
    gobject_class->get_property = slider_get_property;

    ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS(klass);
    actor_class->destroy = slider_destroy;
    actor_class->get_preferred_height = slider_get_preferred_height;
    actor_class->get_preferred_width = slider_get_preferred_width;
    actor_class->allocate = slider_allocate;
    actor_class->paint = slider_paint;

    g_type_class_add_private(klass, sizeof(SliderPrivate));

    GParamSpec* pspec = g_param_spec_float(
        "position", "pos", "where it's at", 0.0, 1.0, 0.5, G_PARAM_READWRITE);
    g_object_class_install_property(gobject_class, PROP_POS, pspec);

    slider_signals[SLID] = g_signal_new(
        "slid", G_TYPE_FROM_CLASS(klass), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(SliderClass, clicked), NULL, NULL,
        g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 1, G_TYPE_FLOAT);
}

static void slider_init(Slider *self) {
    SliderPrivate *priv;
    ClutterLayoutManager *layout;
    priv = self->priv = SLIDER_GET_PRIVATE(self);
    priv->pos = 0.5;

    layout = clutter_bin_layout_new(
        CLUTTER_BIN_ALIGNMENT_CENTER, CLUTTER_BIN_ALIGNMENT_CENTER);

    ClutterColor col = {127, 127, 127, 255};
    priv->box = clutter_actor_new();
    clutter_actor_set_background_color(priv->box, &col);
    clutter_actor_set_layout_manager(priv->box, layout);
    clutter_actor_set_reactive(CLUTTER_ACTOR(priv->box), TRUE);
    clutter_actor_add_child(CLUTTER_ACTOR(self), priv->box);

    col.red = 40;
    col.green = 40;
    col.blue = 70;
    priv->handle = clutter_actor_new();
    clutter_actor_set_background_color(priv->handle, &col);
    clutter_actor_set_layout_manager(priv->handle, layout);
    clutter_actor_set_reactive(CLUTTER_ACTOR(priv->handle), TRUE);
    clutter_actor_add_child(CLUTTER_ACTOR(self), priv->handle);

    priv->track_clicked = clutter_click_action_new();
    clutter_actor_add_action(CLUTTER_ACTOR(priv->box), priv->track_clicked);
    g_signal_connect(
        priv->track_clicked, "clicked", G_CALLBACK(slider_clicked), self);

    priv->handle_dragged = clutter_drag_action_new();
    clutter_actor_add_action(CLUTTER_ACTOR(priv->handle), priv->handle_dragged);
    g_signal_connect(
        priv->handle_dragged, "drag-motion", G_CALLBACK(handle_dragged), self);

    g_signal_connect(priv->box, "scroll-event", G_CALLBACK(scrolled), self);
    g_signal_connect(priv->handle, "scroll-event", G_CALLBACK(scrolled), self);
}

void slider_set_pos(Slider *self, gfloat const pos) {
    g_return_if_fail(IS_SLIDER(self));
    self->priv->pos = (pos > 1) ? 1 : (pos < 0) ? 0 : pos;
    g_signal_emit(self, slider_signals[SLID], 0, self->priv->pos);
    clutter_actor_queue_relayout(CLUTTER_ACTOR(self));
}

gfloat slider_get_pos(Slider const * const self) {
    g_return_if_fail(IS_SLIDER(self));
    return self->priv->pos;
}

ClutterActor* slider_new() {
    return g_object_new(TYPE_SLIDER, NULL);
}
