/* MindGuard v 0.0.0.5 header */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>

#include "mg.xpm"
#include "lz.xpm"

/* Maybe I should organize these... */
void reload_mods(void);
void initialize_jam(void);
void time_string (char *);
void set_up_colors(void);
void context_name(char *, int);
void source_name(char *, int);
void context_name_label(char *, int);
void decipher(char *, char *, int, int);
int rs(int);
int dir_exists(char *);
int quit (void);
void quit_2 (void);
void open_main_window(void);
void open_psident_window(void);
void close_psident_window(void);
void open_about_window(void);
void close_about_window(void);
void open_depsych_window(void);
void close_depsych_window(void);
void popup_dialog(char *, char *, char *, void (*) (void), void (*) (void), int);
void open_webpage(GtkWidget *, int);
void load_prefs(void);
void save_prefs(void);
void update_gui_from_prefs(void);
int load_mods(void);
int chars_value(char *c, int n);
void add_mods_to_clist(void);
void sort_mods(void);
int parse_mod(char *, int);
void depsych(void);
void update_username_eponymolog(void);
void update_eponymolog(void);
int eponymolog(void);
int days_since(void);
int ratadie(int, int, int);
void (*pcheck) (unsigned int);
void create_biorhythm(GtkWidget *);
void draw_biorhythm(GtkWidget *, GdkEventExpose *);
void update_biorhythm(void);
void day_adjust(GtkAdjustment *, GtkWidget *);
void month_adjust(GtkWidget *, int);
void year_adjust(GtkAdjustment *, GtkWidget *);
void update_day_spin(void);
int days_in_month(int, int);
void algo_adjust(GtkWidget *, int);
void scan_mode_adjust(GtkWidget *, int);
void scan_depth_adjust(GtkAdjustment *);
void diag_display(int, char *, int);
gint diag_display_clear(gpointer);
void diag_set_clear_to(int);
void scanbox_label(char *);
void scanbox_prog(float);
void scanbox_state(int);
void create_scan(GtkWidget *);
void update_scan();
void append_log(char *, int, int, int, char *, int, int);
gint jam(gpointer);
