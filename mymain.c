#include <stdlib.h>
#include "slider.h"

static void slid(Slider *slider, gfloat pos, gpointer data) {
    printf("Slid to %f\n", pos);
}

int main(int argc, char *argv[]) {
      if (clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
        return EXIT_FAILURE;

    ClutterActor *stage = clutter_stage_new();
    clutter_actor_set_size(stage, 400, 400);

    ClutterActor *slider = slider_new();
    clutter_actor_set_position(slider, 150, 150);
    g_signal_connect(slider, "slid", G_CALLBACK(slid), NULL);

    clutter_actor_add_child(stage, slider);
    clutter_actor_show(stage);

    clutter_main();
    return EXIT_SUCCESS;
}
