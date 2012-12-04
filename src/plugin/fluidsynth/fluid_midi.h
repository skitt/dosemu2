/* This is a rip-off of the fluid_midi.h found if fluidsynth sources,
 * done by stsp for dosemu project. */

/* FluidSynth - A Software Synthesizer
 *
 * Copyright (C) 2003  Peter Hanappe and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License
 * as published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#ifndef _FLUID_MIDI_H
#define _FLUID_MIDI_H

typedef struct _fluid_midi_parser_t fluid_midi_parser_t;

fluid_midi_parser_t* new_fluid_midi_parser(void);
int delete_fluid_midi_parser(fluid_midi_parser_t* parser);
fluid_midi_event_t* fluid_midi_parser_parse(fluid_midi_parser_t* parser, unsigned char c);

/*
 * fluid_midi_event_t
 */
struct _fluid_midi_event_t {
  fluid_midi_event_t* next; /* Link to next event */
  void *paramptr;           /* Pointer parameter (for SYSEX data), size is stored to param1, param2 indicates if pointer should be freed (dynamic if TRUE) */
  unsigned int dtime;       /* Delay (ticks) between this and previous event. midi tracks. */
  unsigned int param1;      /* First parameter */
  unsigned int param2;      /* Second parameter */
  unsigned char type;       /* MIDI event type */
  unsigned char channel;    /* MIDI channel */
};

#define FLUID_MIDI_PARSER_MAX_DATA_SIZE 1024    /**< Maximum size of MIDI parameters/data (largest is SYSEX data) */

/*
 * fluid_midi_parser_t
 */
struct _fluid_midi_parser_t {
  unsigned char status;           /* Identifies the type of event, that is currently received ('Noteon', 'Pitch Bend' etc). */
  unsigned char channel;          /* The channel of the event that is received (in case of a channel event) */
  unsigned int nr_bytes;          /* How many bytes have been read for the current event? */
  unsigned int nr_bytes_total;    /* How many bytes does the current event type include? */
  unsigned char data[FLUID_MIDI_PARSER_MAX_DATA_SIZE]; /* The parameters or SYSEX data */
  fluid_midi_event_t event;        /* The event, that is returned to the MIDI driver. */
};

#endif /* _FLUID_MIDI_H */
