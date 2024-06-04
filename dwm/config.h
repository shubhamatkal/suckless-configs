/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include <unistd.h>

/* defining some functions */
static const char *chrome_cmd[] = { "google-chrome-stable", NULL };
static const char *alacritty_cmd[]={ "alacritty", NULL};
static const char *nautilus_cmd[]={"nautilus", NULL};
static const char *flameshotcmd[]  = { "flameshot", "gui", NULL };
static const char *shutdowncmd[] = { "shutdown", "now", NULL };
static const char *upbright[] = { "brightnessctl", "set", "+10%", NULL };
static const char *downbright[] = { "brightnessctl", "set", "10%-", NULL };
static const char *playpause[] = { "playerctl", "play-pause", NULL}; 
static const char *calcul[] = {"gnome-calculator", NULL};

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */

static unsigned int snap = 32;    /* snap pixel */
static int swallowfloating =
    0; /* 1 means swallow floating windows by default */
static unsigned int systraypinning =
    0; /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor
          X */
static unsigned int systrayspacing = 4; /* systray spacing */
static int systraypinningfailfirst =
    1; /* 1: if pinning fails, display systray on the first monitor, False:
          display systray on the last monitor*/
static int showsystray = 1; /* 0 means no systray */
static int showbar = 1;     /* 0 means no bar */
static int topbar = 1;      /* 0 means bottom bar */
static int startontag = 1;  /* 0 means no tag active on start */
static int user_bh = 28;    /* 0 means that dwm will calculate bar height, >= 1
                               means dwm will user_bh as bar height */
static float activeopacity =
    1.0f; /* Window opacity when it's focused (0 <= opacity <= 1) */
static float inactiveopacity =
    0.875f; /* Window opacity when it's inactive (0 <= opacity <= 1) */
static Bool bUseOpacity =
    True; /* Starts with opacity on any unfocused windows */

static const char *fonts[] = {
    "SF Mono:size=12",
    "fontawesome:size=12",
    "Noto Color Emoji:size=12",
};
static char dmenufont[] = "Bitstream Vera Sans:size=12";
// background color
static char normbgcolor[] = "#000000";
// inactive window border color
static char normbordercolor[] = "#dddddd";
// font color
static char normfgcolor[] = "#bbbbbb";
// current tag and current window font color (font on cyan)
static char selfgcolor[] = "#000000";
// top bar second color (blue) and active window border color (cyan)
static char selbordercolor[] = "#1a9942";
static char selbgcolor[] = "#1a9942";
static const char *colors[][3] = {
    /*               fg           bg           border   */
    [SchemeNorm] = {normfgcolor, normbgcolor, normbordercolor},
    [SchemeSel] = {selfgcolor, selbgcolor, selbordercolor},
};

/* tagging */
static const char *tags[] = { "1", "", "", "", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
	{ "gnome-calculator",    NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "| tile",      tile },    /* first entry is default */
	{ "| float",      NULL },    /* no layout function means floating behavior */
	{ "| [M]",      monocle },
};

/* audio constants */
static const char *upvol[] = {"pamixer" , "-i", "5",NULL};
static const char *downvol[] = {"pamixer", "-d" , "5", NULL};
static const char *mute[] = {"pamixer" , "-t" , NULL};

/* shutdown */
static const char *shutcmd[] = {"shutdown", "now", NULL};


/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/usr/local/bin/st", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = {"dmenu_run", "-m",  dmenumon,       "-fn",
                                 dmenufont,   "-nb", normbgcolor,    "-nf",
                                 normfgcolor, "-sb", selbordercolor, "-sf",
                                 selfgcolor,  NULL};
static const char *termcmd[]  = { "alacritty", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY|ShiftMask,                       XK_2,      spawn,          {.v = chrome_cmd } }, 
	{MODKEY|ShiftMask, XK_3, spawn, {.v = alacritty_cmd}},
	{MODKEY|ShiftMask, XK_4, spawn, {.v = nautilus_cmd}},
	{ 0,                        XK_Print,      spawn,          {.v = flameshotcmd } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY	  ,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_Escape,      killclient,     {0} },
	{ MODKEY|ShiftMask, 		XK_Escape, spawn, {.v = shutcmd }},
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{0, XF86XK_AudioLowerVolume , spawn, {.v = downvol}},
	{0,XF86XK_AudioRaiseVolume, spawn , {.v = upvol}},
	{0,XF86XK_AudioMute, spawn  , {.v = mute}},
	{ 0,                   XF86XK_MonBrightnessUp,   spawn,          {.v = upbright } },
    	{ 0,                   XF86XK_MonBrightnessDown, spawn,          {.v = downbright } },
	{ 0, XF86XK_AudioPlay, spawn, {.v = playpause} },
	{ 0, XF86XK_Calculator, spawn,{.v = calcul}},
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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

