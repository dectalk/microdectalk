#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/xpm.h>

#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/Scale.h>

#include "icons/pau203a.xpm"
#include "icons/bet203a.xpm"
#include "icons/har203a.xpm"
#include "icons/fra203a.xpm"
#include "icons/den203a.xpm"
#include "icons/kid203a.xpm"
#include "icons/urs203a.xpm"
#include "icons/rit203a.xpm"
#include "icons/wen203a.xpm"

#include <stdio.h>
#include <string.h>

#include <stb_ds.h>
#include <miniaudio.h>

#include <epsonapi.h>

XtAppContext ctx;
Widget top;
Widget w_main, w_form, w_brc, w_text, w_speaking, w_rate, w_wpm, w_play, w_stop;

ma_device_config config;
ma_device device;
ma_mutex speaking;

int rate = 200;

Widget w_paul, w_betty, w_harry, w_frank, w_dennis, w_kit, w_ursula, w_rita, w_wendy;
Pixmap p_paul, p_betty, p_harry, p_frank, p_dennis, p_kit, p_ursula, p_rita, p_wendy;
Pixmap m_paul, m_betty, m_harry, m_frank, m_dennis, m_kit, m_ursula, m_rita, m_wendy;

void speak(const char* speak);

void button_pressed(Widget w, void* data, void* ptr){
	const char* person = "placeholder";
	char buffer[64]; /* bad */
	if(w == w_paul) person = "Paul";
	if(w == w_betty) person = "Betty";
	if(w == w_harry) person = "Harry";
	if(w == w_frank) person = "Frank";
	if(w == w_dennis) person = "Dennis";
	if(w == w_kit) person = "Kit";
	if(w == w_ursula) person = "Ursula";
	if(w == w_rita) person = "Rita";
	if(w == w_wendy) person = "Wendy";

	sprintf(buffer, "[:name %s]%s", person, person);
	speak(buffer);
}

void play_text(Widget w, void* data, void* ptr){
	char* str = XmTextGetString(w_text);
	speak(str);
	XtFree(str);
}

void rate_changed(Widget w, void* data, void* ptr){
	XmScaleCallbackStruct* s = (XmScaleCallbackStruct*)ptr;
	char buf[32];
	Arg args[1];
	XmString str;
	rate = s->value;
	sprintf(buf, "%d WPM", rate);

	str = XmStringCreateLocalized(buf);
	XtSetArg(args[0], XmNlabelString, str);
	XtSetValues(w_wpm, args, 1);
	XmStringFree(str);
}

void load(Widget* w, Pixmap* p, Pixmap* m, char* px[]){
	XpmCreatePixmapFromData(XtDisplay(top), DefaultRootWindow(XtDisplay(top)), px, p, m, NULL);
	*w = XtVaCreateWidget("paul", xmPushButtonWidgetClass, w_brc,
		XmNlabelType, XmPIXMAP,
		XmNlabelPixmap, *p,
		XmNbackground, 0xb3353f,
	NULL);

	XtAddCallback(*w, XmNactivateCallback, button_pressed, NULL);

	XtManageChild(*w);
}

typedef struct buffer {
	short* data;
	unsigned int length;
	unsigned int seek;
} buffer_t;

buffer_t* buffers = NULL;

unsigned int g_length;
short* g_wave = NULL;

void stop_text(Widget w, void* data, void* ptr){
	ma_mutex_lock(&speaking);
	if(arrlen(buffers) > 0){
		while(arrlen(buffers) > 0){
			free(buffers[0].data);
			arrdel(buffers, 0);
		}
	}
	ma_mutex_unlock(&speaking);
}

short* write_wav(short* iwave, unsigned int length){
	if(g_wave == NULL){
		g_wave = malloc(length * 2);
		memcpy(g_wave, iwave, length * 2);
	}else{
		short* old = g_wave;
		g_wave = malloc((g_length + length) * 2);
		memcpy(g_wave, old, g_length * 2);
		memcpy(g_wave + g_length, iwave, length * 2);
		free(old);
	}

	g_length += length;

	return NULL;
}

void speak(const char* speak){
	buffer_t buf;
	char ratebuf[32];
	char* sbuf;
	sprintf(ratebuf, "[:rate %d]", rate);

	sbuf = malloc(strlen(ratebuf) + strlen(speak) + 1);
	sbuf[0] = 0;
	strcat(sbuf, ratebuf);
	strcat(sbuf, speak);

	if(g_wave != NULL){
		free(g_wave);
		g_wave = NULL;
	}
	g_length = 0;

	TextToSpeechStart(sbuf, NULL, WAVE_FORMAT_1M16);

	free(sbuf);

	buf.data = malloc(g_length * 2);
	memcpy(buf.data, g_wave, g_length * 2);
	buf.length = g_length;
	buf.seek = 0;

	ma_mutex_lock(&speaking);
	arrput(buffers, buf);
	ma_mutex_unlock(&speaking);
}

void data_callback(ma_device* dev, void* out, const void* in, ma_uint32 frame){
	unsigned int seek = 0;
	memset(out, 0, frame * 2);
	ma_mutex_lock(&speaking);
	while(frame > 0 && arrlen(buffers) > 0){
		unsigned int bsz = buffers[0].length - buffers[0].seek;
		unsigned int sz = bsz > frame ? frame : bsz;

		memcpy(((short*)out) + seek, buffers[0].data + buffers[0].seek, sz * 2);

		buffers[0].seek += sz;
		frame -= sz;
		seek += sz;

		if((buffers[0].length - buffers[0].seek) == 0){
			free(buffers[0].data);
			arrdel(buffers, 0);
		}
	}
	ma_mutex_unlock(&speaking);
}

int main(int argc, char** argv){
	Arg args[16];
	int i;
	XmString str;

	config = ma_device_config_init(ma_device_type_playback);
	config.playback.format = ma_format_s16;
	config.playback.channels = 1;
	config.sampleRate = 11025;
	config.dataCallback = data_callback;

	if(ma_device_init(NULL, &config, &device) != MA_SUCCESS || ma_device_start(&device) != MA_SUCCESS){
		fprintf(stderr, "your sound is broken\n");
		return 1;
	}

	ma_mutex_init(&speaking);

	top = XtVaAppInitialize(&ctx, "Speak", NULL, 0, &argc, argv, NULL, XtNtitle, "Speak",
		XmNmaxWidth, 660,
		XmNmaxHeight, 480,
		XmNminWidth, 660,
		XmNminHeight, 480,
	NULL);
	if(top == NULL){
		return 1;
	}

	TextToSpeechInit(write_wav, NULL);

	w_main = XtVaCreateManagedWidget("main", xmMainWindowWidgetClass, top,
	NULL);

	w_form = XtVaCreateWidget("mainform", xmFormWidgetClass, w_main,
	NULL);

	w_brc = XtVaCreateWidget("buttonrc", xmRowColumnWidgetClass, w_form,
		XmNpacking, XmPACK_COLUMN,
		XmNorientation, XmHORIZONTAL,
		XmNspacing, 0,
		XmNentryAlignment, XmALIGNMENT_CENTER,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNheight, 74,
	NULL);

	load(&w_paul, &p_paul, &m_paul, pau203a);
	load(&w_betty, &p_betty, &m_betty, bet203a);
	load(&w_harry, &p_harry, &m_harry, har203a);
	load(&w_frank, &p_frank, &m_frank, fra203a);
	load(&w_dennis, &p_dennis, &m_dennis, den203a);
	load(&w_kit, &p_kit, &m_kit, kid203a);
	load(&w_ursula, &p_ursula, &m_ursula, urs203a);
	load(&w_rita, &p_rita, &m_rita, rit203a);
	load(&w_wendy, &p_wendy, &m_wendy, wen203a);

	XtManageChild(w_brc);

	i = 0;
	XtSetArg(args[i], XmNeditMode, XmMULTI_LINE_EDIT); i++;
	XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET); i++;
	XtSetArg(args[i], XmNtopWidget, w_brc); i++;
	XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM); i++;
	XtSetArg(args[i], XmNheight, 380); i++;
	w_text = XmCreateScrolledText(w_form, "text", args, i);

	XtManageChild(w_text);

	w_speaking = XtVaCreateWidget("Speaking Rate", xmLabelWidgetClass, w_form,
		XmNleftAttachment, XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, w_text,
		XmNbottomAttachment, XmATTACH_FORM,
	NULL);

	XtManageChild(w_speaking);

	i = 0;
	XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET); i++;
	XtSetArg(args[i], XmNbottomWidget, w_speaking); i++;
	XtSetValues(w_text, args, i);

	w_rate = XtVaCreateWidget("rate", xmScaleWidgetClass, w_form,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, w_speaking,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, w_text,
		XmNwidth, 200,
		XmNscaleHeight, 30,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNorientation, XmHORIZONTAL,
		XmNminimum, 75,
		XmNmaximum, 600,
		XmNscaleMultiple, 5,
		XmNvalue, rate,
	NULL);
	XtAddCallback(w_rate, XmNvalueChangedCallback, rate_changed, NULL);

	XtManageChild(w_rate);

	str = XmStringCreateLocalized("200 WPM");
	w_wpm = XtVaCreateWidget("wpm", xmLabelWidgetClass, w_form,
		XmNleftAttachment, XmATTACH_WIDGET,
		XmNleftWidget, w_rate,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, w_text,
		XmNmarginWidth, 50,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNlabelString, str,
		XmNborderWidth, 1,
	NULL);
	XmStringFree(str);
	
	XtManageChild(w_wpm);

	w_stop = XtVaCreateWidget("Stop", xmPushButtonWidgetClass, w_form,
		XmNwidth, 48,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, w_text,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
	NULL);
	XtAddCallback(w_stop, XmNactivateCallback, stop_text, NULL);

	XtManageChild(w_stop);

	w_play = XtVaCreateWidget("Play", xmPushButtonWidgetClass, w_form,
		XmNwidth, 48,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, w_text,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_WIDGET,
		XmNrightWidget, w_stop,
	NULL);
	XtAddCallback(w_play, XmNactivateCallback, play_text, NULL);

	XtManageChild(w_play);

	XtManageChild(w_form);

	XtResizeWidget(top, 660, 480, 1);
	XtRealizeWidget(top);

	XtAppMainLoop(ctx);

	ma_device_uninit(&device);
	ma_mutex_uninit(&speaking);
}
