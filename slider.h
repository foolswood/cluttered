#pragma once

#include <clutter/clutter.h>

GType slider_get_type();

#define TYPE_SLIDER (slider_get_type())
#define SLIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_SLIDER, Slider))
#define IS_SLIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), TYPE_SLIDER))
#define SLIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), TYPE_SLIDER, Slider))
#define IS_SLIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), TYPE_SLIDER))
#define SLIDER_GET_CLASS(obj) \
    (G_TYPE_INSTANCE_GET_CLASS((obj), TYPE_SLIDER, Slider))


typedef struct _SliderPrivate SliderPrivate;
typedef struct _Slider Slider;
typedef struct _SliderClass SliderClass;

struct _Slider {
    ClutterActor parent_instance;
    SliderPrivate *priv;
};

struct _SliderClass {
    void (*clicked)(Slider *slider);
    ClutterActorClass parent_class;
};

ClutterActor *slider_new();

void slider_set_pos(Slider *self, const gfloat val);
gfloat slider_get_pos(Slider const * const self);
