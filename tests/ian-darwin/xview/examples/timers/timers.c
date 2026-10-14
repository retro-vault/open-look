/* Extracted from xview.timers.dist; see adjacent archived message for author. */
#include <stdio.h>
#include <xview/xview.h>
#include <xview/panel.h>


static Attr_attribute	INSTANCE = 0;
typedef struct {
    Frame		frame;
    Panel		panel;
    Panel_button_item	btn1;
    Panel_button_item	btn2;
    Panel_gauge_item	gg1;
    Panel_gauge_item	gg2;
} My_ui;


static void
my_timer_func( Notify_client client, int which )
{
    My_ui *ui = (My_ui *) xv_get((Xv_opaque)client, XV_KEY_DATA, INSTANCE);
    Panel_gauge_item gg = (client == ui->btn1) ? ui->gg1 : ui->gg2;
    int val = (int) xv_get(gg, PANEL_VALUE);

    xv_set(gg, PANEL_VALUE, ++val, NULL);
}


static void
fire_proc(Panel_button_item item, Event *event)
{
    struct itimerval timer = {0};

    timer.it_interval.tv_sec = 1;
    timer.it_value.tv_sec = 1;
    notify_set_itimer_func( (Notify_client)item, (Notify_func) my_timer_func, 
			   ITIMER_REAL, &timer, NULL
			   );
}

int
main(int argc, char **argv)
{
    My_ui ui;

    (void) xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
    INSTANCE = xv_unique_key();

    ui.frame = xv_create(XV_NULL, FRAME, NULL);
    ui.panel = xv_create(ui.frame, PANEL, NULL);

    ui.btn1 = xv_create(ui.panel, PANEL_BUTTON,
			PANEL_LABEL_STRING,	"Fire 1",
			PANEL_NOTIFY_PROC,	fire_proc,
			XV_KEY_DATA,		INSTANCE, &ui,
			NULL);
    
    ui.btn2 = xv_create(ui.panel, PANEL_BUTTON,
			PANEL_LABEL_STRING,	"Fire 2",
			PANEL_NOTIFY_PROC,	fire_proc,
			XV_KEY_DATA,		INSTANCE, &ui,
			NULL);

    ui.gg1 = xv_create(ui.panel, PANEL_GAUGE,
		       PANEL_NEXT_ROW,		-1,
		       PANEL_LABEL_STRING,	"Gauge1:",
		       XV_KEY_DATA,		INSTANCE, &ui,
		       NULL);

    ui.gg2 = xv_create(ui.panel, PANEL_GAUGE,
		       PANEL_NEXT_ROW,		-1,
		       PANEL_LABEL_STRING,	"Gauge2:",
		       XV_KEY_DATA,		INSTANCE, &ui,
		       NULL);

    window_fit( ui.panel );
    window_fit( ui.frame );
    xv_main_loop( ui.frame );
}

