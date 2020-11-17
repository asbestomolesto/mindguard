/* MindGuard X - v 0.0.0.5
 * Personal anti-psychotronic mind-control software.
 *
 * Copyright (C) 1999-2003 Lyle Zapato
 * <lyle@zapatopi.net>
 * <zoam@juno.com>
 *
 * Updates may be found at:
 *   http://zapatopi.net/mindguard/
 *   https://github.com/asbestomolesto/mindguard
 *
 * This program is free anti-mind-control software;
 * you can redistribute it and/or modify it under
 * the terms of the MindGuard Public License as
 * published by Zapato Productions Intradimensional,
 * or its founder Lyle Zapato; the most recent
 * version of the License applies.
 *
 * This software is provided AS IS with NO WARRANTY
 * of ANY SORT WHATSOEVER as to its MERCHANTABILITY
 * or FITNESS, actual or PERCEIVED, for any PURPOSE,
 * PARTICULAR OR GENERAL, or for PURPOSE RELATED
 * PURPOSES (hereafter referred to as METAPURPOSES);
 * nor can this VEHEMENT DENIAL of WARRANTY in any
 * way BE CONSTRUED or TAKEN TO IMPLY evidence of a
 * SECRET WARRANTY, the EXISTENCE of which is being
 * COVERED UP for NEFARIOUS PURPOSES or METAPURPOSES.
 * See the MindGuard Public License for details.
 *
 * If you did not receive a copy of the MindGuard
 * Public License along with this program, you may
 * find a copy at:
 *
 *   http://zapatopi.net/mgpl.html
 */

#include "mindguard.h"

#define GTK_WINDOW_DIALOG GTK_WINDOW_TOPLEVEL

char mg_version[] = "0.0.0.5";
char mg_year[] = "2020";

/* Webbrowser & email commands. %s will be replaced with URL.
   This would be better if it could detect your prefered browser/email
   program instead of having it hard-coded here. How would one do that? */
/* 
char webpage_command[] = "netscape -remote openURL\\(%s\\)";
char email_command[] = "netscape -remote mailto\\(%s\\)";
*/
char webpage_command[] = "xdg-open %s";
char email_command[] = "xdg-open %s";


char user_name_data[256];
char *user_name = &user_name_data[0];
int bday, bmonth, byear, max, algo, scan_mode, scan_depth;
int state_silent, state_auto, state_log, state_bio, state_epo,
	center_windows, save_on_exit, ask_quit,
	phase_check, dopplerator, kill_feedback;

int to_diag_clear = FALSE;
int to_scan = FALSE;
int to_jam = FALSE;
int to_depsych = FALSE;
int to_monitor = FALSE;
int gcar, gmode;

GdkGC *bio_gc[3];
GdkGC *depsych_gc[16];
GtkStyle *style_web;
GtkStyle *style_ok;
GtkStyle *style_warn;

/* Main Window */
GtkWidget *window;
GtkWidget *but_scan;
GtkWidget *but_logview;
GtkWidget *but_about;
GtkWidget *but_psid;
GtkWidget *check_silent;
GtkWidget *check_auto;
GtkWidget *check_log;
GtkWidget *check_soe;
GtkWidget *check_aq;
GtkWidget *check_center;
GtkWidget *check_phase;
GtkWidget *check_doppler;
GtkWidget *check_feedback;
GtkWidget *opt_algo;
GtkWidget *opt_scan_mode;
GtkWidget *carlist;
GtkObject *adj_scan;
GtkWidget *label_diag[5];
GtkWidget *scan_label;
GtkWidget *scan_progress;
GtkWidget *scan_draw;

/* PsIdent Window */
GtkWidget *window_psid;
GtkWidget *check_bio;
GtkWidget *check_epo;
GtkWidget *entry_epo;
GtkWidget *label_epo;
GtkWidget *draw_bio;
GtkWidget *spin_day;
GtkWidget *spin_year;
GtkWidget *opt_month;
GtkAdjustment *adjust_day;

/* Other Windows */
GtkWidget *window_depsych;
GtkWidget *draw_depsych;
GtkWidget *window_about;
GtkWidget *window_logview;
GtkWidget *text_log;

static GdkPixmap *pixmap_bio = NULL;
static GdkPixmap *pixmap_scan = NULL;
static GdkPixmap *pixmap_depsych = NULL;

typedef enum
{
	CHECK_AUTO,
	CHECK_SILENT,
	CHECK_PHASE,
	CHECK_FEEDBACK,
	CHECK_DOPPLER,
	CHECK_BIO,
	CHECK_EPO,
	CHECK_LOG,
	CHECK_CENTER,
	CHECK_ATQ,
	CHECK_SOE
} CheckId;

typedef enum
{
	DIAG_BLACK,
	DIAG_WARN,
	DIAG_OK,
	DIAG_CALM
} DiagnosticBoxColor;

typedef enum
{
	SCANBOX_PROGRESS,
	SCANBOX_DRAW,
	SCANBOX_LABEL,
	SCANBOX_CLEAR
} ScanBoxId;

typedef enum
{
	DIALOG_QUIT   = 1 << 0,	/* quit program on closing the dialog with wm */
	DIALOG_MODAL  = 1 << 1,	/* window is modal */
	DIALOG_WARN   = 1 << 2,	/* red text */
	DIALOG_BEEP   = 1 << 3,	/* make an annoying beep */
	DIALOG_CENTER = 1 << 4,	/* center in screen */
	DIALOG_MOUSE  = 1 << 5	/* center on mouse */
} PDOptions;

typedef enum
{
	MODLOAD_WORKING,
	MODLOAD_SUCCESS,
	MODLOAD_NODIR,
	MODLOAD_NOMODS
} ModuleLoadErrors;

struct carmod
{
	char file[256];
	char name[256];
	char version[256];
	char copyright[256];
	int layer;
	int encrypt;
	int syntax;
	int phase;
};

struct carmod module[20];
int mod_num = 0;
char car_path[256];



/**** MAIN AND QUIT ****/
int main (int argc, char *argv[])
{
	int i, quit = FALSE;

	/* DEFAULTS */
	strcpy(car_path, "./carriers/");
	strcpy(user_name_data, "");
	bday = 1; bmonth = 1; byear = 1900;
	algo = 0; scan_mode = 0; scan_depth = 50;
	state_silent   = FALSE;
	state_auto     = TRUE;
	state_log      = FALSE;
	state_bio      = FALSE;
	state_epo      = FALSE;
	phase_check    = FALSE;
	dopplerator    = FALSE;
	kill_feedback  = FALSE;
	center_windows = TRUE;
	save_on_exit   = TRUE;
	ask_quit       = TRUE;

	load_prefs();

	for (i = 1; i < argc; i++)
		{
		if (!strcmp(argv[i],"-version"))
			{ printf("MindGuard X version %s\n", mg_version); quit = TRUE; }
		if (!strcmp(argv[i],"-user_name"))
			{ i++; strcpy(user_name_data, argv[i]); state_epo = TRUE;}
		if (!strcmp(argv[i],"-user_bdate"))
			{ i++; sscanf(argv[i],"%d-%d-%d",&byear,&bmonth,&bday); state_bio = TRUE;}
		if (!strcmp(argv[i],"-silent")) state_silent = TRUE;
		if (!strcmp(argv[i],"-phase_check")) phase_check = TRUE;
		if (!strcmp(argv[i],"-dopplerator")) dopplerator = TRUE;
		if (!strcmp(argv[i],"-kill_feedback")) kill_feedback = TRUE;
		if (!strcmp(argv[i],"-carrier_path"))
			{
			i++;
			if (dir_exists(argv[i])) strcpy(car_path, argv[i]);
			else printf("ERROR: %s doesn't exist or isn't a directory."
								"Using defaults.\n", argv[i]);
			if (car_path[strlen(car_path)-1]!='/') strcat(car_path,"/");
			}
		}

	if (quit==TRUE) return 0;

	gtk_init (&argc, &argv);

	carlist = gtk_clist_new (1);

	switch (load_mods())
		{
		case MODLOAD_NODIR:
			popup_dialog("ERROR:\n"
			"Couldn't find a carrier module dir\n"
			"in the default locations. Specify\n"
			"one using the command line option:\n"
			"-carrier_path",
			"Okay", NULL, quit_2, NULL, DIALOG_QUIT | DIALOG_MODAL);
			break;
		case MODLOAD_NOMODS:
			popup_dialog("ERROR:\n"
			"You have no carrier modules or\n"
			"you installed them improperly!\n"
			"Please consult the documentation for help.",
			"Okay", NULL, quit_2, NULL, DIALOG_QUIT | DIALOG_MODAL);
			break;
		case MODLOAD_WORKING:
			popup_dialog("ERROR:\n"
			"Unknown carrier module error\n"
			"Please consult the documentation for help.",
			"Okay", NULL, quit_2, NULL, DIALOG_QUIT | DIALOG_MODAL);
			break;
		default:
			open_main_window();
			set_up_colors();
			diag_display(1, "Welcome to MindGuard", 0);
			if (state_epo)
				{
				if (strlen(user_name) == 0)
					{
					state_epo = FALSE;
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (check_epo), FALSE);
					}
				else
					{
					diag_display(3, "Optimized for:", 0);
					diag_display(4, user_name, 0);
					}
				}
			initialize_jam();
			to_monitor = gtk_timeout_add(5000, jam, GINT_TO_POINTER(2));
			diag_set_clear_to(15);
		}

	gtk_main();

	return 0;
}

int quit ()
{
	if (ask_quit)
		popup_dialog("Are you sure?\nYou will be vulnerable to\npsychotronic manipulation.",
				"Cancel", "Quit", NULL, quit_2, DIALOG_MODAL | DIALOG_MOUSE);
	else quit_2();

	return TRUE; /* Keeps the WM from destroying our loverly window too soon. */
}
void quit_2 () /* call this for forced bypass of ask_quit */
{
	if (save_on_exit) save_prefs();

	gtk_main_quit();

	return;
}


/**** SCAN ****/
int rs(int h)
{
	/*
	Although conveniently left out of most C textbooks, it's
	a well-known fact in the anti-mind-control community
	that the rand() function -- originally developed by the
	RAND Corporation for use in its political mind-control
	research -- interacts with a computer's underlying
	circuitry in complexly emergent ways, making the
	function useful for mind-control detection due to the
	quantum	effects of psychotronics on aluminum atoms.  A
	side-effect of this is that it also works well for
	pseudo-stochastic number generation, something many hack
	programmers ignorantly use it for.
	*/

	return (rand() / (RAND_MAX / h));
}
void scan_controls_sensitive(int state)
{
	gtk_widget_set_sensitive(GTK_WIDGET(but_scan), state);
	gtk_widget_set_sensitive(GTK_WIDGET(check_silent), state);
	gtk_widget_set_sensitive(GTK_WIDGET(opt_scan_mode), state);
	return;
}
int carrier_identify()
{
	int car;
	car = rs(mod_num);
	return car;
}
void signal_info(char *infotext)
{
	char t[256];

	sprintf(t, "%i.%i%i", rs(9),rs(9),rs(9));
	strcpy(infotext, t);
	if (rs(1))
		strcat(infotext, "kHz ");
	else
		strcat(infotext, "MHz ");
	sprintf(t, "%i.%i%i", rs(9),rs(9),rs(9));
	strcat(infotext, t);
	if (rs(1))
		strcat(infotext, "mm");
	else
		strcat(infotext, "nm");

	return;
}
void parse_decipher(int car, int mode)
{
	int context, index, source;
	char text[256], infotext[256];

	context = rs(4);
	switch (context)
		{
		case 0:
			index = rs(module[car].layer);
			break;
		case 1:
			index = rs(module[car].encrypt);
			break;
		case 2:
			index = rs(module[car].syntax);
			break;
		case 3:
			index = rs(module[car].phase);
			break;
		}
	source = rs(5);

	diag_display_clear(NULL);
	strcpy(text, "Carrier: ");
	strcat(text, module[car].name);
	diag_display(0, text, 0);


	strcpy(text, "Info: ");
	signal_info(infotext);
	strcat(text, infotext);
	diag_display(1, text, 0);

	strcpy(text, "Context: ");
	context_name(text, context);
	diag_display(2, text, 0);
	strcpy(text, "Source: ");
	source_name(text, source);
	diag_display(3, text, 0);
	diag_set_clear_to(30);

	decipher(text, module[car].file, context, index);

	if (state_log)
		append_log (text, car, context, source, infotext, TRUE, gmode);

	context_name_label(text, context);

	popup_dialog(text, "Ok", NULL, NULL, NULL, DIALOG_BEEP | DIALOG_MOUSE);
	return;
}
gint decipher_check(gpointer data)
{
	static float c = 0.0;

	//if ((int)data == 1)
  if (GPOINTER_TO_INT(data) == 1)
		{
		gtk_timeout_remove(to_scan);
		diag_display(1, "Deciphering...", 0);
		to_scan = gtk_timeout_add (2, decipher_check, GINT_TO_POINTER(2));
		}
	//else if ((int)data == 2)
	else if (GPOINTER_TO_INT(data) == 2)
		{
		scanbox_prog(c);
		c = c + 0.005;
		if (c > 1.0)
			{
			gtk_timeout_remove(to_scan);
			c = 0.0;
			scanbox_state(SCANBOX_CLEAR);
			if (gmode == 3)
				gcar = carrier_identify();
			if (rs(100) > 20)
				parse_decipher(gcar, gmode);
			else
				{
				diag_display(2, "Undecipherable", 0);
				diag_set_clear_to(10);
				if (state_log)
					append_log ("", gcar, 0, 0, "", FALSE, gmode);
				}
			to_scan = FALSE;
			scan_controls_sensitive(TRUE);
			if (! to_jam)
				to_monitor = gtk_timeout_add (1000, jam, GINT_TO_POINTER(2));
			}
		}

	return TRUE;
}
gint scan_for_signal(gpointer data)
{
	static int i = 0;

	//if ((int)data == 1)
  if (GPOINTER_TO_INT(data) == 1)
		{
		diag_display_clear(NULL);
		diag_display(0, "Isolating Signal", 0);
		gtk_timeout_remove(to_scan);
		to_scan = gtk_timeout_add (100, scan_for_signal, GINT_TO_POINTER(3));
		}
	//if ((int)data == 2)
	if (GPOINTER_TO_INT(data) == 2)
		{
		gmode = 3;
		to_scan = gtk_timeout_add (100, scan_for_signal, GINT_TO_POINTER(3));
		}
	//if ((int)data == 3)
	if (GPOINTER_TO_INT(data) == 3)
		{
		update_scan();
		i++;
		if (i > 50)
			{
			i = 0;
			gtk_timeout_remove(to_scan);
			to_scan = FALSE;
			scanbox_state(SCANBOX_CLEAR);
			if (gmode == 3)
				{
				if (rs(100) > 20)
					{
					diag_display_clear(NULL);
					diag_display(0, "Signal Isolated", 0);
					to_scan = gtk_timeout_add (2000, decipher_check,
									GINT_TO_POINTER(1));
					}
				else
					{
					diag_display_clear(NULL);
					scanbox_label("No Signals");
					scan_controls_sensitive(TRUE);
					if (! to_jam)
						to_monitor = gtk_timeout_add (1000, jam,
									GINT_TO_POINTER(2));
					}
				}
			else
				to_scan = gtk_timeout_add (2000, decipher_check, GINT_TO_POINTER(1));
			}
		}
	return TRUE;
}
void scan()
{
	if (! to_scan & ! to_jam)
		{
		scan_controls_sensitive(FALSE);
		diag_display_clear(NULL);
		diag_display(0, "Scanning...", 0);
		scan_for_signal(GINT_TO_POINTER(2));
		}
	return;
}


/**** SCAN BOX ****/
void scanbox_state(int state)
{
	switch(state)
		{
		case SCANBOX_PROGRESS:
			gtk_widget_hide (scan_draw);
			gtk_widget_hide (scan_label);
			gtk_widget_show (scan_progress);
			break;
		case SCANBOX_DRAW:
			gtk_widget_hide (scan_progress);
			gtk_widget_hide (scan_label);
			gtk_widget_show (scan_draw);
			break;
		case SCANBOX_LABEL:
			gtk_widget_hide (scan_draw);
			gtk_widget_hide (scan_progress);
			gtk_widget_show (scan_label);
			break;
		case SCANBOX_CLEAR:
			gtk_widget_hide (scan_draw);
			gtk_widget_hide (scan_progress);
			gtk_widget_hide (scan_label);
			break;
		}
	return;
}
void scanbox_label(char *text)
{
	gtk_label_set(GTK_LABEL(scan_label), text);
	scanbox_state (SCANBOX_LABEL);
	return;
}
void scanbox_prog(float i)
{
	if (i > 1.0) i = 1.0;
	gtk_progress_bar_update(GTK_PROGRESS_BAR(scan_progress), (float)i);
	scanbox_state (SCANBOX_PROGRESS);
	return;
}
void create_scan(GtkWidget *widget)
{
	int w, h, x, y, r;

	scanbox_state (SCANBOX_DRAW);
	w = widget->allocation.width;
	h = widget->allocation.height;

	if (pixmap_scan)
		gdk_pixmap_unref(pixmap_scan);

	pixmap_scan = gdk_pixmap_new (widget->window, w, h, -1);

	gdk_draw_rectangle (pixmap_scan, widget->style->bg_gc[GTK_WIDGET_STATE(widget)],
			TRUE, 0, 0, w, h);

	y = rs(h);
	r = rs(h) + 1;
	for (x = 0; x < w; x++)
		{

		switch (scan_mode)
			{
			case 5:
				y = rs(h / (rs(r) + 1));
				gdk_draw_line (pixmap_scan, widget->style->black_gc,
				x, 0, x, y);
				break;
			case 6:	/* Drunks are easily affected by psionic mind-control. */
				if (rs(10) < 5) y++;
				else y--;
				if (y > h) y = h;
				if (y < 0) y = 0;
				gdk_draw_point (pixmap_scan, widget->style->black_gc,
				x, y);
				break;
			default:
				y = rs(h / (rs(r) + 1));
				gdk_draw_point (pixmap_scan, widget->style->black_gc,
				x, y);
				break;
			}
		}

	return;
}
void draw_scan(GtkWidget *widget, GdkEventExpose *event)
{
	gdk_draw_pixmap(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		pixmap_scan,
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
	return;
}
void update_scan()
{
	create_scan(scan_draw);
	gdk_draw_pixmap(scan_draw->window,
		scan_draw->style->fg_gc[GTK_WIDGET_STATE(scan_draw)],
		pixmap_scan, 0, 0, 0, 0,
		scan_draw->allocation.width, scan_draw->allocation.height);
	return;
}


/**** JAM/SCRAMBLE ****/
int jam_mode(int car)
{
	int mode = FALSE;
	mode = (rs(100) > 50);
	return mode;
}
int jam_success()
{
	int val = FALSE;
	val = (rs(100) != 42);
	return val;
}
void initialize_jam()
{
	time_t now;
	struct tm *tm;
	now = time(0);
	tm = localtime(&now);
	pcheck(tm->tm_sec * tm->tm_mday);
}
gint jam_action(gpointer data)
{
	char text[256];
	static float c = 0.0;
	static int car, mode;

	if (data == NULL)
		{
		diag_display_clear(NULL);
		diag_display(0, "Carrier Hit Detected.", 0);
		to_jam = gtk_timeout_add (2000, jam_action, GINT_TO_POINTER(1));
		}
	//else if ((int)data == 1)
	else if(GPOINTER_TO_INT(data) == 1)
		{
		gtk_timeout_remove(to_jam);
		car = carrier_identify();
		strcpy(text, "Carrier: ");
		strcat(text, module[car].name);
		diag_display(1, text, 0);
		to_jam = gtk_timeout_add (2000, jam_action, GINT_TO_POINTER(2));
		}
	//else if ((int)data == 2)
	else if (GPOINTER_TO_INT(data) == 2)
		{
		gtk_timeout_remove(to_jam);
		mode = jam_mode(car);
		if (mode)
			diag_display(2, "Jamming...", 0);
		else
			diag_display(2, "Scrambling...", 0);
		to_jam = gtk_timeout_add (2, jam_action, GINT_TO_POINTER(3));
		}
	//else if ((int)data == 3)
	else if(GPOINTER_TO_INT(data) == 3)
		{
		scanbox_prog(c);
		c = c + 0.005;
		if (c > 1.0)
			{
			gtk_timeout_remove(to_jam);
			to_jam = FALSE;
			scanbox_state(SCANBOX_CLEAR);
			c = 0.0;
			if (jam_success())
				{
				if (mode) gmode = 1;
				else gmode = 2;
				gcar = car;
				diag_display(3, "SUCCESSFULL", 0);
				if (state_auto)
					{
					diag_display(4, "Preparing For Decipher...", 0);
					to_scan = gtk_timeout_add (3000, scan_for_signal,
									GINT_TO_POINTER(1));
					}
				else
					{
					append_log ("", gcar, 0, 0, "", FALSE, gmode);
					diag_set_clear_to(10);
					gtk_widget_set_sensitive(GTK_WIDGET(but_scan), TRUE);
					to_monitor = gtk_timeout_add (1000, jam,
									GINT_TO_POINTER(2));
					}
				}
			else			/* Pray this never happens! */
				{
				gmode = 0;
				diag_display(3, "FAILURE", DIAG_WARN);
				popup_dialog("WARNING! WARNING! WARNING!\n\n"
					"MindGuard Failed To Jam Or\n"
					"Scramble Incomming Signal!\n\n"
					"Please Consult Documentation\n"
					"On What To Do.", "Oh No!",
					NULL, NULL, NULL, DIALOG_BEEP | DIALOG_CENTER | DIALOG_WARN);
				gtk_widget_set_sensitive(GTK_WIDGET(but_scan), TRUE);
				append_log ("", gcar, 0, 0, "", FALSE, gmode);
				to_monitor = gtk_timeout_add (1000, jam, GINT_TO_POINTER(2));
				}
			}
		}
	return TRUE;
}
int jam_detect()
{
	int detect = FALSE, r;
	r = rs(100)&13;
	if (r == 0)
		detect = TRUE;
	return detect;
}
gint jam(gpointer data)
{
	//if ((int)data == 2)
  if(GPOINTER_TO_INT(data) == 2)
		{
		gtk_timeout_remove(to_monitor);
		to_monitor = gtk_timeout_add (1000, jam, NULL);
		}
	if (! to_monitor)
		to_monitor = gtk_timeout_add (1000, jam, NULL);
	else
		if (jam_detect() & ! to_scan & ! to_depsych & ! state_silent & ! to_jam)
			{
			gtk_timeout_remove(to_monitor);
			to_monitor = FALSE;
			gtk_widget_set_sensitive(GTK_WIDGET(but_scan), FALSE);
			jam_action(NULL);
			}
	return TRUE;
}


/**** DECIPHER SIGNALS ****/
void apply_matrix(char *matrix, char *data, int phase)
{
	int i, ii;
	char buffer[256];

	/* Do not alter these! Dire consequences may ensue! */
	char phoneme[7][9] = {
		{'A','S','T','F','X','L','Z','P','R'},
		{'E','R','J','N','V','W','F','K','S'},
		{'I','X','S','T','F','D','L','P','M'},
		{'O','H','R','Q','W','T','S','V','N'},
		{'U','Q','K','G','H','P','W','Y','B'},
		{'1','2','3','4','5','6','7','8','9'},
		{'.',',','!','?','$','(',')','~','0'}};

	for (i = 0; i < strlen(matrix); i++)
		{
		for (ii = 0; ii < phase; ii++)
			/* DEPRECATED!!!!!!
			 * matrix[i] = (char)(matrix[i]--);
			 */
			matrix[i] = (char)((int)matrix[i]-1);
		buffer[i] = phoneme[(int)(i/phase)][(int)(matrix[i]/phase)];
		}
	for (i = 256; i > 0; i--)
		{
			if ((i < 100) | (i > 200)){
				data[i] ^= (data[i] >> buffer[i]) & 0x9d2c5680;
			}
			else{
				data[i] ^= (data[i] << buffer[i]) & 0xefc60000;
			}
		}
	return;
}
void parse_data(char *data, char *matrix)
{
	char buffer[256];
	strcpy(buffer, data);
	apply_matrix(buffer, matrix, max);
	strcpy(matrix, buffer);
	return;
}
void decipher(char *data, char *mod_path, int context, int index)
{
	int i, matrix_size;
	char matrix[256], word[4], twobyte[2];
	char *cword[] = {"LING","EMOT","SENS","PICT"};
	FILE *mod;

	if ((mod = fopen (mod_path, "r")) != NULL)
		{
	    while (fgets(word, 5, mod) != NULL)
			{
			if (strcmp(word, cword[context]) == 0)
				{
				for (i = 0; i <= index; i++)
					{
					while (fgets(twobyte, 3, mod) != NULL)
						if (strcmp(twobyte, "&&") == 0)
							break;
					fgets(twobyte, 3, mod);
					}
				matrix_size = chars_value(twobyte, 2);
				fgets(matrix, matrix_size + 1, mod);
				parse_data(matrix, data);
				break;
				}
			}
		fclose(mod);
		}
	return;
}

/**** TEXT CONCATENATIONS ****/
void context_name(char *text, int context)
{
	char *context_name[] = {"linguistic","physio-emotive","sensual","visual"};
	strcat(text, context_name[context]);
	return;
}
void source_name(char *text, int source)
{
	char *source_name[] = {"unknown","moving vector","stratospheric","relay","temporal"};
	strcat(text, source_name[source]);
	return;
}

void context_name_label(char *data, int context)
{
	char *context_name_label[] = {"Linguistic Message:\n","Physio-Emotive Description:\n",
					"Sensation Description:\n","Visual Description:\n"};
	char text[256];
	strcpy(text, context_name_label[context]);
	strcat(text, data);
	strcpy(data, text);
	return;
}


/**** BIORHYTHM ****/
void create_biorhythm(GtkWidget *widget)
{
	int i, ii, w, h, y, x2, y2, md, days, new;
	float r, x, bio_len[3] = {23, 28, 33}, pi2;

	pi2 = 3.14159265358979323846 * 2;

	/*
	23 = Physical (red)
	28 = Emotional (green)
	33 = Intellectual (blue)
	There is also a hypothesized 38 day Intuitional biorhythm,
	but it hasn't been full tested by the scientific community
	so we will forgo its inclusion until its existence is put
	on a more empirically sound footing.
	*/

	days = days_since();

	w = widget->allocation.width;
	h = widget->allocation.height;

	if (pixmap_bio)
		gdk_pixmap_unref(pixmap_bio);

	pixmap_bio = gdk_pixmap_new (widget->window, w, h, -1);

	gdk_draw_rectangle (pixmap_bio, widget->style->bg_gc[GTK_WIDGET_STATE(widget)],
		TRUE, 0, 0, w, h);

	/* axes and ticks */
	gdk_draw_line (pixmap_bio, widget->style->black_gc, 0, (h/2), w, (h/2));
	gdk_draw_line (pixmap_bio, widget->style->black_gc, (w/2), 0, (w/2), h);
	for ( i = 1 ; i < 14; i++)
		{
		x = (float) w / 28;
		r = i;
	    	gdk_draw_line (pixmap_bio, widget->style->black_gc,
			(w/2)+(x * r), (h/2) - 3,
			(w/2)+(x * r), (h/2) + 3);
	    	gdk_draw_line (pixmap_bio, widget->style->black_gc,
			(w/2)-(x * r), (h/2) - 3,
			(w/2)-(x * r), (h/2) + 3);
		}

 	/* bio-sines */
	for ( i = 0 ; i < 3; i++)
		{
		x2 = bio_len[i];
		md = (days - 14) % x2;
		x2 = 0; new = TRUE;
		x = (md / bio_len[i]) * pi2;
		for ( ii = 0 ; ii < w + 1; ii++)
			{
			r = (ii / (float) w) * pi2 * (28 / bio_len[i]) + x;
			y = -1 * sin(r) * ((h/2) - 2) + (h/2);
			if (new)
				{
				y2 = y; new = FALSE;
				}
		    	gdk_draw_line (pixmap_bio, bio_gc[i], x2, y2, ii, y);
			x2 = ii; y2 = y;
			}
		}

	return;
}
void draw_biorhythm(GtkWidget *widget, GdkEventExpose *event)
{
	gdk_draw_pixmap(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		pixmap_bio,
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
	return;
}
void update_biorhythm()
{
	create_biorhythm(draw_bio);
	gdk_draw_pixmap(draw_bio->window,
		draw_bio->style->fg_gc[GTK_WIDGET_STATE(draw_bio)],
		pixmap_bio, 0, 0, 0, 0,
		draw_bio->allocation.width, draw_bio->allocation.height);
	return;
}


/**** EPONYMOLOG ****/
void update_username_eponymolog()
{
	strcpy (user_name_data, gtk_entry_get_text(GTK_ENTRY(entry_epo)));
	update_eponymolog();

	if (strlen(user_name) == 0)
		{
		state_epo = FALSE;
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_epo), FALSE);
		}

	/* MindGuard cares about your wellbeingness. */
	if (eponymolog() == 13)
		popup_dialog("Your eponymolog is the unlucky number 13.\n"
			"MindGuard suggests that you consult your\n"
			"local chapter of The Society of Kabalarians\n"
			"for advice on coping with this.", "Ok",
			NULL, NULL, NULL, DIALOG_BEEP | DIALOG_CENTER);
	return;
}
void update_eponymolog()
{
	char text[4];
	sprintf (text, "%i", eponymolog());
	gtk_label_set_text (GTK_LABEL(label_epo), text);
	return;
}
int eponymolog(void)
{
	/*
	Ancient Numerological Algorithm as used by the Pythagorians. Later
	rediscovered and developed for eponymological use by the noted
	19th Century logistician and crazed mystic, George Boole.
	*/

	int i;
	char value;

	value = 0;
	for ( i = 0 ; user_name[i] > 0; i++)
		value = value ^ user_name[i];

	/*
	Mystical Exclusive Or. Note: operator symbol forms a partial
	isosceles triangle. This is not a coincidence!
	*/

	return value;
}


/**** DEPSYCH ****/
gint depsych_hologram(gpointer data)
{
	int w, h;
	static int i = 0, c = 8, d = 0;

	w = draw_depsych->allocation.width;
	h = draw_depsych->allocation.height;

	if (! window_depsych) return FALSE;

	//if ((int)data == 1)
  if (GPOINTER_TO_INT(data) == 1)
		{
		i = 0; c = 8; d = 0;
		gdk_draw_rectangle (pixmap_depsych, depsych_gc[14], TRUE,
					(w / 2) - 50, (h - 110) / 2, 10, 10);
		gdk_draw_rectangle (pixmap_depsych, depsych_gc[14], TRUE,
					(w / 2) + 50, (h - 110) / 2, 10, 10);
		gtk_timeout_remove(to_depsych);
		to_depsych = gtk_timeout_add (5, depsych_hologram, GINT_TO_POINTER(2));
		}

	//if ((int)data == 2)
	if (GPOINTER_TO_INT(data) == 2)
		{
		if (d == 0) i++;
		if (d == 1) i--;
		if (i == 0) d = 0;
		if (i == w) d = 1;
		c = c + (1 - rs(3));
		if (c < 0) c = 0;
		if (c > 14) c = 14;
		gdk_draw_line (pixmap_depsych, depsych_gc[c], i, h - 100, i, h);
		if (d == 0)
			gdk_draw_line (pixmap_depsych, depsych_gc[15], i + 1, h - 100, i + 1, h);
		if (d == 1)
			gdk_draw_line (pixmap_depsych, depsych_gc[15], i - 1, h - 100, i - 1, h);
		gdk_draw_pixmap(draw_depsych->window,
			draw_depsych->style->fg_gc[GTK_WIDGET_STATE(draw_depsych)], pixmap_depsych,
			i - 1, h - 100, i - 1, h - 100, 3, 100);
		}

	return TRUE;
}
void create_dholo(GtkWidget *widget)
{
	int w, h;

	w = widget->allocation.width;
	h = widget->allocation.height;

	if (pixmap_depsych)
		gdk_pixmap_unref(pixmap_depsych);
	pixmap_depsych = gdk_pixmap_new (widget->window, w, h, -1);
	gdk_draw_rectangle (pixmap_depsych, widget->style->black_gc, TRUE, 0, 0, w, h);

	return;
}
void draw_dholo(GtkWidget *widget, GdkEventExpose *event)
{
	gdk_draw_pixmap(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		pixmap_depsych,
		event->area.x, event->area.y,
		event->area.x, event->area.y,
		event->area.width, event->area.height);
	return;
}
void depsych()
{
	if (! to_scan & ! to_jam)
		{
		diag_display_clear(NULL);
		diag_display(0, "Deprogramming...", 0);
		open_depsych_window();
		to_depsych = gtk_timeout_add (10, depsych_hologram, GINT_TO_POINTER(1));
		}
	return;
}


/**** DIAGNOSTIC BOX ***/
void diag_display(int n, char *text, int style)
{
	if (window)
		{
		gtk_label_set_text(GTK_LABEL(label_diag[n]), text);
		switch (style)
			{
			case DIAG_WARN:
				gtk_widget_set_style (label_diag[n], style_warn);
				break;
			case DIAG_OK:
				gtk_widget_set_style (label_diag[n], style_ok);
				break;
			case DIAG_CALM:
				gtk_widget_set_style (label_diag[n], style_web);
				break;
			default:
				gtk_widget_restore_default_style (label_diag[n]);
				break;
			}
		if (to_diag_clear != FALSE)
			{
			gtk_timeout_remove(to_diag_clear);
			to_diag_clear = FALSE;
			}
		}
	return;
}
gint diag_display_clear(gpointer data)
{
	int i;
	if (window)
		for ( i=0 ; i < 5; i++)
			gtk_label_set_text(GTK_LABEL(label_diag[i]), NULL);
	if (to_diag_clear != FALSE)
		{
		gtk_timeout_remove(to_diag_clear);
		to_diag_clear = FALSE;
		}
	return FALSE;
}
void diag_set_clear_to(int secs)
{
	to_diag_clear = gtk_timeout_add (1000 * secs, diag_display_clear, NULL);
	return;
}


/**** LOG ****/
void check_for_mg_home_dir()
{
	DIR *dp;
	char path[256];
	sprintf(path, "%s/MindGuard/", getenv("HOME"));
	dp = opendir(path);
	if(dp != NULL){
		closedir(dp);
	}
	else{
		sprintf(path, "mkdir %s/MindGuard", getenv("HOME"));
		system(path);
	}
	return;
}
void view_log()
{
	FILE *log;
	char buffer[1024], log_path[256];
	int nc;
	GdkFont *font;

	font = gdk_font_load ("-misc-fixed-medium-r-*-*-*-120-*-*-*-*-*-*");
	gtk_text_freeze (GTK_TEXT(text_log));

	check_for_mg_home_dir();

	sprintf(log_path, "%s/MindGuard/mindguard.log", getenv("HOME"));
	log = fopen(log_path, "r");
	if (log != NULL)
		{
		gtk_text_set_point (GTK_TEXT(text_log), 0);
		gtk_text_forward_delete (GTK_TEXT(text_log), gtk_text_get_length (GTK_TEXT(text_log)));
		while (feof(log) == 0)
			{
			nc = fread(buffer, 1, 1024, log);
			gtk_text_insert (GTK_TEXT(text_log), font, NULL, NULL, buffer, nc);
			if (nc < 1024)
				break;
			}
		fclose(log);
		}

	gtk_text_thaw (GTK_TEXT(text_log));

	return;
}
void flush_log()
{
	FILE *log;
	char log_path[256];

	sprintf(log_path, "%s/MindGuard/mindguard.log", getenv("HOME"));
	log = fopen(log_path, "w");
	if (log != NULL)
		{
		fprintf(log, "%s", "");
		fclose(log);
		if (window_logview != NULL)
			view_log();
		}
	return;
}
void append_log(char *text, int car, int context, int source, char *infotext,
		int deciphered, int mode)
{
	FILE *log;
	char buffer[256], log_path[256];

	check_for_mg_home_dir();

	sprintf(log_path, "%s/MindGuard/mindguard.log", getenv("HOME"));
	log = fopen(log_path, "a");
	if (log != NULL)
		{
		time_string (buffer);
		fprintf(log, "---------------------------\nTime:     %s\n", buffer);
		strcpy (buffer, module[car].name);
		fprintf(log, "Carrier:  %s\n", buffer);
		if ((( (mode == 1) | (mode == 2)) & state_auto) | (mode == 3))
			{
			if (deciphered)
				{
				strcpy (buffer, "");
				context_name (buffer, context);
				fprintf(log, "Context:  %s\n", buffer);

				fprintf(log, "Info:     %s\n", infotext);

				strcpy (buffer, "");
				source_name(buffer, source);
				fprintf(log, "Source:   %s\n", buffer);

				fprintf(log, "Contents:\n%s\n", text);
				}
			else
				fprintf(log, "Contents: Undecipherable\n");
			}
		fprintf(log, "Action:   ");
		switch (mode)
			{
			case 0:
				fprintf(log, "FAILURE! DANGER!!\n");
				break;
			case 1:
				fprintf(log, "SUCCESSFULLY JAMMED\n");
				break;
			case 2:
				fprintf(log, "SUCCESSFULLY SCRAMBLED\n");
				break;
			case 3:
				fprintf(log, "SCAN FILTERED\n");
				break;
			}
		fclose(log);
		if (window_logview != NULL) view_log();
		}
	return;
}


/*** PREFS ***/
void load_prefs()
{
	int version;
	FILE *pref_file;
	char pref_path[256];

	sprintf(pref_path, "%s/.mindguard", getenv("HOME"));
	if ((pref_file = fopen (pref_path, "r")) != NULL)
		{
		fscanf(pref_file, "MindGuard X pref version: %i\n", &version); /* for future use */
		fscanf(pref_file, "%[^\n]\n", user_name_data);
		if (strcmp(user_name, "-") == 0)
			strcpy(user_name, "");
		fscanf(pref_file, "%i %i %i\n%i %i %i\n%i %i %i %i %i\n%i %i %i %i %i %i\n",
			&bday, &bmonth, &byear, &algo, &scan_mode, &scan_depth,
			&state_silent, &state_auto, &state_log, &state_bio, &state_epo,
			&center_windows, &save_on_exit, &ask_quit,
			&phase_check, &dopplerator, &kill_feedback);
		fclose(pref_file);
		}
	else save_prefs();

	update_gui_from_prefs();

	return;
}
void save_prefs()
{
	FILE *pref_file;
	char pref_path[256];

	sprintf(pref_path, "%s/.mindguard", getenv("HOME"));
	if ((pref_file = fopen (pref_path, "w")) != NULL)
		{
		fprintf(pref_file, "MindGuard X pref version: 0\n");
		if (strlen(user_name) == 0)
			fprintf(pref_file, "-\n");	/* If you are named "-" you will have problems. */
		else
			fprintf(pref_file, "%s\n", user_name);
		fprintf(pref_file, "%i %i %i\n%i %i %i\n%i %i %i %i %i\n%i %i %i %i %i %i\n",
			bday, bmonth, byear, algo, scan_mode, scan_depth,
			state_silent, state_auto, state_log, state_bio, state_epo,
			center_windows, save_on_exit, ask_quit,
			phase_check, dopplerator, kill_feedback);
		fclose(pref_file);
		}
	else printf("ERROR: Could not open %s to save prefs!\n", pref_path);

	return;
}


void set_scan_button_state()
{
	if (state_silent)
		gtk_widget_set_sensitive(GTK_WIDGET(but_scan), TRUE);
	else if (state_auto)
		gtk_widget_set_sensitive(GTK_WIDGET(but_scan), FALSE);
	else
		gtk_widget_set_sensitive(GTK_WIDGET(but_scan), TRUE);
}
void update_gui_from_prefs()
{
	if (window != NULL)
		{
		gtk_option_menu_set_history (GTK_OPTION_MENU (opt_algo), (guint) algo);
		gtk_option_menu_set_history (GTK_OPTION_MENU (opt_scan_mode), (guint) scan_mode);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_silent), state_silent);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_phase), phase_check);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_doppler), dopplerator);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_feedback), kill_feedback);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_auto), state_auto);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_log), state_log);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_center), center_windows);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_soe), save_on_exit);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_aq), ask_quit);
		gtk_adjustment_set_value (GTK_ADJUSTMENT(adj_scan), (float) scan_depth);
		set_scan_button_state();
		}
	if (window_psid != NULL)
		{
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_bio), state_bio);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_epo), state_epo);
		gtk_option_menu_set_history (GTK_OPTION_MENU (opt_month), (guint) bmonth - 1);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_day), (float) bday);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_year), (float) byear);
		gtk_entry_set_text(GTK_ENTRY(entry_epo), user_name);
		update_eponymolog();
		update_biorhythm();
		}
	return;
}


/*** MODULE FUNCTIONS ***/
int load_mods()
{
	int success = MODLOAD_WORKING, try = 0;
	char path[256];
	DIR *dp;
	struct dirent *dirp;

	mod_num = 0;

	diag_display_clear(NULL);
	diag_display(0, "Installing Modules...", 0);

	gtk_clist_freeze(GTK_CLIST (carlist));
	gtk_clist_clear(GTK_CLIST (carlist));
	gtk_clist_thaw(GTK_CLIST (carlist));

	while (try < 4)
		{
		if ((dp = opendir(car_path)))
			{
			while ((dirp = readdir(dp)) != NULL)
				{
				if ( (strcmp(dirp->d_name, ".") == 0) || (strcmp(dirp->d_name, "..") == 0) )
					continue;
				if (strcmp(&dirp->d_name[strlen(dirp->d_name)-8], ".carrier") == 0)
					{
					strcpy(path, car_path);
					strcat(path, dirp->d_name);
					if (parse_mod(path, mod_num))
						{
						sort_mods();
						mod_num++;
						}
					}
				}
			closedir(dp);

			if (mod_num > 0)
				{
				add_mods_to_clist();
				success = MODLOAD_SUCCESS;
				sprintf(path, "%i Modules Installed.", mod_num);
				diag_display(1, path, 0);
				diag_display(3, "Module Path:", 0);
				diag_display(4, car_path, 0);
				diag_set_clear_to(20);
				}
			else success = MODLOAD_NOMODS;

			break;
			}
		else
			{
			switch (try)
				{
				case 0:
					strcpy(car_path, "./carriers/");
					break;
				case 1:
					sprintf(car_path, "%s/MindGuard/carriers/", getenv("HOME"));
					break;
				case 2:
					strcpy(car_path, "/usr/local/share/MindGuard/carriers/");
					break;
				default:
					break;
				}
			try++;
			if (try > 3)
				{
				success = MODLOAD_NODIR;
				try++;
				}
			}
		}

	return success;
}
void reload_mods()
{
	if (load_mods() != MODLOAD_SUCCESS)
		{
		diag_display(1, "Modules are missing!", DIAG_WARN);
		popup_dialog("ERROR:\n"
		"Carrier modules have disappeared!\n"
		"Agents of mind control may have\n"
		"hacked into your system! MindGuard\n"
		"might be useless to you now!",
		"Quit & FLEE!", "Try reloading", quit_2, reload_mods, DIALOG_QUIT | DIALOG_MODAL);
		}
	return;
}
void sort_mods()
{
	struct carmod module_temp;
	int i, j;

	if (mod_num > 0)
		{
		for (i = mod_num; i > 0; i--)
			{
			for (j = 0; j <= 255; j++)
				{
				if ((module[i].name[j] == 0) | (module[i-1].name[j] == 0))
          return;
				if (module[i].name[j] < module[i-1].name[j])
					{
					module_temp = module[i-1];
					module[i-1] = module[i];
					module[i] = module_temp;
					break;
					}
				else if (module[i].name[j] > module[i-1].name[j]) return;
				}
			}
		}
	return;
}
void add_mods_to_clist()
{
	char *cars[1][2];
	int i;

	gtk_clist_freeze(GTK_CLIST (carlist));
	for (i = 0; i < mod_num; i++)
		{
		cars[0][0] = module[i].name;
		cars[0][1] = module[i].version;
		gtk_clist_append (GTK_CLIST (carlist), cars[0]);
		}
	gtk_clist_thaw(GTK_CLIST (carlist));
	return;
}
int chars_value(char *c, int n)
{
	int i, mu, s = 0, value = 0;
	max = 0;
	mu = pow (16, n);
	if (n == 4) s = 1;
	for (i = s; i < n; i++)
		{
		value = value + ((int)c[i] * mu);
		mu = mu / 256;
		if (n == 4) max = max + 16;
		else max = max + 32;
		}
	return value;
}
void word_offset(FILE *file, int len)
{
	int off;
	char buf[3];
	off = 4 - (len - ((len / 4) * 4));
	if (off == 4) return;
	fread (buf, sizeof (char), off, file);
	return;
}
int parse_mod(char *path, int num)
{
	int result = FALSE, len;
	static int i;
	char word[5];
	FILE *file;

	if ((file = fopen (path, "r")) != NULL)
		{
		if (fgets(word, 5, file) == NULL)
			{
			fclose(file);
			return result;
			}
		if (strcmp(word, "FORM") != 0)
			{
			fclose(file);
			return result;
			}

		fgets(word, 5, file);
		fgets(word, 5, file);
		if (strcmp(word, "PSYC") != 0)
			{
			fclose(file);
			return result;
			}

		strcpy(module[num].file, path);
		strcpy(module[num].name, "");
		strcpy(module[num].version, "");
		strcpy(module[num].copyright, "");
		pcheck = srand;
		module[num].layer = 0;
		module[num].encrypt = 0;
		module[num].syntax = 0;
		module[num].phase = 0;

		i = 0;
		while (fgets(word, 5, file) != NULL)
			{
			if (strcmp(word, "NAME") == 0)
				{
				fgets(word, 5, file);
				len = chars_value(word, 4);
				fread(module[num].name, sizeof (char), len, file);
				module[num].name[len] = 0; /* add to make a proper string */
				word_offset(file, len);
				i++;
				continue;
				}
			if (strcmp(word, "(c) ") == 0)
				{
				fgets(word, 5, file);
				len = chars_value(word, 4);
				fread(module[num].copyright, sizeof (char), len, file);
				module[num].copyright[len] = 0;
				word_offset(file, len);
				continue;
				}
			if (strcmp(word, "VERS") == 0)
				{
				fgets(word, 5, file);
				len = chars_value(word, 4);
				fread(module[num].version, sizeof (char), len, file);
				module[num].version[len] = 0;
				word_offset(file, len);
				continue;
				}
			if (strcmp(word, "LNUM") == 0)
				{
				fgets(word, 5, file);
				module[num].layer = chars_value(word, 4);
				i++;
				continue;
				}
			if (strcmp(word, "ENUM") == 0)
				{
				fgets(word, 5, file);
				module[num].encrypt = chars_value(word, 4);
				i++;
				continue;
				}
			if (strcmp(word, "SNUM") == 0)
				{
				fgets(word, 5, file);
				module[num].syntax = chars_value(word, 4);
				i++;
				continue;
				}
			if (strcmp(word, "PNUM") == 0)
				{
				fgets(word, 5, file);
				module[num].phase = chars_value(word, 4);
				i++;
				continue;
				}
			if (strcmp(word, "END ") == 0)
				break;
			}

		if (i == 5)
			result = TRUE;
		else
			{
			printf("ERROR: Module '%s' corrupted structure\n", module[num].name);
			result = FALSE;
			}

		fclose(file);
		}

	return result;
}
void carrier_info(GtkWidget *widget, int row)
{
	if (! to_scan & ! to_jam)
		{
		diag_display_clear(NULL);
		diag_display(0, module[row].name, 0);
		diag_display(1, module[row].version, 0);
		diag_display(2, module[row].copyright, 0);
		diag_display(3, "Carrier module location:", 0);
		diag_display(4, module[row].file, 0);
		diag_set_clear_to(20);
		}
	return;
}


/**** DATE/TIME STUFF ****/
void time_string (char *text)
{
	time_t now;
	struct tm *tm;
	now = time(0);
	tm = localtime(&now);
	strftime (text, 256, "%H:%M:%S %Y/%m/%d", tm);
	return;
}
int days_in_month(int month, int year)
{
	int days, mon[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	days = mon[month-1];
	if (month == 2)
		if ((((year % 4) == 0) & ((year % 100) > 0)) | ((year % 400) == 0))
			days = 29;
	return days;
}
void update_day_spin()
{
	GTK_ADJUSTMENT(adjust_day)->upper = days_in_month(bmonth, byear);
	gtk_signal_emit_by_name (GTK_OBJECT(adjust_day), "changed");
	gtk_spin_button_update (GTK_SPIN_BUTTON(spin_day));
	bday = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_day));
	return;
}
int days_since()
{
	int days, day, month, year;
	time_t now;
	struct tm *tm;

	now = time(0);
	tm = localtime(&now);

	day = tm->tm_mday;
	month = tm->tm_mon + 1;
	year = tm->tm_year;
	if (year >= 90)
		year = year + 1900;
	else
		year = year + 2000;

	/*
	MindGuard is Y2K compliant -- but not Y209DA compliant.
	This will be fixed in MindGuard version 0.0.2.0, which
	should be done before 2090. Until then, please don't run
	MindGuard more than ten years ago.
	*/

	days = ratadie(day, month, year) - ratadie(bday, bmonth, byear);

	return days;
}
int ratadie(int day, int month, int year)
{
	int a, b, c, d, rd;

	a = 365 * (year - 1);
	b = ((year - 1) / 4) - ((year - 1) / 100) + ((year - 1) / 400);
	c = ((367 * month) - 362) / 12;

	if (month <= 2)
		d = 0;
	else if ((month > 2) & ((((year % 4) == 0) & ((year % 100) > 0)) | ((year % 400) == 0)))
		d = -1;
	else
		d = -2;

	rd = a + b + c + d + day;	/* Math is purty. */

	return rd;
}


/*** MISC USEFUL FUNCTIONS ***/
int dir_exists (char path[255])
{
	DIR *dp;
	int ret;
	dp = opendir(path);
	if(dp != NULL) ret = TRUE;
	else ret = FALSE;
	closedir(dp);
	return ret;
}


/**** VARIOUS GUI CALLBACKS ****/
void open_webpage(GtkWidget *widget, int url)
{
	char command[255];
	char *urls[] = {"lyle@zapatopi.net","http://zapatopi.net/",
			"http://zapatopi.net/mindguard/","http://zapatopi.net/mgpl.html"};

	strcpy(command, "");

	if (url > 3) return;
	if (url == 0)
		sprintf(command, email_command, urls[url]);
	else
		sprintf(command, webpage_command, urls[url]);

	system(command);

	return;
}
void day_adjust(GtkAdjustment *adj, GtkWidget *widget)
{
	bday = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
	update_biorhythm();
	return;
}
void month_adjust(GtkWidget *widget, int month)
{
	bmonth = month;
	update_day_spin();
	update_biorhythm();
	return;
}
void year_adjust(GtkAdjustment *adj, GtkWidget *widget)
{
	byear = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
	update_day_spin();
	update_biorhythm();
	return;
}
void scan_mode_adjust(GtkWidget *widget, int sm)
{
	scan_mode = sm;
	return;
}
void algo_adjust(GtkWidget *widget, int a)
{
	algo = a;
	return;
}
void scan_depth_adjust(GtkAdjustment *adj)
{
	scan_depth = adj->value;
	return;
}
void check_widget_callback (GtkWidget *widget, int id)
{
	int value;
	value = GTK_TOGGLE_BUTTON(widget)->active;
	switch (id)
		{
		case CHECK_AUTO:
			state_auto = value;
			set_scan_button_state();
			break;
		case CHECK_SILENT:
			state_silent = value;
			set_scan_button_state();
			break;
		case CHECK_PHASE:
			phase_check = value;
			break;
		case CHECK_DOPPLER:
			dopplerator = value;
			break;
		case CHECK_FEEDBACK:
			kill_feedback = value;
			break;
		case CHECK_LOG:
			state_log = value;
			break;
		case CHECK_BIO:
			state_bio = value;
			break;
		case CHECK_EPO:
			state_epo = value;
			if (strlen(user_name) == 0)
				{
				static int called_once = FALSE;
				state_epo = FALSE;
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_epo), FALSE);
				if (called_once == FALSE)
					{
					popup_dialog("You MUST enter your full name\n"
							"to turn on this feature!",
					"Okay", NULL, NULL, NULL, DIALOG_MOUSE | DIALOG_BEEP);
					called_once = TRUE;
					}
				else called_once = FALSE;
				}
			break;
		case CHECK_CENTER:
			center_windows = value;
			break;
		case CHECK_ATQ:
			ask_quit = value;
			break;
		case CHECK_SOE:
			save_on_exit = value;
			break;
		default:
			break;
		}
	return;
}


/**** WIDGET HELPERS ****/
GtkWidget *make_web_link(char *url, int url_num)
{
	GtkWidget *box, *button, *label;
	box = gtk_hbox_new(FALSE, 0);
	button = gtk_button_new ();
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (open_webpage), GINT_TO_POINTER(url_num));
	gtk_button_set_relief (GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_box_pack_start (GTK_BOX(box), button, TRUE, FALSE, 0);
	gtk_widget_show (button);
	label = gtk_label_new (url);
	gtk_widget_set_style (label, style_web);
	gtk_container_add (GTK_CONTAINER (button), label);
	gtk_widget_show (label);
	return box;
}


/**** COLORS, STYLES, GC'S ****/
GdkColor *make_color(int red, int grn, int blu)
{
	GdkColor *color;

	color = (GdkColor *)g_malloc(sizeof(GdkColor));
	color->red   = red;
	color->green = grn;
	color->blue  = blu;

	if (! gdk_colormap_alloc_color (gdk_colormap_get_system(), color, FALSE, TRUE))
		printf("ERROR: Couldn't allocate color (%i,%i,%i)\n", red, grn, blu);

	return color;
}
GtkStyle *make_style(GdkColor *fg)
{
	GtkStyle *def;
	GtkStyle *style;
	int i;

	def = gtk_widget_get_default_style();
	style = gtk_style_copy(def);

	for (i = 0; i < 5; i++)
		style->fg[i] = *fg;

	return style;
}
GdkGC *make_gc(GtkWidget *widget, GdkColor *c)
{
	GdkGC *gc;
	gc = gdk_gc_new (widget->window);
	gdk_gc_set_foreground (gc, c);
	return gc;
}
void set_up_colors() /* and styles and graphic contexts */
{
	int i, c = 0;
	GdkColor *colors[3];

	colors[0] = make_color (65535, 0, 0);
	colors[1] = make_color (0, 40000, 0);
	colors[2] = make_color (0, 0, 65535);

	style_warn = make_style(colors[0]);
	style_ok = make_style(colors[1]);
	style_web = make_style(colors[2]);

	for (i = 0; i < 15; i++)
		{
		depsych_gc[i] = make_gc(window, make_color (0, 0, c));
		c = c + 4096;
		}
	depsych_gc[15] = make_gc(window, make_color (0, 65535, 32767));

	for (i = 0; i < 3; i++)
		bio_gc[i] = make_gc(window, colors[i]);

	return;
}


/**** WINDOWS, WINDOWS, AND MORE WINDOWS ****/
void close_popup_dialog_1(GtkWidget *w, GtkWidget *window)
{
	gtk_widget_destroy (GTK_WIDGET(window));
	return;
}
void close_popup_dialog_2(GtkWidget *w1, GtkWidget *w2, GtkWidget *window)
{
	gtk_widget_destroy (GTK_WIDGET(window));
	return;
}
void popup_dialog(char *text_, char *ok, char *cancel,
		void (*func_pointer_1) (void), void (*func_pointer_2) (void), int opts)
{
	GtkWidget *dialog, *label, *button, *box, *box_1, *separator;
	gchar *text;
	text = g_locale_to_utf8(text_, -1, NULL, NULL, NULL);

	dialog = gtk_window_new (GTK_WINDOW_DIALOG);
	gtk_window_set_title (GTK_WINDOW (dialog), "MindGuard");
	gtk_container_set_border_width (GTK_CONTAINER (dialog), 2);
	if (opts & DIALOG_QUIT)
		gtk_signal_connect (GTK_OBJECT (dialog), "delete_event",
				GTK_SIGNAL_FUNC (quit_2), (gpointer) dialog);
	else
		gtk_signal_connect (GTK_OBJECT (dialog), "delete_event",
				GTK_SIGNAL_FUNC (close_popup_dialog_2), (gpointer) dialog);
	gtk_window_set_policy (GTK_WINDOW (dialog), FALSE, FALSE, FALSE);

	if (opts & DIALOG_MOUSE)
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);

	if (opts & DIALOG_CENTER)
		gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

	if (opts & DIALOG_MODAL)
		gtk_window_set_modal (GTK_WINDOW(dialog),TRUE);

	box = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER (dialog), box);
	gtk_widget_show (box);

	box_1 = gtk_vbox_new(FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box_1), 10);
	gtk_box_pack_start(GTK_BOX(box), box_1, TRUE, TRUE, 0);
	gtk_widget_show (box_1);

	label = gtk_label_new (text);
	gtk_box_pack_start(GTK_BOX(box_1), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	if (opts & DIALOG_WARN)
		gtk_widget_set_style (label, style_warn);
	else
		gtk_widget_restore_default_style (label);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(box), separator, FALSE, TRUE, 2);
	gtk_widget_show (separator);

	box_1 = gtk_hbox_new(TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (box_1), 0);
	gtk_box_pack_start(GTK_BOX(box), box_1, FALSE, TRUE, 0);
	gtk_widget_show (box_1);

	button = gtk_button_new_with_label (ok);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (func_pointer_1), NULL);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (close_popup_dialog_1), (gpointer) dialog);
	gtk_box_pack_start(GTK_BOX(box_1), button, FALSE, TRUE, 0);
	gtk_widget_grab_focus (button);

	gtk_widget_show (button);

	if (cancel != NULL)
		{
		button = gtk_button_new_with_label (cancel);
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (func_pointer_2), NULL);
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (close_popup_dialog_1), (gpointer) dialog);
		gtk_box_pack_start(GTK_BOX(box_1), button, FALSE, TRUE, 0);
		gtk_widget_grab_focus (button);
		gtk_widget_show (button);
		}

	gtk_widget_show (dialog);

	if (opts & DIALOG_BEEP)
		gdk_beep();

	return;
}

void close_logview_window()
{
	gtk_widget_destroy (window_logview);
	window_logview = NULL;
	gtk_widget_set_sensitive(GTK_WIDGET(but_logview), TRUE);
	return;
}
void open_logview_window()
{
	GtkWidget *box_m, *box_1, *vscroll, *button, *separator;

	if (window_logview == NULL)
		{
		gtk_widget_set_sensitive(GTK_WIDGET(but_logview), FALSE);

		window_logview = gtk_window_new (GTK_WINDOW_DIALOG);
		gtk_window_set_title (GTK_WINDOW (window_logview), "MindGuard Log Viewer");
		gtk_container_set_border_width (GTK_CONTAINER (window_logview), 2);
		gtk_signal_connect (GTK_OBJECT (window_logview), "delete_event",
					GTK_SIGNAL_FUNC (close_logview_window), NULL);
		if (center_windows)
			gtk_window_set_position(GTK_WINDOW(window_logview), GTK_WIN_POS_CENTER);

		box_m = gtk_vbox_new(FALSE, 0);
		gtk_container_add (GTK_CONTAINER (window_logview), box_m);
		gtk_widget_show (box_m);

		box_1 = gtk_hbox_new(FALSE, 0);
		gtk_box_pack_start(GTK_BOX(box_m), box_1, TRUE, TRUE, 0);
		gtk_widget_show (box_1);

		text_log = gtk_text_new (NULL, NULL);
		gtk_box_pack_start(GTK_BOX(box_1), text_log, TRUE, TRUE, 0);
		//gtk_widget_set_usize (GTK_WIDGET(text_log), 300, 200);
		 gtk_widget_set_size_request (GTK_WIDGET(text_log), 300, 200);
		gtk_widget_show (text_log);

		gtk_widget_realize(text_log);


		vscroll = gtk_vscrollbar_new(gtk_text_view_get_vadjustment(GTK_TEXT_VIEW(GTK_TEXT(text_log))));
		//vscroll = gtk_vscrollbar_new (GTK_TEXT(text_log)->vadj);
		gtk_box_pack_start(GTK_BOX(box_1), vscroll, TRUE, FALSE, 0);
		gtk_widget_show (vscroll);

		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(box_m), separator, FALSE, TRUE, 2);
		gtk_widget_show (separator);

		box_1 = gtk_hbox_new(TRUE, 0);
		gtk_box_pack_start(GTK_BOX(box_m), box_1, FALSE, FALSE, 0);
		gtk_widget_show (box_1);

		button = gtk_button_new_with_label ("Flush Log");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
			GTK_SIGNAL_FUNC (flush_log), NULL);
		gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
		gtk_widget_show (button);

		button = gtk_button_new_with_label ("Close");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
					GTK_SIGNAL_FUNC (close_logview_window), NULL);
		gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
		gtk_widget_show (button);

		gtk_widget_show (window_logview);
		}

	view_log();

	return;
}

void close_depsych_window()
{
	gtk_widget_destroy (window_depsych);
	window_depsych = NULL;
	diag_display(1, "Deprogramming aborted!", DIAG_WARN);
	diag_display(2, "Psychotronic residue may", DIAG_WARN);
	diag_display(3, "still be present.", DIAG_WARN);
	diag_set_clear_to(15);

	gtk_widget_set_sensitive(GTK_WIDGET(window), TRUE);
	return;
}
void open_depsych_window()
{
	GtkWidget *box_m, *frame, *label, *button, *separator;
	GtkTooltips *tooltip;

	if (window_depsych == NULL)
		{
		window_depsych = gtk_window_new (GTK_WINDOW_DIALOG);
		gtk_window_set_title (GTK_WINDOW (window_depsych), "DePsych");
		gtk_container_set_border_width (GTK_CONTAINER (window_depsych), 2);
		gtk_signal_connect (GTK_OBJECT (window_depsych), "delete_event",
					GTK_SIGNAL_FUNC (close_depsych_window), NULL);
		gtk_window_set_policy (GTK_WINDOW (window_depsych), FALSE, FALSE, FALSE);

		gtk_window_set_position(GTK_WINDOW(window_depsych), GTK_WIN_POS_CENTER);

		gtk_window_set_modal (GTK_WINDOW(window_depsych),TRUE);
		gtk_widget_set_usize (GTK_WIDGET(window_depsych),400,180);

		gtk_widget_set_sensitive(GTK_WIDGET(window), FALSE);

		box_m = gtk_vbox_new(FALSE, 0);
		gtk_container_add (GTK_CONTAINER (window_depsych), box_m);
		gtk_widget_show (box_m);

		frame = gtk_frame_new (NULL);
		gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
		gtk_box_pack_start(GTK_BOX(box_m), frame, TRUE, TRUE, 0);
		gtk_widget_show (frame);

		button = gtk_event_box_new ();
		gtk_container_add (GTK_CONTAINER (frame), button);
		gtk_widget_show (button);

		draw_depsych = gtk_drawing_area_new();
		gtk_drawing_area_size(GTK_DRAWING_AREA(draw_depsych),100,130);
		gtk_container_add (GTK_CONTAINER (button), draw_depsych);
		gtk_signal_connect (GTK_OBJECT(draw_depsych), "expose_event",
					(GtkSignalFunc) draw_dholo, NULL);
		gtk_signal_connect (GTK_OBJECT(draw_depsych), "configure_event",
					(GtkSignalFunc) create_dholo, NULL);
		gtk_widget_show (draw_depsych);

		tooltip = gtk_tooltips_new ();
		gtk_tooltips_set_tip (GTK_TOOLTIPS(tooltip), button,
			"DePsych Instructions:           \n"
			"Stare deeply into the blue band, focusing until the two squares converge.\n"
			"Do not stop until MindGuard tells you to.\n"
			"Do not let your gaze follow the green line!", NULL);

		label = gtk_label_new ("Please Concentrate Intently");
		gtk_box_pack_start(GTK_BOX(box_m), label, FALSE, FALSE, 0);
		gtk_widget_show (label);

		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(box_m), separator, FALSE, TRUE, 2);
		gtk_widget_show (separator);

		button = gtk_button_new_with_label ("Abort DePsych");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
					GTK_SIGNAL_FUNC (close_depsych_window), NULL);
		gtk_box_pack_start(GTK_BOX(box_m), button, FALSE, FALSE, 0);
		gtk_widget_grab_focus (button);
		gtk_widget_show (button);

		gtk_widget_show (window_depsych);
		}

	return;
}

void close_about_window()
{
	gtk_widget_destroy (window_about);
	window_about = NULL;
	gtk_widget_set_sensitive(GTK_WIDGET(but_about), TRUE);
	return;
}
void open_about_window()
{
	GtkWidget *box_about, *box_1, *box_2, *notebook, *label, *button, *separator, *pix_mg;
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GtkStyle *style;
	char text[255];

	if (window_about == NULL)
		{
		gtk_widget_set_sensitive(GTK_WIDGET(but_about), FALSE);

		window_about = gtk_window_new (GTK_WINDOW_DIALOG);
		gtk_window_set_title (GTK_WINDOW (window_about), "About");
		gtk_container_set_border_width (GTK_CONTAINER (window_about), 2);
		gtk_signal_connect (GTK_OBJECT (window_about), "delete_event",
					GTK_SIGNAL_FUNC (close_about_window), NULL);
		gtk_window_set_policy (GTK_WINDOW (window_about), FALSE, FALSE, FALSE);

		if (center_windows)
		gtk_window_set_position(GTK_WINDOW(window_about), GTK_WIN_POS_CENTER);

		box_about = gtk_vbox_new(FALSE, 0);
		gtk_container_add (GTK_CONTAINER (window_about), box_about);
		gtk_widget_show (box_about);

		notebook = gtk_notebook_new ();
		gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
		gtk_box_pack_start(GTK_BOX(box_about), notebook, TRUE, TRUE, 0);
		gtk_widget_show (notebook);

		box_1 = gtk_vbox_new(FALSE, 10);
		gtk_container_set_border_width (GTK_CONTAINER (box_1), 10);
		gtk_widget_show (box_1);

		gtk_notebook_append_page (GTK_NOTEBOOK(notebook), box_1, gtk_label_new("MindGuard"));

		style = gtk_widget_get_style (window_about);
		pixmap = gdk_pixmap_colormap_create_from_xpm_d (window_about->window,
				gdk_colormap_get_system(),
				&mask, &style->bg[GTK_STATE_NORMAL],
				(gchar **)mg_xpm);
		pix_mg = gtk_pixmap_new (pixmap, mask);
		gtk_box_pack_start (GTK_BOX(box_1), pix_mg, TRUE, TRUE, 0);
		gtk_widget_show (pix_mg);
		gdk_pixmap_unref (pixmap);

		sprintf(text, "MindGuard X %s\nPersonal Mind Protection Software\n"
				"Modular Psychotronic Detection & Jamming\nFreaknet Medialab revised version\n\n"
				"Copyright (C) 1999-%s Lyle Zapato\nCopyright (C) 2019-2020 Freaknet Medialab\nAll Rights Reserved\n",
				mg_version, mg_year);

		label = gtk_label_new (text);
		gtk_box_pack_start(GTK_BOX(box_1), label, TRUE, FALSE, 0);
		gtk_widget_show (label);

		box_2 = make_web_link("http://zapatopi.net/mindguard.html", 1);
		gtk_box_pack_start (GTK_BOX(box_1), box_2, FALSE, FALSE, 0);
		gtk_widget_show (box_2);

		box_2 = make_web_link("https://github.com/asbestomolesto/mindguard", 2);
		gtk_box_pack_start (GTK_BOX(box_1), box_2, FALSE, FALSE, 0);
		gtk_widget_show (box_2);

		box_1 = gtk_vbox_new(FALSE, 0);
		gtk_container_set_border_width (GTK_CONTAINER (box_1), 10);
		gtk_widget_show (box_1);

		gtk_notebook_append_page (GTK_NOTEBOOK(notebook), box_1, gtk_label_new("Lyle Zapato"));

		pixmap = gdk_pixmap_colormap_create_from_xpm_d (window_about->window,
				gdk_colormap_get_system(),
				&mask, &style->bg[GTK_STATE_NORMAL],
				(gchar **)lz_xpm);
		pix_mg = gtk_pixmap_new (pixmap, mask);
		gtk_box_pack_start(GTK_BOX(box_1), pix_mg, TRUE, TRUE, 0);
		gtk_widget_show (pix_mg);
		gdk_pixmap_unref (pixmap);

		label = gtk_label_new ("Lyle Zapato v4.20beta\nCarbon Based Monkey Boy\n\n"
				"Copyright (C) 1972 Mrs. Zapato\nAll Rights Reserved");
		gtk_box_pack_start (GTK_BOX(box_1), label, TRUE, FALSE, 0);
		gtk_widget_show (label);

		box_2 = make_web_link("lyle@zapatopi.net", 0);
		gtk_box_pack_start (GTK_BOX(box_1), box_2, FALSE, FALSE, 0);
		gtk_widget_show (box_2);

		box_2 = make_web_link("http://zapatopi.net/", 1);
		gtk_box_pack_start (GTK_BOX(box_1), box_2, FALSE, FALSE, 0);
		gtk_widget_show (box_2);

		box_1 = gtk_vbox_new(FALSE, 0);
		gtk_container_set_border_width (GTK_CONTAINER (box_1), 10);
		gtk_widget_show (box_1);

		gtk_notebook_append_page (GTK_NOTEBOOK(notebook), box_1, gtk_label_new("Copying"));

		label = gtk_label_new ("MindGuard X is free software and\n"
					"comes with NO FREAKING WARRANTY.\n"
					"You may copy and/or distributed\n"
					"it under the MindGuard Public\n"
					"License. If you didn't get a copy\n"
					"of the License you can at:");
		gtk_box_pack_start (GTK_BOX(box_1), label, TRUE, FALSE, 0);
		gtk_widget_show (label);

		box_2 = make_web_link("http://zapatopi.net/mgpl.html", 3);
		gtk_box_pack_start (GTK_BOX(box_1), box_2, FALSE, FALSE, 0);
		gtk_widget_show (box_2);

		gtk_notebook_set_page (GTK_NOTEBOOK(notebook), 0);

		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(box_about), separator, FALSE, TRUE, 2);
		gtk_widget_show (separator);

		button = gtk_button_new_with_label ("Okay!");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (close_about_window), NULL);
		gtk_box_pack_start(GTK_BOX(box_about), button, FALSE, FALSE, 0);
		gtk_widget_grab_focus (button);
		gtk_widget_show (button);

		gtk_widget_show (window_about);
		}

	return;
}

void close_psident_window()
{
	gtk_widget_destroy (window_psid);
	gdk_pixmap_unref (pixmap_bio);
	window_psid = NULL;
	pixmap_bio = NULL;
	gtk_widget_set_sensitive(GTK_WIDGET(but_psid), TRUE);
	return;
}
void open_psident_window()
{
	GtkWidget *box_psidm, *box_1, *frame, *separator, *button, *menu, *item;
	GtkTooltips *tooltip;
	GtkAdjustment *adj;

	int i;
	char *month_name[] = {"January","February","March","April","May","June",
				"July","August","September","October","November","December"};

	if (window_psid == NULL)
		{
		gtk_widget_set_sensitive(GTK_WIDGET(but_psid), FALSE);

		window_psid = gtk_window_new (GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title (GTK_WINDOW (window_psid), "PsIdent Control");
		gtk_container_set_border_width (GTK_CONTAINER (window_psid), 2);
		gtk_signal_connect (GTK_OBJECT (window_psid), "delete_event",
				GTK_SIGNAL_FUNC (close_psident_window), NULL);

		if (center_windows)
			gtk_window_set_position(GTK_WINDOW(window_psid), GTK_WIN_POS_CENTER);

		box_psidm = gtk_vbox_new(FALSE, 0);
		gtk_container_add (GTK_CONTAINER (window_psid), box_psidm);
		gtk_widget_show (box_psidm);

	/** BIORHYTHM SYNC **/
		check_bio = gtk_check_button_new_with_label ("Biorhythm Sync");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_bio), state_bio);
		gtk_signal_connect (GTK_OBJECT (check_bio), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback), GINT_TO_POINTER(CHECK_BIO));
		gtk_box_pack_start(GTK_BOX(box_psidm), check_bio, FALSE, FALSE, 0);
		gtk_widget_show (check_bio);

		/** date setter **/
		frame = gtk_frame_new ("Birthdate");
		gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
		gtk_box_pack_start(GTK_BOX(box_psidm), frame, FALSE, FALSE, 0);
		gtk_widget_show (frame);

		box_1 = gtk_hbox_new(FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame), box_1);
		gtk_container_set_border_width (GTK_CONTAINER (box_1), 2);
		gtk_widget_show (box_1);

		adjust_day = (GtkAdjustment *) gtk_adjustment_new (bday, 1.0,
					days_in_month(bmonth, byear), 1.0, 5.0, 0.0);
		spin_day = gtk_spin_button_new (adjust_day, 0, 0);
		gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_day), TRUE);
		gtk_signal_connect (GTK_OBJECT (adjust_day), "value_changed",
					GTK_SIGNAL_FUNC (day_adjust), (gpointer) spin_day);
		gtk_box_pack_start(GTK_BOX(box_1), spin_day, TRUE, TRUE, 0);
		gtk_widget_show (spin_day);

		opt_month = gtk_option_menu_new ();
		menu = gtk_menu_new ();
		for ( i = 0 ; i < 12; i++)
			{
			item = gtk_menu_item_new_with_label (month_name[i]);
			gtk_signal_connect (GTK_OBJECT (item), "activate",
					GTK_SIGNAL_FUNC (month_adjust), GINT_TO_POINTER (i + 1));
			gtk_widget_show (item);
			gtk_menu_append (GTK_MENU (menu), item);
			}
		gtk_option_menu_set_menu (GTK_OPTION_MENU (opt_month), menu);
		gtk_option_menu_set_history (GTK_OPTION_MENU (opt_month), (guint) bmonth - 1);
		gtk_box_pack_start(GTK_BOX(box_1), opt_month, TRUE, TRUE, 0);
		gtk_widget_show (opt_month);

		adj = (GtkAdjustment *) gtk_adjustment_new (byear, 0.0, 4200.0, 1.0, 10.0, 0.0);
		spin_year = gtk_spin_button_new (adj, 0, 0);
		gtk_spin_button_set_numeric (GTK_SPIN_BUTTON(spin_year), TRUE);
		gtk_signal_connect (GTK_OBJECT (adj), "value_changed",
					GTK_SIGNAL_FUNC (year_adjust), (gpointer) spin_year);
		gtk_box_pack_start(GTK_BOX(box_1), spin_year, TRUE, TRUE, 0);
		gtk_widget_show (spin_year);

		/** drawing area **/
		frame = gtk_frame_new (NULL);
		gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
		gtk_box_pack_start(GTK_BOX(box_psidm), frame, TRUE, TRUE, 0);
		gtk_widget_show (frame);

		button = gtk_event_box_new ();
		gtk_container_add (GTK_CONTAINER (frame), button);
		gtk_widget_show (button);

		draw_bio = gtk_drawing_area_new();
		gtk_drawing_area_size(GTK_DRAWING_AREA(draw_bio),250,50);
		gtk_container_add (GTK_CONTAINER (button), draw_bio);
		gtk_signal_connect (GTK_OBJECT(draw_bio), "expose_event",
					(GtkSignalFunc) draw_biorhythm, NULL);
		gtk_signal_connect (GTK_OBJECT(draw_bio), "configure_event",
					(GtkSignalFunc) create_biorhythm, NULL);
		gtk_widget_show (draw_bio);

		tooltip = gtk_tooltips_new ();
		gtk_tooltips_set_tip (GTK_TOOLTIPS(tooltip), button,
				"BIORHYTHM LEGEND:\n"
				"Red = Physical\n"
				"Green = Emotional\n"
				"Blue = Intellectual", NULL);

	/** EPONYMOLOG **/
		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(box_psidm), separator, FALSE, TRUE, 2);
		gtk_widget_show (separator);

		check_epo = gtk_check_button_new_with_label ("Eponymolog");
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_epo), state_epo);
		gtk_signal_connect (GTK_OBJECT (check_epo), "toggled",
					GTK_SIGNAL_FUNC (check_widget_callback),
					GINT_TO_POINTER(CHECK_EPO));
		gtk_box_pack_start(GTK_BOX(box_psidm), check_epo, FALSE, FALSE, 0);
		gtk_widget_show (check_epo);

		frame = gtk_frame_new ("Full Name");
		gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_ETCHED_IN);
		gtk_box_pack_start(GTK_BOX(box_psidm), frame, FALSE, FALSE, 0);
		gtk_widget_show (frame);

		box_1 = gtk_hbox_new(FALSE, 0);
		gtk_container_add (GTK_CONTAINER (frame), box_1);
		gtk_container_set_border_width (GTK_CONTAINER (box_1), 2);
		gtk_widget_show (box_1);

		entry_epo = gtk_entry_new_with_max_length (256);
		gtk_entry_set_text (GTK_ENTRY (entry_epo), user_name);
		gtk_signal_connect (GTK_OBJECT (entry_epo), "changed",
					GTK_SIGNAL_FUNC (update_username_eponymolog), entry_epo);
		gtk_box_pack_start(GTK_BOX(box_1), entry_epo, TRUE, TRUE, 0);
		gtk_widget_show (entry_epo);

		label_epo = gtk_label_new (NULL);
		update_eponymolog();  /* put proper value in */
		gtk_box_pack_start(GTK_BOX(box_1), label_epo, FALSE, FALSE, 10);
		gtk_widget_show (label_epo);

	/** LOAD SAVE CLOSE **/
		separator = gtk_hseparator_new();
		gtk_box_pack_start(GTK_BOX(box_psidm), separator, FALSE, TRUE, 2);
		gtk_widget_show (separator);

		box_1 = gtk_hbox_new(TRUE, 0);
		gtk_box_pack_start(GTK_BOX(box_psidm), box_1, FALSE, FALSE, 0);
		gtk_widget_show (box_1);

		button = gtk_button_new_with_label ("Load");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
					GTK_SIGNAL_FUNC (load_prefs), NULL);
		gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
		gtk_widget_show (button);

		button = gtk_button_new_with_label ("Save");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
					GTK_SIGNAL_FUNC (save_prefs), NULL);
		gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
		gtk_widget_show (button);

		button = gtk_button_new_with_label ("Close");
		gtk_signal_connect (GTK_OBJECT (button), "clicked",
					GTK_SIGNAL_FUNC (close_psident_window), NULL);
		gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
		gtk_widget_show (button);

		gtk_widget_show (window_psid);
		}

	return;
}

void open_main_window()
{
	int i;
	GtkWidget *boxl, *boxr, *box_1, *box_2, *table, *table_main, *separator,
			*notebook, *frame, *button, *menu, *item, *carscroll, *scale;

	char *algo_name[5] = {"Dishevel-Bippsie","LZI","LZII","PsyDET","Xebec"};
	char *scan_mode_name[7] = {"EM","VLF","ELF","ULF","RLF","Sonic","Psi"};

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), "MindGuard X");
	gtk_container_set_border_width (GTK_CONTAINER (window), 2);
	gtk_signal_connect (GTK_OBJECT (window), "delete_event",
				GTK_SIGNAL_FUNC (quit), NULL);
	gtk_window_set_policy (GTK_WINDOW (window), FALSE, FALSE, FALSE);

	if (center_windows)
		gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	table_main = gtk_table_new (3, 3, FALSE);
	gtk_container_add (GTK_CONTAINER (window), table_main);
	gtk_widget_show (table_main);

	boxl = gtk_vbox_new(FALSE, 0);
	gtk_table_attach (GTK_TABLE(table_main), boxl, 0, 1, 0, 1,
				FALSE, GTK_FILL | GTK_EXPAND, 0, 0);
	gtk_widget_show (boxl);

	separator = gtk_vseparator_new();
	gtk_table_attach (GTK_TABLE(table_main), separator, 1, 2, 0, 1,
				FALSE, GTK_FILL | GTK_EXPAND, 2, 0);
	gtk_widget_show (separator);

	separator = gtk_hseparator_new();
	gtk_table_attach (GTK_TABLE(table_main), separator, 0, 3, 1, 2,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 2);
	gtk_widget_show (separator);

	boxr = gtk_vbox_new(FALSE, 0);
	gtk_table_attach (GTK_TABLE(table_main), boxr, 2, 3, 0, 1,
				GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);
	gtk_widget_show (boxr);

    /** CARRIER LIST **/
	button = gtk_button_new_with_label ("Known Carriers");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (reload_mods), NULL);
	gtk_box_pack_start(GTK_BOX(boxl), button, FALSE, FALSE, 0);
	gtk_widget_show (button);

	carscroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(carscroll),
					GTK_POLICY_NEVER,
					GTK_POLICY_AUTOMATIC);
	gtk_box_pack_start(GTK_BOX(boxl), carscroll, TRUE, TRUE, 0);
	gtk_widget_show (carscroll);

	/* created carlist in main() */
	gtk_signal_connect (GTK_OBJECT (carlist), "select_row",
				GTK_SIGNAL_FUNC (carrier_info), NULL);
	gtk_clist_set_column_width (GTK_CLIST(carlist), 0, 130);
	gtk_container_add(GTK_CONTAINER(carscroll), carlist);
	gtk_widget_show (carlist);

	separator = gtk_hseparator_new();
	gtk_box_pack_start(GTK_BOX(boxl), separator, FALSE, FALSE, 2);
	gtk_widget_show (separator);

    /** SCAN SETTINGS TABLE **/
	table = gtk_table_new (3, 2, FALSE);
	gtk_box_pack_start(GTK_BOX(boxl), table, FALSE, FALSE, 0);
	gtk_widget_show (table);

	/* ALGORITHM MENU */
	opt_algo = gtk_option_menu_new ();
	menu = gtk_menu_new ();
	for ( i = 0 ; i < 5; i++)
		{
		item = gtk_menu_item_new_with_label (algo_name[i]);
		gtk_signal_connect (GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (algo_adjust), GINT_TO_POINTER (i));
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU (menu), item);
		}
	gtk_option_menu_set_menu (GTK_OPTION_MENU (opt_algo), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (opt_algo), (guint) algo);
	gtk_table_attach (GTK_TABLE(table), opt_algo, 0, 2, 0, 1,
				GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
	gtk_widget_show (opt_algo);

	/* SCAN MODE MENU */
	opt_scan_mode = gtk_option_menu_new ();
	menu = gtk_menu_new ();
	for ( i = 0 ; i < 7; i++)
		{
		item = gtk_menu_item_new_with_label (scan_mode_name[i]);
		gtk_signal_connect (GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (scan_mode_adjust), GINT_TO_POINTER (i));
		gtk_widget_show (item);
		gtk_menu_append (GTK_MENU (menu), item);
		}
	gtk_option_menu_set_menu (GTK_OPTION_MENU (opt_scan_mode), menu);
	gtk_option_menu_set_history (GTK_OPTION_MENU (opt_scan_mode), (guint) scan_mode);
	gtk_table_attach (GTK_TABLE(table), opt_scan_mode, 0, 1, 1, 2,
				GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
	gtk_widget_show (opt_scan_mode);

	/* AUTO DECIPHER */
	check_auto = gtk_check_button_new_with_label ("Auto Decipher");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_auto), state_auto);
	gtk_signal_connect (GTK_OBJECT (check_auto), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_AUTO));
	gtk_table_attach (GTK_TABLE(table), check_auto, 1, 2, 1, 2,
				GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
	gtk_widget_show (check_auto);

	/* SCANNING DEPTH */
	adj_scan = gtk_adjustment_new (scan_depth, 0, 51, 1, 1, 1);
	gtk_signal_connect (GTK_OBJECT (adj_scan), "value_changed",
				GTK_SIGNAL_FUNC (scan_depth_adjust), NULL);
	scale = gtk_hscale_new (GTK_ADJUSTMENT (adj_scan));
	gtk_scale_set_value_pos (GTK_SCALE (scale), GTK_POS_RIGHT);
	gtk_scale_set_digits (GTK_SCALE (scale), 0);
	gtk_table_attach (GTK_TABLE(table), scale, 0, 2, 2, 3,
				GTK_FILL | GTK_EXPAND, GTK_FILL, 0, 0);
	gtk_widget_show (scale);

    /** SCAN PSIDENT DEPSYCH **/
	box_1 = gtk_hbox_new(TRUE, 0);
	gtk_box_pack_start(GTK_BOX(boxr), box_1, FALSE, FALSE, 0);
	gtk_widget_show (box_1);

	but_scan = gtk_button_new_with_label ("Scan");
	gtk_signal_connect (GTK_OBJECT (but_scan), "clicked",
				GTK_SIGNAL_FUNC (scan), NULL);
	gtk_box_pack_start(GTK_BOX(box_1), but_scan, TRUE, TRUE, 0);
	gtk_widget_show (but_scan);

	set_scan_button_state();

	but_psid = gtk_button_new_with_label ("PsIdent");
	gtk_signal_connect (GTK_OBJECT (but_psid), "clicked",
				GTK_SIGNAL_FUNC (open_psident_window), NULL);
	gtk_box_pack_start(GTK_BOX(box_1), but_psid, TRUE, TRUE, 0);
	gtk_widget_show (but_psid);

	button = gtk_button_new_with_label ("DePsych");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (depsych), NULL);
	gtk_box_pack_start (GTK_BOX(box_1), button, TRUE, TRUE, 0);
	gtk_widget_show (button);

    /** DIAGNOSTIC BOX **/
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_box_pack_start (GTK_BOX(boxr), frame, TRUE, TRUE, 0);
	gtk_widget_show (frame);

	box_1 = gtk_vbox_new (FALSE, 0);
	gtk_container_add (GTK_CONTAINER(frame), box_1);
	gtk_widget_show (box_1);

	for ( i = 0 ; i < 5; i++)
		{
		box_2 = gtk_hbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX(box_1), box_2, FALSE, FALSE, 0);
		gtk_widget_show (box_2);

		label_diag[i] = gtk_label_new (NULL);
		gtk_misc_set_alignment (GTK_MISC(label_diag[i]), 0, 0);
		gtk_box_pack_start (GTK_BOX(box_2), label_diag[i], FALSE, FALSE, 2);
		gtk_widget_show (label_diag[i]);
		}

    /** PREFS NOTEBOOK **/
	notebook = gtk_notebook_new ();
	gtk_notebook_set_tab_pos (GTK_NOTEBOOK(notebook), GTK_POS_TOP);
	gtk_box_pack_start (GTK_BOX(boxr), notebook, FALSE, FALSE, 0);
	gtk_widget_show (notebook);

	table = gtk_table_new (2, 2, TRUE);
	gtk_widget_show (table);

	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), table, gtk_label_new("Jam"));

	check_silent = gtk_check_button_new_with_label ("Silent");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_silent), state_silent);
	gtk_signal_connect (GTK_OBJECT (check_silent), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_SILENT));
	gtk_table_attach (GTK_TABLE(table), check_silent, 0, 1, 0, 1,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_silent);

	check_phase = gtk_check_button_new_with_label ("Phase Check");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_phase), phase_check);
	gtk_signal_connect (GTK_OBJECT (check_phase), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_PHASE));
	gtk_table_attach (GTK_TABLE(table), check_phase, 0, 1, 1, 2,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_phase);

	check_doppler = gtk_check_button_new_with_label ("Dopplerator");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_doppler), dopplerator);
	gtk_signal_connect (GTK_OBJECT (check_doppler), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_DOPPLER));
	gtk_table_attach (GTK_TABLE(table), check_doppler, 1, 2, 0, 1,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_doppler);

	check_feedback = gtk_check_button_new_with_label ("Kill Feedback");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_feedback), kill_feedback);
	gtk_signal_connect (GTK_OBJECT (check_feedback), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_FEEDBACK));
	gtk_table_attach (GTK_TABLE(table), check_feedback, 1, 2, 1, 2,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_feedback);

	table = gtk_table_new (2, 2, TRUE);
	gtk_widget_show (table);

	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), table, gtk_label_new("Log"));

	check_log = gtk_check_button_new_with_label ("Log");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_log), state_log);
	gtk_signal_connect (GTK_OBJECT (check_log), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_LOG));
	gtk_table_attach (GTK_TABLE(table), check_log, 0, 1, 0, 1,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_log);

	button = gtk_button_new_with_label ("Flush Log");
	gtk_table_attach (GTK_TABLE(table), button, 1, 2, 0, 1,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (flush_log), NULL);
	gtk_widget_show (button);

	but_logview = gtk_button_new_with_label ("View Log");
	gtk_table_attach (GTK_TABLE(table), but_logview, 1, 2, 1, 2,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_signal_connect (GTK_OBJECT (but_logview), "clicked",
				GTK_SIGNAL_FUNC (open_logview_window), NULL);
	gtk_widget_show (but_logview);

	table = gtk_table_new (2, 2, TRUE);
	gtk_widget_show (table);

	gtk_notebook_append_page (GTK_NOTEBOOK(notebook), table, gtk_label_new("Misc"));

	check_aq = gtk_check_button_new_with_label ("Ask To Quit");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_aq), ask_quit);
	gtk_signal_connect (GTK_OBJECT (check_aq), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_ATQ));
	gtk_table_attach (GTK_TABLE(table), check_aq, 0, 1, 0, 1,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_aq);

	check_soe = gtk_check_button_new_with_label ("Save On Exit");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_soe), save_on_exit);
	gtk_signal_connect (GTK_OBJECT (check_soe), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_SOE));
	gtk_table_attach (GTK_TABLE(table), check_soe, 0, 1, 1, 2,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_soe);

	check_center = gtk_check_button_new_with_label ("Center Windows");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_center), center_windows);
	gtk_signal_connect (GTK_OBJECT (check_center), "toggled",
				GTK_SIGNAL_FUNC (check_widget_callback),
				GINT_TO_POINTER(CHECK_CENTER));
	gtk_table_attach (GTK_TABLE(table), check_center, 1, 2, 0, 1,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (check_center);

	but_about = gtk_button_new_with_label ("About");
	gtk_table_attach (GTK_TABLE(table), but_about, 1, 2, 1, 2,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_signal_connect (GTK_OBJECT (but_about), "clicked",
				GTK_SIGNAL_FUNC (open_about_window), NULL);
	gtk_widget_show (but_about);

    /** STATUS/SCANNER BOX **/
	frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
	gtk_widget_set_usize (GTK_WIDGET(frame), FALSE, 22);
	gtk_table_attach (GTK_TABLE(table_main), frame, 0, 1, 2, 3,
				GTK_FILL, FALSE, 0, 0);
	gtk_widget_show (frame);

	box_1 = gtk_vbox_new(FALSE, 0);
	gtk_container_add (GTK_CONTAINER(frame), box_1);
	gtk_widget_show (box_1);

	/** don't show these until later **/
	scan_label = gtk_label_new ("NO CARRIERS");
	gtk_box_pack_start(GTK_BOX(box_1), scan_label, TRUE, TRUE, 0);

	scan_progress = gtk_progress_bar_new ();
	gtk_box_pack_start(GTK_BOX(box_1), scan_progress, TRUE, TRUE, 0);

	scan_draw = gtk_drawing_area_new();
	gtk_box_pack_start(GTK_BOX(box_1), scan_draw, TRUE, TRUE, 0);
	gtk_signal_connect (GTK_OBJECT(scan_draw), "expose_event",
				(GtkSignalFunc) draw_scan, NULL);
	gtk_signal_connect (GTK_OBJECT(scan_draw), "configure_event",
				(GtkSignalFunc) create_scan, NULL);

    /** LOAD SAVE QUIT **/
	box_1 = gtk_hbox_new(TRUE, 0);
	gtk_table_attach (GTK_TABLE(table_main), box_1, 2, 3, 2, 3,
				GTK_FILL | GTK_EXPAND, FALSE, 0, 0);
	gtk_widget_show (box_1);

	button = gtk_button_new_with_label ("Load");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (load_prefs), NULL);
	gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
	gtk_widget_show (button);

	button = gtk_button_new_with_label ("Save");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (save_prefs), NULL);
	gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
	gtk_widget_show (button);

	button = gtk_button_new_with_label ("Quit");
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
				GTK_SIGNAL_FUNC (quit), NULL);
	gtk_box_pack_start(GTK_BOX(box_1), button, TRUE, TRUE, 0);
	gtk_widget_show (button);


	gtk_widget_show (window);

	return;
}


/* ...And your mind lived happily ever after.  The End */
