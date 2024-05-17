#include <allegro.h>
#include "game.h"
#include "regles.h"

int main() {


    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(32);
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, LARGEUR_ECRAN, HAUTEUR_ECRAN, 0, 0);

    afficherMenu();

    allegro_exit();
    return 0;
}
END_OF_MAIN()

