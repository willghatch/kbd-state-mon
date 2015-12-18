/*
kbd-state-mon
Copyright (C) 2008 John Goerzen
<jgoerzen@complete.org>
Copyright (C) 2015 William Hatch
<willghatch@gmail.com>
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/


#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>


static int xkb_event_base = 0;
static int xkb_error_base = 0;
static Bool onlyCurrentStateP = True;

void displayState(Display *d, unsigned int latch, unsigned int lock) {

  int shift = XkbKeysymToModifiers(d, XK_Shift_L);
  int caps = XkbKeysymToModifiers(d, XK_Caps_Lock);
  int l3 = XkbKeysymToModifiers(d, XK_ISO_Level3_Shift);
  int l5 = XkbKeysymToModifiers(d, XK_ISO_Level5_Shift);
  int control = XkbKeysymToModifiers(d, XK_Control_L);
  int alt = XkbKeysymToModifiers(d, XK_Alt_L);
  int super = XkbKeysymToModifiers(d, XK_Super_L);
  int hyper = XkbKeysymToModifiers(d, XK_Hyper_L);
  int num_lock = XkbKeysymToModifiers(d, XK_Num_Lock);
  int scroll_lock = XkbKeysymToModifiers(d, XK_Scroll_Lock);

  /* latches */
  if(latch) printf("Latch: ");
  if(latch & shift) printf("Shift ");
  if(latch & caps) printf("Caps ");
  if(latch & l3) printf("L3 ");
  if(latch & l5) printf("L5 ");
  if(latch & control) printf("Ctl ");
  if(latch & alt) printf("Alt ");
  if(latch & super) printf("Sup ");
  if(latch & hyper) printf("Hyp ");
  if(latch & num_lock) printf("Num ");
  if(latch & scroll_lock) printf("Scroll ");


  /* locks */
  if(lock) printf("Lock: ");
  if(lock & shift) printf("Shift ");
  if(lock & caps) printf("Caps ");
  if(lock & l3) printf("L3 ");
  if(lock & l5) printf("L5 ");
  if(lock & control) printf("Ctl ");
  if(lock & alt) printf("Alt ");
  if(lock & super) printf("Sup ");
  if(lock & hyper) printf("Hyp ");
  if(lock & num_lock) printf("Num ");
  if(lock & scroll_lock) printf("Scroll ");

  printf("\n");
  fflush(stdout);
}

int main(int argc, char **argv) {
  Display *disp;
  int opcode;
  int maj = XkbMajorVersion;
  int min = XkbMinorVersion;
  XkbEvent ev;
  XkbStateRec state;
  int opt;


  while ((opt = getopt (argc, argv, "w")) != -1) {
    switch (opt) {
    case 'w':
      onlyCurrentStateP = False;
      break;
    default:
      printf("Usage: %s [-w]\n", argv[0]);
      printf("-w keep watching -- don't exit after getting the initial state\n");
      return 0;
    }
  }

  /* Open Display */
  if ( !(disp = XOpenDisplay(NULL))) {
    fprintf(stderr, "Can't open display: CHECK DISPLAY VARIABLE\n");
    exit(1);
  }

  if (!XkbLibraryVersion(&maj, &min)) {
    fprintf(stderr, "Couldn't get Xkb library version\n");
    exit(1);
  }
  
  if (!XkbQueryExtension(disp, &opcode, &xkb_event_base, &xkb_error_base, &maj, &min)) {
    fprintf(stderr, "XkbQueryExtension error\n");
    exit(1);
  }

  // print initial status
  XkbGetState(disp, XkbUseCoreKbd, &state);
  displayState(disp, state.latched_mods, state.locked_mods);

  if (onlyCurrentStateP){
    exit(0);
  }

  // loop forever, printing status

  if (!XkbSelectEvents(disp, XkbUseCoreKbd, XkbStateNotifyMask, 
                       XkbStateNotifyMask)) {
    fprintf(stderr, "XkbSelectEvents\n");
    exit(1);
  }

  while (1) {
    XNextEvent(disp, &ev.core);
    if (ev.type == xkb_event_base && ev.any.xkb_type == XkbStateNotify) {
      displayState(disp, ev.state.latched_mods, ev.state.locked_mods);
    }
  }
  return 0;
}


/*
   TODO - I might want to display info about groups

   This is all the info that is contained in the xkb state event:

typedef struct {
      int            type;            << Xkb extension base event code >>
      unsigned long  serial;          << X server serial number for event >>
      Bool           send_event;      <<  True => synthetically generated >>
      Display *      display;         << server connection where event generated >>
      Time           time;            << server time when event generated >>
      int            xkb_type;        <<  XkbStateNotify >>
      int            device;          << Xkb device ID, will not be  XkbUseCoreKbd >>
      unsigned int   changed;         << bits indicating what has changed >>
      int            group;           << group index of effective group >>
      int            base_group;      << group index of base group >>
      int            latched_group;   << group index of latched group >>
      int            locked_group;    << group index of locked group >>
      unsigned int   mods;            << effective modifiers >>
      unsigned int   base_mods;       << base modifiers >>
      unsigned int   latched_mods;    << latched modifiers >>
      unsigned int   locked_mods;     << locked modifiers >>
      int            compat_state;    << computed compatibility state >>
      unsigned char  grab_mods;       << modifiers used for grabs >>
      unsigned char  compat_grab_mods;  << modifiers used for compatibility grabs >>
      unsigned char  lookup_mods;     << modifiers used to lookup symbols >>
      unsigned char  compat_lookup_mods;                  << mods used for compatibility look up >>
      int            ptr_buttons;     << core pointer buttons >>
      KeyCode        keycode;         << keycode causing event, 0 if programmatic >>
      char           event_type;      << core event if  req_major or
                                          req_minor non zero >>
      char           req_major;       << major request code if program trigger, else 0 >>
      char           req_minor;       << minor request code if program trigger, else 0 >>
} XkbStateNotifyEvent
*/

