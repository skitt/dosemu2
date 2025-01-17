/*
 *  Copyright (C) 2006 Stas Sergeev <stsp@users.sourceforge.net>
 *
 * The below copyright strings have to be distributed unchanged together
 * with this file. This prefix can not be modified or separated.
 */

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include "emu.h"
#include "init.h"
#include "sound/midi.h"
#include <alsa/asoundlib.h>


static snd_rawmidi_t *handle, *handle_v;
#define midoalsa_name "alsa_midi"
#define midoalsa_longname "MIDI Output: ALSA device"
#define midoalsav_name "alsa_virmidi"
#define midoalsav_longname "MIDI Output: ALSA virtual device (for MT32)"
static const char *device_name_param = "dev_name";
static const char *device = "default";
static const char *device_v = "virtual";
static const char *cur_dev;

static int midoalsa_open(snd_rawmidi_t **handle_p, const char *dev_name)
{
    int err;
    cur_dev = dev_name; // for logging only
    err = snd_rawmidi_open(NULL, handle_p, dev_name,
			   SND_RAWMIDI_NONBLOCK | SND_RAWMIDI_SYNC);
    if (err) {
	S_printf("%s: unable to open %s for writing: %s\n",
		 midoalsa_name, dev_name, snd_strerror(err));
	if (err == -2) {
	    error("@snd-virmidi module not loaded or device \"%s\" not configured\n",
		    dev_name);
	    error("@see \"amidi -l\" for the list of midi devices\n");
	}
	return 0;
    }
    /* NONBLOCK flag is needed only so that open() not to block forever */
    snd_rawmidi_nonblock(*handle_p, 0);
    return 1;
}

static void alsa_log_handler(const char *file, int line, const char *function,
                             int err, const char *fmt,...)
{
    char s[1024];
    int l;
    va_list arg;

    va_start(arg, fmt);

    l = snprintf(s, sizeof(s), "%s:%s (ALSA err %i): ",
            midoalsa_name, cur_dev, err);
    if (l >= 0)
        l += vsnprintf(s+l, sizeof(s)-l, fmt, arg);
    if(err && l >= 0)
        l += snprintf(s+l, sizeof(s)-l, ": %s", snd_strerror(err));

    s[sizeof(s)-1] = '\0';

    va_end(arg);

    if (/*err && */!config.quiet)
        error("%s\n", s);
    else
        warn("%s\n", s);
}

static int do_alsa_open(snd_rawmidi_t **handle_p, const char *plu_name,
	const char *def_dev)
{
    char *dname = pcm_parse_params(config.snd_plugin_params,
	    plu_name, device_name_param);
    const char *dev_name = dname ?: def_dev;
    int ret;

    snd_lib_error_set_handler(&alsa_log_handler);
    ret = midoalsa_open(handle_p, dev_name);
    /* reset back the error handler to not steal logs from
     * other subsystems, like audio */
    snd_lib_error_set_handler(NULL);
    free(dname);
    return ret;
}

static int midoalsa_init(void *arg)
{
    return do_alsa_open(&handle, midoalsa_name, device);
}

static void midoalsa_done(void *arg)
{
    if (!handle)
	return;
    snd_rawmidi_close(handle);
    handle = NULL;
}

static void midoalsa_write(unsigned char val)
{
    if (!handle)
	return;
    snd_rawmidi_write(handle, &val, 1);
}

static int midoalsa_cfg(void *arg)
{
    return pcm_parse_cfg(config.midi_driver, midoalsa_name);
}

static const struct midi_out_plugin midoalsa
#ifdef __cplusplus
{
    midoalsa_name,
    midoalsa_longname,
    midoalsa_cfg,
    midoalsa_init,
    midoalsa_done,
    MIDI_W_PREFERRED,
    midoalsa_write,
    NULL, NULL,
    ST_GM,
    0
};
#else
= {
    .name = midoalsa_name,
    .longname = midoalsa_longname,
    .get_cfg = midoalsa_cfg,
    .open = midoalsa_init,
    .close = midoalsa_done,
    .weight = MIDI_W_PREFERRED,
    .write = midoalsa_write,
    .stype = ST_GM,
};
#endif

static int midoalsav_init(void *arg)
{
    return do_alsa_open(&handle_v, midoalsav_name, device_v);
}

static void midoalsav_done(void *arg)
{
    if (!handle_v)
	return;
    snd_rawmidi_close(handle_v);
    handle_v = NULL;
}

static void midoalsav_write(unsigned char val)
{
    if (!handle_v)
	return;
    snd_rawmidi_write(handle_v, &val, 1);
}

static int midoalsav_cfg(void *arg)
{
    return pcm_parse_cfg(config.midi_driver, midoalsav_name);
}

static const struct midi_out_plugin midoalsa_v
#ifdef __cplusplus
{
    midoalsav_name,
    midoalsav_longname,
    midoalsav_cfg,
    midoalsav_init,
    midoalsav_done,
    0,
    midoalsav_write,
    NULL, NULL,
    ST_MT32,
    0
};
#else
= {
    .name = midoalsav_name,
    .longname = midoalsav_longname,
    .get_cfg = midoalsav_cfg,
    .open = midoalsav_init,
    .close = midoalsav_done,
    .write = midoalsav_write,
    .stype = ST_MT32,
};
#endif

CONSTRUCTOR(static int midoalsa_register(void))
{
    midi_register_output_plugin(&midoalsa);
    midi_register_output_plugin(&midoalsa_v);
    return 0;
}
