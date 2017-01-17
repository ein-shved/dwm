/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>

/* appearance */
static const char font[]            = "-*-terminus-medium-r-*-*-16-*-*-*-*-*-*-*";
static const char normbordercolor[] = "#444444";
static const char normbgcolor[]     = "#222222";
static const char normfgcolor[]     = "#bbbbbb";
static const char selbordercolor[]  = "#005577";
static const char selbgcolor[]      = "#005577";
static const char selfgcolor[]      = "#eeeeee";
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const Bool systraypinningfailfirst = True;   /* True: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const Bool showsystray       = True;     /* False means no systray */
static const Bool showbar           = True;     /* False means no bar */
static const Bool topbar            = True;     /* False means bottom bar */

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8" , "9", "10", "11", "12" };

/* grid of tags */
#define DRAWCLASSICTAGS             1 << 0
#define DRAWTAGGRID                 1 << 1

#define SWITCHTAG_UP                1 << 0
#define SWITCHTAG_DOWN              1 << 1
#define SWITCHTAG_LEFT              1 << 2
#define SWITCHTAG_RIGHT             1 << 3
#define SWITCHTAG_TOGGLETAG         1 << 4
#define SWITCHTAG_TAG               1 << 5
#define SWITCHTAG_VIEW              1 << 6
#define SWITCHTAG_TOGGLEVIEW        1 << 7

static const unsigned int drawtagmask = DRAWTAGGRID; /* | DRAWCLASSICTAGS to show classic row of tags */
static const int tagrows = 2;

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor xkb_layout*/
    { "Firefox",  NULL,       NULL,       1 << 2,       False,       -1,      -1 },
    { "chromium", NULL,       NULL,       1 << 2,       False,       -1,      -1 },
    { "google-chrome", NULL,  NULL,       1 << 2,       False,       -1,      -1 },
    { NULL,       NULL,       "Guake!",   ~0,           True,        ~0,      ~0 },
    { "guake",    NULL,       NULL,       ~0,           True,        ~0,      ~0 },
    { "Skype",    NULL,       NULL,       1 << 8,       False,       -1,      -1 },
    { "Pidgin",   NULL,       NULL,       1 << 8,       False,       -1,       1 },
    { "hipchat",  NULL,       NULL,       1 << 8,       False,       -1,       1 },
};

/* layout(s) */
static const float mfact      = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster      = 1;    /* number of clients in master area */
static const Bool resizehints = True; /* True means respect size hints in tiled resizals */

/* xkb frontend */
static const Bool showxkb         = True; /* False means no xkb layout text */
static const char *xkb_layouts [] = {
    "en",
    "ru",
};

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod1Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-fn", font, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, "-f", NULL };
static const char *termcmd[]  = { "st", NULL };

static const char *volume_mute[]      = { "amixer", "sset", "Master", "togglemute", NULL };
static const char *volume_up[]        = { "amixer", "sset", "Master", "10+", NULL };
static const char *volume_down[]      = { "amixer", "sset", "Master", "10-", NULL };
static const char *volume_player_up[]   = { "clementine", "--volume-up", NULL };
static const char *volume_player_down[] = { "clementine", "--volume-down", NULL };
static const char *player_toggle[]      = { "clementine", "-t", NULL };
static const char *player_next[]        = { "clementine", "-f", NULL };
static const char *player_prev[]        = { "clementine", "-r", NULL };
static const char *screensaver[]      = { "dm-tool", "lock", NULL };
static const char *backlight_up[]     = { "xbacklight", "+", "10", NULL };
static const char *backlight_down[]   = { "xbacklight", "-", "10", NULL };


static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_F2,     spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_F3,     spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_Tab,    focusstack,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_Tab,    focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
//	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_F4,     killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[3]} },
//	{ MODKEY,                       XK_space,  setlayout,      {0} },
//	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
//	{ MODKEY,                       XK_F11,    togglefullscreen,      {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ControlMask,           XK_Delete,     quit,           {0} },
    { MODKEY|ControlMask,           XK_Up,     switchtag,      { .ui = SWITCHTAG_UP     | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,           XK_Down,   switchtag,      { .ui = SWITCHTAG_DOWN   | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,           XK_Right,  switchtag,      { .ui = SWITCHTAG_RIGHT  | SWITCHTAG_VIEW } },
    { MODKEY|ControlMask,           XK_Left,   switchtag,      { .ui = SWITCHTAG_LEFT   | SWITCHTAG_VIEW } },

    { MODKEY|Mod4Mask,              XK_Up,     switchtag,      { .ui = SWITCHTAG_UP     | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|Mod4Mask,              XK_Down,   switchtag,      { .ui = SWITCHTAG_DOWN   | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|Mod4Mask,              XK_Right,  switchtag,      { .ui = SWITCHTAG_RIGHT  | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { MODKEY|Mod4Mask,              XK_Left,   switchtag,      { .ui = SWITCHTAG_LEFT   | SWITCHTAG_TAG | SWITCHTAG_VIEW } },
    { 0,            XF86XK_AudioMute,          spawn,          { .v = volume_mute } },
    { 0,            XF86XK_AudioRaiseVolume,   spawn,          { .v = volume_up } },
    { 0,            XF86XK_AudioLowerVolume,   spawn,          { .v = volume_down } },
    { ControlMask,  XF86XK_AudioRaiseVolume,   spawn,          { .v = volume_player_up } },
    { ControlMask,  XF86XK_AudioLowerVolume,   spawn,          { .v = volume_player_down } },
    { 0,            XF86XK_AudioPlay,          spawn,          { .v = player_toggle } },
    { 0,            XF86XK_AudioNext,          spawn,          { .v = player_next } },
    { 0,            XF86XK_AudioPrev,          spawn,          { .v = player_prev } },
    { 0,            XK_Redo,                   spawn,          { .v = player_toggle } },
    { 0,            XF86XK_MyComputer,         spawn,          { .v = player_toggle } },
    { 0,            XF86XK_Cut,                spawn,          { .v = player_next } },
    { 0,            XK_Find,                   spawn,          { .v = player_prev } },
    { 0,            XF86XK_Forward,            spawn,          { .v = player_next } },
    { 0,            XF86XK_Back,               spawn,          { .v = player_next } },
    { MODKEY|ControlMask,   XK_l,              spawn,          { .v = screensaver } },
    { 0,            XF86XK_MonBrightnessUp,    spawn,          { .v = backlight_up } },
    { 0,            XF86XK_MonBrightnessDown,  spawn,          { .v = backlight_down } },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

