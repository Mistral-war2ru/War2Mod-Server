#include <windows.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>
#include "ezini.h"
#include "ver.h"
#include "patch.h"//foid patcher
#include "defs.h"//some addresses and constants
#include "names.h"//some constants
#include "chars.h"//font and sprites

bool replaced = false;//if trigger was replaced
bool war2mod = false;//if war2mod enabled
byte war2_game_mode = 0;//game mode
bool ogre_mode = false;//two head ogre mode (team-melee)
bool w13 = false;//if orig gamemode was 13
bool jmsend = false;//lobby message sended
byte t[MAX_COMMANDS] = { 0 };//trigger byte code
int MCOM = MAX_COMMANDS;//max size of trigger
char crs[5] = { 0 };//CRC of map
bool campanign = false;//if this is campanign map
bool prev = false;//player not completed previous map of campanign
bool table = false;//show win/lose table
bool first_step = true;//first step of trigger (when screen is black)
bool game_started = false;//game ticks started
byte reg[4] = { 0 };//region
bool gflag[256] = { false };//global flag
int* unit[1610];//array for units
int* unitt[1610];//temp array for units
int* capt[1610];//array for captured units
int units = 0;//number of units in array
int captk = 0;//number of units in captured array
byte ua[256] = { 255 };//attackers array
byte ut[256] = { 255 };//targets array     only unit ua[i] can deal damage to unit ut[i]
byte runes[9] = { 0 };//runestone special abilities flags
bool agr = false;//allied comps aggro if attacked 
bool cpt = false;//can buildings be captured on low hp
bool pcpt = false;//only peons can capture low hp build
bool thcpt = false;//if th captured on low hp, capture all units of that player
bool ucpt = false;//units capture
bool steal = false;//if can steal resources
bool aport = false;//only allies can teleport in portal
bool mport = false;//portal can teleport only if have mage(or dk) nearby
bool b3rune = false;//can build runestone
bool b3port = false;//can build portal
bool b3cirl = false;//can build circle of power
bool b3mine = false;//can build gold mine
bool b3mine_anywhere = false;//can build gold mine anywhere
bool b3forest = false;//can build forest
bool b3forest_anywhere = false;//can build gold mine anywhere
byte b3forest_price = 50;//forest price gold
bool apn = false;//can build attack peons
bool manaburn = false;//if demons have manaburn
byte heros[16] = { 0 };//heroes ids
bool herosb[16] = { false };//heroes buildable ids
char hhero[180] = { 0 };//buttons for build human heroes array
char ohero[180] = { 0 };//buttons for build orc heroes array
byte forced_race[8] = { 0 };//race for campanign
bool A_runestone = false;//runestone activated
bool A_portal = false;//portals activated
bool A_transport = false;//transport activated
bool A_autoheal = false;//paladins autoheal activated
bool A_rally = false;//rally point
bool A_tower = false;//tower control
bool blood_f = false;//blood fixed
bool send_res = false;//can send resources
bool more_res = false;//can get more resources
bool path_fixed = false;//path fix
bool ai_fixed = false;//ai fix
bool peon_steal = false;//peons can steal from other peons
bool saveload_fixed = false;//saveload ai break bug fix
bool ships_speed_tower = false;//use SHIPS_SPEED upgrade for towers attack speed
byte real_local = 255;//remeber real local id for ogre mode
byte m_slow_aura[256] = { 255 };//units that have slow aura
byte m_death_aura[256] = { 255 };//units that have death aura
byte m_sneak[256] = { 255 };//units that have hide ability
byte m_devotion[256] = { 255 };//units that have defence aura
byte m_vampire[256] = { 255 };//units that have vampire aura
byte m_prvnt[256] = { 255 };//units that have prevent loss

byte arhons[240] = { 255 };//old ogre mode players not used now (still here for compatibility)
byte maxtir[8] = { 0 };//builded buildings for anti build hack
byte teams_host[8] = { 0 };//teams ids received from host
byte teams_host_r[8] = { 0 };
DWORD teams_lag[8] = { 0 };//players lag
byte obs_lose[8] = { 0 };//obs during game surrenders

byte all_kills[16 * 16 * U_OWALL] = { 0 };

struct Vizs
{
    byte x = 0;
    byte y = 0;
    byte p = 0;
    byte s = 0;
};
Vizs vizs_areas[2000];
int vizs_n = 0;

char* mission_objective;
char* unit_names[U_EMPTY_BUTTONS];
char* mission_briefing;
char* mission_briefing_title;

char sheep_build[] = "\x0\x0\x73\x0\xf0\x40\x44\x0\xd0\xe6\x40\x0\x1\x39\x28\x1\x0\x0\x0\x0";
char demon_build[] = "\x0\x0\x25\x0\xf0\x40\x44\x0\xd0\xe6\x40\x0\x1\x38\x18\x1\x0\x0\x0\x0";
char build_3[] = "\x0\x0\x52\x0\xf0\x40\x44\x0\x80\x6b\x43\x0\x0\x66\x31\x1\x0\x0\x0\x0\x1\x0\x50\x0\xf0\x40\x44\x0\x80\x6b\x43\x0\x0\x65\x3e\x1\x0\x0\x0\x0\x2\x0\x51\x0\xf0\x40\x44\x0\x80\x6b\x43\x0\x0\x64\x46\x1\x0\x0\x0\x0\x3\x0\x4A\x0\xf0\x40\x44\x0\x80\x6b\x43\x0\x0\x5c\x38\x1\x0\x0\x0\x0\x4\x0\x67\x0\xf0\x40\x44\x0\x80\x6b\x43\x0\x0\x5c\x91\x1\x0\x0\x0\x0\x8\x0\x5b\x0\xf0\x40\x44\x0\x70\xa6\x44\x0\x0\x6e\x97\x1\x0\x0\x0\x0";
char peon_build3[] = "\x0\x0\x54\x0\xf0\x40\x44\x0\x90\x66\x43\x0\x0\x0\x88\x1\x2\x0\x0\x0\x1\x0\xa7\x0\xf0\x40\x44\x0\xf0\x61\x43\x0\x0\x0\x89\x1\x1\x0\x0\x0\x2\x0\x77\x0\xf0\x40\x44\x0\x40\x69\x43\x0\x0\x0\x8a\x1\x8\x0\x0\x0\x3\x0\x55\x0\xf0\x40\x44\x0\x70\x6a\x43\x0\x0\x0\x8b\x1\x0\x1\x0\x0\x4\x0\x56\x0\x60\x51\x44\x0\x10\x6a\x43\x0\x0\x0\x8c\x1\x40\x0\x0\x0\x5\x0\x5a\x0\xa0\x51\x44\x0\x30\x6b\x43\x0\x0\x0\x91\x1\x80\x0\x0\x0\x6\x0\x57\x0\xe0\x4d\x44\x0\x70\xa6\x44\x0\x0\x71\x8e\x1\x0\x0\x0\x0\x7\x0\x58\x0\x50\x50\x44\x0\x70\xa6\x44\x0\x0\x72\x8f\x1\x0\x0\x0\x0\x8\x0\x6d\x0\xf0\x40\x44\x0\x70\xa6\x44\x0\x0\x6d\x38\x1\x0\x0\x0\x0";
char oth_build[] = "\x0\x0\x1\x0\xa0\x44\x44\x0\xd0\xe6\x40\x0\x1\x3\xd\x1\x0\x0\x0\x0\x1\x0\x43\x0\xb0\x42\x44\x0\x10\xe7\x40\x0\x0\x59\x3e\x1\x0\x0\x0\x0\x1\x0\x45\x0\xb0\x42\x44\x0\x10\xe7\x40\x0\x1\x5b\x3c\x1\x0\x0\x0\x0\x2\x0\x4f\x0\xf0\x40\x44\x0\x70\xa6\x44\x0\x0\x6c\x90\x1\x0\x0\x0\x0\x3\x0\xb5\x0\xf0\x40\x44\x0\xd0\xe6\x40\x0\x1\x11\x8a\x1\x0\x0\x0\x0";
char hth_build[] = "\x2\x0\x4f\x0\xf0\x40\x44\x0\x70\xa6\x44\x0\x0\x6b\x90\x1\x0\x0\x0\x0\x3\x0\xb4\x0\xf0\x40\x44\x0\xd0\xe6\x40\x0\x1\x10\x8a\x1\x0\x0\x0\x0";
char churc[] = "\x0\x0\xa\x0\xc0\x43\x44\x0\x90\xe6\x40\x0\x14\x21\x72\x1\x0\x0\x0\x0\x0\x0\x6b\x0\x10\x44\x44\x0\x90\xe6\x40\x0\x1\x23\x81\x1\x0\x0\x0\x0\x1\x0\x6e\x0\x10\x44\x44\x0\x90\xe6\x40\x0\x3\x24\x83\x1\x0\x0\x0\x0\x3\x0\x6d\x0\xf0\x40\x44\x0\xf0\x40\x44\x0\x0\x0\x6c\x1\x0\x0\x0\x0";

const char RECORD_PATH[] = "record.save";

int apm_mt = 0;
int apm_ft = 0;
int apm_st = 0;
int apm_gct = 0;
int apm_rt = 0;
WORD apm_rx = 0;
WORD apm_ry = 0;
byte apm_s[9 * 4] = { 0 };

#define C_COLORS_SIZE 12
byte colors[C_COLORS_SIZE] = { 0,254,250,251,246,113,200,189,155,001,253,249 };
#define C_BCOLORS_SIZE 11
byte bcolors[C_BCOLORS_SIZE] = { 0,239,181,146,148,100,130,128,162,163,202 };

bool menu_1 = false;//show mod menu settings
bool menu_button_hover = false;//mouse on button

byte m_ticks1 = 0;//tiks (every 100)
int m_ticks = 0;//tiks div 100

byte m_drag = 0;
int m_drag_x = 0;
int m_drag_y = 0;

WORD m_screen_w = 640;
WORD m_screen_h = 480;
WORD m_minx = 176;
WORD m_miny = 16;
WORD m_maxx = 616;
WORD m_maxy = 456;
WORD m_added_height = m_screen_w - 480;
WORD m_added_width = m_screen_h - 640;
WORD m_align_y = 0;
WORD m_align_x = 0;

byte m_show_timer = 0;//show timer
WORD m_timer_x = m_maxx;
WORD m_timer_y = m_maxy - 8;
byte m_timer_col = 200;//color for timer
byte m_timer_bcol = 0;//back color for timer
WORD m_timer_size = 0;

byte m_show_ticker = 0;//show ticker
WORD m_ticker_x = m_maxx;
WORD m_ticker_y = m_miny;
byte m_ticker_col = 113;//color for ticker
byte m_ticker_bcol = 0;//back color for ticker
WORD m_ticker_size = 0;

byte m_show_apm = 0;//show apm
WORD m_apm_x = m_maxx;
WORD m_apm_y = m_miny + 8;
byte m_apm_col = 246;//color for apm
byte m_apm_bcol = 0;//back color for apm
WORD m_apm_size = 0;
byte m_apm_type = 0;

byte m_holy_cam = 1;//holy vision move camera
byte m_double_click = 1;//double click pick units

byte m_lobby_comb = 0;//combobox type in lobby
byte m_lobby_interface = 0;//hide interface in lobby for host

byte m_show_button = 1;
bool m_show_button_last = false;

const char INI_PATH[] = "war2mod.ini";
const char INI_TXT_TIMER[] = "Timer";
const char INI_TXT_TICKER[] = "Ticker";
const char INI_TXT_APM[] = "APM";
const char INI_TXT_GAMEPLAY[] = "Gameplay";
const char INI_TXT_LOBBY[] = "Lobby";
const char INI_TXT_WAR2MOD[] = "War2Mod";

const char INI_TXT_ENABLED[] = "Enabled";
const char INI_TXT_POSX[] = "PosX";
const char INI_TXT_POSY[] = "PosY";
const char INI_TXT_COL[] = "Color";
const char INI_TXT_BCOL[] = "Background_Color";
const char INI_TXT_TYPE[] = "Type";
const char INI_TXT_HOLY[] = "Holy_Cam";
const char INI_TXT_DCLICK[] = "DClick";
const char INI_TXT_BUTTON[] = "Button";
const char INI_TXT_INTERFACE[] = "Interface";

char str_gold[] = "gold";
char str_lumber[] = "lumber";
char str_oil[] = "oil";
char* str_res[] = { str_gold,str_lumber,str_oil };

char str_red[] = "red";
char str_blue[] = "blue";
char str_green[] = "green";
char str_teal[] = "teal";
char str_violet[] = "violet";
char str_purple[] = "purple";
char str_orange[] = "orange";
char str_black[] = "black";
char str_white[] = "white";
char str_yellow[] = "yellow";
char str_col1[] = "1";
char str_col2[] = "2";
char str_col3[] = "3";
char str_col4[] = "4";
char str_col5[] = "5";
char str_col6[] = "6";
char str_col7[] = "7";
char str_col8[] = "8";
char str_col_me[] = "me";
char* str_colors[] = { str_red,str_blue,str_green,str_violet,
str_orange,str_black,str_white,str_yellow,str_teal,str_purple,
str_col1,str_col2,str_col3,str_col4,str_col5,str_col6,str_col7,str_col8,str_col_me };

void inval_game_view()
{
    ((void (*)(int, int, int, int))F_INVALIDATE)(m_minx, m_miny, m_maxx + 8, m_maxy + 8);
}

void show_message(byte time, char* text)
{
    ((void (*)(char*, int, int))F_MAP_MSG)(text, 15, time * 10);//original war2 show msg func
}

void clear_chat()
{
    for (int i = 0; i < 12; i++)
    {
        int* p = (int*)MULTPL_CHAT;
        p = (int*)(*((int*)((uintptr_t)p)));
        p -= 100 / 4 * i;
        char buf[] = "\x0";//just set 0 in all messages to empty
        PATCH_SET((char*)p, buf);
    }
}

byte get_player_team(int p)
{
    return *(byte*)(PLAYERS_INFO + 4 + 3 + 38 * p);
}

void set_player_team(int p, byte t)
{
    char buf[] = "\x0";
    buf[0] = t;
    PATCH_SET((char*)(PLAYERS_INFO + 4 + 3 + 38 * p), buf);
}

byte get_player_controler(int p)
{
    return *(byte*)(PLAYERS_INFO + 4 + 38 * p);
}

int get_val(int adress, int player)
{
    return (int)(*(WORD*)(adress + player * 2));//player*2 cause all vals is WORD
}

int t_val(int c, int p)
{
    //this func just get address by id from this list
    int a;
    switch (c)
    {
    case 0:a = CIRCLE_UNITS; break;
    case 1:a = RESCUED_UNITS; break;
    case 2:a = LOST_BUILDINGS; break;
    case 3:a = LOST_UNITS; break;
    case 4:a = KILLS_BUILDINGS; break;
    case 5:a = KILLS_UNITS; break;
    case 6:a = ALL_BUILDINGS; break;
    case 7:a = ALL_UNITS; break;
    case 8:a = GRUNT; break;
    case 9:a = ACTIVE_GRUNT; break;
    case 10:a = ARCHER; break;
    case 11:a = ACTIVER_ARCHER; break;
    case 12:a = RANGER; break;
    case 13:a = ACTIVE_RANGER; break;
    case 14:a = CATAPULT; break;
    case 15:a = ACTIVE_CATAPULT; break;
    case 16:a = KNIGHT; break;
    case 17:a = ACTIVE_KNIGHT; break;
    case 18:a = MAGUS; break;
    case 19:a = ACTIVE_MAGUS; break;
    case 20:a = SAPPERS; break;
    case 21:a = ACTIVE_SAPPERS; break;
    case 22:a = FLYER; break;
    case 23:a = ACTIVE_FLYER; break;
    case 24:a = DRAGON; break;
    case 25:a = ACTIVE_DRAGON; break;
    case 26:a = DESTROYER; break;
    case 27:a = ACTIVE_DESTROYER; break;
    case 28:a = BATTLESHIP; break;
    case 29:a = ACTIVE_BATTLESHIP; break;
    case 30:a = SUBMARINE; break;
    case 31:a = ACTIVE_SUBMARINE; break;
    case 32:a = TRANSPORT; break;
    case 33:a = ACTIVE_TRANSPORT; break;
    case 34:a = TANKER; break;
    case 35:a = PEON; break;
    case 36:a = ACTIVE_WATER; break;
    case 37:a = ACTIVE_AIR; break;
    case 38:a = NPC; break;
    case 39:a = FARM; break;
    case 40:a = BARRACKS; break;
    case 41:a = TH1; break;
    case 42:a = TH2; break;
    case 43:a = TH3; break;
    case 44:a = LUMBERMILL; break;
    case 45:a = SMITH; break;
    case 46:a = TOWER; break;
    case 47:a = INVENTOR; break;
    case 48:a = STABLES; break;
    case 49:a = SHIPYARD; break;
    case 50:a = FOUNDRY; break;
    case 51:a = REFINERY; break;
    case 52:a = OIL_PLATFORM; break;
    case 53:a = AVIARY; break;
    case 54:a = ALTAR; break;
    case 55:a = WAR_BUILDINGS; break;
    case 56:a = FOOD_LIMIT; break;
    case 57:a = RUNESTONE; break;
    case 58:a = PORTAL; break;
    case 59:a = BUILINGS_; break;
    default:a = CIRCLE_UNITS; break;
    }
    return get_val(a, p);//get val from address and player
}

bool cmp_args(byte m, byte v, byte c)
{//compare bytes
    bool f = false;
    switch (m)
    {
    case CMP_EQ:f = (v == c); break;
    case CMP_NEQ:f = (v != c); break;
    case CMP_BIGGER_EQ:f = (v >= c); break;
    case CMP_SMALLER_EQ:f = (v <= c); break;
    case CMP_BIGGER:f = (v > c); break;
    case CMP_SMALLER:f = (v < c); break;
    default: f = false; break;
    }
    return f;
}

bool cmp_args2(byte m, WORD v, WORD c)
{//compare words
    bool f = false;
    switch (m)
    {
    case CMP_EQ:f = (v == c); break;
    case CMP_NEQ:f = (v != c); break;
    case CMP_BIGGER_EQ:f = (v >= c); break;
    case CMP_SMALLER_EQ:f = (v <= c); break;
    case CMP_BIGGER:f = (v > c); break;
    case CMP_SMALLER:f = (v < c); break;
    default: f = false; break;
    }
    return f;
}

bool cmp_args4(byte m, int v, int c)
{//comapre 4 bytes (for resources)
    bool f = false;
    switch (m)
    {
    case CMP_EQ:f = (v == c); break;
    case CMP_NEQ:f = (v != c); break;
    case CMP_BIGGER_EQ:f = (v >= c); break;
    case CMP_SMALLER_EQ:f = (v <= c); break;
    case CMP_BIGGER:f = (v > c); break;
    case CMP_SMALLER:f = (v < c); break;
    default: f = false; break;
    }
    return f;
}

void lose(bool t)
{
    if (get_player_team(*(byte*)LOCAL_PLAYER) != 9)
    {
        if (t == true)
        {
            char buf[] = "\x0";//if need to show table
            PATCH_SET((char*)LOSE_SHOW_TABLE, buf);
        }
        else
        {
            char buf[] = "\x3b";
            PATCH_SET((char*)LOSE_SHOW_TABLE, buf);
        }
        char l[] = "\x2";
        PATCH_SET((char*)(ENDGAME_STATE + (*(byte*)LOCAL_PLAYER)), l);
        ((void (*)())F_LOSE)();//original lose func
    }
}

void win(bool t)
{
    if (get_player_team(*(byte*)LOCAL_PLAYER) != 9)
    {
        if (t == true)
        {
            char buf[] = "\xEB";//if need to show table
            PATCH_SET((char*)WIN_SHOW_TABLE, buf);
        }
        else
        {
            char buf[] = "\x74";
            PATCH_SET((char*)WIN_SHOW_TABLE, buf);
        }
        char l[] = "\x3";
        PATCH_SET((char*)(ENDGAME_STATE + (*(byte*)LOCAL_PLAYER)), l);
        ((void (*)())F_WIN)();//original win func
    }
}

void lose2(bool t, byte vid)
{
    if (get_player_team(*(byte*)LOCAL_PLAYER) != 9)
    {
        lose(t);
        ((void (*)(int, byte))F_VIDEO)(vid, 1);//original war2 func that show video by id
    }
}

void win2(bool t, byte vid)
{
    if (get_player_team(*(byte*)LOCAL_PLAYER) != 9)
    {
        win(t);
        ((void (*)(int, byte))F_VIDEO)(vid, 1);//original war2 func that show video by id
    }
}

bool check_ally(byte p1, byte p2)
{
    //check allied table
    return ((*(byte*)(ALLY + p1 + 16 * p2) != 0) && (*(byte*)(ALLY + p2 + 16 * p1) != 0));
}

bool check_av(byte p1, byte p2)
{
    if (check_ally(p1, p2))
    {
        byte av1 = *(byte*)AVED;//AV byte
        byte av2 = av1;
        av1 = av1 & (1 << p1);
        av2 = av2 & (1 << p2);
        return ((av1 != 0) && (av2 != 0));
    }
    return false;
}

void tile_remove_trees(int x, int y)
{
    ((void (*)(int, int))F_TILE_REMOVE_TREES)(x, y);
}

void tile_remove_rocks(int x, int y)
{
    ((void (*)(int, int))F_TILE_REMOVE_ROCKS)(x, y);
}

void tile_remove_walls(int x, int y)
{
    ((void (*)(int, int))F_TILE_REMOVE_WALLS)(x, y);
}

byte get_all_kills(byte id, byte pla, byte plt)
{
    if ((id <= U_OWALL) && (pla <= 15) && (plt <= 15))return all_kills[pla * 16 * U_OWALL + plt * U_OWALL + id];
    else return 0;
}

bool stat_byte(byte s)
{//check if unit stat is 1 or 2 byte
    bool f = (s == S_DRAW_X) || (s == S_DRAW_Y) || (s == S_X) || (s == S_Y) || (s == S_HP)
        || (s == S_INVIZ) || (s == S_SHIELD) || (s == S_BLOOD) || (s == S_HASTE)
        || (s == S_AI_SPELLS) || (s == S_NEXT_FIRE)
        || (s == S_LAST_HARVEST_X) || (s == S_LAST_HARVEST_Y)
        || (s == S_BUILD_PROGRES) || (s == S_BUILD_PROGRES_TOTAL)
        || (s == S_RESOURCES) || (s == S_ORDER_X) || (s == S_ORDER_Y)
        || (s == S_RETARGET_X1) || (s == S_RETARGET_Y1) || (s == S_RETARGET_X2) || (s == S_RETARGET_Y2);
    return !f;
}

bool cmp_stat(int* p, int v, byte pr, byte cmp)
{
    //p - unit
    //v - value
    //pr - property
    //cmp - compare method
    bool f = false;
    if (stat_byte(pr))
    {
        byte ob = v % 256;
        char buf[3] = { 0 };
        buf[0] = ob;
        buf[1] = *((byte*)((uintptr_t)p + pr));
        if (cmp_args(cmp, buf[1], buf[0]))
        {
            f = true;
        }
    }
    else
    {
        if (cmp_args2(cmp, *((WORD*)((uintptr_t)p + pr)), (WORD)v))
        {
            f = true;
        }
    }
    return f;
}

void set_stat(int* p, int v, byte pr)
{
    if (stat_byte(pr))
    {
        char buf[] = "\x0";
        buf[0] = v % 256;
        PATCH_SET((char*)((uintptr_t)p + pr), buf);
    }
    else
    {
        char buf[] = "\x0\x0";
        buf[0] = v % 256;
        buf[1] = (v / 256) % 256;
        PATCH_SET((char*)((uintptr_t)p + pr), buf);
    }
}

int get_stat(int* p, byte pr)
{
    int r = 0;
    if (stat_byte(pr))
    {
        r = *((byte*)((uintptr_t)p + pr));
    }
    else
    {
        r = *((WORD*)((uintptr_t)p + pr));
    }
    return r;
}

void gamesound_play(WORD s, byte x, byte y)
{
    DWORD xy = x + 256 * 256 * y;
    ((void (*)(WORD, DWORD, byte, byte))F_GAMESOUND_XY)(s, xy, 1, 1);
}

void bullet_create(WORD x,WORD y,byte id)
{
    int* b = ((int* (*)(WORD, WORD, byte))F_BULLET_CREATE)(x, y, id);
    if (b)
    {
        if ((id == B_LIGHT_FIRE) || (id == B_HEAVY_FIRE))
        {
            char buf[] = "\x0";
            buf[0] = 5;//bullet action
            PATCH_SET((char*)((uintptr_t)b + 54), buf);//54 bullet action
            buf[0] = 1;//bullet info
            PATCH_SET((char*)((uintptr_t)b + 58), buf);//58 bullet user info
            buf[0] = 4;//bullet flags
            PATCH_SET((char*)((uintptr_t)b + 53), buf);//53 bullet flags
            buf[0] = 80;//ticks
            PATCH_SET((char*)((uintptr_t)b + 56), buf);//56 bullet life (WORD)
        }
    }
    //all moving - move to -16 -16 relative
    //0 -  move and die (anim ok)
    //1 -  crash when hit (no owner?)
    //2 -  1
    //3 -  crash (no target?)
    //4 -  3
    //5 -  1
    //6 -  anim ok
    //7 -  anim ok + sound (better just use 26)
    //8 -  anim ok
    //9 -  anim ok
    //10 - anim ok
    //11 - anim ok
    //12 - 1
    //13 - anim ok + sound (better just use 21)
    //14 - 13
    //15 - no crash but insta dies
    //16 - 15
    //17 - anim ok + sound (better just use 23)
    //18 - 17
    //19 - anim ok but prob infinitie life
    //20 - 19
    //21 - anim ok + sound
    //22 - anim ok
    //23 - anim ok no sound
    //24 - anim ok + sound (better just use 26)
    //25 - anim ok
    //26 - anim ok + sound
    //27 - anim ok
    //28 - anim ok
}

void bullet_disp_fires()
{
    int* p = (int*)BULLETS_MASSIVE;//pointer to bullets
    p = (int*)(*p);
    int k = *(int*)BULLETS_NUMBER;//unit number
    while (k > 0)
    {
        bool f = (((*((byte*)((uintptr_t)p + 52)) == B_LIGHT_FIRE)
            || (*((byte*)((uintptr_t)p + 52)) == B_HEAVY_FIRE))//both fires
            && (*((byte*)((uintptr_t)p + 58)) == 1));//user info
        if (f)
        {
            byte life = *((byte*)((uintptr_t)p + 56));
            if (life > 0)life--;
            char buf[] = "\x0";
            buf[0] = life;
            PATCH_SET((char*)((uintptr_t)p + 56), buf);//56 bullet life (WORD)
            if (life == 0)
            {
                buf[0] = 1;
                PATCH_SET((char*)((uintptr_t)p + 53), buf);//53 bullet flags
            }
            else
            {
                buf[0] = 4;
                PATCH_SET((char*)((uintptr_t)p + 53), buf);//53 bullet flags
            }
        }
        p = (int*)((int)p + 0x40);
        k--;
    }
}

void bullet_create_unit(int* u, byte b)
{
    WORD x = *((WORD*)((uintptr_t)u + S_DRAW_X));
    WORD y = *((WORD*)((uintptr_t)u + S_DRAW_Y));
    bullet_create(x + 16, y + 16, b);
}

void bullet_create8_around_unit(int* u, byte b)
{
    WORD ux = *((WORD*)((uintptr_t)u + S_DRAW_X));
    WORD uy = *((WORD*)((uintptr_t)u + S_DRAW_Y));
    WORD x = ux + 16;
    WORD y = uy + 16;
    if ((b == B_LIGHT_FIRE) || (b == B_HEAVY_FIRE))y -= 8;
    if ((b == B_LIGHTNING) || (b == B_COIL))
    {
        x += 16;
        y += 16;
    }
    WORD xx = x;
    WORD yy = y;
    bullet_create(xx + 48, yy, b);//right
    bullet_create(xx, yy + 48, b);//down
    bullet_create(xx + 32, yy + 32, b);//right down
    if (xx <= 32)xx = 0;
    else xx -= 32;
    bullet_create(xx, yy + 32, b);//left down
    if (yy <= 32)yy = 0;
    else yy -= 32;
    bullet_create(xx, yy, b);//left up
    xx = x;
    bullet_create(xx + 32, yy, b);//right up
    yy = y;
    if (xx <= 48)xx = 0;
    else xx -= 48;
    bullet_create(xx, yy, b);//left
    xx = x;
    if (yy <= 48)yy = 0;
    else yy -= 48;
    bullet_create(xx, yy, b);//up
}

void unit_convert(byte player, int who, int tounit, int a)
{
    //original war2 func converts units
    ((void (*)(byte, int, int, int))F_UNIT_CONVERT)(player, who, tounit, a);
}

void unit_create(int x, int y, int id, byte owner, byte n)
{
    while (n > 0)
    {
        n -= 1;
        int* p = (int*)(UNIT_SIZE_TABLE + 4 * id);//unit sizes table
        ((void (*)(int, int, int, byte, int*))F_UNIT_CREATE)(x, y, id, owner, p);//original war2 func creates unit
        //just call n times to create n units
    }
}

int* unit_create_place(int x, int y, int id, byte owner)
{
    int* p = (int*)(UNIT_SIZE_TABLE + 4 * id);//unit sizes table
    return ((int* (*)(int, int, int, byte, int*))F_UNIT_CREATE)(x, y, id, owner, p);
}

void unit_kill(int* u)
{
    ((void (*)(int*))F_UNIT_KILL)(u);//original war2 func kills unit
}

void unit_hide(int* u)
{
    byte f = *((byte*)((uintptr_t)u + S_FLAGS3));
    f |= SF_HIDDEN;
    set_stat(u, f, S_FLAGS3);
}

void unit_remove(int* u)
{
    unit_hide(u);
    unit_kill(u);
}

void unit_cast(int* u)
{
    ((void (*)(int*))F_AI_CAST)(u);//original war2 ai cast spells
}

void set_region(int x1, int y1, int x2, int y2)
{
    byte tmp;
    if (x1 < 0)x1 = 0;
    if (x1 > 127)x1 = 127;
    if (y1 < 0)y1 = 0;
    if (y1 > 127)y1 = 127;
    if (x2 < 0)x2 = 0;
    if (x2 > 127)x2 = 127;
    if (y2 < 0)y2 = 0;
    if (y2 > 127)y2 = 127;
    if (x2 < x1)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y2 < y1)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    reg[0] = x1 % 256;
    reg[1] = y1 % 256;
    reg[2] = x2 % 256;
    reg[3] = y2 % 256;
}

WORD region_center()
{
    return (reg[0] + (byte)floor((double)(reg[2] - reg[0]) / 2)) + 256 * (reg[1] + (byte)floor((double)(reg[3] - reg[1]) / 2));
}

bool in_region(byte x, byte y, byte x1, byte y1, byte x2, byte y2)
{
    byte tmp;
    tmp = x % 256;
    x = tmp;
    tmp = y % 256;
    y = tmp;
    tmp = x1 % 256;
    x1 = tmp;
    tmp = y1 % 256;
    y1 = tmp;
    tmp = x2 % 256;
    x2 = tmp;
    tmp = y2 % 256;
    y2 = tmp;
    if (x < 0)x = 0;
    if (x > 127)x = 127;
    if (y < 0)y = 0;
    if (y > 127)y = 127;
    if (x1 < 0)x1 = 0;
    if (x1 > 127)x1 = 127;
    if (y1 < 0)y1 = 0;
    if (y1 > 127)y1 = 127;
    if (x2 < 0)x2 = 0;
    if (x2 > 127)x2 = 127;
    if (y2 < 0)y2 = 0;
    if (y2 > 127)y2 = 127;
    if (x2 < x1)
    {
        tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y2 < y1)
    {
        tmp = y1;
        y1 = y2;
        y2 = tmp;
    }
    //just check if coords inside region
    return ((x >= x1) && (y >= y1) && (x <= x2) && (y <= y2));
}

bool check_unit_dead(int* p)
{
    bool dead = false;
    if (p)
    {
        if ((*((byte*)((uintptr_t)p + S_FLAGS3))
            & (SF_DEAD | SF_DIEING | SF_UNIT_FREE)) != 0)
            dead = true;
    }
    else
        dead = true;
    return dead;
}

bool check_unit_complete(int* p)
{
    bool f = false;
    if (p)
    {
        if ((*((byte*)((uintptr_t)p + S_FLAGS3)) & SF_COMPLETED) != 0)//flags3 last bit
            f = true;
    }
    else
        f = false;
    return f;
}

bool check_unit_hidden(int* p)
{
    bool f = false;
    if (p)
    {
        if ((*((byte*)((uintptr_t)p + S_FLAGS3)) & SF_HIDDEN) != 0)//flags3 4 bit
            f = true;
    }
    else
        f = true;
    return f;
}

bool check_unit_preplaced(int* p)
{
    bool f = false;
    if (p)
    {
        if ((*((byte*)((uintptr_t)p + S_FLAGS3)) & SF_PREPLACED) != 0)//flags3
            f = true;
    }
    else
        f = false;
    return f;
}

bool check_unit_near_death(int* p)
{
    bool dead = false;
    if (p)
    {
        if (((*((byte*)((uintptr_t)p + S_FLAGS3)) & SF_DIEING) != 0)
            && ((*((byte*)((uintptr_t)p + S_FLAGS3)) & (SF_DEAD | SF_UNIT_FREE)) == 0))
            dead = true;
    }
    else
        dead = true;
    return dead;
}

bool check_peon_loaded(int* p, byte r)
{
    bool f = false;
    if (p)
    {
        if (r == 0)
        {
            if (((*((byte*)((uintptr_t)p + S_PEON_FLAGS)) & PEON_LOADED) != 0)
                && ((*((byte*)((uintptr_t)p + S_PEON_FLAGS)) & PEON_HARVEST_GOLD) != 0))
                f = true;
        }
        if (r == 1)
        {
            if (((*((byte*)((uintptr_t)p + S_PEON_FLAGS)) & PEON_LOADED) != 0)
                && ((*((byte*)((uintptr_t)p + S_PEON_FLAGS)) & PEON_HARVEST_LUMBER) != 0))
                f = true;
        }
        if (r == 2)
        {
            if (((*((byte*)((uintptr_t)p + S_PEON_FLAGS)) & PEON_LOADED) != 0))
                f = true;
        }
    }
    return f;
}

void find_all_units(byte id)
{
    //all units by id will go in array
    units = 0;
    int* p = (int*)UNITS_MASSIVE;//pointer to units
    p = (int*)(*p);
    int k = *(int*)UNITS_NUMBER;
    while (k > 0)
    {
        bool f = *((byte*)((uintptr_t)p + S_ID)) == (byte)id;
        if (f)
        {
            unit[units] = p;
            units++;
        }
        p = (int*)((int)p + 0x98);
        k--;
    }
}

void find_all_alive_units(byte id)
{
    //all units by id will go in array
    units = 0;
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);//pointer to units list for each player
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = *((byte*)((uintptr_t)p + S_ID)) == (byte)id;
                if (id == ANY_BUILDING)
                    f = *((byte*)((uintptr_t)p + S_ID)) >= 58;//buildings
                if (id == ANY_MEN)
                    f = *((byte*)((uintptr_t)p + S_ID)) < 58;//all nonbuildings
                if (id == ANY_UNITS)
                    f = true;//all ALL units
                if (id == ANY_BUILDING_2x2)//small buildings
                {
                    byte sz = *((byte*)UNIT_SIZE_TABLE + *((byte*)((uintptr_t)p + S_ID)) * 4);
                    f = sz == 2;
                }
                if (id == ANY_BUILDING_3x3)//med buildings
                {
                    byte sz = *((byte*)UNIT_SIZE_TABLE + *((byte*)((uintptr_t)p + S_ID)) * 4);
                    f = sz == 3;
                }
                if (id == ANY_BUILDING_4x4)//big buildings
                {
                    byte sz = *((byte*)UNIT_SIZE_TABLE + *((byte*)((uintptr_t)p + S_ID)) * 4);
                    f = sz == 4;
                }
                if (f)
                {
                    if (!check_unit_dead(p))
                    {
                        unit[units] = p;
                        units++;
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void sort_complete()
{
    //only completed units stay in array
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (check_unit_complete(unit[i]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_in_region()
{
    //only units in region stay in array
    int k = 0;
    WORD x = 0, y = 0;
    for (int i = 0; i < units; i++)
    {
        x = *((WORD*)((uintptr_t)unit[i] + S_DRAW_X)) / 32;
        y = *((WORD*)((uintptr_t)unit[i] + S_DRAW_Y)) / 32;
        if (in_region((byte)x, (byte)y, reg[0], reg[1], reg[2], reg[3]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_not_in_region()
{
    //only units not in region stay in array
    int k = 0;
    WORD x = 0, y = 0;
    for (int i = 0; i < units; i++)
    {
        x = *((WORD*)((uintptr_t)unit[i] + S_DRAW_X)) / 32;
        y = *((WORD*)((uintptr_t)unit[i] + S_DRAW_Y)) / 32;
        if (!in_region((byte)x, (byte)y, reg[0], reg[1], reg[2], reg[3]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_target_in_region()
{
    //only units that have order in region stay in array
    int k = 0;
    byte x = 0, y = 0;
    for (int i = 0; i < units; i++)
    {
        x = *((byte*)((uintptr_t)unit[i] + S_ORDER_X));
        y = *((byte*)((uintptr_t)unit[i] + S_ORDER_Y));
        if (in_region(x, y, reg[0], reg[1], reg[2], reg[3]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_stat(byte pr, int v, byte cmp)
{
    //only units stay in array if have property compared to value is true
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (cmp_stat(unit[i], v, pr, cmp))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_ally(byte pl, bool a)
{
    //only units stay in array if allied to some player
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        byte o = get_stat(unit[i], S_OWNER);
        if (check_ally(o, pl) == a)
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_tp_flag()
{
    //if not teleported by portal
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if ((*((byte*)((uintptr_t)unit[i] + S_FLAGS3)) & SF_TELEPORT) == 0)//unused in actual game flag
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_hidden()
{
    //only not hidden units stay in array
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (!check_unit_hidden(unit[i]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_self(int* u)
{
    //unit remove self from array
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (!(unit[i] == u))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_full_hp()
{
    //if hp not full
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        byte id = *((byte*)((uintptr_t)unit[i] + S_ID));//unit id
        WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * id);//max hp
        WORD hp = *((WORD*)((uintptr_t)unit[i] + S_HP));//unit hp
        if (hp < mhp)//hp not full
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_fleshy()
{
    //only fleshy units stay in array
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        byte id = *((byte*)((uintptr_t)unit[i] + S_ID));//unit id
        if ((*(int*)(UNIT_GLOBAL_FLAGS + id * 4) & IS_FLESHY) != 0)//fleshy global flag
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_order_hp()
{
    //order array by hp from low to high
    for (int i = 0; i < units; i++)
    {
        int sm = i;
        for (int j = i + 1; j < units; j++)
        {
            WORD hpsm = *((WORD*)((uintptr_t)unit[sm] + S_HP));//unit hp
            WORD hpj = *((WORD*)((uintptr_t)unit[j] + S_HP));//unit hp
            if (hpj < hpsm)
            {
                sm = j;
            }
        }
        int* tmp = unit[i];
        unit[i] = unit[sm];
        unit[sm] = tmp;
    }
}

void sort_preplaced()
{
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (!check_unit_preplaced(unit[i]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_near_death()
{
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (check_unit_near_death(unit[i]))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_attack_can_hit(int* p)
{
    //only units stay in array that *p can attack them
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        int a = 0;
        a = ((int(*)(int*, int*))F_ATTACK_CAN_HIT)(p, unit[i]);//attack can hit original function
        if (a != 0)
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_attack_can_hit_range(int* p)
{
    //only units stay in array that *p can attack them and have passable terrain in attack range
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        int a = 0;
        a = ((int(*)(int*, int*))F_ATTACK_CAN_HIT)(p, unit[i]);//attack can hit
        if (a != 0)
        {
            byte id = *((byte*)((uintptr_t)unit[i] + S_ID));
            byte szx = *(byte*)(UNIT_SIZE_TABLE + 4 * id);
            byte szy = *(byte*)(UNIT_SIZE_TABLE + 4 * id + 2);
            byte idd = *((byte*)((uintptr_t)p + S_ID));
            byte rng = *(byte*)(UNIT_RANGE_TABLE + idd);
            byte ms = *(byte*)MAP_SIZE;
            byte xx = *((byte*)((uintptr_t)unit[i] + S_X));
            byte yy = *((byte*)((uintptr_t)unit[i] + S_Y));
            if (xx < rng)xx = 0;
            else xx -= rng;
            if (yy < rng)yy = 0;
            else yy -= rng;
            byte cl = *((byte*)((uintptr_t)p + S_MOVEMENT_TYPE));//movement type
            WORD mt = *(WORD*)(GLOBAL_MOVEMENT_TERRAIN_FLAGS + 2 * cl);//movement terrain flags

            bool f = false;
            for (int x = xx; (x < szx + xx + rng * 2 + 1) && (x < 127); x++)
            {
                for (int y = yy; (y < szy + yy + rng * 2 + 1) && (x < 127); y++)
                {
                    int aa = 1;
                    if ((cl == 0) || (cl == 3))//land and docked transport
                    {
                        aa = ((int (*)(int, int, int))F_XY_PASSABLE)(x, y, (int)mt);//original war2 func if terrain passable with that movement type
                    }
                    if ((x % 2 == 0) && (y % 2 == 0))//air and water
                    {
                        if ((cl == 1) || (cl == 2))
                        {
                            aa = ((int (*)(int, int, int))F_XY_PASSABLE)(x, y, (int)mt);
                        }
                    }
                    if (aa == 0)f = true;
                }
            }
            if (f)
            {
                unitt[k] = unit[i];
                k++;
            }
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_rune_near()
{
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        byte x = *((byte*)((uintptr_t)unit[i] + S_X));
        byte y = *((byte*)((uintptr_t)unit[i] + S_Y));
        bool f = false;
        for (int r = 0; r < 50; r++)//max runes 50
        {
            WORD d = *(WORD*)(RUNEMAP_TIMERS + 2 * r);
            if (d != 0)
            {
                byte xx = *(byte*)(RUNEMAP_X + r);
                byte yy = *(byte*)(RUNEMAP_Y + r);
                if (xx == x)
                {
                    if (yy > y)
                    {
                        if ((yy - y) == 1)f = true;
                    }
                    else
                    {
                        if ((y - yy) == 1)f = true;
                    }
                }
                if (yy == y)
                {
                    if (xx > x)
                    {
                        if ((xx - x) == 1)f = true;
                    }
                    else
                    {
                        if ((x - xx) == 1)f = true;
                    }
                }
            }
        }
        if (!f)
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_peon_loaded(byte r)
{
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (check_peon_loaded(unit[i], r))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void sort_peon_not_loaded(byte r)
{
    int k = 0;
    for (int i = 0; i < units; i++)
    {
        if (!check_peon_loaded(unit[i], r))
        {
            unitt[k] = unit[i];
            k++;
        }
    }
    units = k;
    for (int i = 0; i < units; i++)
    {
        unit[i] = unitt[i];
    }
}

void set_stat_all(byte pr, int v)
{
    for (int i = 0; i < units; i++)
    {
        set_stat(unit[i], v, pr);//set stat to all units in array
    }
}

void set_tp_flag(bool f)
{
    for (int i = 0; i < units; i++)
    {
        //set if unit can be teleported by portal (that flag unused in actual game)
        if (f)
            *((byte*)((uintptr_t)unit[i] + S_FLAGS3)) |= SF_TELEPORT;
        else
            *((byte*)((uintptr_t)unit[i] + S_FLAGS3)) &= ~SF_TELEPORT;
    }
}

void kill_all()
{
    for (int i = 0; i < units; i++)
    {
        unit_kill(unit[i]);//just kill all in array
    }
    units = 0;
}

void remove_all()
{
    for (int i = 0; i < units; i++)
    {
        unit_remove(unit[i]);//just kill all in array
    }
    units = 0;
}

void hide_all()
{
    for (int i = 0; i < units; i++)
    {
        unit_hide(unit[i]);
    }
}

void cast_all()
{
    for (int i = 0; i < units; i++)
    {
        unit_cast(unit[i]);//casting spells
    }
}

void flame(int* p)
{
    //p - unit
    //original war2 func creates 1 flame with selected animation frame
    //flameshield have 5 flames
    ((void (*)(int*, int, int))F_CREATE_FLAME)(p, 0, 0);
    ((void (*)(int*, int, int))F_CREATE_FLAME)(p, 10, 4);
    ((void (*)(int*, int, int))F_CREATE_FLAME)(p, 20, 2);
    ((void (*)(int*, int, int))F_CREATE_FLAME)(p, 30, 3);
    ((void (*)(int*, int, int))F_CREATE_FLAME)(p, 40, 5);
}

void flame_all()
{
    for (int i = 0; i < units; i++)
    {
        flame(unit[i]);
    }
}

void damag(int* p, byte n1, byte n2)
{
    WORD hp = *((WORD*)((uintptr_t)p + S_HP));//unit hp
    WORD n = n1 + 256 * n2;
    if (hp > n)
    {
        hp -= n;
        set_stat(p, hp, S_HP);
    }
    else
    {
        set_stat(p, 0, S_HP);
        unit_kill(p);
    }
}

void damag_by_player(int* p, byte n1, byte n2, byte pl)
{
    WORD hp = *((WORD*)((uintptr_t)p + S_HP));//unit hp
    WORD n = n1 + 256 * n2;
    if (hp > n)
    {
        hp -= n;
        set_stat(p, hp, S_HP);
    }
    else
    {
        set_stat(p, 0, S_HP);
        unit_kill(p);
        byte id = *((byte*)((uintptr_t)p + S_ID));
        byte o = *((byte*)((uintptr_t)p + S_OWNER));
        if ((id <= U_OWALL) && (o <= 15) && (pl <= 15))
        {
            if (all_kills[pl * 16 * U_OWALL + o * U_OWALL + id] < 255)
                all_kills[pl * 16 * U_OWALL + o * U_OWALL + id]++;
        }
    }
}

void damag_all(byte n1, byte n2)
{
    for (int i = 0; i < units; i++)
    {
        damag(unit[i], n1, n2);
    }
}

void damag_all_by_player(byte n1, byte n2, byte pl)
{
    for (int i = 0; i < units; i++)
    {
        damag_by_player(unit[i], n1, n2, pl);
    }
}

void heal(int* p, byte n1, byte n2)
{
    byte id = *((byte*)((uintptr_t)p + S_ID));//unit id
    WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * id);//max hp
    WORD hp = *((WORD*)((uintptr_t)p + S_HP));//unit hp
    WORD n = n1 + 256 * n2;
    if (hp < mhp)
    {
        hp += n;
        if (hp > mhp)
            hp = mhp;//canot heal more than max hp
        set_stat(p, hp, S_HP);
    }
}

void heal_all(byte n1, byte n2)
{
    for (int i = 0; i < units; i++)
    {
        heal(unit[i], n1, n2);
    }
}

void mana_regen(int* p, byte n)
{
    byte tid = *((byte*)((uintptr_t)p + S_ID));
    bool f = (tid == U_MAGE) || (tid == U_DK) || (tid == U_PALADIN) || (tid == U_OGREMAGE) ||
        (tid == U_HADGAR) || (tid == U_TERON) || (tid == U_GULDAN) ||
        (tid == U_UTER) || (tid == U_TYRALYON) || (tid == U_CHOGAL) || (tid == U_DENTARG);
    if (f)
    {
        byte mp = *((byte*)((uintptr_t)p + S_MANA));//unit mana
        if (((int)mp + n) > 255)mp = 255;
        else mp += n;
        set_stat(p, mp, S_MANA);
    }
}

void mana_regen_all(byte n)
{
    for (int i = 0; i < units; i++)
    {
        mana_regen(unit[i], n);
    }
}

void give(int* p, byte owner)
{
    ((void (*)(int*, byte, byte))F_CAPTURE)(p, owner, 1);//original capture unit war2 func
    *(byte*)(RESCUED_UNITS + 2 * owner) -= 1;//reset number of captured units
}

void give_all(byte o)
{
    for (int i = 0; i < units; i++)
    {
        give(unit[i], o);
    }
}

void peon_load(int* u, byte r)
{
    byte f = *((byte*)((uintptr_t)u + S_PEON_FLAGS));
    if (!(f & PEON_LOADED))
    {
        if (r == 0)
        {
            f |= PEON_LOADED;
            f |= PEON_HARVEST_GOLD;
            set_stat(u, f, S_PEON_FLAGS);
            ((void (*)(int*))F_GROUP_SET)(u);
        }
        else
        {
            f |= PEON_LOADED;
            f |= PEON_HARVEST_LUMBER;
            set_stat(u, f, S_PEON_FLAGS);
            ((void (*)(int*))F_GROUP_SET)(u);
        }
    }
}

void peon_load_all(byte r)
{
    for (int i = 0; i < units; i++)
    {
        peon_load(unit[i], r);
    }
}

void viz_area(byte x, byte y, byte pl, byte sz)
{
    int Vf = F_VISION2;
    switch (sz)
    {
    case 0:Vf = F_VISION2; break;
    case 1:Vf = F_VISION2; break;
    case 2:Vf = F_VISION2; break;
    case 3:Vf = F_VISION3; break;
    case 4:Vf = F_VISION4; break;
    case 5:Vf = F_VISION5; break;
    case 6:Vf = F_VISION6; break;
    case 7:Vf = F_VISION7; break;
    case 8:Vf = F_VISION8; break;
    case 9:Vf = F_VISION9; break;
    default: Vf = F_VISION2; break;
    }
    for (byte i = 0; i < 8; i++)
    {
        if (((1 << i) & pl) != 0)
        {
            ((void (*)(WORD, WORD, byte))Vf)(x, y, i);
        }
    }
}

void viz_area_add(byte x, byte y, byte pl, byte sz)
{
    if ((vizs_n >= 0) && (vizs_n <= 255))
    {
        vizs_areas[vizs_n].x = x;
        vizs_areas[vizs_n].y = y;
        vizs_areas[vizs_n].p = pl;
        vizs_areas[vizs_n].s = sz;
        vizs_n++;
    }
}

void viz_area_all(byte pl, byte sz)
{
    for (int i = 0; i < units; i++)
    {
        byte x = *((byte*)((uintptr_t)unit[i] + S_X));
        byte y = *((byte*)((uintptr_t)unit[i] + S_Y));
        viz_area_add(x, y, pl, sz);
    }
}

bool unit_move(byte x, byte y, int* unit, bool inval)
{
    if (x < 0)return false;
    if (y < 0)return false;//canot go negative
    byte mxs = *(byte*)MAP_SIZE;//map size
    if (x >= mxs)return false;
    if (y >= mxs)return false;//canot go outside map
    if (check_unit_hidden(unit))return false;//if unit not hidden
    byte cl = *((byte*)((uintptr_t)unit + S_MOVEMENT_TYPE));//movement type
    WORD mt = *(WORD*)(GLOBAL_MOVEMENT_TERRAIN_FLAGS + 2 * cl);//movement terrain flags

    int aa = 1;
    if ((cl == 0) || (cl == 3))//land and docked transport
    {
        aa = ((int (*)(int, int, int))F_XY_PASSABLE)(x, y, (int)mt);//original war2 func if terrain passable with that movement type
    }
    if ((x % 2 == 0) && (y % 2 == 0))//air and water
    {
        if ((cl == 1) || (cl == 2))
        {
            aa = ((int (*)(int, int, int))F_XY_PASSABLE)(x, y, (int)mt);
        }
    }
    if (aa == 0)
    {
        ((void (*)(int*))F_UNIT_UNPLACE)(unit);//unplace
        set_stat(unit, x, S_X);
        set_stat(unit, y, S_Y);//change real coords
        set_stat(unit, x * 32, S_DRAW_X);
        set_stat(unit, y * 32, S_DRAW_Y);//change draw sprite coords
        ((void (*)(int*))F_UNIT_PLACE)(unit);//place
        if (inval)inval_game_view();
        return true;
    }
    return false;
}

void move_all(byte x, byte y)
{
    sort_stat(S_ID, U_FARM, CMP_SMALLER);//non buildings
    sort_stat(S_ANIMATION, 2, CMP_EQ);//only if animation stop
    for (int i = 0; i < units; i++)
    {
        int xx = 0, yy = 0, k = 1;
        bool f = unit_move(x, y, unit[i], false);
        xx--;
        while ((!f) & (k < 5))//goes in spiral like original war2 (size 5)
        {
            while ((!f) & (yy < k))
            {
                f = unit_move(x + xx, y + yy, unit[i], false);
                yy++;
            }
            while ((!f) & (xx < k))
            {
                f = unit_move(x + xx, y + yy, unit[i], false);
                xx++;
            }
            while ((!f) & (yy > -k))
            {
                f = unit_move(x + xx, y + yy, unit[i], false);
                yy--;
            }
            while ((!f) & (xx >= -k))
            {
                f = unit_move(x + xx, y + yy, unit[i], false);
                xx--;
            }
            k++;
        }
        if (f)inval_game_view();
    }
}

void give_order(int* u, byte x, byte y, byte o)
{
    byte id = *((byte*)((uintptr_t)u + S_ID));
    if (id < U_FARM)
    {
        char buf[] = "\x0";
        bool f = ((o >= ORDER_SPELL_VISION) && (o <= ORDER_SPELL_ROT));
        if (f)
        {
            buf[0] = o;
            PATCH_SET((char*)GW_ACTION_TYPE, buf);
        }
        int* tr = NULL;
        for (int i = 0; i < 16; i++)
        {
            int* p = (int*)(UNITS_LISTS + 4 * i);//pointer to units list for each player
            if (p)
            {
                p = (int*)(*p);
                while (p)
                {
                    if (p!=u)
                    {
                        if (!check_unit_dead(p) && !check_unit_hidden(p))
                        {
                            byte xx = *(byte*)((uintptr_t)p + S_X);
                            byte yy = *(byte*)((uintptr_t)p + S_Y);
                            if ((abs(x - xx) <= 2) && (abs(y - yy) <= 2))
                            {
                                if (f)
                                {
                                    byte idd = *(byte*)((uintptr_t)p + S_ID);
                                    if (idd < U_FARM)
                                    {
                                        bool trf = true;
                                        if (o == ORDER_SPELL_ARMOR)
                                        {
                                            WORD ef = *(WORD*)((uintptr_t)p + S_SHIELD);
                                            trf = ef == 0;
                                        }
                                        if (o == ORDER_SPELL_BLOODLUST)
                                        {
                                            WORD ef = *(WORD*)((uintptr_t)p + S_BLOOD);
                                            trf = ef == 0;
                                        }
                                        if (o == ORDER_SPELL_HASTE)
                                        {
                                            WORD ef = *(WORD*)((uintptr_t)p + S_HASTE);
                                            trf = (ef == 0) || (ef > 0x7FFF);
                                        }
                                        if (o == ORDER_SPELL_SLOW)
                                        {
                                            WORD ef = *(WORD*)((uintptr_t)p + S_HASTE);
                                            trf = (ef == 0) || (ef <= 0x7FFF);
                                        }
                                        if (o == ORDER_SPELL_POLYMORPH)
                                        {
                                            trf = idd != U_CRITTER;
                                        }
                                        if (o == ORDER_SPELL_HEAL)
                                        {
                                            WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * idd);
                                            WORD hp = *((WORD*)((uintptr_t)p + S_HP));
                                            trf = hp < mhp;
                                        }
                                        if (trf)
                                        {
                                            WORD efi = *(WORD*)((uintptr_t)p + S_INVIZ);
                                            trf = efi == 0;
                                        }
                                        if (trf)
                                            tr = p;
                                    }
                                }
                                else
                                    tr = p;
                            }
                        }
                    }
                    p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
                }
            }
        }
        bool aoe = (o == ORDER_SPELL_VISION) || (o == ORDER_SPELL_EXORCISM) || (o == ORDER_SPELL_FIREBALL) ||
            (o == ORDER_SPELL_BLIZZARD) || (o == ORDER_SPELL_EYE) || (o == ORDER_SPELL_RAISEDEAD) ||
            (o == ORDER_SPELL_DRAINLIFE) || (o == ORDER_SPELL_WHIRLWIND) || (o == ORDER_SPELL_RUNES) ||
            (o == ORDER_SPELL_ROT) || (o == ORDER_MOVE) || (o == ORDER_PATROL) ||
            (o == ORDER_ATTACK_AREA) || (o == ORDER_ATTACK_WALL) || (o == ORDER_STAND) ||
            (o == ORDER_ATTACK_GROUND) || (o == ORDER_ATTACK_GROUND_MOVE) || (o == ORDER_DEMOLISH) ||
            (o == ORDER_HARVEST) || (o == ORDER_RETURN) || (o == ORDER_UNLOAD_ALL) || (o == ORDER_STOP);

        if (o != ORDER_ATTACK_WALL)
        {
            int ord = *(int*)(ORDER_FUNCTIONS + 4 * o);//orders functions
            if (!aoe && (tr != NULL) && (tr != u))
                ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(u, 0, 0, tr, ord);//original war2 order
            if (aoe)
                ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(u, x, y, NULL, ord);//original war2 order
        }
        else
        {
            byte oru = *(byte*)((uintptr_t)u + S_ORDER);
            if (oru!=ORDER_ATTACK_WALL)
            {
                int ord = *(int*)(ORDER_FUNCTIONS + 4 * ORDER_STOP);//orders functions
                ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(u, 0, 0, NULL, ord);//original war2 order
            }
            set_stat(u, ORDER_ATTACK_WALL, S_NEXT_ORDER);
            set_stat(u, x, S_ORDER_X);
            set_stat(u, y, S_ORDER_Y);
        }

        if (f)
        {
            buf[0] = 0;
            PATCH_SET((char*)GW_ACTION_TYPE, buf);
        }
    }
}

void give_order_spell_target(int* u, int* t, byte o)
{
    if ((u != NULL) && (t != NULL))
    {
        byte id = *((byte*)((uintptr_t)u + S_ID));
        if (id < U_FARM)
        {
            char buf[] = "\x0";
            if ((o >= ORDER_SPELL_VISION) && (o <= ORDER_SPELL_ROT))
            {
                buf[0] = o;
                PATCH_SET((char*)GW_ACTION_TYPE, buf);

                int ord = *(int*)(ORDER_FUNCTIONS + 4 * o);//orders functions
                ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(u, 0, 0, t, ord);//original war2 order

                buf[0] = 0;
                PATCH_SET((char*)GW_ACTION_TYPE, buf);
            }
        }
    }
}

void order_all(byte x, byte y, byte o)
{
    for (int i = 0; i < units; i++)
    {
        give_order(unit[i], x, y, o);
    }
}

void runestone()
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = *((byte*)((uintptr_t)p + S_ID)) == U_RUNESTONE;
                if (f)
                {
                    if (!check_unit_dead(p) && check_unit_complete(p))
                    {//alive and completed runestone
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        set_region((int)x - 4, (int)y - 4, (int)x + 5, (int)y + 5);//set region around myself
                        find_all_alive_units(ANY_MEN);
                        sort_in_region();
                        if (runes[8] == 1)//only allied units can can recieve bufs
                        {
                            byte o = *((byte*)((uintptr_t)p + S_OWNER));
                            for (int ui = 0; ui < 16; ui++)
                            {
                                if (!check_ally(o, ui))// tp
                                    sort_stat(S_OWNER, ui, CMP_NEQ);
                            }
                        }
                        if (runes[0] == 1)set_stat_all(S_INVIZ, (int)(*(WORD*)INVIZ_TIME));//inviz
                        if (runes[1] == 1)set_stat_all(S_SHIELD, (int)(*(WORD*)SHIELD_TIME));//shield
                        if (runes[2] == 1)set_stat_all(S_BLOOD, (int)(*(WORD*)BLOOD_TIME));//blood
                        if (runes[3] == 1)set_stat_all(S_HASTE, (int)(*(WORD*)HASTE_TIME1));//haste
                        if (runes[4] == 1)flame_all();
                        //5 mana
                        //6 heal
                        if (runes[7] == 1)//kill all not my owner units
                        {
                            byte o = *((byte*)((uintptr_t)p + S_OWNER));
                            sort_stat(S_OWNER, o, CMP_NEQ);
                            kill_all();
                        }
                        sort_stat(S_KILLS + 1, 0, CMP_EQ);
                        set_stat_all(S_KILLS + 1, 100);
                        if (runes[5] != 0)mana_regen_all(runes[5]);
                        if (runes[6] != 0)heal_all(runes[6], 0);
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void portal()
{
    bool mp = true;
    for (int i = 0; i < 16; i++)
    {
        units = 0;
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            int* fp = NULL;
            while (p)
            {
                bool f = *((byte*)((uintptr_t)p + S_ID)) == U_PORTAL;
                if (f)
                {
                    if (!check_unit_dead(p) && check_unit_complete(p))
                    {//alive and completed portal
                        if (!fp)fp = p;//remember first portal
                        byte tx = *((byte*)((uintptr_t)p + S_X)) + 1;
                        byte ty = *((byte*)((uintptr_t)p + S_Y)) + 1;//exit point is in center of portal
                        move_all(tx, ty);//teleport from previous portal
                        set_tp_flag(true);
                        set_stat_all(S_NEXT_ORDER, ORDER_STOP);
                        set_stat_all(S_ORDER_X, 128);
                        set_stat_all(S_ORDER_Y, 128);
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        set_region((int)x - 1, (int)y - 1, (int)x + 4, (int)y + 4);//set region around myself
                        find_all_alive_units(ANY_MEN);
                        sort_in_region();
                        if (aport)
                        {
                            byte o = *((byte*)((uintptr_t)p + S_OWNER));
                            for (int ui = 0; ui < 16; ui++)
                            {
                                if (!check_ally(o, ui))//only allied units can tp
                                    sort_stat(S_OWNER, ui, CMP_NEQ);
                            }
                        }
                        sort_tp_flag();//flag show if unit was not teleported
                        mp = true;
                        if (mport)//only teleport if some caster near
                        {
                            mp = false;
                            for (int ui = 0; ui < units; ui++)
                            {
                                byte uid = *((byte*)((uintptr_t)unit[ui] + S_ID));
                                if ((uid == U_MAGE) || (uid == U_DK) || (uid == U_TERON) || (uid == U_HADGAR) || (uid == U_GULDAN))
                                    mp = true;//can tp only if mage nearby (teron hadgar and guldan too)
                            }
                        }
                        if (!mp)units = 0;
                        else
                        {
                            sort_stat(S_ORDER, ORDER_STOP, CMP_EQ);
                            sort_stat(S_ORDER_UNIT_POINTER, 0, CMP_EQ);
                            sort_stat(S_ORDER_UNIT_POINTER + 1, 0, CMP_EQ);
                            sort_stat(S_ORDER_UNIT_POINTER + 2, 0, CMP_EQ);
                            sort_stat(S_ORDER_UNIT_POINTER + 3, 0, CMP_EQ);
                            set_region((int)x, (int)y, (int)x + 3, (int)y + 3);//set region inside myself
                            sort_target_in_region();
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
            if (fp)//first portal teleports from last
            {
                byte tx = *((byte*)((uintptr_t)fp + S_X)) + 1;
                byte ty = *((byte*)((uintptr_t)fp + S_Y)) + 1;
                move_all(tx, ty);
                set_tp_flag(true);
                set_stat_all(S_NEXT_ORDER, ORDER_STOP);
                set_stat_all(S_ORDER_X, 128);
                set_stat_all(S_ORDER_Y, 128);
            }
        }
    }
    find_all_alive_units(ANY_MEN);
    set_tp_flag(false);//reset tp flags to all
}

void wharf()
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = (*((byte*)((uintptr_t)p + S_ID)) == U_SHIPYARD) || (*((byte*)((uintptr_t)p + S_ID)) == U_WHARF);
                if (f)
                {
                    if (!check_unit_dead(p) && check_unit_complete(p))
                    {
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        set_region((int)x - 2, (int)y - 2, (int)x + 4, (int)y + 4);//set region around myself
                        find_all_alive_units(ANY_MEN);
                        sort_in_region();
                        sort_hidden();
                        sort_stat(S_MOVEMENT_TYPE, MOV_WATER, CMP_BIGGER_EQ);//find ships - movement type >= water (2 or 3 actually(ships=2 transport=3))
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        for (int ui = 0; ui < 16; ui++)
                        {
                            if (!check_ally(o, ui))//only allied ships
                                sort_stat(S_OWNER, ui, CMP_NEQ);
                        }
                        heal_all(4, 0);
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void paladin()
{
    for (int ii = 0; ii < 16; ii++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * ii);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = ((*((byte*)((uintptr_t)p + S_ID)) == U_PALADIN) ||
                    (*((byte*)((uintptr_t)p + S_ID)) == U_UTER) ||
                    (*((byte*)((uintptr_t)p + S_ID)) == U_TYRALYON));
                if (f)
                {
                    if (!check_unit_dead(p) && !check_unit_hidden(p))
                    {
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        if (((*(byte*)(SPELLS_LEARNED + 4 * o) & (1 << L_HEAL)) != 0) &&
                            ((*(byte*)(SPELLS_LEARNED + 4 * o) & (1 << L_GREATER_HEAL)) != 0))
                            //if player learned heal and autoheal
                        {
                            byte x = *((byte*)((uintptr_t)p + S_X));
                            byte y = *((byte*)((uintptr_t)p + S_Y));
                            set_region((int)x - 5, (int)y - 5, (int)x + 5, (int)y + 5);//set region around myself
                            find_all_alive_units(ANY_MEN);
                            sort_in_region();
                            sort_hidden();
                            sort_fleshy();//fleshy units (not heal cata and ships)
                            sort_full_hp();//if unit hp not full
                            sort_self(p);//not heal self
                            sort_order_hp();//heal lovest hp first
                            for (int ui = 0; ui < 16; ui++)
                            {
                                if (!check_ally(o, ui))//only allied units
                                    sort_stat(S_OWNER, ui, 1);
                            }
                            byte cost = *(byte*)(MANACOST + 2 * GREATER_HEAL);//2* cause manacost is WORD
                            if (war2mod)
                                if (*((byte*)((uintptr_t)p + S_ID)) == U_UTER)cost = 1;
                            for (int i = 0; i < units; i++)
                            {
                                byte mp = *((byte*)((uintptr_t)p + S_MANA));//paladin mp
                                if (mp >= cost)
                                {
                                    byte id = *((byte*)((uintptr_t)unit[i] + S_ID));//unit id
                                    WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * id);//max hp
                                    WORD hp = *((WORD*)((uintptr_t)unit[i] + S_HP));//unit hp
                                    WORD shp = mhp - hp;//shortage of hp
                                    while (!(mp >= (shp * cost)) && (shp > 0))shp -= 1;
                                    if (shp > 0)//if can heal at least 1 hp
                                    {
                                        heal(unit[i], (byte)shp, 0);
                                        mp -= shp * cost;
                                        *((byte*)((uintptr_t)p + S_MANA)) = mp;
                                        WORD xx = *((WORD*)((uintptr_t)unit[i] + S_DRAW_X));
                                        WORD yy = *((WORD*)((uintptr_t)unit[i] + S_DRAW_Y));
                                        bullet_create(xx + 16, yy + 16, B_HEAL);//create heal effect
                                        if (shp >= 3)
                                            ((void (*)(WORD, WORD, byte))F_SPELL_SOUND_XY)(xx + 16, yy + 16, SS_HEAL);//heal sound
                                    }
                                }
                                else i = units;
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void transport()
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = (*((byte*)((uintptr_t)p + S_ID)) == U_HTRANSPORT) || (*((byte*)((uintptr_t)p + S_ID)) == U_OTRANSPORT);
                if (f)
                {
                    if (!check_unit_dead(p) && (cmp_stat(p, ANIM_STOP, S_ANIMATION, CMP_EQ)))//if transport stop
                    {
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        for (byte ui = 0; ui < 16; ui++)
                        {
                            set_region((int)x - 1, (int)y - 1, (int)x + 1, (int)y + 1);//set region around myself
                            find_all_alive_units(ANY_MEN);
                            sort_in_region();
                            sort_hidden();
                            sort_stat(S_MOVEMENT_TYPE, MOV_LAND, CMP_EQ);
                            sort_stat(S_ORDER, ORDER_STOP, CMP_EQ);
                            sort_stat(S_ANIMATION, ANIM_STOP, CMP_EQ);
                            sort_stat(S_OWNER, ui, CMP_EQ);
                            bool f = false;
                            if ((*(byte*)(CONTROLER_TYPE + o) == C_PLAYER))
                            {
                                f = true;
                                if ((*(byte*)(CONTROLER_TYPE + ui) == C_COMP))
                                {
                                    if (!check_ally(o, ui))//only allied comps
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                }
                                if ((*(byte*)(CONTROLER_TYPE + ui) == C_PLAYER))
                                {
                                    if (!check_ally(o, ui) || (ui == o))//only allied players
                                    {
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                    }
                                    sort_stat(S_ORDER_UNIT_POINTER, 0, CMP_EQ);
                                    sort_stat(S_ORDER_UNIT_POINTER + 1, 0, CMP_EQ);
                                    sort_stat(S_ORDER_UNIT_POINTER + 2, 0, CMP_EQ);
                                    sort_stat(S_ORDER_UNIT_POINTER + 3, 0, CMP_EQ);
                                    set_region((int)x, (int)y, (int)x, (int)y);//set region inside myself
                                    sort_target_in_region();
                                }
                            }
                            if ((*(byte*)(CONTROLER_TYPE + o) == C_COMP))
                            {
                                f = true;
                                if ((*(byte*)(CONTROLER_TYPE + ui) == C_COMP))
                                {
                                    if (!check_ally(o, ui) || (ui == o))//only allied comps
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                }
                                if ((*(byte*)(CONTROLER_TYPE + ui) == C_PLAYER))
                                {
                                    if (!check_ally(o, ui))//only allied players
                                    {
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                    }
                                    sort_stat(S_ORDER_UNIT_POINTER, 0, CMP_EQ);
                                    sort_stat(S_ORDER_UNIT_POINTER + 1, 0, CMP_EQ);
                                    sort_stat(S_ORDER_UNIT_POINTER + 2, 0, CMP_EQ);
                                    sort_stat(S_ORDER_UNIT_POINTER + 3, 0, CMP_EQ);
                                    set_region((int)x, (int)y, (int)x, (int)y);//set region inside myself
                                    sort_target_in_region();
                                }
                            }
                            if (f)
                            {
                                sort_stat(S_KILLS + 1, 0, CMP_EQ);
                                set_stat_all(S_KILLS + 1, 100);
                                set_stat_all(S_NEXT_ORDER, ORDER_ENTER_TRANSPORT);
                                set_stat_all(S_ORDER_UNIT_POINTER, ((int)p) % 256);
                                set_stat_all(S_ORDER_UNIT_POINTER + 1, (((int)p) / 256) % 256);
                                set_stat_all(S_ORDER_UNIT_POINTER + 2, ((((int)p) / 256) / 256) % 256);
                                set_stat_all(S_ORDER_UNIT_POINTER + 3, (((((int)p) / 256) / 256) / 256) % 256);
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void slow_aura(byte id)
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = (*((byte*)((uintptr_t)p + S_ID)) == id);
                if (f)
                {
                    if (!check_unit_dead(p))
                    {
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        set_region((int)x - 5, (int)y - 5, (int)x + 5, (int)y + 5);//set region around myself
                        find_all_alive_units(ANY_MEN);
                        sort_in_region();
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        for (int ui = 0; ui < 16; ui++)
                        {
                            if (check_ally(o, ui))//only enemies
                                sort_stat(S_OWNER, ui, CMP_NEQ);
                        }
                        set_stat_all(S_HASTE, 0xfcdf);//-800
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void death_aura(byte id)
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = (*((byte*)((uintptr_t)p + S_ID)) == id);
                if (f)
                {
                    if (!check_unit_dead(p))
                    {
                        byte mp = *((byte*)((uintptr_t)p + S_MANA));
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        byte xx = *((byte*)((uintptr_t)p + S_ORDER_X));
                        byte yy = *((byte*)((uintptr_t)p + S_ORDER_Y));
                        set_stat(p, 255, S_MANA);
                        set_stat(p, x, S_ORDER_X);
                        set_stat(p, y, S_ORDER_Y);
                        char buf[] = "\x90\x90\x90\x90\x90\x90\x90\x90";
                        PATCH_SET((char*)RAISE_DEAD_DOING_SPELL1, buf);
                        char buf3[] = "\x90\x90\x90";
                        PATCH_SET((char*)RAISE_DEAD_DOING_SPELL2, buf3);
                        ((void (*)(int*))F_RAISE_DEAD)(p);
                        char buf2[] = "\x6a\x2\x53\xe8\x9f\x3\x1\x0";
                        PATCH_SET((char*)RAISE_DEAD_DOING_SPELL1, buf2);
                        char buf4[] = "\x83\xc4\x8";
                        PATCH_SET((char*)RAISE_DEAD_DOING_SPELL2, buf4);
                        set_stat(p, mp, S_MANA);
                        set_stat(p, xx, S_ORDER_X);
                        set_stat(p, yy, S_ORDER_Y);
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void sneak(byte id)
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = (*((byte*)((uintptr_t)p + S_ID)) == id);
                if (f)
                {
                    if (!check_unit_dead(p))
                    {
                        byte o = *((byte*)((uintptr_t)p + S_ORDER));
                        WORD n = *((WORD*)((uintptr_t)p + S_INVIZ));
                        if (o == ORDER_STAND)
                        {
                            if (n <= 10)set_stat(p, 10, S_INVIZ);
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

bool slot_alive(byte p)
{
    return (get_val(ALL_BUILDINGS, p) + get_val(ALL_UNITS, p)) != 0;//no units and buildings
}

void ally(byte p1, byte p2, byte a)
{
    //set ally bytes in table
    *(byte*)(ALLY + p1 + 16 * p2) = a;
    *(byte*)(ALLY + p2 + 16 * p1) = a;
    ((void (*)())F_RESET_COLORS)();//orig war2 func reset colors of sqares around units
}

void ally_one_sided(byte p1, byte p2, byte a)
{
    //set ally bytes in table
    *(byte*)(ALLY + p1 + 16 * p2) = a;
    ((void (*)())F_RESET_COLORS)();//orig war2 func reset colors of sqares around units
}

bool check_opponents(byte player)
{
    //check if player have opponents
    bool f = false;
    if (get_player_team(player) != 9)
    {
        byte o = C_NOBODY;
        for (byte i = 0; i < 8; i++)
        {
            if (player != i)
            {
                o = *(byte*)(CONTROLER_TYPE + i);//player type
                if (o == C_PLAYER)//if real player
                {
                    if ((slot_alive(i) && !check_av(player, i)) && (obs_lose[i] == 0))//if not AVed and not dead and not surrendered obs
                        f = true;
                }
                if (o == C_COMP)//if computer
                {
                    if (slot_alive(i) && !check_ally(player, i))//if enemy and not dead
                        f = true;
                }
            }
        }
    }
    return f;
}

void viz(int p1, int p2, byte a)
{
    //set vision bits
    byte v = *(byte*)(VIZ + p1);
    if (a == 0)
        v = v & (~(1 << p2));
    else
        v = v | (1 << p2);
    *(byte*)(VIZ + p1) = v;

    v = *(byte*)(VIZ + p2);
    if (a == 0)
        v = v & (~(1 << p1));
    else
        v = v | (1 << p1);
    *(byte*)(VIZ + p2) = v;
}

void viz_one_sided(int p1, int p2, byte a)
{
    //set vision bits
    byte v = *(byte*)(VIZ + p1);
    if (a == 0)
        v = v & (~(1 << p2));
    else
        v = v | (1 << p2);
    *(byte*)(VIZ + p1) = v;
}

void viz_obs(int p1, int p2)
{
    //set vision bits one sided obs
    viz_one_sided(p1, p2, 0);
    viz_one_sided(p2, p1, 1);
}

void set_mission(char b)
{
    if (b == '\xFF')//if 255 just replace to default
    {
        char o[] = "\x29";
        PATCH_SET((char*)MULTPL_SHOW_MISSION1, o);
        o[0] = '\x20';
        PATCH_SET((char*)MULTPL_SHOW_MISSION2, o);
    }
    else
    {
        char o[] = "\x00";
        PATCH_SET((char*)MULTPL_SHOW_MISSION1, o);
        PATCH_SET((char*)MULTPL_SHOW_MISSION2, o);
        char obj[2];
        obj[0] = b;//mission obj text from objectives.tbl from MPQ
        PATCH_SET((char*)LEVEL_OBJ, obj);
    }
}

void comps_vision(bool v)
{
    //comps can give vision too
    if (v)
    {
        char o[] = "\x0";
        PATCH_SET((char*)COMPS_VIZION, o);
        char o2[] = "\x90\x90";
        PATCH_SET((char*)COMPS_VIZION2, o2);
        char o3[] = "\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)COMPS_VIZION3, o3);
    }
    else
    {
        char o[] = "\xAA";
        PATCH_SET((char*)COMPS_VIZION, o);
        //char o2[] = "\x84\xC9";
        //PATCH_SET((char*)COMPS_VIZION2, o2);
        char o3[] = "\xF\x85\x8C\x0\x0\x0";
        PATCH_SET((char*)COMPS_VIZION3, o3);
    }
}

void change_res(byte p, byte r, byte k, int m)
{
    int a = GOLD;
    int* rs = (int*)a;
    DWORD res = 0;
    bool s = false;
    if (p >= 0 && p <= 8)//player id
    {
        switch (r)//select resource and add or substract it
        {
        case 0:
            a = GOLD + 4 * p;
            s = false;
            break;
        case 1:
            a = LUMBER + 4 * p;
            s = false;
            break;
        case 2:
            a = OIL + 4 * p;
            s = false;
            break;
        case 3:
            a = GOLD + 4 * p;
            s = true;
            break;
        case 4:
            a = LUMBER + 4 * p;
            s = true;
            break;
        case 5:
            a = OIL + 4 * p;
            s = true;
            break;
        default:break;
        }
        if (r >= 0 && r <= 5)
        {
            rs = (int*)a;//resourse pointer
            if (s)
            {
                if (*rs > (int)(k * m))
                    res = *rs - (k * m);
                else
                    res = 0;//canot go smaller than 0
            }
            else
            {
                if (*rs <= (256 * 256 * 256 * 32))
                    res = *rs + (k * m);
            }
            patch_setdword((DWORD*)a, res);
        }
    }
}

void add_total_res(byte p, byte r, byte k, int m)
{
    int a = GOLD_TOTAL;
    int* rs = (int*)a;
    DWORD res = 0;
    if (p >= 0 && p <= 8)//player id
    {
        switch (r)//select resource and add or substract it
        {
        case 0:
            a = GOLD_TOTAL + 4 * p;
            break;
        case 1:
            a = LUMBER_TOTAL + 4 * p;
            break;
        case 2:
            a = OIL_TOTAL + 4 * p;
            break;
        default:break;
        }
        if (r >= 0 && r <= 2)
        {
            rs = (int*)a;//resourse pointer
            if (*rs <= (256 * 256 * 256 * 32))
                res = *rs + (k * m);
            patch_setdword((DWORD*)a, res);
        }
    }
}

void set_res(byte p, byte r, byte k1, byte k2, byte k3, byte k4)
{
    //as before but dnt add or sub res, just set given value
    char buf[4] = { 0 };
    int a = 0;
    if (p >= 0 && p <= 8)
    {
        switch (r)
        {
        case 0:
            a = GOLD + 4 * p;
            break;
        case 1:
            a = LUMBER + 4 * p;
            break;
        case 2:
            a = OIL + 4 * p;
            break;
        default:break;
        }
        if (r >= 0 && r <= 2)
        {
            buf[0] = k1;
            buf[1] = k2;
            buf[2] = k3;
            buf[3] = k4;
            PATCH_SET((char*)a, buf);
        }
    }
}

bool cmp_res(byte p, byte r, byte k1, byte k2, byte k3, byte k4, byte cmp)
{
    //compare resource to value
    int a = GOLD;
    int* rs = (int*)a;
    if (p >= 0 && p <= 8)
    {
        switch (r)
        {
        case 0:
            a = GOLD + 4 * p;
            break;
        case 1:
            a = LUMBER + 4 * p;
            break;
        case 2:
            a = OIL + 4 * p;
            break;
        default:break;
        }
        if (r >= 0 && r <= 2)
        {
            rs = (int*)a;
            return cmp_args4(cmp, *rs, k1 + 256 * k2 + 256 * 256 * k3 + 256 * 256 * 256 * k4);
        }
    }
    return false;
}

byte ogre_find_player(byte p)
{
    if (get_player_team(p) == 0)return p;
    for (int i = 0; i < 8; i++)
    {
        if ((get_player_team(p) == get_player_team(i)) && (get_player_controler(i) != C_COMP))return i;
    }
    return p;
}

byte ogre_find_real_player(byte p)
{
    if (get_player_team(p) == 0)return p;
    for (int i = 0; i < 8; i++)
    {
        if ((get_player_team(p) == get_player_team(i)) && (get_player_controler(i) == C_PLAYER))return i;
    }
    return p;
}

int empty_false(byte) { return 0; }//always return false function
int empty_true(byte) { return 1; }//always return true function
void empty_build(int id)
{
    ((void (*)(int))F_TRAIN_UNIT)(id);//original build unit func
}
void empty_build_building(int id)
{
    ((void (*)(int))F_BUILD_BUILDING)(id);//original build func
}
void empty_build_research(int id)
{
    ((void (*)(int))F_BUILD_RESEARCH)(id);
}
void empty_build_research_spell(int id)
{
    ((void (*)(int))F_BUILD_RESEARCH_SPELL)(id);
}
void empty_build_upgrade_self(int id)
{
    ((void (*)(int))F_BUILD_UPGRADE_SELF)(id);
}
void empty_cast_spell(int id)
{
    ((void (*)(int))F_CAST_SPELL)(id);
}
int empty_research_swords(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_SWORDS)(id); }//0 or 1
int empty_research_shield(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_SHIELD)(id); }//0 or 1
int empty_research_cat(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_CAT)(id); }//0 or 1
int empty_research_arrows(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_ARROWS)(id); }//0 or 1
int empty_research_ships_at(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_SHIPS_AT)(id); }//0 or 1
int empty_research_ships_def(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_SHIPS_DEF)(id); }//0 or 1
int empty_research_ranger(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_RANGER)(id); }
int empty_research_scout(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_SCOUT)(id); }
int empty_research_long(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_LONG)(id); }
int empty_research_marks(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_MARKS)(id); }
int empty_research_spells(byte id) { return ((int (*)(int))F_CHECK_RESEARCH_SPELL)(id); }
//00444410
int empty_upgrade_th(byte id) { return ((int (*)(int))F_CHECK_UPGRADE_TH)(id); }//0 or 1
int empty_upgrade_tower(byte id) { return ((int (*)(int))F_CHECK_UPGRADE_TOWER)(id); }//0 or 1
int empty_spell_learned(byte id) { return ((int (*)(int))F_CHECK_SPELL_LEARNED)(id); }

void build_forest(int)
{
    int* p = NULL;
    byte local = *(byte*)LOCAL_PLAYER;
    p = (int*)(UNITS_SELECTED + 9 * 4 * local);
    if (p)
    {
        p = (int*)(*p);
        if (p)
        {
            byte id = *((byte*)((uintptr_t)p + S_ID));
            byte o = *((byte*)((uintptr_t)p + S_OWNER));
            if (o == local)
            {
                if (id == U_PEASANT)
                {
                    ((void (*)(int))F_BUILD_BUILDING)(U_PIGFARM);//original build func
                }
                if (id == U_PEON)
                {
                    ((void (*)(int))F_BUILD_BUILDING)(U_FARM);//original build func
                }
            }
        }
    }
}

int _2tir() 
{ 
    if ((get_val(TH2, *(byte*)LOCAL_PLAYER) != 0) || (get_val(TH3, *(byte*)LOCAL_PLAYER) != 0))
        return 1;
    else
        return 0;
}

int check_sheep(byte id)
{
    int ns = 0;
    int nt = 0;
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                byte o = *((byte*)((uintptr_t)p + S_OWNER));
                if (o == *(byte*)LOCAL_PLAYER)
                {
                    byte idd = *((byte*)((uintptr_t)p + S_ID));
                    bool f = idd == U_CRITTER;
                    if (f)
                    {
                        if (!check_unit_dead(p))
                        {
                            ns += 1;
                        }
                    }
                    f = (idd == U_TOWN_HALL) || (idd == U_GREAT_HALL) ||
                        (idd == U_STRONGHOLD) || (idd == U_KEEP) ||
                        (idd == U_CASTLE) || (idd == U_FORTRESS);
                    if (f)
                    {
                        nt += 1;
                    }
                    f = (idd == U_FARM) || (idd == U_PIGFARM);
                    if (f)
                    {
                        if (!check_unit_dead(p) && check_unit_complete(p))
                        {
                            if (*((byte*)((uintptr_t)p + S_BUILD_ORDER)) == 0)
                            {
                                if (*((byte*)((uintptr_t)p + S_BUILD_TYPE)) == U_CRITTER)
                                {
                                    if (*((WORD*)((uintptr_t)p + S_BUILD_PROGRES)) != 0)
                                    {
                                        ns += 1;
                                    }
                                }
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
    if (ns < (nt * 5))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void build_sheep(int id)
{
    int a = check_sheep(U_CRITTER);
    if (a != 0)
        ((void (*)(int))F_TRAIN_UNIT)(U_CRITTER);//original build unit func
}

int check_hero(byte id)
{
    byte local = *(byte*)LOCAL_PLAYER;
    for (int i = 0; i < 16; i++)
    {
        if (heros[i] == id)
        {
            if (herosb[i])
            {
                //return false if player already builds that unit
                return 0;
            }
        }
    }
    //substitute for original war2 func that check if player have that unit
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                byte o = *((byte*)((uintptr_t)p + S_OWNER));
                bool ff = o == *(byte*)LOCAL_PLAYER;
                if (ogre_mode)ff = o == ogre_find_player(real_local);
                if (ff)
                {
                    byte idd = *((byte*)((uintptr_t)p + S_ID));
                    bool f = idd == id;
                    if (f)
                    {
                        if (!check_unit_dead(p))
                        {
                           //return false if player already have that unit
                            return 0;
                        }
                    }
                    f = (idd == U_TOWN_HALL) || (idd == U_GREAT_HALL) ||
                        (idd == U_STRONGHOLD) || (idd == U_KEEP) ||
                        (idd == U_CASTLE) || (idd == U_FORTRESS);
                    if (f)
                    {
                        if (!check_unit_dead(p) && check_unit_complete(p))
                        {
                            if (*((byte*)((uintptr_t)p + S_BUILD_ORDER)) == 0)
                            {
                                if (*((byte*)((uintptr_t)p + S_BUILD_TYPE)) == id)
                                {
                                    if (*((WORD*)((uintptr_t)p + S_BUILD_PROGRES)) != 0)
                                    {
                                        //return false if player already building that unit
                                        return 0;
                                    }
                                }
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
    if (war2mod)
    {
        if ((id == U_DANATH) || (id == U_GROM) || (id == U_LOTHAR) || (id == U_KARGATH))
        {
            if (get_val(SMITH, local) == 0)
            {
                return 0;
            }
        }
        if ((id == U_ALLERIA) || (id == U_ZULJIN))
        {
            if (get_val(LUMBERMILL, local) == 0)
            {
                return 0;
            }
        }
        if ((id == U_TYRALYON) || (id == U_DENTARG) || (id == U_UTER) || (id == U_CHOGAL))
        {
            if (get_val(STABLES, local) == 0)
            {
                return 0;
            }
        }
        if ((id == U_HADGAR) || (id == U_TERON))
        {
            bool mt = false;
            for (int i = 0; i < 16; i++)
            {
                int* p = (int*)(UNITS_LISTS + 4 * i);
                if (p)
                {
                    p = (int*)(*p);
                    while (p)
                    {
                        byte id = *((byte*)((uintptr_t)p + S_ID));
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        if ((id == U_MAGE_TOWER) || (id == U_TEMPLE))
                        {
                            if (check_unit_complete(p))
                                if (o == local)mt = true;
                        }
                        p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
                    }
                }
            }
            if (!mt)
            {
                return 0;
            }
        }
        if ((id == U_KURDRAN) || (id == U_DEATHWING))
        {
            if (get_val(AVIARY, local) == 0)
            {
                return 0;
            }
        }
    }
    return 1;
}

void build3(bool b)
{
    //third build button for peons
    if (b)
    {
        char psnt[] = "\x9";
        PATCH_SET((char*)PEASANT_BUTTONS, psnt);
        char peon[] = "\x9\x0\x0\x0\x20\xf7\x48\x0";
        patch_setdword((DWORD*)(peon + 4), (DWORD)peon_build3);
        PATCH_SET((char*)PEON_BUTTONS, peon);

        char p1[] = "\x8\x0\x6d\x0\xf0\x40\x44\x0\x70\xa6\x44\x0\x0\x6d\x38\x1\x0\x0\x0\x0";
        PATCH_SET((char*)PEASANT_RE_BUTTONS, p1);

        if (b3rune)
        {
            patch_setdword((DWORD*)(build_3 + 4), (DWORD)F_ALWAYS_TRUE);
        }
        else
        {
            int (*r) (byte) = empty_false;
            patch_setdword((DWORD*)(build_3 + 4), (DWORD)r);
        }

        if (b3port)
        {
            patch_setdword((DWORD*)(build_3 + 24), (DWORD)F_ALWAYS_TRUE);
        }
        else
        {
            int (*r) (byte) = empty_false;
            patch_setdword((DWORD*)(build_3 + 24), (DWORD)r);
        }

        if (b3cirl)
        {
            patch_setdword((DWORD*)(build_3 + 44), (DWORD)F_ALWAYS_TRUE);
        }
        else
        {
            int (*r) (byte) = empty_false;
            patch_setdword((DWORD*)(build_3 + 44), (DWORD)r);
        }

        if (b3mine)
        {
            patch_setdword((DWORD*)(build_3 + 64), (DWORD)F_ALWAYS_TRUE);
        }
        else
        {
            int (*r) (byte) = empty_false;
            patch_setdword((DWORD*)(build_3 + 64), (DWORD)r);
        }

        if (b3forest)
        {
            patch_setdword((DWORD*)(build_3 + 84), (DWORD)F_ALWAYS_TRUE);
        }
        else
        {
            int (*r) (byte) = empty_false;
            patch_setdword((DWORD*)(build_3 + 84), (DWORD)r);
        }

        void (*r) (int) = build_forest;
        patch_setdword((DWORD*)(build_3 + 88), (DWORD)r);

        char b3[] = "\x6\x0\x0\x0\x0\xf8\x48\x0";
        patch_setdword((DWORD*)(b3 + 4), (DWORD)build_3);
        PATCH_SET((char*)DEAD_BLDG_BUTTONS, b3);
    }
    else
    {
        char psnt[] = "\x8";
        PATCH_SET((char*)PEASANT_BUTTONS, psnt);
        char peon[] = "\x8\x0\x0\x0\x28\x22\x4a\x0";
        PATCH_SET((char*)PEON_BUTTONS, peon);
        char p1[] = "\x0\x0\x54\x0\xf0\x40\x44\x0\x90\x66\x43\x0\x0\x0\x88\x1\x2\x0\x0\x0";
        PATCH_SET((char*)PEASANT_RE_BUTTONS, p1);
        char b3[] = "\x0\x0\x0\x0\x0\x0\x0";
        PATCH_SET((char*)DEAD_BLDG_BUTTONS, b3);
    }
}

void sheep(bool b)
{
    //sheeps from farms
    if (b)
    {
        char farm[] = "\x1\x0\x0\x0\x0\xf7\x48\x0\x1\x0\x0\x0\x0\xf7\x48\x0";
        patch_setdword((DWORD*)(farm + 4), (DWORD)sheep_build);
        patch_setdword((DWORD*)(farm + 12), (DWORD)sheep_build);
        PATCH_SET((char*)FARM_BUTTONS, farm);

        int (*r) (byte) = check_sheep;//sheep check function
        void (*r1) (int) = build_sheep;//sheep build function
        if (war2mod)
        {
            r = check_sheep;
            r1 = build_sheep;
        }
        else
        {
            r = empty_true;
            r1 = empty_build;
        }

        patch_setdword((DWORD*)(sheep_build + 4), (DWORD)r);
        patch_setdword((DWORD*)(sheep_build + 8), (DWORD)r1);
    }
    else
    {
        char farm[] = "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
        PATCH_SET((char*)FARM_BUTTONS, farm);
    }
}

void demon(bool b)
{
    if (b)
    {
        char d[] = "\x1\x0\x0\x0\x0\xf7\x48\x0";
        patch_setdword((DWORD*)(d+4), (DWORD)demon_build);
        PATCH_SET((char*)CIRCLE_BUTTONS, d);

        int (*r) () = _2tir;
        patch_setdword((DWORD*)(demon_build + 4), (DWORD)r);
    }
    else
    {
        char d[] = "\x0\x0\x0\x0\x0\x0\x0\x0";
        PATCH_SET((char*)CIRCLE_BUTTONS, d);
    }
}

byte get_icon(byte id)
{
    switch (id)
    {
    case U_ALLERIA:return 187; break;//alleria
    case U_TERON:return 189; break;//teron
    case U_KURDRAN:return 191; break;//kurdran
    case U_DENTARG:return 194; break;//dentarg
    case U_HADGAR:return 193; break;//hadgar
    case U_GROM:return 190; break;//grom
    case U_DEATHWING:return 192; break;//deathwing
    case U_TYRALYON:return 195; break;//tyralyon
    case U_DANATH:return 188; break;//danat
    case U_KARGATH:return 186; break;//kargat
    case U_CHOGAL:return 36; break;//chogal
    case U_LOTHAR:return 32; break;//lotar
    case U_GULDAN:return 33; break;//guldan
    case U_UTER:return 34; break;//uter
    case U_ZULJIN:return 35; break;//zuljin
    default:break;
    }
    return 113;
}

byte get_tbl(byte id)
{
    switch (id)
    {
    case U_ALLERIA:return '\x10'; break;//alleria
    case U_TERON:return '\x17'; break;//teron
    case U_KURDRAN:return '\x27'; break;//kurdran
    case U_DENTARG:return '\x14'; break;//dentarg
    case U_HADGAR:return '\x19'; break;//hadgar
    case U_GROM:return '\x1a'; break;//grom
    case U_DEATHWING:return '\x28'; break;//deathwing
    case U_TYRALYON:return '\x13'; break;//tyralyon
    case U_DANATH:return '\x1b'; break;//danat
    case U_KARGATH:return '\x22'; break;//kargat
    case U_CHOGAL:return '\x16'; break;//chogal
    case U_LOTHAR:return '\x21'; break;//lotar
    case U_GULDAN:return '\x1f'; break;//guldan
    case U_UTER:return '\x15'; break;//uter
    case U_ZULJIN:return '\xf'; break;//zuljin
    default:break;
    }
    return '\xdf';
}

DWORD CALLBACK hero_thread(LPVOID lpParam)
{
    Sleep(2000);
    for (int i = 0; i < 16; i++)
    {
        herosb[i] = false;
    }
    ExitThread(0);
}

void build_hero(int id)
{
    for (int i = 0; i < 16; i++)
    {
        if ((!herosb[i]) && (heros[i] == (id % 256)))
        {
            herosb[i] = true;
            DWORD dw = 0;
            CreateThread(NULL, 0, hero_thread, &dw, 0, NULL);
        }
    }
    ((void (*)(int))F_TRAIN_UNIT)(id);//original build func
    ((void (*)(int))F_CHANGE_STATUS)(U_EMPTY_BUTTONS);//change status
    ((void (*)())F_STATUS_REDRAW)();//status redraw
}

void heroes(bool b)
{
    if (b)
    {
        for (int i = 1; i < 20; i++)
        {
            hhero[i] = build_3[100 + i];
            ohero[i] = build_3[100 + i];
        }
        byte local = *(byte*)LOCAL_PLAYER;
        int l = 0;
        l += local;
        byte mx = 2;
        if (get_val(TH2, l) != 0)mx = 5;
        if (get_val(TH3, l) != 0)mx = 8;
        byte k = 1;
        for (byte i = 1; i < 9; i++)
        {
            byte id = heros[i - 1];
            if (id == 0) id = U_CRITTER;
            if (id != 0)
            {
                k++;
                byte ico = get_icon(id);
                hhero[20 * i + 0] = i;//button id
                hhero[20 * i + 1] = '\x0';//button id?
                hhero[20 * i + 2] = ico;//icon
                hhero[20 * i + 3] = '\x0';//icon

                int (*r) (byte) = check_hero;//hero check function
                if ((i > mx) || (id == U_CRITTER))r = empty_false;
                patch_setdword((DWORD*)(hhero + (20 * i + 4)), (DWORD)r);

                void (*r1) (int) = build_hero;//hero build function
                patch_setdword((DWORD*)(hhero + (20 * i + 8)), (DWORD)r1);

                hhero[20 * i + 12] = id;//arg
                hhero[20 * i + 13] = id;//unit id
                hhero[20 * i + 14] = get_tbl(id);//string from tbl
                hhero[20 * i + 15] = '\x1';//string from tbl
                hhero[20 * i + 16] = '\x0';//flags?
                hhero[20 * i + 17] = '\x0';//flags?
                hhero[20 * i + 18] = '\x0';//flags?
                hhero[20 * i + 19] = '\x0';//flags?
            }
            else i = 10;
        }
        char b1[] = "\x0\x0\x0\x0\x0\xf8\x48\x0";
        b1[0] = k;
        patch_setdword((DWORD*)(b1+4), (DWORD)hhero);
        PATCH_SET((char*)DEAD_BLDG2_BUTTONS, b1);
        k = 1;
        for (byte i = 1; i < 9; i++)
        {
            byte id = heros[i + 8 - 1];
            if (id == 0) id = U_CRITTER;
            if (id != 0)
            {
                k++;
                byte ico = get_icon(id);
                ohero[20 * i + 0] = i;//button id
                ohero[20 * i + 1] = '\x0';//button id?
                ohero[20 * i + 2] = ico;//icon
                ohero[20 * i + 3] = '\x0';//icon

                int (*r) (byte) = check_hero;//hero check function
                if ((i > mx) || (id == U_CRITTER))r = empty_false;
                patch_setdword((DWORD*)(ohero + (20 * i + 4)), (DWORD)r);

                void (*r1) (int) = build_hero;//hero build function
                patch_setdword((DWORD*)(ohero + (20 * i + 8)), (DWORD)r1);

                ohero[20 * i + 12] = id;//arg
                ohero[20 * i + 13] = id;//unit id
                ohero[20 * i + 14] = get_tbl(id);//string from tbl
                ohero[20 * i + 15] = '\x1';//string from tbl
                ohero[20 * i + 16] = '\x0';//flags?
                ohero[20 * i + 17] = '\x0';//flags?
                ohero[20 * i + 18] = '\x0';//flags?
                ohero[20 * i + 19] = '\x0';//flags?
            }
            else i = 10;
        }
        char b2[] = "\x0\x0\x0\x0\x0\xf8\x48\x0";
        b2[0] = k;
        patch_setdword((DWORD*)(b2 + 4), (DWORD)ohero);
        PATCH_SET((char*)DEAD_BLDG3_BUTTONS, b2);
    }
    else
    {
        char buf[] = "\x0\x0\x0\x0\x0\x0\x0";
        PATCH_SET((char*)DEAD_BLDG2_BUTTONS, buf);
        PATCH_SET((char*)DEAD_BLDG3_BUTTONS, buf);
    }
}

void repair_all(bool b)
{
    //peon can repair all units
    if (b)
    {
        char bau[] = "\xeb";//0x75
        PATCH_SET((char*)REPAIR_FLAG_CHECK, bau);
    }
    else
    {
        char bau[] = "\x75";
        PATCH_SET((char*)REPAIR_FLAG_CHECK, bau);
    }
}

void repair_cat(bool b)
{
    //peon can repair unit if it have transport flag OR catapult flag
    if (b)
    {
        char r1[] = "\xeb\x75\x90\x90\x90";//f6 c4 04 74 14
        PATCH_SET((char*)REPAIR_FLAG_CHECK2, r1);
        char r2[] = "\x66\xa9\x04\x04\x74\x9c\xeb\x86";
        PATCH_SET((char*)REPAIR_CODE_CAVE, r2);
    }
    else
    {
        char r1[] = "\xf6\xc4\x4\x74\x14";
        PATCH_SET((char*)REPAIR_FLAG_CHECK2, r1);
    }
}

void fireball_dmg(byte dmg)
{
    char fb[] = "\x28";//40 default
    fb[0] = dmg;
    PATCH_SET((char*)FIREBALL_DMG, fb);
}

void buff_time(byte bf, byte t1, byte t2)
{
    char tm[] = "\x0\x0";
    switch (bf)
    {
    case 0:
        tm[0] = t1;
        tm[1] = t2;
        PATCH_SET((char*)INVIZ_TIME, tm);
        break;
    case 1:
        tm[0] = t1;
        tm[1] = t2;
        PATCH_SET((char*)BLOOD_TIME, tm);
        break;
    case 2:
        tm[0] = t1;
        tm[1] = t2;
        PATCH_SET((char*)SHIELD_TIME, tm);
        break;
    case 3:
        tm[0] = t1;
        tm[1] = t2;
        PATCH_SET((char*)HASTE_TIME1, tm);
        PATCH_SET((char*)HASTE_TIME2, tm);
        break;
    case 4:
        int tt = 0xffff - (t1 + 256 * t2);
        tm[0] = tt % 256;
        tm[1] = tt / 256;
        PATCH_SET((char*)SLOW_TIME1, tm);
        PATCH_SET((char*)SLOW_TIME2, tm);
        break;
    }
}

void fireshield_flyers(bool b)
{
    if (b)
    {
        char bau[] = "\x90\x90";//75 51
        PATCH_SET((char*)FIRESHIELD_FLYERS, bau);
    }
    else
    {
        char bau[] = "\x75\x51";
        PATCH_SET((char*)FIRESHIELD_FLYERS, bau);
    }
}

void trigger_time(byte tm)
{
    //war2 will call victory check function every 200 game ticks
    char ttime[] = "\xc8";//200 default
    ttime[0] = tm;
    PATCH_SET((char*)TRIG_TIME, ttime);
}

void manacost(byte id, byte c)
{
    //spells cost of mana
    char mana[] = "\x1";
    mana[0] = c;
    PATCH_SET((char*)(MANACOST + 2 * id), mana);
}

void upgr(byte id, byte c)
{
    //upgrades power
    char up[] = "\x1";
    up[0] = c;
    PATCH_SET((char*)(UPGRD + id), up);
}

byte get_upgrade(byte id,byte pl)
{
    int a = GB_ARROWS;
    switch (id)
    {
    case ARROWS:a = GB_ARROWS; break;
    case SWORDS:a = GB_SWORDS; break;
    case ARMOR:a = GB_SHIELDS; break;
    case SHIP_DMG:a = GB_BOAT_ATTACK; break;
    case SHIP_ARMOR:a = GB_BOAT_ARMOR; break;
    case SHIP_SPEED:a = GB_BOAT_SPEED; break;
    case CATA_DMG:a = GB_CAT_DMG; break;
    default:a = GB_ARROWS; break;
    }
    return *(byte*)(a + pl);
}

void set_upgrade(byte id, byte pl, byte v)
{
    int a = GB_ARROWS;
    switch (id)
    {
    case ARROWS:a = GB_ARROWS; break;
    case SWORDS:a = GB_SWORDS; break;
    case ARMOR:a = GB_SHIELDS; break;
    case SHIP_DMG:a = GB_BOAT_ATTACK; break;
    case SHIP_ARMOR:a = GB_BOAT_ARMOR; break;
    case SHIP_SPEED:a = GB_BOAT_SPEED; break;
    case CATA_DMG:a = GB_CAT_DMG; break;
    default:a = GB_ARROWS; break;
    }
    char buf[] = "\x0";
    buf[0] = v;
    PATCH_SET((char*)(a+pl),buf);
    ((void (*)())F_STATUS_REDRAW)();//status redraw
}

int upgr_check_swords(byte b)
{
    byte u = get_upgrade(SWORDS, *(byte*)LOCAL_PLAYER);
    if ((b == 0) && (u == 0))return 1;
    if ((b == 1) && (u == 1))return 1;
    if ((b == 2) && (u >= 2))return 1;
    return 0;
}

int upgr_check_shields(byte b)
{
    byte u = get_upgrade(ARMOR, *(byte*)LOCAL_PLAYER);
    if ((b == 0) && (u == 0))return 1;
    if ((b == 1) && (u == 1))return 1;
    if ((b == 2) && (u >= 2))return 1;
    return 0;
}

int upgr_check_boat_attack(byte b)
{
    byte u = get_upgrade(SHIP_DMG, *(byte*)LOCAL_PLAYER);
    if ((b == 0) && (u == 0))return 1;
    if ((b == 1) && (u == 1))return 1;
    if ((b == 2) && (u >= 2))return 1;
    return 0;
}

int upgr_check_boat_armor(byte b)
{
    byte u = get_upgrade(SHIP_ARMOR, *(byte*)LOCAL_PLAYER);
    if ((b == 0) && (u == 0))return 1;
    if ((b == 1) && (u == 1))return 1;
    if ((b == 2) && (u >= 2))return 1;
    return 0;
}

int upgr_check_arrows(byte b)
{
    byte u = get_upgrade(ARROWS, *(byte*)LOCAL_PLAYER);
    if ((b == 0) && (u == 0))return 1;
    if ((b == 1) && (u == 1))return 1;
    if ((b == 2) && (u >= 2))return 1;
    return 0;
}

void upgr_check_replace(bool f)
{
    if (f)
    {
        char buf[] = "\xC3";//ret
        patch_ljmp((char*)UPGR_CHECK_FIX_SWORDS, (char*)upgr_check_swords);
        PATCH_SET((char*)(UPGR_CHECK_FIX_SWORDS + 5), buf);
        patch_ljmp((char*)UPGR_CHECK_FIX_SHIELDS, (char*)upgr_check_shields);
        PATCH_SET((char*)(UPGR_CHECK_FIX_SHIELDS + 5), buf);
        patch_ljmp((char*)UPGR_CHECK_FIX_SHIPS_AT, (char*)upgr_check_boat_attack);
        PATCH_SET((char*)(UPGR_CHECK_FIX_SHIPS_AT + 5), buf);
        patch_ljmp((char*)UPGR_CHECK_FIX_SHIPS_DEF, (char*)upgr_check_boat_armor);
        PATCH_SET((char*)(UPGR_CHECK_FIX_SHIPS_DEF + 5), buf);
        patch_ljmp((char*)UPGR_CHECK_FIX_ARROWS, (char*)upgr_check_arrows);
        PATCH_SET((char*)(UPGR_CHECK_FIX_ARROWS + 5), buf);
    }
    else
    {
        char buf2[] = "\x33\xC0\x33\xC9\xA0\x18";//back
        PATCH_SET((char*)UPGR_CHECK_FIX_SWORDS, buf2);
        PATCH_SET((char*)UPGR_CHECK_FIX_SHIELDS, buf2);
        PATCH_SET((char*)UPGR_CHECK_FIX_SHIPS_AT, buf2);
        PATCH_SET((char*)UPGR_CHECK_FIX_SHIPS_DEF, buf2);
        PATCH_SET((char*)UPGR_CHECK_FIX_ARROWS, buf2);
    }
}

void no_random_dmg(bool b)
{
    if (b)
    {
        char b1[] = "\x1a";
        PATCH_SET((char*)DAMAGE_AREA1, b1);
        char b2[] = "\x19";
        PATCH_SET((char*)DAMAGE_AREA2, b2);
        char b3[] = "\x90\x90";
        PATCH_SET((char*)DAMAGE_AREA3, b3);
        PATCH_SET((char*)DAMAGE_AREA4, b3);
        char b4[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)DAMAGE_UNIT_VS_UNIT, b4);
    }
    else
    {
        char b1[] = "\x6";
        PATCH_SET((char*)DAMAGE_AREA1, b1);
        char b2[] = "\x5";
        PATCH_SET((char*)DAMAGE_AREA2, b2);
        char b3[] = "\xf7\xf1";
        PATCH_SET((char*)DAMAGE_AREA3, b3);
        PATCH_SET((char*)DAMAGE_AREA4, b3);
        char b4[] = "\x8d\x47\x1\x99\x2b\xc2\x8b\xf8\xd1\xff\xe8\x8d\x91\x6\x0\x8d\x4f\x1\x33\xd2\xf7\xf1\x3\xfa";
        PATCH_SET((char*)DAMAGE_UNIT_VS_UNIT, b4);
    }
}

void blood_fix(bool b)
{
    if (b)
    {
        blood_f = true;
        char b1[] = "\x90\x90";
        PATCH_SET((char*)BLOOD_CALC1, b1);
        char b2[] = "\x90\x90";
        PATCH_SET((char*)BLOOD_CALC2, b2);
    }
    else
    {
        blood_f = false;
        char b1[] = "\x3\xf6";
        PATCH_SET((char*)BLOOD_CALC1, b1);
        char b2[] = "\x3\xc0";
        PATCH_SET((char*)BLOOD_CALC2, b2);
    }
}

void center_view(byte x, byte y)
{
    ((void (*)(byte, byte))F_MINIMAP_CLICK)(x, y);//original war2 func that called when player click on minimap
}

void speedhack(byte speed)
{
    //change game speed koef of fastest
    char buf[] = "\xd";//13 is default koef on fastest
    buf[0] = speed;//smaller = faster (1 = ultra fast)(0 = processor blow up)
    PATCH_SET((char*)FASTEST_SPEED, buf);
}

void comp_aggro(int* trg, int* atk)
{
    byte own1 = *((byte*)((uintptr_t)trg + S_OWNER));
    byte own2 = *((byte*)((uintptr_t)atk + S_OWNER));
    if (own1 != own2)
    {
        if (check_ally(own1, own2))//if we allies
        {
            byte o1 = *(byte*)(CONTROLER_TYPE + own1);
            byte o2 = *(byte*)(CONTROLER_TYPE + own2);
            if ((o1 == C_COMP) && (o2 == C_PLAYER))//if target unit is comp and attacker is real player
            {
                ally(own1, own2, 0);
                viz(own1, own2, 0);
                //turn off ally and viz
            }
        }
    }
}

bool capture(int* trg, int* atk)
{
    byte own1 = *((byte*)((uintptr_t)trg + S_OWNER));
    byte own2 = *((byte*)((uintptr_t)atk + S_OWNER));
    if (own1 != own2)
    {
        if (!pcpt || ((*((byte*)((uintptr_t)atk + S_ID)) == U_PEASANT) || (*((byte*)((uintptr_t)atk + S_ID)) == U_PEON)))
        {//if only peons can capture and attacker is peon(or peasant)
            if (cmp_stat(trg, U_FARM, S_ID, CMP_BIGGER_EQ))//only buildings (id >= farm)
            {
                if (cpt)
                {
                    if (check_unit_complete(trg))//completed buildings
                    {
                        byte tid = *((byte*)((uintptr_t)trg + S_ID));//unit id
                        WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * tid);//max hp
                        WORD hp = *((WORD*)((uintptr_t)trg + S_HP));//unit hp
                        if ((float)hp <= (((float)mhp / 100.0 * 5) + 1))//if hp<=5%
                        {
                            WORD hl = (WORD)(((float)mhp / 100.0 * 5) + 1);
                            heal(trg, hl % 256, hl / 256);//heal 5% hp so that it will not die suddenly
                            if (thcpt)//if th captured, capture all
                            {
                                bool mf = (tid == U_TOWN_HALL) || (tid == U_GREAT_HALL);
                                if (get_val(TH2, (int)own1) != 0)mf = (tid == U_KEEP) || (tid == U_STRONGHOLD);
                                if (get_val(TH3, (int)own1) != 0)mf = (tid == U_CASTLE) || (tid == U_FORTRESS);
                                if (mf)
                                {//THs of 1 2 and 3 tier
                                    captk = 0;
                                    for (int i = 0; i < 16; i++)
                                    {
                                        int* p = (int*)(UNITS_LISTS + 4 * i);
                                        if (p)
                                        {
                                            p = (int*)(*p);
                                            while (p)
                                            {
                                                if (!check_unit_dead(p))
                                                {
                                                    if (cmp_stat(p, (int)own1, S_OWNER, CMP_EQ))//capture all units of that player
                                                    {
                                                        capt[captk] = p;
                                                        captk++;
                                                    }
                                                }
                                                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
                                            }
                                        }
                                    }
                                    for (int i = 0; i < captk; i++)
                                        give(capt[i], own2);
                                }
                            }
                            give(trg, own2);
                            return true;
                        }
                    }
                }
            }
            else//units capture
            {
                if (ucpt)
                {
                    if (!check_unit_dead(trg))
                    {
                        byte tid = *((byte*)((uintptr_t)trg + S_ID));//unit id
                        WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * tid);//max hp
                        WORD hp = *((WORD*)((uintptr_t)trg + S_HP));//unit hp
                        if ((float)hp <= (((float)mhp / 100.0 * 5) + 1))//if hp<=5%
                        {
                            WORD hl = (WORD)(((float)mhp / 100.0 * 50) + 1);
                            heal(trg, hl % 256, hl / 256);//heal 50% hp so that it will not die suddenly
                            give(trg, own2);
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void steal_res(int* trg, int* atk)
{
    byte own1 = *((byte*)((uintptr_t)trg + S_OWNER));
    byte own2 = *((byte*)((uintptr_t)atk + S_OWNER));
    if ((own1 != own2) && !check_ally(own1, own2))
    {
        if (cmp_stat(trg, U_FARM, S_ID, CMP_BIGGER_EQ))//only buildings (id >= farm)
        {
            if (check_unit_complete(trg))//completed buildings
            {
                if (cmp_stat(atk, U_FARM, S_ID, CMP_SMALLER))//only units (not towers)
                {
                    byte aid = *((byte*)((uintptr_t)atk + S_ID));
                    byte ar = *(byte*)(UNIT_RANGE_TABLE + aid);
                    if (ar < 2)
                    {
                        if (cmp_res(own1, 0, 2, 0, 0, 0, CMP_BIGGER_EQ))//2 gold
                        {
                            change_res(own1, 3, 2, 1);
                            change_res(own2, 0, 2, 1);
                        }
                        if (cmp_res(own1, 1, 1, 0, 0, 0, CMP_BIGGER_EQ))//1 lumber
                        {
                            change_res(own1, 4, 1, 1);
                            change_res(own2, 1, 1, 1);
                        }
                    }
                }
            }
        }
    }
}

void mana_burn(int* trg, int* atk)
{
    byte aid = *((byte*)((uintptr_t)atk + S_ID));
    if (aid == U_DEMON)
    {
        byte tid = *((byte*)((uintptr_t)trg + S_ID));
        bool f = (tid == U_MAGE) || (tid == U_DK) || (tid == U_PALADIN) || (tid == U_OGREMAGE) ||
            (tid == U_HADGAR) || (tid == U_TERON) || (tid == U_GULDAN) ||
            (tid == U_UTER) || (tid == U_TYRALYON) || (tid == U_CHOGAL) || (tid == U_DENTARG);
        if (f)
        {
            byte mp = *((byte*)((uintptr_t)trg + S_MANA));
            byte dmp = mp;
            if (dmp > 30)dmp = 30;
            mp -= dmp;
            set_stat(trg, mp, S_MANA);
            WORD hp = *((WORD*)((uintptr_t)trg + S_HP));
            if (hp != 0)
            {
                if (hp > dmp)hp -= dmp;
                else hp = 1;
            }
            set_stat(trg, hp, S_HP);
        }
    }
}

bool vamp_aura(int* trg, int* atk, byte id)
{
    byte tid = *((byte*)((uintptr_t)trg + S_ID));//unit id
    if ((*(int*)(UNIT_GLOBAL_FLAGS + tid * 4) & 0x8000000) != 0)//fleshy global flag
    {
        byte aid = *((byte*)((uintptr_t)atk + S_ID));//unit id
        if ((*(int*)(UNIT_GLOBAL_FLAGS + aid * 4) & 0x8000000) != 0)//fleshy global flag
        {
            byte x = *((byte*)((uintptr_t)atk + S_X));
            byte y = *((byte*)((uintptr_t)atk + S_Y));
            set_region((int)x - 5, (int)y - 5, (int)x + 5, (int)y + 5);//set region around myself
            find_all_alive_units(ANY_MEN);
            sort_in_region();
            sort_stat(S_ID, id, CMP_EQ);
            byte o = *((byte*)((uintptr_t)atk + S_OWNER));
            for (int ui = 0; ui < 16; ui++)
            {
                if (!check_ally(o, ui))
                    sort_stat(S_OWNER, ui, CMP_NEQ);
            }
            return units != 0;
        }
    }
    return false;
}

bool devotion_aura(int* trg, byte id)
{
    byte tid = *((byte*)((uintptr_t)trg + S_ID));//unit id
    if (tid < U_FARM)//not buildings
    {
        byte x = *((byte*)((uintptr_t)trg + S_X));
        byte y = *((byte*)((uintptr_t)trg + S_Y));
        set_region((int)x - 5, (int)y - 5, (int)x + 5, (int)y + 5);//set region around myself
        find_all_alive_units(id);
        sort_in_region();
        byte o = *((byte*)((uintptr_t)trg + S_OWNER));
        for (int ui = 0; ui < 16; ui++)
        {
            if (!check_ally(o, ui))
                sort_stat(S_OWNER, ui, CMP_NEQ);
        }
        return units != 0;
    }
    else
        return false;
}

void peon_steal_attack(int* trg, int* atk)
{
    byte tid = *((byte*)((uintptr_t)trg + S_ID));
    byte aid = *((byte*)((uintptr_t)atk + S_ID));
    if ((aid == U_PEASANT) || (aid == U_PEON))
    {
        if ((tid == U_PEASANT) || (tid == U_PEON))
        {
            byte tf = *((byte*)((uintptr_t)trg + S_PEON_FLAGS));
            byte af = *((byte*)((uintptr_t)atk + S_PEON_FLAGS));
            if (!(af & PEON_LOADED))
            {
                if ((tf & PEON_LOADED))
                {
                    tf &= ~PEON_LOADED;
                    af |= PEON_LOADED;
                    if ((tf & PEON_HARVEST_GOLD))
                    {
                        tf &= ~PEON_HARVEST_GOLD;
                        af |= PEON_HARVEST_GOLD;
                    }
                    if ((tf & PEON_HARVEST_LUMBER))
                    {
                        tf &= ~PEON_HARVEST_LUMBER;
                        af |= PEON_HARVEST_LUMBER;
                    }
                    set_stat(trg, tf, S_PEON_FLAGS);
                    set_stat(atk, af, S_PEON_FLAGS);
                    ((void (*)(int*))F_GROUP_SET)(trg);
                    ((void (*)(int*))F_GROUP_SET)(atk);
                }
            }
        }
    }
}

PROC g_proc_00451054;
void count_add_to_tables_load_game(int* u)
{
    if (saveload_fixed)
    {
        byte f = *((byte*)((uintptr_t)u + S_AI_AIFLAGS));
        byte ff = f | AI_PASSIVE;
        set_stat(u, ff, S_AI_AIFLAGS);
        ((void (*)(int*))g_proc_00451054)(u);//original
        set_stat(u, f, S_AI_AIFLAGS);
    }
    else
        ((void (*)(int*))g_proc_00451054)(u);//original
}

PROC g_proc_00438A5C;
PROC g_proc_00438985;
void unset_peon_ai_flags(int* u)
{
    ((void (*)(int*))g_proc_00438A5C)(u);//original
    if (saveload_fixed)
    {
        char rep[] = "\x0\x0";
        WORD p = 0;
        for (int i = 0; i < 8; i++)
        {
            p = *((WORD*)((uintptr_t)SGW_REPAIR_PEONS + 2 * i));
            if (p > 1600)
                PATCH_SET((char*)(SGW_REPAIR_PEONS + 2 * i), rep);
            p = *((WORD*)((uintptr_t)SGW_GOLD_PEONS + 2 * i));
            if (p > 1600)
                PATCH_SET((char*)(SGW_GOLD_PEONS + 2 * i), rep);
            p = *((WORD*)((uintptr_t)SGW_TREE_PEONS + 2 * i));
            if (p > 1600)
                PATCH_SET((char*)(SGW_TREE_PEONS + 2 * i), rep);
        }
    }
}

void tech_built(int p, byte t)
{
    ((void (*)(int, byte))F_TECH_BUILT)(p, t);
}

void tech_reinit()
{
    for (int i = 0; i < 8; i++)
    {
        byte o = *(byte*)(CONTROLER_TYPE + i);
        byte a = 0;
        int s = 0;
        if (o == C_COMP)
        {
            a = *(byte*)(GB_ARROWS + i);
            if (a > 0)tech_built(i, UP_ARROW1);
            if (a > 1)tech_built(i, UP_ARROW2);
            a = *(byte*)(GB_SWORDS + i);
            if (a > 0)tech_built(i, UP_SWORD1);
            if (a > 1)tech_built(i, UP_SWORD2);
            a = *(byte*)(GB_SHIELDS + i);
            if (a > 0)tech_built(i, UP_SHIELD1);
            if (a > 1)tech_built(i, UP_SHIELD2);
            a = *(byte*)(GB_BOAT_ATTACK + i);
            if (a > 0)tech_built(i, UP_BOATATK1);
            if (a > 1)tech_built(i, UP_BOATATK2);
            a = *(byte*)(GB_BOAT_ARMOR + i);
            if (a > 0)tech_built(i, UP_BOATARM1);
            if (a > 1)tech_built(i, UP_BOATARM2);
            a = *(byte*)(GB_CAT_DMG + i);
            if (a > 0)tech_built(i, UP_CATDMG1);
            if (a > 1)tech_built(i, UP_CATDMG2);
            a = *(byte*)(GB_RANGER + i);
            if (a)tech_built(i, UP_RANGER);
            a = *(byte*)(GB_MARKS + i);
            if (a)tech_built(i, UP_SKILL1);
            a = *(byte*)(GB_LONGBOW + i);
            if (a)tech_built(i, UP_SKILL2);
            a = *(byte*)(GB_SCOUTING + i);
            if (a)tech_built(i, UP_SKILL3);

            s = *(int*)(SPELLS_LEARNED + 4 * i);
            if (s & (1 << L_ALTAR_UPGR))tech_built(i, UP_CLERIC);
            if (s & (1 << L_HEAL))tech_built(i, UP_CLERIC1);
            if (s & (1 << L_BLOOD))tech_built(i, UP_CLERIC1);
            if (s & (1 << L_EXORCISM))tech_built(i, UP_CLERIC2);
            if (s & (1 << L_RUNES))tech_built(i, UP_CLERIC2);
            if (s & (1 << L_FLAME_SHIELD))tech_built(i, UP_WIZARD1);
            if (s & (1 << L_RAISE))tech_built(i, UP_WIZARD1);
            if (s & (1 << L_SLOW))tech_built(i, UP_WIZARD2);
            if (s & (1 << L_HASTE))tech_built(i, UP_WIZARD2);
            if (s & (1 << L_INVIS))tech_built(i, UP_WIZARD3);
            if (s & (1 << L_WIND))tech_built(i, UP_WIZARD3);
            if (s & (1 << L_POLYMORF))tech_built(i, UP_WIZARD4);
            if (s & (1 << L_UNHOLY))tech_built(i, UP_WIZARD4);
            if (s & (1 << L_BLIZZARD))tech_built(i, UP_WIZARD5);
            if (s & (1 << L_DD))tech_built(i, UP_WIZARD5);

            find_all_alive_units(U_KEEP);
            sort_stat(S_OWNER, i, CMP_EQ);
            if (units != 0)tech_built(i, UP_KEEP);
            find_all_alive_units(U_STRONGHOLD);
            sort_stat(S_OWNER, i, CMP_EQ);
            if (units != 0)tech_built(i, UP_KEEP);
            find_all_alive_units(U_CASTLE);
            sort_stat(S_OWNER, i, CMP_EQ);
            if (units != 0)
            {
                tech_built(i, UP_KEEP);
                tech_built(i, UP_CASTLE);
            }
            find_all_alive_units(U_FORTRESS);
            sort_stat(S_OWNER, i, CMP_EQ);
            if (units != 0)
            {
                tech_built(i, UP_KEEP);
                tech_built(i, UP_CASTLE);
            }
        }
    }
}

void building_start_build(int* u,byte id,byte o)
{
    ((void (*)(int*, byte, byte))F_BLDG_START_BUILD)(u, id, o);
}

void build_inventor(int* u)
{
    if (check_unit_complete(u))
    {
        byte f = *((byte*)((uintptr_t)u + S_FLAGS1));
        if (!(f & UF_BUILD_ON))
        {
            byte id = *((byte*)((uintptr_t)u + S_ID));
            byte o = *((byte*)((uintptr_t)u + S_OWNER));
            int spr = get_val(ACTIVE_SAPPERS, o);
            byte nspr = *(byte*)(AIP_NEED_SAP + 48 * o);
            if (nspr > spr)
            {
                if (id == U_INVENTOR)building_start_build(u, U_DWARWES, 0);
                if (id == U_ALCHEMIST)building_start_build(u, U_GOBLINS, 0);
            }
            int flr = get_val(ACTIVE_FLYER, o);
            byte nflr = *(byte*)(AIP_NEED_FLYER + 48 * o);
            if (nflr > flr)
            {
                if (id == U_INVENTOR)building_start_build(u, U_FLYER, 0);
                if (id == U_ALCHEMIST)building_start_build(u, U_ZEPPELIN, 0);
            }
        }
    }
}

void build_sap_fix(bool f)
{
    if (f)
    {
        char b1[] = "\x80\xfa\x40\x0";
        void (*r1) (int*) = build_inventor;
        patch_setdword((DWORD*)b1, (DWORD)r1);
        PATCH_SET((char*)BLDG_WAIT_INVENTOR, b1);//human inv
        PATCH_SET((char*)(BLDG_WAIT_INVENTOR + 4), b1);//orc inv
    }
    else
    {
        char b1[] = "\x80\xfa\x40\x0";
        PATCH_SET((char*)BLDG_WAIT_INVENTOR, b1);//human inv
        PATCH_SET((char*)(BLDG_WAIT_INVENTOR + 4), b1);//orc inv
    }
}

void ai_fix_plugin(bool f)
{
    if (f)
    {
        char b1[] = "\xb2\x02";
        PATCH_SET((char*)AIFIX_PEONS_REP, b1);//2 peon rep
        char b21[] = "\xbb\x8";
        PATCH_SET((char*)AIFIX_GOLD_LUMB1, b21);//gold lumber
        char b22[] = "\xb4\x4";
        PATCH_SET((char*)AIFIX_GOLD_LUMB2, b22);//gold lumber
        char b3[] = "\x1";
        PATCH_SET((char*)AIFIX_BUILD_SIZE, b3);//packed build
        char b4[] = "\xbe\x0\x0\x0\x0\x90\x90";
        PATCH_SET((char*)AIFIX_FIND_HOME, b4);//th corner
        char b5[] = "\x90\x90";
        PATCH_SET((char*)AIFIX_DD_BLIZ_FIX, b5);//fix dd/bliz
        char b6[] = "\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)AIFIX_POWERBUILD, b6);//powerbuild
        char b7[] = "\x90\x90\x90\x90";
        PATCH_SET((char*)AIFIX_CATA_AFRAID, b7);//cata afraid
        char b8[] = "\x70\x79";
        PATCH_SET((char*)AIFIX_SHIPS_PATROL, b8);//ships random patrol

        char m1[] = "\xa9\x0\x0\x0\x4\x74\x16\x8b\x44\x24\xc\x66\x83\x78\x44\x0\x75\xb\x90\x90\x90\x90";
        PATCH_SET((char*)AIFIX_INVIZ_COND, m1);//inviz cond
        char m2[] = "\x90\x90\x90";
        PATCH_SET((char*)AIFIX_BLIZ_3MP1, m2);//no x3 bliz mp
        PATCH_SET((char*)AIFIX_BLIZ_3MP2, m2);//no x3 bliz mp
        char m3[] = "\x33\xd2\x8a\x50\x27\x3e\x8b\x4\x95\x24\xf5\x4c\x0\xa8\x20\x75\x19\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)AIFIX_FIREBALL_COND, m3);//fire cond
        char m4[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)AIFIX_BLIZ_COND, m4);//bliz cond
        char m5[] = "\x96";
        PATCH_SET((char*)AIFIX_INV_POLY_JMP, m5);//no inv poly > jmp exit
        char m6[] = "\xe9\x6c\x2\x0\x0";
        PATCH_SET((char*)AIFIX_INV_POLY_CAVE, m6);//jmp to exit
        char m7[] = "\x90\x90";
        PATCH_SET((char*)AIFIX_RUNES_INV, m7);//runes no inviz
        char m8[] = "\xeb";
        PATCH_SET((char*)AIFIX_STARTING_MAGE, m8);//starting mage

        char cw[] = "\x11";
        PATCH_SET((char*)COMPS_WALLS_UNPASSABLE, cw);//comps break walls

        build_sap_fix(true);

        ai_fixed = true;
    }
    else
    {
        char b1[] = "\x8a\xd0";
        PATCH_SET((char*)AIFIX_PEONS_REP, b1);//2 peon rep
        char b21[] = "\xd0\x7";
        PATCH_SET((char*)AIFIX_GOLD_LUMB1, b21);//gold lumber
        char b22[] = "\xf4\x1";
        PATCH_SET((char*)AIFIX_GOLD_LUMB2, b22);//gold lumber
        char b3[] = "\x6";
        PATCH_SET((char*)AIFIX_BUILD_SIZE, b3);//packed build
        char b4[] = "\xe8\xf8\x2a\x1\x0\x8b\xf0";
        PATCH_SET((char*)AIFIX_FIND_HOME, b4);//th corner
        char b5[] = "\x75\x7";
        PATCH_SET((char*)AIFIX_DD_BLIZ_FIX, b5);//fix dd/bliz
        char b6[] = "\xf\x84\x78\x1\x0\x0";
        PATCH_SET((char*)AIFIX_POWERBUILD, b6);//powerbuild
        char b7[] = "\x89\x44\x24\x10";
        PATCH_SET((char*)AIFIX_CATA_AFRAID, b7);//cata afraid
        char b8[] = "\x10\x7A";
        PATCH_SET((char*)AIFIX_SHIPS_PATROL, b8);//ships random patrol

        char m1[] = "\x80\x7a\x5e\x2\x75\x17\xa9\x0\x0\x2\x0\x75\x9\x80\xb9\xf0\xfe\x4c\x0\x1\x74\x7";
        PATCH_SET((char*)AIFIX_INVIZ_COND, m1);//inviz cond
        char m2[] = "\x8d\x4\x40";
        PATCH_SET((char*)AIFIX_BLIZ_3MP1, m2);//no x3 bliz mp
        m2[1] = '\x14';
        PATCH_SET((char*)AIFIX_BLIZ_3MP2, m2);//no x3 bliz mp
        char m3[] = "\x6a\x3\x50\x56\xe8\x3\xff\xff\xff\x83\xc4\xc\x85\xc0\x74\x12\x56\xe8\xe6\xd8\x2\x0\x66\xd1\xe8\x83\xc4\x4\x66\x39\x46\x22\x73\x8";
        PATCH_SET((char*)AIFIX_FIREBALL_COND, m3);//fire cond
        char m4[] = "\x6a\x3\x50\x51\xe8\x96\xfe\xff\xff\x83\xc4\xc\x85\xc0\x75\x07";
        PATCH_SET((char*)AIFIX_BLIZ_COND, m4);//bliz cond
        char m5[] = "\xc";
        PATCH_SET((char*)AIFIX_INV_POLY_JMP, m5);//no inv poly > jmp exit
        char m6[] = "\x90\x90\x90\x90\x90";
        PATCH_SET((char*)AIFIX_INV_POLY_CAVE, m6);//jmp to exit
        char m7[] = "\x74\x1d";
        PATCH_SET((char*)AIFIX_RUNES_INV, m7);//runes no inviz
        char m8[] = "\x74";
        PATCH_SET((char*)AIFIX_STARTING_MAGE, m8);//starting mage

        char cw[] = "\x91";
        PATCH_SET((char*)COMPS_WALLS_UNPASSABLE, cw);//comps break walls

        build_sap_fix(false);

        ai_fixed = false;
    }
}

PROC g_proc_0040EEDD;
void upgrade_tower(int* u, int id, int b)
{
    if (ai_fixed)
    {
        bool c = false;
        byte o = *((byte*)((uintptr_t)u + S_OWNER));
        if ((get_val(LUMBERMILL, o) == 0) && (get_val(SMITH, o) != 0)) c = true;
        if ((get_val(LUMBERMILL, o) != 0) && (get_val(SMITH, o) != 0) && ((get_val(TOWER, o) % 2) == 0)) c = true;
        if (c)id += 2;
    }
    ((void (*)(int*, int, int))g_proc_0040EEDD)(u, id, b);//original
}

PROC g_proc_00442E25;
void create_skeleton(int x, int y, int id, int o)
{
    if (ai_fixed)
    {
        int* s = unit_create_place((x / 32) + 1, y / 32, id, o % 256);
        if (s == NULL)unit_create((x / 32) + 1, y / 32, id, o % 256, 1);
    }
    else
        ((void (*)(int, int, int, int))g_proc_00442E25)(x, y, id, o);//original
}

PROC g_proc_00425D1C;
int* cast_raise(int* u, int a1, int a2, int a3)
{
    if (ai_fixed)
    {
        byte o = *((byte*)((uintptr_t)u + S_OWNER));
        find_all_alive_units(U_SKELETON);
        sort_stat(S_OWNER, o, CMP_EQ);
		sort_preplaced();
        if (units < 10)
        {
            if (((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_RAISE)) == 0))return NULL;
            byte mp = *((byte*)((uintptr_t)u + S_MANA));
            byte cost = *(byte*)(MANACOST + 2 * RAISE_DEAD);
            if (mp < cost)return NULL;
            byte x = *((byte*)((uintptr_t)u + S_X));
            byte y = *((byte*)((uintptr_t)u + S_Y));
            set_region((int)x - 8, (int)y - 8, (int)x + 8, (int)y + 8);//set region around myself
            find_all_units(ANY_BUILDING);//dead body
            sort_in_region();
            sort_hidden();
            sort_near_death();
            if (units != 0)
            {
                byte xx = *((byte*)((uintptr_t)unit[0] + S_X));
                byte yy = *((byte*)((uintptr_t)unit[0] + S_Y));
                give_order(u, xx, yy, ORDER_SPELL_RAISEDEAD);
                return unit[0];
            }
        }
        return NULL;
    }
    else
        return ((int* (*)(int*, int, int, int))g_proc_00425D1C)(u, a1, a2, a3);//original
}

PROC g_proc_00424F94;
PROC g_proc_00424FD7;
int* cast_runes(int* u, int a1, int a2, int a3)
{
    if (ai_fixed)
    {
        byte o = *((byte*)((uintptr_t)u + S_OWNER));
        if (((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_RUNES)) == 0))return NULL;
        byte mp = *((byte*)((uintptr_t)u + S_MANA));
        byte cost = *(byte*)(MANACOST + 2 * RUNES);
        if (mp < cost)return NULL;
        byte x = *((byte*)((uintptr_t)u + S_X));
        byte y = *((byte*)((uintptr_t)u + S_Y));
        set_region((int)x - 14, (int)y - 14, (int)x + 14, (int)y + 14);//set region around myself
        find_all_alive_units(ANY_MEN);
        sort_in_region();
        sort_hidden();
        sort_stat(S_MOVEMENT_TYPE, MOV_LAND, CMP_EQ);
        for (int ui = 0; ui < 16; ui++)
        {
            if (check_ally(o, ui))//only not allied units
                sort_stat(S_OWNER, ui, CMP_NEQ);
        }
        sort_rune_near();
        if (units != 0)
        {
            byte xx = *((byte*)((uintptr_t)unit[0] + S_X));
            byte yy = *((byte*)((uintptr_t)unit[0] + S_Y));
            give_order(u, xx, yy, ORDER_SPELL_RUNES);
            return unit[0];
        }
        return NULL;
    }
    else
        return ((int* (*)(int*, int, int, int))g_proc_00424F94)(u, a1, a2, a3);//original
}

byte get_manacost(byte s)
{
    return *(byte*)(MANACOST + 2 * s);
}

PROC g_proc_0042757E;
int ai_spell(int* u)
{
    if (ai_fixed)
    {
        byte id = *((byte*)((uintptr_t)u + S_ID));
        if ((id == U_MAGE) || (id == U_DK))
        {
            byte x = *((byte*)((uintptr_t)u + S_X));
            byte y = *((byte*)((uintptr_t)u + S_Y));
            set_region((int)x - 24, (int)y - 24, (int)x + 24, (int)y + 24);//set region around myself
            find_all_alive_units(ANY_UNITS);
            sort_in_region();
            byte o = *((byte*)((uintptr_t)u + S_OWNER));
            for (int ui = 0; ui < 16; ui++)
            {
                if (check_ally(o, ui))
                    sort_stat(S_OWNER, ui, CMP_NEQ);
            }
            if (units != 0)
            {
                byte mp = *((byte*)((uintptr_t)u + S_MANA));
                byte ord = *((byte*)((uintptr_t)u + S_ORDER));
                bool new_cast = (ord == ORDER_SPELL_ROT) || (ord == ORDER_SPELL_BLIZZARD) || (ord == ORDER_SPELL_INVIS) || (ord == ORDER_SPELL_ARMOR);
                WORD shl = *((WORD*)((uintptr_t)u + S_SHIELD));
                WORD inv = *((WORD*)((uintptr_t)u + S_INVIZ));
                if ((shl == 0) && (inv == 0))
                {
                    set_region((int)x - 12, (int)y - 12, (int)x + 12, (int)y + 12);//set region around myself
                    find_all_alive_units(ANY_MEN);
                    sort_in_region();
                    for (int ui = 0; ui < 16; ui++)
                    {
                        if (check_ally(o, ui))//enemy
                            sort_stat(S_OWNER, ui, CMP_NEQ);
                    }
                    if (units != 0)
                    {
                        struct GPOINT
                        {
                            WORD x;
                            WORD y;
                        } l;
                        l.x = *((WORD*)((uintptr_t)u + S_X));
                        l.y = *((WORD*)((uintptr_t)u + S_Y));
                        ((int (*)(int*, int, GPOINT*))F_ICE_SET_AI_ORDER)(u, AI_ORDER_DEFEND, &l);
                        set_stat(u, l.x, S_AI_DEST_X);
                        set_stat(u, l.y, S_AI_DEST_Y);
                    }
                    if (mp < 50)new_cast = false;
                    else
                    {
                        if (id == U_MAGE)
                        {
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_INVIS)) != 0)
                            {
                                if (ord != ORDER_SPELL_POLYMORPH)
                                {
                                    if (mp >= get_manacost(INVIS))
                                    {
                                        set_region((int)x - 12, (int)y - 12, (int)x + 12, (int)y + 12);//set region around myself
                                        find_all_alive_units(ANY_MEN);
                                        sort_in_region();
                                        sort_stat(S_ID, U_DK, CMP_SMALLER_EQ);
                                        sort_stat(S_ID, U_MAGE, CMP_BIGGER_EQ);
                                        sort_self(u);
                                        sort_stat(S_INVIZ, 0, CMP_EQ);
                                        sort_stat(S_MANA, 150, CMP_BIGGER_EQ);
                                        sort_stat(S_AI_ORDER, AI_ORDER_ATTACK, CMP_EQ);
                                        for (int ui = 0; ui < 16; ui++)
                                        {
                                            if (!check_ally(o, ui))
                                                sort_stat(S_OWNER, ui, CMP_NEQ);
                                        }
                                        if (units != 0)
                                        {
                                            give_order_spell_target(u, unit[0], ORDER_SPELL_INVIS);
                                            new_cast = true;
                                        }
                                    }
                                }
                            }
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_POLYMORF)) != 0)
                            {
                                if (ord != ORDER_SPELL_POLYMORPH)
                                {
                                    if (mp >= get_manacost(POLYMORPH))
                                    {
                                        set_region((int)x - 18, (int)y - 18, (int)x + 18, (int)y + 18);//set region around myself
                                        find_all_alive_units(ANY_MEN);
                                        sort_in_region();
                                        sort_stat(S_ID, U_DRAGON, CMP_SMALLER_EQ);
                                        sort_stat(S_ID, U_GRIFON, CMP_BIGGER_EQ);
                                        for (int ui = 0; ui < 16; ui++)
                                        {
                                            if (check_ally(o, ui))//enemy
                                                sort_stat(S_OWNER, ui, CMP_NEQ);
                                        }
                                        if (units != 0)
                                        {
                                            give_order_spell_target(u, unit[0], ORDER_SPELL_POLYMORPH);
                                            new_cast = true;
                                        }
                                    }
                                }
                            }
                        }
                        else if (id == U_DK)
                        {
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_UNHOLY)) != 0)
                            {
                                if (ord != ORDER_SPELL_ARMOR)
                                {
                                    if (mp >= get_manacost(UNHOLY_ARMOR))
                                    {
                                        set_region((int)x - 12, (int)y - 12, (int)x + 12, (int)y + 12);//set region around myself
                                        find_all_alive_units(ANY_MEN);
                                        sort_in_region();
                                        sort_stat(S_ID, U_DK, CMP_SMALLER_EQ);
                                        sort_stat(S_ID, U_MAGE, CMP_BIGGER_EQ);
                                        sort_self(u);
                                        sort_stat(S_SHIELD, 0, CMP_EQ);
                                        sort_stat(S_MANA, 150, CMP_BIGGER_EQ);
                                        sort_stat(S_AI_ORDER, AI_ORDER_ATTACK, CMP_EQ);
                                        for (int ui = 0; ui < 16; ui++)
                                        {
                                            if (!check_ally(o, ui))
                                                sort_stat(S_OWNER, ui, CMP_NEQ);
                                        }
                                        if (units != 0)
                                        {
                                            give_order_spell_target(u, unit[0], ORDER_SPELL_ARMOR);
                                            new_cast = true;
                                        }
                                    }
                                }
                                else new_cast = true;
                            }
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_HASTE)) != 0)
                            {
                                if (ord != ORDER_SPELL_HASTE)
                                {
                                    if (mp >= get_manacost(HASTE))
                                    {
                                        set_region((int)x - 16, (int)y - 16, (int)x + 16, (int)y + 16);//set region around myself
                                        find_all_alive_units(ANY_MEN);
                                        sort_in_region();
                                        sort_stat(S_ID, U_DK, CMP_SMALLER_EQ);
                                        sort_stat(S_ID, U_MAGE, CMP_BIGGER_EQ);
                                        sort_self(u);
                                        sort_stat(S_SHIELD, 0, CMP_NEQ);
                                        sort_stat(S_HASTE, 0, CMP_EQ);
                                        for (int ui = 0; ui < 16; ui++)
                                        {
                                            if (!check_ally(o, ui))
                                                sort_stat(S_OWNER, ui, CMP_NEQ);
                                        }
                                        if (units != 0)
                                        {
                                            give_order_spell_target(u, unit[0], ORDER_SPELL_HASTE);
                                            new_cast = true;
                                        }
                                    }
                                }
                                else new_cast = true;
                            }
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_COIL)) != 0)
                            {
                                if (ord != ORDER_SPELL_DRAINLIFE)
                                {
                                    if (mp >= get_manacost(COIL))
                                    {
                                        set_region((int)x - 18, (int)y - 18, (int)x + 18, (int)y + 18);//set region around myself
                                        find_all_alive_units(ANY_MEN);
                                        sort_in_region();
                                        sort_stat(S_ID, U_DRAGON, CMP_SMALLER_EQ);
                                        sort_stat(S_ID, U_GRIFON, CMP_BIGGER_EQ);
                                        sort_stat(S_ANIMATION, ANIM_MOVE, CMP_NEQ);
                                        for (int ui = 0; ui < 16; ui++)
                                        {
                                            if (check_ally(o, ui))//enemy
                                                sort_stat(S_OWNER, ui, CMP_NEQ);
                                        }
                                        if (units != 0)
                                        {
                                            byte xx = *((byte*)((uintptr_t)unit[0] + S_X));
                                            byte yy = *((byte*)((uintptr_t)unit[0] + S_Y));
                                            give_order(u, xx, yy, ORDER_SPELL_DRAINLIFE);
                                            new_cast = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    set_region((int)x - 50, (int)y - 50, (int)x + 50, (int)y + 50);//set region around myself
                    find_all_alive_units(ANY_MEN);
                    sort_in_region();
                    sort_stat(S_ID, U_PEON, CMP_SMALLER_EQ);
                    sort_stat(S_ID, U_PEASANT, CMP_BIGGER_EQ);
                    sort_peon_loaded(0);
                    sort_hidden();
                    for (int ui = 0; ui < 16; ui++)
                    {
                        if (check_ally(o, ui))
                            sort_stat(S_OWNER, ui, CMP_NEQ);//enemy peons
                    }
                    if (units == 0)
                    {
                        find_all_alive_units(ANY_BUILDING);
                        sort_in_region();
                        sort_stat(S_ID, U_FORTRESS, CMP_SMALLER_EQ);//fortres castle stronghold keep
                        sort_stat(S_ID, U_KEEP, CMP_BIGGER_EQ);
                        for (int ui = 0; ui < 16; ui++)
                        {
                            if (check_ally(o, ui))
                                sort_stat(S_OWNER, ui, CMP_NEQ);
                        }
                        if (units == 0)
                        {
                            find_all_alive_units(ANY_BUILDING);
                            sort_in_region();
                            sort_stat(S_ID, U_GREAT_HALL, CMP_SMALLER_EQ);
                            sort_stat(S_ID, U_TOWN_HALL, CMP_BIGGER_EQ);
                            for (int ui = 0; ui < 16; ui++)
                            {
                                if (check_ally(o, ui))
                                    sort_stat(S_OWNER, ui, CMP_NEQ);
                            }
                        }
                    }
                    if (units != 0)
                    {
                        if (id == U_MAGE)
                        {
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_BLIZZARD)) != 0)
                            {
                                if (ord != ORDER_SPELL_BLIZZARD)
                                {
                                    if (mp >= get_manacost(BLIZZARD))
                                    {
                                        byte tx = *((byte*)((uintptr_t)unit[0] + S_X));
                                        byte ty = *((byte*)((uintptr_t)unit[0] + S_Y));
                                        give_order(u, tx, ty, ORDER_SPELL_BLIZZARD);
                                        new_cast = true;
                                    }
                                }
                                else new_cast = true;
                            }
                        }
                        if (id == U_DK)
                        {
                            if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_DD)) != 0)
                            {
                                if (ord != ORDER_SPELL_ROT)
                                {
                                    if (mp >= get_manacost(DEATH_AND_DECAY))
                                    {
                                        byte tx = *((byte*)((uintptr_t)unit[0] + S_X));
                                        byte ty = *((byte*)((uintptr_t)unit[0] + S_Y));
                                        give_order(u, tx, ty, ORDER_SPELL_ROT);
                                        new_cast = true;
                                    }
                                }
                                else new_cast = true;
                            }
                            else if ((*(DWORD*)(SPELLS_LEARNED + 4 * o) & (1 << L_WIND)) != 0)
                            {
                                if (!new_cast && (ord != ORDER_SPELL_WHIRLWIND))
                                {
                                    if (mp >= get_manacost(WHIRLWIND))
                                    {
                                        byte tx = *((byte*)((uintptr_t)unit[0] + S_X));
                                        byte ty = *((byte*)((uintptr_t)unit[0] + S_Y));
                                        give_order(u, tx, ty, ORDER_SPELL_WHIRLWIND);
                                        new_cast = true;
                                    }
                                }
                                else new_cast = true;
                            }
                        }
                    }
                }
                if (!new_cast)
                    return ((int (*)(int*))g_proc_0042757E)(u);//original
            }
        }
        else
            return ((int (*)(int*))g_proc_0042757E)(u);//original
        return 0;
    }
    else
        return ((int (*)(int*))g_proc_0042757E)(u);//original
}

PROC g_proc_00427FAE;
void ai_attack(int* u, int b, int a)
{
    if (ai_fixed)
    {
        byte o = *((byte*)((uintptr_t)u + S_OWNER));
        for (int i = 0; i < 16; i++)
        {
            int* p = (int*)(UNITS_LISTS + 4 * i);
            if (p)
            {
                p = (int*)(*p);
                while (p)
                {
                    bool f = ((*((byte*)((uintptr_t)p + S_ID)) == U_MAGE) || (*((byte*)((uintptr_t)p + S_ID)) == U_DK));
                    if (f)
                    {
                        if (!check_unit_dead(p) && !check_unit_hidden(p))
                        {
                            byte ow = *((byte*)((uintptr_t)p + S_OWNER));
                            if (ow == o)
                            {
                                if ((*(byte*)(CONTROLER_TYPE + o) == C_COMP))
                                {
                                    WORD inv = *((WORD*)((uintptr_t)p + S_INVIZ));
                                    if (inv == 0)
                                    {
                                        byte aor = *((byte*)((uintptr_t)p + S_AI_ORDER));
                                        if (aor != AI_ORDER_ATTACK)
                                        {
                                            byte mp = *((byte*)((uintptr_t)p + S_MANA));
                                            if (mp >= 200)
                                            {
                                                ((void (*)(int*, int, int))F_ICE_SET_AI_ORDER)(p, AI_ORDER_ATTACK, a);//ai attack
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
                }
            }
        }

        find_all_alive_units(ANY_MEN);
        sort_stat(S_ID, U_GOBLINS, CMP_SMALLER_EQ);
        sort_stat(S_ID, U_DWARWES, CMP_BIGGER_EQ);
        sort_stat(S_OWNER, o, CMP_EQ);
        sort_stat(S_AI_ORDER, AI_ORDER_ATTACK, CMP_NEQ);//not attack already
        for (int i = 0; i < units; i++)
        {
            ((void (*)(int*, int, int))F_ICE_SET_AI_ORDER)(unit[i], AI_ORDER_ATTACK, a);//ai attack
        }
    }
    ((void (*)(int*, int, int))g_proc_00427FAE)(u, b, a);//original
}

void sap_behaviour()
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = ((*((byte*)((uintptr_t)p + S_ID)) == U_DWARWES) || (*((byte*)((uintptr_t)p + S_ID)) == U_GOBLINS));
                if (f)
                {
                    if (!check_unit_dead(p) && !check_unit_hidden(p))
                    {
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        if ((*(byte*)(CONTROLER_TYPE + o) == C_COMP))
                        {
                            byte ord = *((byte*)((uintptr_t)p + S_ORDER));
                            byte x = *((byte*)((uintptr_t)p + S_X));
                            byte y = *((byte*)((uintptr_t)p + S_Y));
                            if ((ord != ORDER_DEMOLISH) && (ord != ORDER_DEMOLISH_NEAR) && (ord != ORDER_DEMOLISH_AT))
                            {
                                set_region((int)x - 12, (int)y - 12, (int)x + 12, (int)y + 12);//set region around myself
                                find_all_alive_units(ANY_UNITS);
                                sort_in_region();
                                sort_stat(S_MOVEMENT_TYPE, MOV_LAND, CMP_EQ);
                                for (int ui = 0; ui < 16; ui++)
                                {
                                    if (check_ally(o, ui))//only not allied units
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                }
                                if (units != 0)
                                {
                                    int ord = *(int*)(ORDER_FUNCTIONS + 4 * ORDER_DEMOLISH);
                                    ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(p, 0, 0, unit[0], ord);
                                }
                                set_region((int)x - 5, (int)y - 5, (int)x + 5, (int)y + 5);//set region around myself
                                find_all_alive_units(ANY_UNITS);
                                sort_in_region();
                                sort_stat(S_MOVEMENT_TYPE, MOV_LAND, CMP_EQ);
                                for (int ui = 0; ui < 16; ui++)
                                {
                                    if (check_ally(o, ui))//only not allied units
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                }
                                if (units != 0)
                                {
                                    int ord = *(int*)(ORDER_FUNCTIONS + 4 * ORDER_DEMOLISH);
                                    ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(p, 0, 0, unit[0], ord);
                                }
                                set_region((int)x - 1, (int)y - 1, (int)x + 1, (int)y + 1);//set region around myself
                                find_all_alive_units(ANY_UNITS);
                                sort_in_region();
                                sort_stat(S_MOVEMENT_TYPE, MOV_LAND, CMP_EQ);
                                for (int ui = 0; ui < 16; ui++)
                                {
                                    if (check_ally(o, ui))//only not allied units
                                        sort_stat(S_OWNER, ui, CMP_NEQ);
                                }
                                if (units != 0)
                                {
                                    int ord = *(int*)(ORDER_FUNCTIONS + 4 * ORDER_DEMOLISH);
                                    ((void (*)(int*, int, int, int*, int))F_GIVE_ORDER)(p, 0, 0, unit[0], ord);
                                }
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void unstuk()
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                byte id = *((byte*)((uintptr_t)p + S_ID));
                byte ord = *((byte*)((uintptr_t)p + S_ORDER));
                byte aord = *((byte*)((uintptr_t)p + S_AI_ORDER));
                bool f = ((((id < U_CRITTER))
                    && !check_unit_preplaced(p) && (ord == ORDER_STOP) && (aord == AI_ORDER_ATTACK)) ||
                    ((id == U_PEASANT) || (id == U_PEON)));
                if (*(byte*)GB_MULTIPLAYER)
                {
                    if (!((id == U_PEASANT) || (id == U_PEON)))
                        f = false;
                }
                if (f)
                {
                    if (!check_unit_dead(p) && !check_unit_hidden(p))
                    {
                        byte o = *((byte*)((uintptr_t)p + S_OWNER));
                        if ((*(byte*)(CONTROLER_TYPE + o) == C_COMP))
                        {
                            byte st = *((byte*)((uintptr_t)p + S_NEXT_FIRE));
                            byte frm = *((byte*)((uintptr_t)p + S_FRAME));
                            byte pfrm = *((byte*)((uintptr_t)p + S_NEXT_FIRE + 1));
                            if (st == 0)
                            {
                                byte map = *(byte*)MAP_SIZE - 1;
                                byte x = *((byte*)((uintptr_t)p + S_X));
                                byte y = *((byte*)((uintptr_t)p + S_Y));
                                int xx = 0, yy = 0, dir = 0;
                                xx += x;
                                yy += y;
                                dir = ((int (*)())F_NET_RANDOM)();
                                dir %= 8;
                                if (dir == 0)
                                {
                                    if (yy > 0)yy -= 1;
                                }
                                if (dir == 1)
                                {
                                    if (yy > 0)yy -= 1;
                                    if (xx < map)xx += 1;
                                }
                                if (dir == 2)
                                {
                                    if (xx < map)xx += 1;
                                }
                                if (dir == 3)
                                {
                                    if (xx < map)xx += 1;
                                    if (yy < map)yy += 1;
                                }
                                if (dir == 4)
                                {
                                    if (yy < map)yy += 1;
                                }
                                if (dir == 5)
                                {
                                    if (yy < map)yy += 1;
                                    if (xx > 0)xx -= 1;
                                }
                                if (dir == 6)
                                {
                                    if (xx > 0)xx -= 1;
                                }
                                if (dir == 7)
                                {
                                    if (xx > 0)xx -= 1;
                                    if (yy > 0)yy -= 1;
                                }
                                if ((id != U_PEON) && (id != U_PEASANT))
                                {
                                    struct GPOINT
                                    {
                                        WORD x;
                                        WORD y;
                                    };
                                    int* trg = NULL;
                                    find_all_alive_units(ANY_UNITS);
                                    sort_hidden();
                                    sort_attack_can_hit_range(p);
                                    for (int ui = 0; ui < 16; ui++)
                                    {
                                        if (check_ally(o, ui))//only not allied units
                                            sort_stat(S_OWNER, ui, CMP_NEQ);
                                    }
                                    if (units != 0)
                                    {
                                        WORD dist = 0xFFFF;
                                        int ndu = -1;
                                        for (int j = 0; j < units; j++)
                                        {
                                            GPOINT l;
                                            l.x = *((WORD*)((uintptr_t)p + S_X));
                                            l.y = *((WORD*)((uintptr_t)p + S_Y));
                                            WORD dst = 0;
                                            dst = ((WORD(*)(GPOINT*, int*))F_MTX_DIST)(&l, unit[j]);//mtx dist
                                            if (dst < dist)
                                            {
                                                dist = dst;
                                                ndu = j;
                                            }
                                        }
                                        if (ndu != -1)trg = unit[ndu];
                                    }
                                    if (trg)
                                    {
                                        GPOINT l;
                                        l.x = *((WORD*)((uintptr_t)trg + S_X));
                                        l.y = *((WORD*)((uintptr_t)trg + S_Y));
                                        units = 1;
                                        unit[0] = trg;
                                        sort_attack_can_hit_range(p);
                                        int sa = 0;
                                        if (units != 0)
                                            sa = ((int (*)(int*, int, GPOINT*))F_ICE_SET_AI_ORDER)(p, AI_ORDER_ATTACK, &l);
                                        if (sa)give_order(p, xx % 256, yy % 256, ORDER_ATTACK_AREA);
                                    }
                                }
                                else
                                    give_order(p, xx % 256, yy % 256, ORDER_MOVE);
                                st = 10;
                            }
                            if (st > 0)st -= 1;
                            if (frm != pfrm)st = 255;
                            set_stat(p, st, S_NEXT_FIRE);
                            set_stat(p, frm, S_NEXT_FIRE + 1);
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void goldmine_ai()
{
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                bool f = ((*((byte*)((uintptr_t)p + S_ID)) == U_MINE));
                if (f)
                {
                    if (check_unit_complete(p))
                    {
                        byte x = *((byte*)((uintptr_t)p + S_X));
                        byte y = *((byte*)((uintptr_t)p + S_Y));
                        set_region((int)x - 9, (int)y - 9, (int)x + 8, (int)y + 8);
                        find_all_alive_units(ANY_BUILDING_4x4);
                        sort_in_region();
                        sort_stat(S_ID, U_PORTAL, CMP_NEQ);
                        bool th = units != 0;
                        byte x1, y1, x2, y2;
                        if (x > 3)x1 = x - 3;
                        else x1 = 0;
                        if (y > 3)y1 = y - 3;
                        else y1 = 0;
                        x += 3;
                        y += 3;
                        if (x >= (127 - 3))x2 = 127;
                        else x2 = x + 3;
                        if (y >= (127 - 3))y2 = 127;
                        else y2 = y + 3;
                        char* sq = (char*)*(int*)(MAP_SQ_POINTER);
                        byte mxs = *(byte*)MAP_SIZE;//map size
                        for (int xx = x1; xx < x2; xx++)
                        {
                            for (int yy = y1; yy < y2; yy++)
                            {
                                char buf[] = "\x0";
                                buf[0] = *(char*)(sq + 2 * xx + 2 * yy * mxs + 1);
                                if (th)buf[0] |= SQ_AI_BUILDING >> 8;
                                else buf[0] &= ~(SQ_AI_BUILDING >> 8);
                                PATCH_SET((char*)(sq + 2 * xx + 2 * yy * mxs + 1), buf);
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void unit_timer()
{
    find_all_alive_units(ANY_MEN);
    sort_hidden();
    sort_stat(S_ID, U_HTRANSPORT, CMP_NEQ);
    sort_stat(S_ID, U_OTRANSPORT, CMP_NEQ);
    for (int i = 0; i < units; i++)
    {
        byte r = *(byte*)((uintptr_t)unit[i] + S_KILLS + 1);
        if (r > 0)r -= 1;
        set_stat(unit[i], r, S_KILLS + 1);
    }
}

void ogre_init(bool b)
{
    if (b)
    {
        char buf1[] = "\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)ARCHON_RCLICK, buf1);//rclick
        PATCH_SET((char*)ARCHON_ACTION, buf1);//action

        char buf2[] = "\x90\x90";
        PATCH_SET((char*)ARCHON_PEONS_STOP, buf2);//peons stop
        PATCH_SET((char*)ARCHON_PEONS_MOVE, buf2);//peons move
        PATCH_SET((char*)ARCHON_ACTION_PEONS, buf2);//action peons move
        PATCH_SET((char*)ARCHON_PLACEBOX_PEONS, buf2);//placebox peons move
        PATCH_SET((char*)ARCHON_CANCEL_UNIT, buf2);//stop build unit
        PATCH_SET((char*)ARCHON_TRAIN_UNIT, buf2);//build unit
        PATCH_SET((char*)ARCHON_CANCEL_BULDING, buf2);//build kill
        PATCH_SET((char*)ARCHON_PLACEBOX, buf2);//placebox
        PATCH_SET((char*)ARCHON_GROUP_A, buf2);//group ctrl a
        PATCH_SET((char*)ARCHON_GROUP_B, buf2);//group ctrl b
        PATCH_SET((char*)ARCHON_GROUP_SELECT, buf2);//group select
        PATCH_SET((char*)ARCHON_GROUP_KILL, buf2);//group kill

        char buf3[] = "\xeb";
        PATCH_SET((char*)ARCHON_SELECT_DRAW_UNIT, buf3);//sel draw unit
        PATCH_SET((char*)ARCHON_SELECT_DRAW_BUILDING, buf3);//sel draw build

        PATCH_SET((char*)ARCHON_SELECT, buf3);//sel draw unit
    }
    else
    {
        char buf1[] = "\xf\x85\x9E\x0\x0\x0";
        PATCH_SET((char*)ARCHON_RCLICK, buf1);//rclick
        buf1[2] = '\x24';
        buf1[3] = '\x1';
        PATCH_SET((char*)ARCHON_ACTION, buf1);//action

        char buf2[] = "\x75\x14";
        PATCH_SET((char*)ARCHON_PEONS_STOP, buf2);//peons stop
        PATCH_SET((char*)ARCHON_PEONS_MOVE, buf2);//peons move
        PATCH_SET((char*)ARCHON_ACTION_PEONS, buf2);//action peons move
        PATCH_SET((char*)ARCHON_PLACEBOX_PEONS, buf2);//placebox peons move
        buf2[1] = '\x7';
        PATCH_SET((char*)ARCHON_CANCEL_UNIT, buf2);//stop build unit
        buf2[1] = '\x1a';
        PATCH_SET((char*)ARCHON_TRAIN_UNIT, buf2);//build unit
        buf2[1] = '\xd';
        PATCH_SET((char*)ARCHON_CANCEL_BULDING, buf2);//build kill
        buf2[1] = '\x41';
        PATCH_SET((char*)ARCHON_PLACEBOX, buf2);//placebox
        buf2[1] = '\x6b';
        PATCH_SET((char*)ARCHON_GROUP_A, buf2);//group ctrl a
        buf2[1] = '\x35';
        PATCH_SET((char*)ARCHON_GROUP_B, buf2);//group ctrl b
        buf2[1] = '\x22';
        PATCH_SET((char*)ARCHON_GROUP_SELECT, buf2);//group select
        buf2[1] = '\x1a';
        PATCH_SET((char*)ARCHON_GROUP_KILL, buf2);//group kill

        char buf3[] = "\x75";
        PATCH_SET((char*)ARCHON_SELECT_DRAW_UNIT, buf3);//sel draw unit
        PATCH_SET((char*)ARCHON_SELECT_DRAW_BUILDING, buf3);//sel draw build

        char buf4[] = "\x74";
        PATCH_SET((char*)ARCHON_SELECT, buf4);
    }
}

void fix_chat_full()
{
    char buf[] = "\xFF";
    PATCH_SET((char*)ARCHON_FIX_CHAT, buf);
}

PROC g_proc_0044CFAF;
void fix_chat1(int a1)
{
    byte l = *(byte*)LOCAL_PLAYER;
    char buf[] = "\x0";
    if (real_local != 255)
    {
        buf[0] = real_local;
        PATCH_SET((char*)LOCAL_PLAYER, buf);
    }
    ((void (*)(int))g_proc_0044CFAF)(a1);//original
    if (real_local != 255)
    {
        buf[0] = l;
        PATCH_SET((char*)LOCAL_PLAYER, buf);
    }
}

void ogre_run(byte p1, byte p2)
{
    if (p1 != p2)
    {
        viz(p1, p2, 1);
        ally(p1, p2, 1);
        if (real_local == 255)
            real_local = *(byte*)LOCAL_PLAYER;
        if (real_local == p1)
        {
            char buf[] = "\x0";
            buf[0] = p2;
            PATCH_SET((char*)LOCAL_PLAYER, buf);
        }
    }
}

struct apm_act
{
    int tiks = 0;
    byte val = 0;
};
apm_act apm_list[1000];

void apm_add(byte val)
{
	if (m_show_apm!=0)
	{
		byte spd = *(byte*)CURRENT_SPEED;
		spd = *(byte*)(SLOWEST_SPEED + spd);
		if (spd == 0)spd = 1;
		int tik_per_min = 1000 / spd * 60;
        int n = 0;
        int m = apm_list[0].tiks;
        for (int i = 0; i < 1000; i++)
        {
            if (apm_list[i].tiks < m)
            {
                m = apm_list[i].tiks;
                n = i;
            }
        }
        apm_list[n].val = val;
        apm_list[n].tiks = tik_per_min;
	}
}

void apm_del(int i)
{
    apm_list[i].tiks = 0;
    apm_list[i].val = 0;
}

void apm_clear()
{
    for (int i = 0; i < 1000; i++)
        apm_del(i);
}

void obss()
{
    comps_vision(true);
    for (int i = 0; i < 8; i++)
    {
        if (get_player_team(i) == 9)
        {
            byte v = *(byte*)AVED;
            v = v | (1 << i);
            char buf[] = "\x0";
            buf[0] = v;
            PATCH_SET((char*)AVED, buf);
            for (int j = 0; j < 8; j++)
            {
                viz_obs(i, j);
                if (obs_lose[i] == 0)
                {
                    ally(i, j, 1);
                }
                else
                {
                    ally_one_sided(j, i, 1);
                }
            }
        }
    }
}

void war2mod_ai()
{
    for (int i = 0; i < 8; i++)
    {
        if ((*(byte*)(CONTROLER_TYPE + i) == C_COMP))
        {
            bool th1 = get_val(TH1, i) != 0;
            bool th2 = get_val(TH2, i) != 0;
            bool th3 = get_val(TH3, i) != 0;
            byte r = *(byte*)(PLAYER_RACE + i);
            if (r == 0)//human
            {
                byte hid = U_CRITTER;
                if (th1 || th2 || th3)
                {
                    find_all_alive_units(U_DANATH);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(SMITH, i) != 0)hid = U_DANATH;
                    }
                    find_all_alive_units(U_ALLERIA);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(LUMBERMILL, i) != 0)hid = U_ALLERIA;
                    }
                }
                if (th2 || th3)
                {
                    find_all_alive_units(U_LOTHAR);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)hid = U_LOTHAR;
                    find_all_alive_units(U_TYRALYON);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(STABLES, i) != 0)hid = U_TYRALYON;
                    }
                }
                if (th3)
                {
                    find_all_alive_units(U_UTER);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(ALTAR, i) != 0)hid = U_UTER;
                    }
                    find_all_alive_units(U_HADGAR);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        find_all_alive_units(U_MAGE_TOWER);
                        sort_stat(S_OWNER, i, CMP_EQ);
                        if (units != 0)hid = U_HADGAR;
                    }
                    find_all_alive_units(U_KURDRAN);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(AVIARY, i) != 0)hid = U_KURDRAN;
                    }
                }
                if (hid != U_CRITTER)
                {
                    find_all_alive_units(U_CASTLE);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                    if (units != 0)building_start_build(unit[0], hid, 0);
                    else
                    {
                        find_all_alive_units(U_KEEP);
                        sort_stat(S_OWNER, i, CMP_EQ);
                        sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                        if (units != 0)building_start_build(unit[0], hid, 0);
                        else
                        {
                            find_all_alive_units(U_TOWN_HALL);
                            sort_stat(S_OWNER, i, CMP_EQ);
                            sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                            if (units != 0)building_start_build(unit[0], hid, 0);
                        }
                    }
                }
            }
            else//orc
            {
                byte hid = U_CRITTER;
                if (th1 || th2 || th3)
                {
                    find_all_alive_units(U_GROM);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(SMITH, i) != 0)hid = U_GROM;
                    }
                    find_all_alive_units(U_ZULJIN);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(LUMBERMILL, i) != 0)hid = U_ZULJIN;
                    }
                }
                if (th2 || th3)
                {
                    find_all_alive_units(U_KARGATH);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)hid = U_KARGATH;
                    find_all_alive_units(U_DENTARG);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(STABLES, i) != 0)hid = U_DENTARG;
                    }
                }
                if (th3)
                {
                    find_all_alive_units(U_CHOGAL);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(ALTAR, i) != 0)hid = U_CHOGAL;
                    }
                    find_all_alive_units(U_TERON);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        find_all_alive_units(U_TEMPLE);
                        sort_stat(S_OWNER, i, CMP_EQ);
                        if (units != 0)hid = U_TERON;
                    }
                    find_all_alive_units(U_DEATHWING);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    if (units == 0)
                    {
                        if (get_val(AVIARY, i) != 0)hid = U_DEATHWING;
                    }
                }
                if (hid != U_CRITTER)
                {
                    find_all_alive_units(U_FORTRESS);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                    if (units != 0)building_start_build(unit[0], hid, 0);
                    else
                    {
                        find_all_alive_units(U_STRONGHOLD);
                        sort_stat(S_OWNER, i, CMP_EQ);
                        sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                        if (units != 0)building_start_build(unit[0], hid, 0);
                        else
                        {
                            find_all_alive_units(U_GREAT_HALL);
                            sort_stat(S_OWNER, i, CMP_EQ);
                            sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                            if (units != 0)building_start_build(unit[0], hid, 0);
                        }
                    }
                }
            }

            find_all_alive_units(U_CRITTER);
            sort_stat(S_OWNER, i, CMP_EQ);
            if (units < 5)
            {
                find_all_alive_units(ANY_BUILDING);
                sort_stat(S_OWNER, i, CMP_EQ);
                sort_stat(S_ID, U_FARM, CMP_BIGGER_EQ);
                sort_stat(S_ID, U_PIGFARM, CMP_SMALLER_EQ);
                sort_stat(S_ORDER, ORDER_BLDG_BUILD, CMP_NEQ);
                if (units != 0)
                {
                    building_start_build(unit[0], U_CRITTER, 0);
                }
            }
        }
    }
}

PROC g_proc_0045271B;
void update_spells()
{
    game_started = true;
    ((void (*)())g_proc_0045271B)();//original

    m_ticks1++;
    if (m_ticks1 >= 100)
    {
        m_ticks1 = 0;
        m_ticks++;
    }

    for (int i = 0; i < 1000; i++)
    {
        if (apm_list[i].tiks > 0)
            apm_list[i].tiks--;
        else
            apm_del(i);
    }

    if (apm_rt > 0)
        apm_rt--;
    if (apm_gct > 0)
        apm_gct--;
    if (apm_st > 0)
        apm_st--;
    if (apm_ft > 0)
        apm_ft--;
    if (apm_mt > 0)
        apm_mt--;

    if (ogre_mode)
    {
        for (int i = 0; i < 8; i++)
        {
            if (get_player_controler(i) != C_COMP)
            {
                ogre_run(i, ogre_find_player(i));
            }
        }
    }

    if (replaced)
    {
        bullet_disp_fires();
        unit_timer();
        if (A_portal)portal();
        if (A_transport)transport();
        if (A_autoheal)paladin();
        if (A_runestone)runestone();
        for (int i = 0; i < 255; i++)
        {
            if (m_slow_aura[i] != 255)
                slow_aura(m_slow_aura[i]);
            else
                i = 256;
        }
        for (int i = 0; i < 255; i++)
        {
            if (m_death_aura[i] != 255)
                death_aura(m_death_aura[i]);
            else
                i = 256;
        }
        for (int i = 0; i < 255; i++)
        {
            if (m_sneak[i] != 255)
                sneak(m_sneak[i]);
            else
                i = 256;
        }
        if (saveload_fixed)tech_reinit();
        if (ai_fixed)
        {
            sap_behaviour();
            unstuk();
            goldmine_ai();
        }
        if (vizs_n > 0)
        {
            for (int i = 0; i < vizs_n; i++)
            {
                viz_area(vizs_areas[i].x, vizs_areas[i].y, vizs_areas[i].p, vizs_areas[i].s);
            }
        }
        if (war2mod)
        {
            war2mod_ai();
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (get_val(TH2, i) != 0)maxtir[i] |= 1;
        if (get_val(TH3, i) != 0)maxtir[i] |= 3;
        if (get_val(LUMBERMILL, i) != 0)maxtir[i] |= 4;
        if (get_val(SHIPYARD, i) != 0)maxtir[i] |= 8;
    }
    obss();
}

void seq_change(int* u, byte tt)
{
    if (tt == 1)
    {
        byte id = *((byte*)((uintptr_t)u + S_ID));
        byte t = *((byte*)((uintptr_t)u + S_ANIMATION_TIMER));
        byte a = *((byte*)((uintptr_t)u + S_ANIMATION));
        byte f = *((byte*)((uintptr_t)u + S_FRAME));
        WORD h = *((WORD*)((uintptr_t)u + S_HASTE));
        byte o = *((byte*)((uintptr_t)u + S_OWNER));
        if (ships_speed_tower)
        {
            if ((id == U_HARROWTOWER) || (id == U_OARROWTOWER) || (id == U_HCANONTOWER) || (id == U_OCANONTOWER))
            {
                if (a == ANIM_ATTACK)
                {
                    byte up = *(byte*)(GB_BOAT_SPEED + o) * (*(byte*)(UPGRD + SHIP_SPEED));
                    if (t > up)t -= up;
                    else t = 1;
                    set_stat(u, t, S_ATTACK_COUNTER);
                }
                set_stat(u, t, S_ANIMATION_TIMER);
            }
        }
        if (war2mod)
        {
            if ((id == U_ARCHER) || (id == U_RANGER) || (id == U_ALLERIA))
            {

                if (a == ANIM_MOVE)
                    if (t > 1)t -= 1;
                if (a == ANIM_ATTACK)
                {
                    if (f != 0)
                    {
                        if (h > 0)
                        {
                            if (t > 2)t -= 2;
                            else t = 1;
                        }
                        else
                        {
                            if (t > 5)t -= 5;
                            else t = 1;
                        }
                    }
                    else
                    {
                        if (h > 0)
                        {
                            if (t > 12)t -= 12;
                            else t = 1;
                        }
                        else
                        {
                            if (t > 25)t -= 25;
                            else t = 1;
                        }
                    }
                    set_stat(u, 1, S_ATTACK_COUNTER);
                }
                set_stat(u, t, S_ANIMATION_TIMER);
            }
            if ((id == U_TROLL) || (id == U_BERSERK) || (id == U_ZULJIN))
            {
                if (a == ANIM_MOVE)
                    if (t > 1)t -= 1;
                if (a == ANIM_ATTACK)
                {
                    if (f != 0)
                    {
                        if (t > 1)t -= 1;
                        else t = 1;
                    }
                    else
                    {
                        if (h > 0)
                        {
                            if (t > 15)t -= 15;
                            else t = 1;
                        }
                        else
                        {
                            if (t > 31)t -= 31;
                            else t = 1;
                        }
                    }
                    set_stat(u, 1, S_ATTACK_COUNTER);
                }
                set_stat(u, t, S_ANIMATION_TIMER);
            }
        }
    }
}

PROC g_proc_004522B9;
int seq_run(int* u)
{
    byte t = *((byte*)((uintptr_t)u + S_ANIMATION_TIMER));
    int original = ((int (*)(int*))g_proc_004522B9)(u);//original
    if (replaced)seq_change(u, t);
    return original;
}

PROC g_proc_00409F3B, //action
g_proc_0040AF70, g_proc_0040AF99, //demolish
g_proc_0041038E, g_proc_00410762, //bullet
g_proc_004428AD;//spell
char fdmg = 0;
void damage(int* atk, int* trg, char dmg)
{
    fdmg = dmg;
    if (replaced)
    {
        if ((trg != NULL) && (atk != NULL))
        {
            if (!check_unit_dead(trg))
            {
                byte aid = *((byte*)((uintptr_t)atk + S_ID));//attacker id
                byte tid = *((byte*)((uintptr_t)trg + S_ID));//target id
                byte dmg2 = dmg;//can deal damage by default
                int i = 0;
                while (i < 255)
                {
                    if ((tid == ut[i]) && (aid != ua[i]))
                    {
                        dmg2 = 0;//canot deal damage
                    }
                    if ((tid == ut[i]) && (aid == ua[i]))//check if only some certain units can attack that unit
                    {
                        dmg2 = dmg;//can deal damage
                        i = 255;
                    }
                    i++;
                    if (ua[i] == 255)//pairs must go in a row
                    {
                        i = 255;
                    }
                }
                if (*((WORD*)((uintptr_t)trg + S_SHIELD)) != 0)dmg2 = 0;
                if (blood_f)
                {
                    int bdmg = dmg2;
                    if (*((WORD*)((uintptr_t)atk + S_BLOOD)) != 0)bdmg *= 2;
                    if (bdmg > 255)bdmg = 255;
                    if (bdmg != 0)dmg2 = bdmg % 256;
                }
                if (war2mod)
                {
                    if (aid == U_DEMON)
                    {
                        byte tmov = *((byte*)((uintptr_t)trg + S_MOVEMENT_TYPE));//target mov
                        if (tmov == MOV_AIR)
                        {
                            if (dmg2 != 0)
                            {
                                int bdmg = dmg2 * 7;
                                if (bdmg > 255)bdmg = 255;
                                if (bdmg != 0)dmg2 = bdmg % 256;
                            }
                        }
                    }
                }
                fdmg = dmg2;
                if (agr)comp_aggro(trg, atk);//check if allied comps go agro
                if (fdmg != 0)
                {
                    if (cpt || ucpt)if (capture(trg, atk))fdmg = 0;//check if buildings or units captured on low hp
                    if (steal)steal_res(trg, atk);
                    if (manaburn)mana_burn(trg, atk);
                    if (peon_steal)peon_steal_attack(trg, atk);
                    bool f = false;
                    for (int i = 0; i < 255; i++)
                    {
                        if ((m_devotion[i] != 255) && (!f))
                            f = devotion_aura(trg, m_devotion[i]);
                        else
                            i = 256;
                    }
                    if (f)//defence
                    {
                        dmg2 = fdmg;
                        if (dmg2 > 3)dmg2 -= 3;
                        else dmg2 = 0;
                        fdmg = dmg2;
                    }
                    f = false;
                    for (int i = 0; i < 255; i++)
                    {
                        if ((m_prvnt[i] != 255) && (!f))
                            f = m_prvnt[i] == tid;
                        else
                            i = 256;
                    }
                    if (f)//prevent
                    {
                        WORD hp = *((WORD*)((uintptr_t)trg + S_HP));
                        if (hp > 0)
                        {
                            if (hp <= fdmg)
                            {
                                fdmg = 0;
                                set_stat(trg, 300, S_SHIELD);
                                set_stat(trg, 0, S_HP);
                                flame(trg);
                            }
                        }
                    }
                    f = false;
                    for (int i = 0; i < 255; i++)
                    {
                        if ((m_vampire[i] != 255) && (!f))
                            f = vamp_aura(trg, atk, m_vampire[i]);
                        else
                            i = 256;
                    }
                    if (f)//vampire
                    {
                        WORD hp = *((WORD*)((uintptr_t)trg + S_HP));
                        byte mult = 2;//2=50%
                        if (hp > (fdmg / mult + 1))heal(atk, (fdmg / mult + 1), 0);
                        else heal(atk, hp % 256, 0);
                        WORD xx = *((WORD*)((uintptr_t)atk + S_DRAW_X));
                        WORD yy = *((WORD*)((uintptr_t)atk + S_DRAW_Y));
                        bullet_create(xx + 16, yy + 16, B_SHOT_FIRE);
                    }
                }
                WORD hp = *((WORD*)((uintptr_t)trg + S_HP));//unit hp
                if ((tid < U_FARM) && (fdmg >= hp))
                {
                    if ((aid != U_HTRANSPORT) || (aid != U_OTRANSPORT))
                    {
                        byte k = *((byte*)((uintptr_t)atk + S_KILLS));
                        if (k < 255)k++;
                        set_stat(atk, (int)k, S_KILLS);
                        byte to = *((byte*)((uintptr_t)trg + S_OWNER));
                        byte ao = *((byte*)((uintptr_t)atk + S_OWNER));
                        if ((tid <= U_OWALL) && (to <= 15) && (ao <= 15))
                        {
                            if (all_kills[ao * 16 * U_OWALL + to * U_OWALL + tid] < 255)
                                all_kills[ao * 16 * U_OWALL + to * U_OWALL + tid]++;
                        }
                        if (war2mod)
                        {
                            if (tid == U_CRITTER)
                            {
                                if (aid < U_FARM)
                                {
                                    byte axx = *((byte*)((uintptr_t)atk + S_X));
                                    byte ayy = *((byte*)((uintptr_t)atk + S_Y));
                                    byte txx = *((byte*)((uintptr_t)trg + S_X));
                                    byte tyy = *((byte*)((uintptr_t)trg + S_Y));
                                    int rx = abs(axx - txx);
                                    int ry = abs(ayy - tyy);
                                    if ((rx <= 1) && (ry <= 1))
                                    {
                                        heal(atk, 255, 0);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void damage1(int* atk, int* trg, char dmg)
{
    if (replaced)
        damage(atk, trg, dmg);
    else fdmg = dmg;
    ((void (*)(int*, int*, char))g_proc_00409F3B)(atk, trg, fdmg);
}

void damage2(int* atk, int* trg, char dmg)
{
    if (replaced)
        damage(atk, trg, dmg);
    else fdmg = dmg;
    ((void (*)(int*, int*, char))g_proc_0041038E)(atk, trg, fdmg);
}

void damage3(int* atk, int* trg, char dmg)
{
    if (replaced)
        damage(atk, trg, dmg);
    else fdmg = dmg;
    ((void (*)(int*, int*, char))g_proc_0040AF70)(atk, trg, fdmg);
}

void damage4(int* atk, int* trg, char dmg)
{
    if (replaced)
        damage(atk, trg, dmg);
    else fdmg = dmg;
    ((void (*)(int*, int*, char))g_proc_0040AF99)(atk, trg, fdmg);
}

void damage5(int* atk, int* trg, char dmg)
{
    if (replaced)
        damage(atk, trg, dmg);
    else fdmg = dmg;
    ((void (*)(int*, int*, char))g_proc_00410762)(atk, trg, fdmg);
}

void damage6(int* atk, int* trg, char dmg)
{
    if (replaced)
        damage(atk, trg, dmg);
    else fdmg = dmg;
    ((void (*)(int*, int*, char))g_proc_004428AD)(atk, trg, fdmg);
}

bool check_campanign(byte c1, byte c2, byte c3, byte c4)
{
    FILE* fp;
    if ((fp = fopen(RECORD_PATH, "rb")) != NULL)//file opened
    {
        unsigned char buf[5] = { 0 };//CRC of completed maps
        unsigned char buf2[5] = { c1,c2,c3,c4,0 };
        while (!feof(fp))
        {
            fread(buf, sizeof(unsigned char), 4, fp);//read

            if ((buf[0] == buf2[0]) && (buf[1] == buf2[1]) && (buf[2] == buf2[2]) && (buf[3] == buf2[3]))
            {
                fclose(fp);
                return true;
            }//if found
        }
        fclose(fp);
    }
    return false;
}

void tower_set_target(int* p, int x, int y)
{
    set_stat(p, 0, S_RETARGET_X1 - 2);
    set_stat(p, 0, S_RETARGET_X1 - 1);
    int* u = NULL;
    set_region(x - 3, y - 3, x, y);
    find_all_alive_units(ANY_BUILDING_4x4);
    sort_in_region();
    sort_hidden();
    sort_attack_can_hit(p);
    if (units != 0)u = unit[0];
    set_region(x - 2, y - 2, x, y);
    find_all_alive_units(ANY_BUILDING_3x3);
    sort_in_region();
    sort_hidden();
    sort_attack_can_hit(p);
    if (units != 0)u = unit[0];
    set_region(x - 1, y - 1, x, y);
    find_all_alive_units(ANY_BUILDING_2x2);
    sort_in_region();
    sort_hidden();
    sort_attack_can_hit(p);
    if (units != 0)u = unit[0];
    set_region(x, y, x, y);
    find_all_alive_units(ANY_UNITS);
    sort_in_region();
    sort_hidden();
    sort_attack_can_hit(p);
    if (units != 0)u = unit[0];
    if (u)
    {
        int fx = ((int (*)(int*, int))F_UNIT_FIXUP)(u, 1);//fixup save
        set_stat(p, (int)fx % 256, S_RETARGET_X1 - 2);
        set_stat(p, (int)fx / 256, S_RETARGET_X1 - 1);
    }
}

PROC g_proc_0043BAE1;
void rc_snd(int* p)
{
    if (*((byte*)((uintptr_t)p + S_ID)) < U_FARM)
        ((void (*)(int*))g_proc_0043BAE1)(p);
}

PROC g_proc_0043B943;
void rc_build_click(int* p,int x,int y,int* t,int a)
{
    byte id = *(byte*)((uintptr_t)p + S_ID);
    if (id >= U_FARM)
    {
        if ((id == U_HARROWTOWER) || (id == U_OARROWTOWER) 
            || (id == U_HCANONTOWER) || (id == U_OCANONTOWER)
            || (id == U_HTOWER) || (id == U_OTOWER))
        {
            if (A_tower)
                tower_set_target(p, x, y);
        }
        else
        {
            if (A_rally)
            {
                set_stat(p, x | 128, S_RETARGET_X1 - 2);
                set_stat(p, y, S_RETARGET_X1 - 1);
                set_stat(p, 0, S_RETARGET_ORDER - 1);
                bool h = false;
                set_region(x - 3, y - 3, x, y);
                find_all_alive_units(U_MINE);
                sort_in_region();
                if (units != 0)h = true;
                find_all_alive_units(ANY_BUILDING);
                sort_stat(S_ID, U_HPLATFORM, CMP_BIGGER_EQ);
                sort_stat(S_ID, U_OPLATFORM, CMP_SMALLER_EQ);
                sort_in_region();
                if (units != 0)h = true;

                struct GPOINT
                {
                    WORD x;
                    WORD y;
                };
                GPOINT l;
                l.x = (WORD)x;
                l.y = (WORD)y;
                int tr1 = ((int (*)(GPOINT*))F_TILE_IS_TREE)(&l);
                int tr2 = ((int (*)(GPOINT*))F_TILE_IS_CHOPPING_TREE)(&l);
                if ((tr1 == 1) || (tr2 == 1))h = true;
                if (h)set_stat(p, 1, S_RETARGET_ORDER - 1);
            }
        }
    }
    else
        ((void (*)(int*, int, int, int*, int))g_proc_0043B943)(p, x, y, t, a);//original
}

void rc_jmp(bool b)
{
    if (b)
    {
        char r[] = "\xf\x84\xa2\x0\x0\x0";
        PATCH_SET((char*)RIGHT_CLICK_1, r);
        patch_ljmp((char*)RIGHT_CLICK_CODE_CAVE, (char*)RIGHT_CLICK_2);
    }
    else
    {
        char r[] = "\xf\x84\x8b\x0\x0\x0";
        PATCH_SET((char*)RIGHT_CLICK_1, r);
    }
}

void brclik(bool b)
{
    if (b)
    {
        char r[] = "\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)RIGHT_CLICK_ALLOW_BUILDINGS, r);
        rc_jmp(true);
    }
    else
    {
        char r[] = "\xf\x84\x26\x01\x0\x0";
        PATCH_SET((char*)RIGHT_CLICK_ALLOW_BUILDINGS, r);
        rc_jmp(false);
    }
}

PROC g_proc_0040DF71;
int* bld_unit_create(int a1,int a2,int a3,byte a4,int* a5)
{
    int *b = (int*)*(int*)UNIT_RUN_UNIT_POINTER;
    int* u = ((int* (*)(int, int, int, byte, int*))g_proc_0040DF71)(a1, a2, a3, a4, a5);
    if (replaced)
    {
        if (b)
        {
            if (u)
            {
                byte x = *((byte*)((uintptr_t)b + S_RETARGET_X1 - 2));
                byte y = *((byte*)((uintptr_t)b + S_RETARGET_X1 - 1));
                byte ro = *((byte*)((uintptr_t)b + S_RETARGET_ORDER - 1));
                byte bp = x & 128;
                if (bp != 0)
                {
                    x &= ~128;
                    byte uid = *((byte*)((uintptr_t)u + S_ID));
                    byte o = ORDER_ATTACK_AREA;
                    if ((uid == U_PEON) || (uid == U_PEASANT) || (uid == U_HTANKER) || (uid == U_OTANKER))
                    {
                        if (ro == 1)o = ORDER_HARVEST;
                    }
                    give_order(u, x, y, o);
                    set_stat(u, x, S_RETARGET_X1);
                    set_stat(u, y, S_RETARGET_Y1);
                    set_stat(u, o, S_RETARGET_ORDER);
                }
                if (ai_fixed)
                {
                    byte o = *((byte*)((uintptr_t)u + S_OWNER));
                    byte m = *((byte*)((uintptr_t)u + S_MANA));
                    if ((*(byte*)(CONTROLER_TYPE + o) == C_COMP))
                    {
                        if (m == 0x55)//85 default starting mana
                        {
                            char buf[] = "\xA0";//160
                            PATCH_SET((char*)u + S_MANA, buf);
                        }
                    }
                }
            }
        }
    }
    return u;
}

PROC g_proc_0040AFBF;
int* tower_find_attacker(int* p)
{
    int* tr = NULL;
    byte id = *((byte*)((uintptr_t)p + S_ID));
    if ((id == U_HARROWTOWER) || (id == U_OARROWTOWER) || (id == U_HCANONTOWER) || (id == U_OCANONTOWER))
    {
        byte a1 = *((byte*)((uintptr_t)p + S_RETARGET_X1 - 2));
        byte a2 = *((byte*)((uintptr_t)p + S_RETARGET_X1 - 1));
        tr = (int*)(a1 + 256 * a2);
        tr = ((int* (*)(int*, int))F_UNIT_FIXUP)(tr, 0);//fixup load
        if (tr)
        {
            if (!check_unit_near_death(tr) && !check_unit_dead(tr) && !check_unit_hidden(tr))
            {
                int a = ((int(*)(int*, int*))F_ATTACK_CAN_HIT)(p, tr);
                if (a != 0)
                {
                    byte id = *((byte*)((uintptr_t)tr + S_ID));
                    byte szx = *(byte*)(UNIT_SIZE_TABLE + 4 * id);
                    byte szy = *(byte*)(UNIT_SIZE_TABLE + 4 * id + 2);
                    byte idd = *((byte*)((uintptr_t)p + S_ID));
                    byte rng = *(byte*)(UNIT_RANGE_TABLE + idd);
                    byte ms = *(byte*)MAP_SIZE;
                    byte xx = *((byte*)((uintptr_t)tr + S_X));
                    byte yy = *((byte*)((uintptr_t)tr + S_Y));
                    byte x1 = *((byte*)((uintptr_t)p + S_X));
                    byte y1 = *((byte*)((uintptr_t)p + S_Y));
                    byte x2 = x1;
                    byte y2 = y1;
                    if (x1 < rng)x1 = 0;
                    else x1 -= rng;
                    if (y1 < rng)y1 = 0;
                    else y1 -= rng;
                    if ((x2 + rng + 1) > ms)x2 = ms;
                    else x2 += rng + 1;
                    if ((y2 + rng + 1) > ms)y2 = ms;
                    else y2 += rng + 1;
                    if (!((xx >= x1) && (xx <= x2) && (yy >= y1) && (yy <= y2)))
                        tr = NULL;
                }
            }
            else
                tr = NULL;
        }
    }
    if (!tr)
        return ((int* (*)(int*))g_proc_0040AFBF)(p);//original
    else
    {
        return tr;
    }
}

PROC g_proc_00451728;
void unit_kill_deselect(int* u)
{
    int* ud = u;
    ((void (*)(int*))g_proc_00451728)(u);//original
    ((void (*)())F_STATUS_REDRAW)();//status redraw
    for (int i = 0; i < 16; i++)
    {
        int* p = (int*)(UNITS_LISTS + 4 * i);
        if (p)
        {
            p = (int*)(*p);
            while (p)
            {
                byte id = *((byte*)((uintptr_t)p + S_ID));
                bool f = ((id == U_HARROWTOWER) || (id == U_OARROWTOWER)
                    || (id == U_HCANONTOWER) || (id == U_OCANONTOWER));
                bool f2 = ((id == U_DWARWES) || (id == U_GOBLINS));
                if (f)
                {
                    if (!check_unit_dead(p) && check_unit_complete(p))
                    {
                        byte a1 = *((byte*)((uintptr_t)p + S_RETARGET_X1));
                        byte a2 = *((byte*)((uintptr_t)p + S_RETARGET_X1 + 1));
                        byte a3 = *((byte*)((uintptr_t)p + S_RETARGET_X1 + 2));
                        byte a4 = *((byte*)((uintptr_t)p + S_RETARGET_X1 + 3));
                        int* tr = (int*)(a1 + 256 * a2 + 256 * 256 * a3 + 256 * 256 * 256 * a4);
                        if (tr == ud)
                        {
                            set_stat(p, 0, S_RETARGET_X1);
                            set_stat(p, 0, S_RETARGET_X1 + 1);
                            set_stat(p, 0, S_RETARGET_X1 + 2);
                            set_stat(p, 0, S_RETARGET_X1 + 3);
                        }
                    }
                }
                if (f2)
                {
                    if (ai_fixed)
                    {
                        if (!check_unit_dead(p))
                        {
                            byte a1 = *((byte*)((uintptr_t)p + S_ORDER_UNIT_POINTER));
                            byte a2 = *((byte*)((uintptr_t)p + S_ORDER_UNIT_POINTER + 1));
                            byte a3 = *((byte*)((uintptr_t)p + S_ORDER_UNIT_POINTER + 2));
                            byte a4 = *((byte*)((uintptr_t)p + S_ORDER_UNIT_POINTER + 3));
                            int* tr = (int*)(a1 + 256 * a2 + 256 * 256 * a3 + 256 * 256 * 256 * a4);
                            if (tr == ud)
                            {
                                set_stat(p, 0, S_ORDER_UNIT_POINTER);
                                set_stat(p, 0, S_ORDER_UNIT_POINTER + 1);
                                set_stat(p, 0, S_ORDER_UNIT_POINTER + 2);
                                set_stat(p, 0, S_ORDER_UNIT_POINTER + 3);
                                give_order(p, 0, 0, ORDER_STOP);
                            }
                        }
                    }
                }
                p = (int*)(*((int*)((uintptr_t)p + S_NEXT_UNIT_POINTER)));
            }
        }
    }
}

void first_th(bool f)
{
    if (f)
    {
        char rep[] = "\x90\x90";
        PATCH_SET((char*)FIRST_TH_FIX, rep);
    }
    else
    {
        char rep[] = "\x75\x7";
        PATCH_SET((char*)FIRST_TH_FIX, rep);
    }
}

void multicast_fix(bool f)
{
    if (f)
    {
        char rep[] = "\xeb";
        PATCH_SET((char*)MULTICAST_FIX, rep);
    }
    else
    {
        char rep[] = "\x74";
        PATCH_SET((char*)MULTICAST_FIX, rep);
    }
}

PROC g_proc_0044FF20;
PROC g_proc_0044FFE6;
int pathfind_mov(int* um, int s)
{
    if (path_fixed)
    {
        byte x = *((byte*)((uintptr_t)um + S_X - S_MOV_PATH01 + 1));
        byte y = *((byte*)((uintptr_t)um + S_Y - S_MOV_PATH01 + 1));
        byte ox = *((byte*)((uintptr_t)um + S_ORDER_X - S_MOV_PATH01 + 1));
        byte oy = *((byte*)((uintptr_t)um + S_ORDER_Y - S_MOV_PATH01 + 1));
        byte xx = abs(x - ox);
        byte yy = abs(y - oy);
        byte ss = ((xx < yy) ? xx : yy);
        if (ss <= 8)s = 1;
    }
    return ((int (*)(int*, int))g_proc_0044FF20)(um, s);//original
}

void pathfind_fix(bool f)
{
    if (f)
    {
        char rep[] = "\x1";//cheap path
        PATCH_SET((char*)PATHFIND_CHEAP_PATH, rep);
        char rep2[] = "\xeb\x41\x90";
        PATCH_SET((char*)PATHFIND_FIX, rep2);
        char rep3[] = "\x66\x81\xf9\x0\x4\xeb\xb8";//1024 buffer
        PATCH_SET((char*)PATHFIND_FIX_BUFFER, rep3);
        path_fixed = true;
    }
    else
    {
        char rep[] = "\x7";
        PATCH_SET((char*)PATHFIND_CHEAP_PATH, rep);
        char rep2[] = "\x83\xf9\x32";
        PATCH_SET((char*)PATHFIND_FIX, rep2);
        char rep3[] = "\x90\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)PATHFIND_FIX_BUFFER, rep3);
        path_fixed = false;
    }
}

void allow_cheats(bool f)
{
    if (f)
    {
        char rep[] = "\xeb";
        PATCH_SET((char*)CHEATS_ALLOWED, rep);
    }
    else
    {
        char rep[] = "\x74";
        PATCH_SET((char*)CHEATS_ALLOWED, rep);
    }
}

void allow_speed(bool f)
{
    if (f)
    {
        char rep[] = "\x90\x90\x90\x90\x90\x90";
        PATCH_SET((char*)ALLOW_SPEED_UP, rep);
        char rep2[] = "\x90\x90";
        PATCH_SET((char*)ALLOW_SPEED_DOWN, rep2);
        PATCH_SET((char*)ALLOW_SPEED_CHANGES, rep2);
    }
    else
    {
        char rep[] = "\xf\x85\x8e\x0\x0\x0";
        PATCH_SET((char*)ALLOW_SPEED_UP, rep);
        char rep2[] = "\x75\x62";
        PATCH_SET((char*)ALLOW_SPEED_DOWN, rep2);
        char rep3[] = "\x75\xc";
        PATCH_SET((char*)ALLOW_SPEED_CHANGES, rep3);
    }
}

void allow_table(byte p, int t, byte n, byte a)
{
    if (t == 0)t = ALLOWED_UNITS + (4 * p) + (n / 8);
    else if (t == 1)t = ALLOWED_UPGRADES + (4 * p) + (n / 8);
    else if (t == 2)t = ALLOWED_SPELLS + (4 * p) + (n / 8);
    else t = SPELLS_LEARNED + (4 * p) + (n / 8);
    byte b = *(byte*)t;
    if (a == 1)b |= (1 << (n % 8));
    else b &= (~(1 << (n % 8)));
    char buf[] = "\xff";
    buf[0] = b;
    PATCH_SET((char*)t, buf);
}

void draw_stats_fix(bool b)
{
    if (b)
    {
        char buf[] = "\xa0\x5b";
        PATCH_SET((char*)DEMON_STATS_DRAW, buf);//demon
        PATCH_SET((char*)CRITTER_STATS_DRAW, buf);//critter
    }
    else
    {
        char buf[] = "\xf0\x57";
        PATCH_SET((char*)DEMON_STATS_DRAW, buf);//demon
        PATCH_SET((char*)CRITTER_STATS_DRAW, buf);//critter
    }
}

void send_cheat(byte c)
{
    int b = *(int*)CHEATBITS;
    if ((b & (1 << c)) != 0)
        b &= ~(1 << c);
    else
        b |= 1 << c;
    ((void (*)(int))F_SEND_CHEAT_PACKET)(b);
}

void rec_autoheal()
{
    byte p = *(byte*)PACKET_PLAYER;//player
    byte local = *(byte*)LOCAL_PLAYER;
    byte b = *(byte*)(SPELLS_LEARNED + 4 * p);
    byte sp = GREATER_HEAL;
    if ((b & (1 << sp)) != 0)
        b &= ~(1 << sp);
    else
        b |= 1 << sp;
    char buf[] = "\x0";
    buf[0] = b;
    PATCH_SET((char*)(SPELLS_LEARNED + 4 * p), buf);
    if (ogre_mode)PATCH_SET((char*)(SPELLS_LEARNED + 4 * ogre_find_player(p)), buf);

    if ((*(byte*)(SPELLS_LEARNED + 4 * (*(byte*)LOCAL_PLAYER)) & (1 << L_GREATER_HEAL)) != 0)
    {
        churc[20 * 3 + 2] = '\x5b';//icon
        if (p == local)
        {
            char msg[] = "autoheal\x5 enabled";
            show_message(10, msg);
        }
    }
    else
    {
        churc[20 * 3 + 2] = '\x6d';//icon
        if (p == local)
        {
            char msg[] = "autoheal\x3 disabled";
            show_message(10, msg);
        }
    }
    ((void (*)())F_STATUS_REDRAW)();
}

void rec_peons()
{
    int* p = NULL;
    byte l = *(byte*)PACKET_PLAYER;//player
    byte local = *(byte*)LOCAL_PLAYER;
    p = (int*)(UNITS_SELECTED + 9 * 4 * l);
    if (p)
    {
        p = (int*)(*p);
        if (p)
        {
            byte id = *((byte*)((uintptr_t)p + S_ID));
            bool fid = (id == U_TOWN_HALL) || (id == U_GREAT_HALL) ||
                (id == U_KEEP) || (id == U_STRONGHOLD) ||
                (id == U_CASTLE) || (id == U_FORTRESS);
            if (fid)
            {
                byte o = *((byte*)((uintptr_t)p + S_OWNER));
                bool ff = (o == l);
                if (ogre_mode)ff = (o == ogre_find_player(l));
                if (ff)
                {
                    byte x = *((byte*)((uintptr_t)p + S_X));
                    byte y = *((byte*)((uintptr_t)p + S_Y));
                    set_region((int)x - 5, (int)y - 5, (int)x + 8, (int)y + 8);//set region around myself rad 5
                    find_all_alive_units(ANY_MEN);
                    sort_in_region();
                    sort_hidden();
                    sort_stat(S_OWNER, o, CMP_EQ);//my owner
                    sort_stat(S_ID, U_ATTACK_PEASANT, CMP_BIGGER_EQ);
                    sort_stat(S_ID, U_ATTACK_PEON, CMP_SMALLER_EQ);
                    if (units != 0)
                    {
                        for (int i = 0; i < units; i++)
                        {
                            byte id = *((byte*)((uintptr_t)unit[i] + S_ID));//unit id
                            WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * id);//max hp
                            WORD hp = *((WORD*)((uintptr_t)unit[i] + S_HP));//unit hp
                            byte idd = id % 2;
                            WORD mhp2 = *(WORD*)(UNIT_HP_TABLE + 2 * (U_PEASANT + idd));//max hp
                            int thp = hp + (mhp2 - mhp);
                            if (thp < 1)thp = 1;
                            hp = thp % (256 * 256);
                            set_stat(unit[i], hp, S_HP);
                            set_stat(unit[i], (U_PEASANT + idd), S_ID);
                            set_stat(unit[i], (U_PEASANT + idd), S_COMMANDS);
                            set_stat(unit[i], (U_PEASANT + idd), S_SPRITE);
                            set_stat(unit[i], ORDER_STOP, S_NEXT_ORDER);
                        }
                        if (l == local)
                        {
                            char msg[] = "workers attack mode\x3 disabled";
                            show_message(10, msg);
                        }
                    }
                    else
                    {
                        find_all_alive_units(ANY_MEN);
                        sort_in_region();
                        sort_hidden();
                        sort_stat(S_OWNER, o, CMP_EQ);//my owner
                        sort_stat(S_ID, U_PEASANT, CMP_BIGGER_EQ);
                        sort_stat(S_ID, U_PEON, CMP_SMALLER_EQ);
                        if (units != 0)
                        {
                            for (int i = 0; i < units; i++)
                            {
                                byte id = *((byte*)((uintptr_t)unit[i] + S_ID));//unit id
                                WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * id);//max hp
                                WORD hp = *((WORD*)((uintptr_t)unit[i] + S_HP));//unit hp
                                byte idd = id % 2;
                                WORD mhp2 = *(WORD*)(UNIT_HP_TABLE + 2 * (U_ATTACK_PEASANT + idd));//max hp
                                int thp = hp + (mhp2 - mhp);
                                if (thp < 1)thp = 1;
                                hp = thp % (256 * 256);
                                set_stat(unit[i], hp, S_HP);
                                set_stat(unit[i], (U_ATTACK_PEASANT + idd), S_ID);
                                set_stat(unit[i], (U_ATTACK_PEASANT + idd), S_COMMANDS);
                                set_stat(unit[i], (U_PEASANT + idd), S_SPRITE);
                                set_stat(unit[i], ORDER_STOP, S_NEXT_ORDER);
                            }
                            if (l == local)
                            {
                                char msg[] = "workers attack mode\x5 enabled";
                                show_message(10, msg);
                            }
                        }
                    }
                }
            }
        }
    }
    ((void (*)())F_STATUS_REDRAW)();
}

void rec_res(byte pl, byte res, WORD kol)
{
    byte p = *(byte*)PACKET_PLAYER;//player who sended
    byte local = *(byte*)LOCAL_PLAYER;
    char msg[60];
    byte receiver = pl;
    byte sender = *(byte*)(PLAYERS_COLORS + p);
    for (int i = 0; i < 8; i++)
    {
        byte col = *(byte*)(PLAYERS_COLORS + i);
        if (pl == col)
            receiver = i;
    }
    if (ogre_mode)
    {
        if (p == local)
        {
            sprintf(msg, "\x3 Cannot send resources in ogre mode!");
            show_message(20, msg);
        }
    }
    else
    {
        if (*(byte*)(CONTROLER_TYPE + receiver) != C_NOBODY)
        {
            if (get_player_team(receiver) != 9)
            {
                if (get_player_team(sender) != 9)
                {
                    if (cmp_res(p, res, kol % 256, kol / 256, 0, 0, CMP_BIGGER_EQ))
                    {
                        change_res(p, res + 3, 1, kol);
                        change_res(receiver, res, 1, kol);
                        char* ress = NULL;
                        char* color = NULL;
                        char* scolor = NULL;
                        ress = str_res[res];
                        color = str_colors[sender];
                        scolor = str_colors[pl];
                        if (receiver == local)
                        {
                            sprintf(msg, "Received %d %s from %s player", kol, ress, color);
                            show_message(20, msg);
                        }
                        if (p == local)
                        {
                            sprintf(msg, "Sent %d %s to %s player", kol, ress, scolor);
                            show_message(20, msg);
                        }
                    }
                    else
                    {
                        if (p == local)
                        {
                            sprintf(msg, "\x3 Not enough resources!");
                            show_message(20, msg);
                        }
                    }
                }
                else
                {
                    if (p == local)
                    {
                        sprintf(msg, "\x3 Not for OBS!");
                        show_message(20, msg);
                    }
                }
            }
            else
            {
                if (p == local)
                {
                    sprintf(msg, "\x3 Cannot send res to OBS!");
                    show_message(20, msg);
                }
            }
        }
        else
        {
            if (p == local)
            {
                sprintf(msg, "\x3 We not have this player color in current game!");
                show_message(20, msg);
            }
        }
    }
}

void rec_commands(byte type, byte pl)
{
    byte p = *(byte*)PACKET_PLAYER;//player who sended
    byte local = *(byte*)LOCAL_PLAYER;
    byte phost = *(byte*)PLAYER_HOST;
    if (type == 1)
    {
        byte receiver = pl;
        for (int i = 0; i < 8; i++)
        {
            if (pl == *(byte*)(PLAYERS_COLORS + i))
                receiver = i;
        }
        if (*(byte*)(CONTROLER_TYPE + receiver) != C_NOBODY)
        {
            if ((*(byte*)(CONTROLER_TYPE + receiver) == C_COMP) || (get_player_team(receiver) == 9))//comp or obs
            {
                char msg[] = "                                                                     ";
                sprintf(msg, "\x4%s\x5 player was \x3removed\x5 by \x4%s\x5 player",
                    str_colors[pl], str_colors[*(byte*)(PLAYERS_COLORS + p)]);
                show_message(20, msg);
                if (receiver == local)
                {
                    char aa[] = "                                           ";
                    sprintf(msg, "\x5You was \x3removed\x5 by \x4%s\x5 player!", str_colors[p]);
                    ((void (*)(char*, int))F_SHOW_OK_DIALOG)(aa, MENU_MODAL_DIALOG);//show dialog
                }
                else
                {
                    find_all_alive_units(ANY_UNITS);
                    sort_stat(S_OWNER, receiver, CMP_EQ);
                    remove_all();
                }
            }
            else
            {
                if (p == local)
                {
                    char msgn[] = "\x3 You can remove only OBS player or AI";
                    show_message(20, msgn);
                }
            }
        }
        else
        {
            if (p == local)
            {
                char msgn[] = "\x3 We not have this player color in current game!";
                show_message(20, msgn);
            }
        }
    }
    if (type == 2)
    {
        byte receiver = pl;
        for (int i = 0; i < 8; i++)
        {
            if (pl == *(byte*)(PLAYERS_COLORS + i))
                receiver = i;
        }
        if (ogre_mode)
        {
            if (p == local)
            {
                char msgn[] = "\x3 Cannot become obs in ogre mode!";
                show_message(20, msgn);
            }
        }
        else
        {
            if (get_player_team(receiver) != 9)
            {
                if (p == receiver)
                {
                    char msg[] = "                                                                     ";
                    sprintf(msg, "\x4%s\x5 player surrendered and was changed to \x3OBS\x5", str_colors[pl]);
                    show_message(20, msg);
                    find_all_alive_units(ANY_MEN);
                    sort_stat(S_OWNER, receiver, CMP_EQ);
                    order_all(0, 0, ORDER_STOP);
                    unit_convert(receiver, U_HARROWTOWER, U_HTOWER, 0);
                    unit_convert(receiver, U_HCANONTOWER, U_HTOWER, 0);
                    unit_convert(receiver, U_OARROWTOWER, U_OTOWER, 0);
                    unit_convert(receiver, U_OCANONTOWER, U_OTOWER, 0);
                    for (int i = 0; i < 8; i++)
                    {
                        ally_one_sided(i, receiver, 1);
                        viz_obs(receiver, i);
                    }
                    byte x = 8, y = 8;
                    find_all_alive_units(ANY_UNITS);
                    if (units != 0)
                    {
                        x = *((byte*)((uintptr_t)unit[0] + S_X));
                        y = *((byte*)((uintptr_t)unit[0] + S_Y));
                    }
                    int* u = unit_create_place(x, y, U_PEASANT, receiver);
                    if (u == NULL)u = unit_create_place(x, y, U_FLYER, receiver);
                    if (u != NULL)
                    {
                        unit_hide(u);
                        ((void (*)(int*))F_UNIT_UNPLACE)(u);//unplace
                    }
                    set_res(receiver, 0, 0, 0, 0, 0);
                    set_res(receiver, 1, 0, 0, 0, 0);
                    set_res(receiver, 2, 0, 0, 0, 0);
                    set_player_team(receiver, 9);
                    obs_lose[receiver] = 1;
                }
            }
        }
    }
}

PROC g_proc_004763FA;
void receive_cheat(int c,int a1)
{
    bool f = true;
    if ((c & (1 << 6)) != 0)//6 - commands
    {
        byte type = 3 & (c >> 12);
        byte pl = 7 & (c >> 14);
        rec_commands(type, pl);
        f = false;
    }
    if (replaced)
    {
        if ((c & (1 << 8)) != 0)//8 - autoheal
        {
            rec_autoheal();
            f = false;
        }
        if ((c & (1 << 9)) != 0)//9 - attack peons
        {
            rec_peons();
            f = false;
        }
        if ((c & (1 << 7)) != 0)//7 - send resources
        {
            byte pl = 7 & (c >> 12);
            byte res = 3 & (c >> 15);
            WORD kol= 8191 & (c >> 17);
            rec_res(pl, res, kol);
            f = false;
        }
        if (f)
        {
            char ac = *(char*)CHEATS_ALLOWED;
            if (ac == '\xeb')
            {
                c &= 133183;//0 on screen;1 fast build;2 god;3 magic;4 upgrades;5 money;11 hatchet;17 disco
                ((void (*)(int, int))g_proc_004763FA)(c, a1);//orig
            }
            else
            {
                byte l = *(byte*)PACKET_PLAYER;//player
                find_all_alive_units(ANY_UNITS);
                sort_stat(S_OWNER, l, CMP_EQ);
                kill_all();
            }
        }
        else
        {
            char buf[] = "\x0";
            PATCH_SET((char*)PLAYER_CHEATED, buf);
        }
    }
    else
    {
        if (f)
        {
            byte l = *(byte*)PACKET_PLAYER;//player
            find_all_alive_units(ANY_UNITS);
            sort_stat(S_OWNER, l, CMP_EQ);
            kill_all();
        }
        else
        {
            char buf[] = "\x0";
            PATCH_SET((char*)PLAYER_CHEATED, buf);
        }
    }
}

void button_autoheal(int)
{
    send_cheat(8);
    if ((*(byte*)(SPELLS_LEARNED + 4 * (*(byte*)LOCAL_PLAYER)) & (1 << L_GREATER_HEAL)) != 0)
        churc[20 * 3 + 2] = '\x5b';//icon
    else
        churc[20 * 3 + 2] = '\x6d';//icon
    ((void (*)())F_STATUS_REDRAW)();
}

void autoheal(bool b)
{
    if (b)
    {
        if ((*(byte*)(SPELLS_LEARNED + 4 * (*(byte*)LOCAL_PLAYER)) & (1 << L_GREATER_HEAL)) != 0)
            churc[20 * 3 + 2] = '\x5b';//icon
        else
            churc[20 * 3 + 2] = '\x6d';//icon

        void (*r1) (int) = button_autoheal;
        patch_setdword((DWORD*)(churc + (20 * 3 + 8)), (DWORD)r1);

        char b1[] = "\04\x0\x0\x0\x68\x37\x4a\x0";
        char* repf = churc;
        patch_setdword((DWORD*)(b1 + 4), (DWORD)repf);
        PATCH_SET((char*)CHURCH_BUTTONS, b1);
        A_autoheal = true;
    }
    else
    {
        char b1[] = "\03\x0\x0\x0\x68\x37\x4a\x0";
        PATCH_SET((char*)CHURCH_BUTTONS, b1);
        A_autoheal = false;
    }
}

void button_peons(int)
{
    send_cheat(9);
    ((void (*)())F_STATUS_REDRAW)();
}

char Oth_buttons[7 * BUTTON_SIZE + 1];
char Hth_buttons[7 * BUTTON_SIZE + 1];

void buttons_init_th_double()
{
    int (*r) (byte) = empty_true;
    int (*rr3) (byte) = empty_upgrade_th;
    void (*r1) (int) = empty_build;
    void (*r2) (int) = empty_build_research;
    void (*r5) (int) = empty_build_upgrade_self;
    int k = 0;

    Oth_buttons[20 * k + 0] = '\x0';//button id?
    Oth_buttons[20 * k + 1] = '\x0';//button id?
    Oth_buttons[20 * k + 2] = '\x1';//icon
    Oth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 4)), (DWORD)r);
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 8)), (DWORD)r1);
    Oth_buttons[20 * k + 12] = U_PEON;//arg
    Oth_buttons[20 * k + 13] = U_PEON;//unit id
    Oth_buttons[20 * k + 14] = '\xD';//string from tbl
    Oth_buttons[20 * k + 15] = '\x1';//string from tbl
    Oth_buttons[20 * k + 16] = '\x0';//flags?
    Oth_buttons[20 * k + 17] = '\x0';//flags?
    Oth_buttons[20 * k + 18] = '\x0';//flags?
    Oth_buttons[20 * k + 19] = '\x0';//flags?
    k++;
    Oth_buttons[20 * k + 0] = '\x1';//button id?
    Oth_buttons[20 * k + 1] = '\x0';//button id?
    Oth_buttons[20 * k + 2] = '\x43';//icon
    Oth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 4)), (DWORD)rr3);
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 8)), (DWORD)r5);
    Oth_buttons[20 * k + 12] = '\x0';//arg
    Oth_buttons[20 * k + 13] = U_STRONGHOLD;//unit id
    Oth_buttons[20 * k + 14] = '\x3E';//string from tbl
    Oth_buttons[20 * k + 15] = '\x1';//string from tbl
    Oth_buttons[20 * k + 16] = '\x0';//flags?
    Oth_buttons[20 * k + 17] = '\x0';//flags?
    Oth_buttons[20 * k + 18] = '\x0';//flags?
    Oth_buttons[20 * k + 19] = '\x0';//flags?
    k++;
    Oth_buttons[20 * k + 0] = '\x1';//button id?
    Oth_buttons[20 * k + 1] = '\x0';//button id?
    Oth_buttons[20 * k + 2] = '\x45';//icon
    Oth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 4)), (DWORD)rr3);
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 8)), (DWORD)r5);
    Oth_buttons[20 * k + 12] = '\x1';//arg
    Oth_buttons[20 * k + 13] = U_FORTRESS;//unit id
    Oth_buttons[20 * k + 14] = '\x3C';//string from tbl
    Oth_buttons[20 * k + 15] = '\x1';//string from tbl
    Oth_buttons[20 * k + 16] = '\x0';//flags?
    Oth_buttons[20 * k + 17] = '\x0';//flags?
    Oth_buttons[20 * k + 18] = '\x0';//flags?
    Oth_buttons[20 * k + 19] = '\x0';//flags?
    k++;
    Oth_buttons[20 * k + 0] = '\x2';//button id?
    Oth_buttons[20 * k + 1] = '\x0';//button id?
    Oth_buttons[20 * k + 2] = '\x0';//icon
    Oth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 4)), (DWORD)r);
    patch_setdword((DWORD*)(Oth_buttons + (20 * k + 8)), (DWORD)r1);
    Oth_buttons[20 * k + 12] = U_PEASANT;//arg
    Oth_buttons[20 * k + 13] = U_PEASANT;//unit id
    Oth_buttons[20 * k + 14] = '\xE';//string from tbl
    Oth_buttons[20 * k + 15] = '\x1';//string from tbl
    Oth_buttons[20 * k + 16] = '\x0';//flags?
    Oth_buttons[20 * k + 17] = '\x0';//flags?
    Oth_buttons[20 * k + 18] = '\x0';//flags?
    Oth_buttons[20 * k + 19] = '\x0';//flags?
    k++;

    char b1[] = "\x0\x0\x0\x0\x0\0\x0\x0";
    b1[0] = k;
    patch_setdword((DWORD*)(b1 + 4), (DWORD)Oth_buttons);
    PATCH_SET((char*)BUTTONS_CARDS + 8 * U_GREAT_HALL, b1);
    PATCH_SET((char*)BUTTONS_CARDS + 8 * U_STRONGHOLD, b1);
    PATCH_SET((char*)BUTTONS_CARDS + 8 * U_FORTRESS, b1);

    k = 0;

    Hth_buttons[20 * k + 0] = '\x0';//button id?
    Hth_buttons[20 * k + 1] = '\x0';//button id?
    Hth_buttons[20 * k + 2] = '\x0';//icon
    Hth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 4)), (DWORD)r);
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 8)), (DWORD)r1);
    Hth_buttons[20 * k + 12] = U_PEASANT;//arg
    Hth_buttons[20 * k + 13] = U_PEASANT;//unit id
    Hth_buttons[20 * k + 14] = '\xE';//string from tbl
    Hth_buttons[20 * k + 15] = '\x1';//string from tbl
    Hth_buttons[20 * k + 16] = '\x0';//flags?
    Hth_buttons[20 * k + 17] = '\x0';//flags?
    Hth_buttons[20 * k + 18] = '\x0';//flags?
    Hth_buttons[20 * k + 19] = '\x0';//flags?
    k++;
    Hth_buttons[20 * k + 0] = '\x1';//button id?
    Hth_buttons[20 * k + 1] = '\x0';//button id?
    Hth_buttons[20 * k + 2] = '\x42';//icon
    Hth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 4)), (DWORD)rr3);
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 8)), (DWORD)r5);
    Hth_buttons[20 * k + 12] = '\x0';//arg
    Hth_buttons[20 * k + 13] = U_KEEP;//unit id
    Hth_buttons[20 * k + 14] = '\x3D';//string from tbl
    Hth_buttons[20 * k + 15] = '\x1';//string from tbl
    Hth_buttons[20 * k + 16] = '\x0';//flags?
    Hth_buttons[20 * k + 17] = '\x0';//flags?
    Hth_buttons[20 * k + 18] = '\x0';//flags?
    Hth_buttons[20 * k + 19] = '\x0';//flags?
    k++;
    Hth_buttons[20 * k + 0] = '\x1';//button id?
    Hth_buttons[20 * k + 1] = '\x0';//button id?
    Hth_buttons[20 * k + 2] = '\x44';//icon
    Hth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 4)), (DWORD)rr3);
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 8)), (DWORD)r5);
    Hth_buttons[20 * k + 12] = '\x1';//arg
    Hth_buttons[20 * k + 13] = U_CASTLE;//unit id
    Hth_buttons[20 * k + 14] = '\x3B';//string from tbl
    Hth_buttons[20 * k + 15] = '\x1';//string from tbl
    Hth_buttons[20 * k + 16] = '\x0';//flags?
    Hth_buttons[20 * k + 17] = '\x0';//flags?
    Hth_buttons[20 * k + 18] = '\x0';//flags?
    Hth_buttons[20 * k + 19] = '\x0';//flags?
    k++;
    Hth_buttons[20 * k + 0] = '\x2';//button id?
    Hth_buttons[20 * k + 1] = '\x0';//button id?
    Hth_buttons[20 * k + 2] = '\x1';//icon
    Hth_buttons[20 * k + 3] = '\x0';//icon
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 4)), (DWORD)r);
    patch_setdword((DWORD*)(Hth_buttons + (20 * k + 8)), (DWORD)r1);
    Hth_buttons[20 * k + 12] = U_PEON;//arg
    Hth_buttons[20 * k + 13] = U_PEON;//unit id
    Hth_buttons[20 * k + 14] = '\xD';//string from tbl
    Hth_buttons[20 * k + 15] = '\x1';//string from tbl
    Hth_buttons[20 * k + 16] = '\x0';//flags?
    Hth_buttons[20 * k + 17] = '\x0';//flags?
    Hth_buttons[20 * k + 18] = '\x0';//flags?
    Hth_buttons[20 * k + 19] = '\x0';//flags?
    k++;

    b1[0] = k;
    patch_setdword((DWORD*)(b1 + 4), (DWORD)Hth_buttons);
    PATCH_SET((char*)BUTTONS_CARDS + 8 * U_TOWN_HALL, b1);
    PATCH_SET((char*)BUTTONS_CARDS + 8 * U_KEEP, b1);
    PATCH_SET((char*)BUTTONS_CARDS + 8 * U_FORTRESS, b1);
}

void th_change(bool b)
{
    //add new buttons to TH
    if (b)
    {
        if (war2_game_mode == 3)//3 - double race mode
        {
            buttons_init_th_double();
        }
        else
        {
            heroes(true);
            char hth[] = "\x5";
            PATCH_SET((char*)HUMAN_TH1_BUTTONS, hth);
            PATCH_SET((char*)HUMAN_TH2_BUTTONS, hth);
            PATCH_SET((char*)HUMAN_TH3_BUTTONS, hth);
            char oth[] = "\x5\x0\x0\x0\x20\xf7\x48\x0";
            char* repf = oth_build;
            patch_setdword((DWORD*)(oth + 4), (DWORD)repf);
            PATCH_SET((char*)ORC_TH1_BUTTONS, oth);
            PATCH_SET((char*)ORC_TH2_BUTTONS, oth);
            PATCH_SET((char*)ORC_TH3_BUTTONS, oth);

            bool af = false;
            for (int i = 0; i < 8; i++)
            {
                if (heros[i] != 0)af = true;
            }
            if (af)
            {
                patch_setdword((DWORD*)(hth_build + 4), (DWORD)F_ALWAYS_TRUE);
            }
            else
            {
                int (*repf) (byte) = empty_false;
                patch_setdword((DWORD*)(hth_build + 4), (DWORD)repf);
            }
            af = false;
            for (int i = 8; i < 16; i++)
            {
                if (heros[i] != 0)af = true;
            }
            if (af)
            {
                patch_setdword((DWORD*)(oth_build + 64), (DWORD)F_ALWAYS_TRUE);
            }
            else
            {
                int (*repf) (byte) = empty_false;
                patch_setdword((DWORD*)(oth_build + 64), (DWORD)repf);
            }

            if (apn)//if can build attack peons
            {
                patch_setdword((DWORD*)(hth_build + 24), (DWORD)F_ALWAYS_TRUE);
                patch_setdword((DWORD*)(oth_build + 84), (DWORD)F_ALWAYS_TRUE);

                void (*repf) (int) = button_peons;
                patch_setdword((DWORD*)(hth_build + 28), (DWORD)repf);
                patch_setdword((DWORD*)(oth_build + 88), (DWORD)repf);

                char bufb[] = "\xa\x0\x0\x0\x38\x1e\x4a\x0\xa\x0\x0\x0\x0\x1f\x4a\x0";
                PATCH_SET((char*)HUMAN_TH_ONE_BUTTON, bufb);
            }
            else
            {
                int (*repf) (byte) = empty_false;
                patch_setdword((DWORD*)(hth_build + 24), (DWORD)repf);
                patch_setdword((DWORD*)(oth_build + 84), (DWORD)repf);

                char bufb[] = "\x2\x0\x0\x0\xc8\x1f\x4a\x0\x2\x0\x0\x0\xf0\x1f\x4a\x0";
                PATCH_SET((char*)HUMAN_TH_ONE_BUTTON, bufb);
            }

            PATCH_SET((char*)HUMAN_TH_COMMON, hth_build);
        }
    }
    else
    {
        heroes(false);
        char hth[] = "\x3\x0\x0\x0\x38\x34\x4a\x0";
        PATCH_SET((char*)HUMAN_TH1_BUTTONS, hth);
        PATCH_SET((char*)HUMAN_TH2_BUTTONS, hth);
        PATCH_SET((char*)HUMAN_TH3_BUTTONS, hth);
        char oth[] = "\x3\x0\x0\x0\x78\x34\x4a\x0";
        PATCH_SET((char*)ORC_TH1_BUTTONS, oth);
        PATCH_SET((char*)ORC_TH2_BUTTONS, oth);
        PATCH_SET((char*)ORC_TH3_BUTTONS, oth);
        char p1[] = "\x0\x0\x0\x0\x0\x0\x1\x0\xa0\x44\x44\x0\xd0\xe6\x40\x0\x1\x3\xd\x1\x0\x0\x0\x0\x1\x0\x43\x0\xb0\x42\x44\x0\x10\xe7\x40\x0\x0\x59\x3e\x1\x0\x0\x0\x0";
        PATCH_SET((char*)HUMAN_TH_COMMON, p1);
        char bufb[] = "\x2\x0\x0\x0\xc8\x1f\x4a\x0\x2\x0\x0\x0\xf0\x1f\x4a\x0";
        PATCH_SET((char*)HUMAN_TH_ONE_BUTTON, bufb);
    }
}

void comps_ffa(bool f)
{
    if (f)
    {
        char buf[] = "\x0";
        PATCH_SET((char*)COMPS_FFA1, buf);
        PATCH_SET((char*)COMPS_FFA2, buf);
        PATCH_SET((char*)COMPS_FFA3, buf);
    }
    else
    {
        char buf[] = "\x18";
        PATCH_SET((char*)COMPS_FFA1, buf);
        PATCH_SET((char*)COMPS_FFA2, buf);
        PATCH_SET((char*)COMPS_FFA3, buf);
    }
}

void call_default()
{
    byte l = *(byte*)LOCAL_PLAYER;
    if (*(byte*)GB_MULTIPLAYER == 1)
    {
        if (ogre_mode)
        {
            if (real_local != 255)
                if (!slot_alive(ogre_find_player(l)))lose(false);
        }
        bool fc = true;
            for (int i = 0; i < 8; i++)
            {
                if ((*(byte*)(CONTROLER_TYPE + i) == C_PLAYER) || (*(byte*)(CONTROLER_TYPE + i) == C_COMP))
                {
                    if (check_opponents(i))fc = false;
                }
            }
        if (fc)
        {
            for (int i = 0; i < 8; i++)
            {
                if ((*(byte*)(CONTROLER_TYPE + i) == C_COMP) || (obs_lose[i] != 0))
                {
                    find_all_alive_units(ANY_UNITS);
                    sort_stat(S_OWNER, i, CMP_EQ);
                    remove_all();
                }
            }
        }
        bool plr = true;
        for (int i = 0; i < 8; i++)
        {
            if (*(byte*)(CONTROLER_TYPE + i) == C_PLAYER)
            {
                if (get_player_team(i) != 9)plr = false;
            }
        }
        if (plr)comps_ffa(true);
        ((void (*)())F_MULTIPLAYER_DEFAULT)();//orig war2 func original multiplayer victory function
    }
    else
    {
        //singleplayer
        if (!slot_alive(l))lose(true);
        else
        {
            bool f = true;
            for (int i = 0; i < 8; i++)
            {
                if (i != l)
                {
                    if (slot_alive(i))f = false;
                }
            }
            if (f)win(true);
        }
    }
}

void trig_default()
{
    call_default();
    first_step = false;
}

void trig()
{
    bool awin = false;
    bool alose = false;
    bool wasend = false;
    units = 0;//units array considered empty every time
    vizs_n = 0;//no reveal map areas every time
    char message[60] = { 0 };
    char gval[] = "\x0";
    DWORD val[256] = { 0 };//values for triggers uses
    memset(val, 0, sizeof(val));
    byte multi = *(byte*)GB_MULTIPLAYER;
    byte local = *(byte*)LOCAL_PLAYER;
    int COM = 0, add = 0;
    int flag = 0;//IF..END flag (skip commands)
    char alarm[] = "\x0";
    for (int i = 0; i < 16; i++)
    {
        alarm[0] = *(byte*)(GB_HORSES + i);
        if (alarm[0] > 0)alarm[0] -= 1;//all timers going
        PATCH_SET((char*)(GB_HORSES + i), alarm);
        
    }
    alarm[0] = '\x0';
    for (byte i = 0; i < 8; i++)
    {
        if ((*(byte*)(CONTROLER_TYPE + i) == C_COMP) && !slot_alive(i))
        {
            char sdd[] = "\x3"; sdd[0] = C_NOBODY;
            PATCH_SET((char*)(CONTROLER_TYPE + i), sdd);//place nobody (3) in dead comps slot
        }
    }
    if (!first_step)
    {
        if (prev)
        {
            char prev_mes[] = "\x5 Complete previous campanign map\x3 before playing this!!!";
            show_message(20, prev_mes);
            show_message(20, prev_mes);
            show_message(20, prev_mes);
            lose(true);
        }
        if (campanign && (*(byte*)BNE_GAME_TYPE != 10))
        {
            char prev_mes[] = "\x5 Campanign maps can only be played in\x3 UMS mode!!!";
            show_message(20, prev_mes);
            show_message(20, prev_mes);
            show_message(20, prev_mes);
            lose(true);
        }
    }
    else
    {
        if (campanign)first_th(true);
    }
    
    while ((t[COM] != 0) && (COM < MCOM))//triggers byte code read cycle
    {
        if (flag == 0)//if not in skip mode (IF..END IF)
        {
            switch (t[COM])//check id of command
            {
            case 0: COM = MCOM; break;
            case 1: // victory/loss show table
                table = t[COM + 1] == 1;
                COM += 2; break;
            case 2:
                if (multi)
                    set_mission(t[COM + 1]);//set level id to show mission obj
                COM += 2; break;
            case 3: ally(t[COM + 1], t[COM + 2], t[COM + 3]);//ally 2 pl
                COM += 4; break;
            case 4: viz(t[COM + 1], t[COM + 2], t[COM + 3]);//viz 2 pl
                COM += 4; break;
            case 5: val[t[COM + 1]] = (int)t[COM + 2];//set val
                COM += 3; break;
            case 6: //array of units actions
                add = 0;
                if (t[COM + 1] == 0)
                {
                    find_all_alive_units(t[COM + 2]);
                    add = 1;
                }
                if (t[COM + 1] == 1)sort_in_region();
                if (t[COM + 1] == 2)
                {
                    sort_stat(t[COM + 2], t[COM + 3] + 256 * t[COM + 4], t[COM + 5]);
                    add = 4;
                }
                if (t[COM + 1] == 3)
                {
                    set_stat_all(t[COM + 2], t[COM + 3] + 256 * t[COM + 4]);
                    add = 3;
                }
                if (t[COM + 1] == 4)kill_all();
                if (t[COM + 1] == 5)flame_all();
                if (t[COM + 1] == 6)
                {
                    heal_all(t[COM + 2], 0);
                    add = 1;
                }
                if (t[COM + 1] == 7)sort_complete();
                if (t[COM + 1] == 8)
                {
                    move_all(t[COM + 2], t[COM + 3]);
                    add = 2;
                }
                if (t[COM + 1] == 9)
                {
                    give_all(t[COM + 2]);
                    add = 1;
                }
                if (t[COM + 1] == 10)
                {
                    if (units > (t[COM + 2] + 256 * t[COM + 3]))
                        units = (t[COM + 2] + 256 * t[COM + 3]);
                    add = 2;
                }
                if (t[COM + 1] == 11)cast_all();
                if (t[COM + 1] == 12)
                {
                    order_all(t[COM + 3], t[COM + 4], t[COM + 2]);
                    add = 3;
                }
                if (t[COM + 1] == 13)
                {
                    damag_all(t[COM + 2], t[COM + 3]);
                    add = 2;
                }
                if (t[COM + 1] == 14)remove_all();
                if (t[COM + 1] == 15)
                {
                    peon_load_all(t[COM + 2]);
                    add = 1;
                }
                if (t[COM + 1] == 16)
                {
                    sort_peon_loaded(t[COM + 2]);
                    add = 1;
                }
                if (t[COM + 1] == 17)
                {
                    sort_peon_not_loaded(t[COM + 2]);
                    add = 1;
                }
                if (t[COM + 1] == 18)
                {
                    viz_area_all(t[COM + 2], t[COM + 3]);
                    add = 2;
                }
                if (t[COM + 1] == 19)
                {
                    if (units != 0)
                    {
                        byte x = *((byte*)((uintptr_t)unit[0] + S_X));
                        byte y = *((byte*)((uintptr_t)unit[0] + S_Y));
                        byte id = *((byte*)((uintptr_t)unit[0] + S_ID));
                        byte w = *(byte*)(UNIT_SIZE_TABLE + 4 * id) - 1;
                        byte h = *(byte*)(UNIT_SIZE_TABLE + 4 * id + 2) - 1;
                        byte s = t[COM + 2];
                        set_region((int)x - s, (int)y - s, (int)x + w + s, (int)y + h + s);
                    }
                    add = 1;
                }
                if (t[COM + 1] == 20)
                {
                    for (int i = 0; i < units; i++)
                    {
                        bullet_create_unit(unit[i], t[COM + 2]);
                    }
                    add = 1;
                }
                if (t[COM + 1] == 21)
                {
                    for (int i = 0; i < units; i++)
                    {
                        bullet_create8_around_unit(unit[i], t[COM + 2]);
                    }
                    add = 1;
                }
                if (t[COM + 1] == 22)
                {
                    sort_stat(t[COM + 2], val[t[COM + 3]], t[COM + 4]);
                    add = 3;
                }
                if (t[COM + 1] == 23)
                {
                    if (units != 0)
                    {
                        gamesound_play(t[COM + 2] + 256 * t[COM + 3],
                            get_stat(unit[0], S_X), get_stat(unit[0], S_Y));
                    }
                    add = 2;
                }
                if (t[COM + 1] == 24)
                {
                    sort_ally(t[COM + 2], t[COM + 3] == 1);
                    add = 2;
                }
                if (t[COM + 1] == 25)
                {
                    damag_all_by_player(t[COM + 2], t[COM + 3], t[COM + 4]);
                    add = 3;
                }
                if (t[COM + 1] == 26)sort_not_in_region();
                if (t[COM + 1] == 27)
                {
                    move_all(val[t[COM + 2]] % 256, val[t[COM + 3]] % 256);
                    add = 2;
                }
                COM += add;
                COM += 2; break;
            case 7://val_id players_flags local_neutral_flags info_id (add info_id to val[id] from all players in flags)
                if ((1 & t[COM + 3]) != 0)
                    val[t[COM + 1]] += t_val(t[COM + 4], 15);
                if ((2 & t[COM + 3]) != 0)
                {
                    if (t[COM + 2] == 0)
                        val[t[COM + 1]] += t_val(t[COM + 4], local);
                    for (byte i = 0; i < 8; i++)
                    {
                        if (i != local)
                        {
                            if (((1 << i) & t[COM + 2]) != 0)
                            {
                                val[t[COM + 1]] += t_val(t[COM + 4], i);
                            }
                        }
                    }
                }
                else
                {
                    for (byte i = 0; i < 8; i++)
                    {
                        if (((1 << i) & t[COM + 2]) != 0)
                        {
                            val[t[COM + 1]] += t_val(t[COM + 4], i);
                        }
                    }
                }
                COM += 5; break;
            case 8://val_id cmp_id cmp_val gflag
                gflag[t[COM + 4]] = cmp_args4(t[COM + 2], val[t[COM + 1]], val[t[COM + 3]]);
                COM += 5; break;
            case 9://set region
                reg[0] = t[COM + 1];
                reg[1] = t[COM + 2];
                reg[2] = t[COM + 3];
                reg[3] = t[COM + 4];
                COM += 5; break;
            case 10: comps_vision(t[COM + 1] == 1);//comps can give viz too
                COM += 2; break;
            case 11: //save val1 to global_val2
                gval[0] = val[t[COM + 1]] % 256;
                if ((t[COM + 2] >= 0) && (t[COM + 2] < 32))
                    PATCH_SET((char*)(GOLD_TOTAL + 32 + t[COM + 2]), gval);
                if ((t[COM + 2] >= 32) && (t[COM + 2] < 64))
                    PATCH_SET((char*)(LUMBER_TOTAL + 32 + t[COM + 2] - 32), gval);
                if ((t[COM + 2] >= 64) && (t[COM + 2] < 96))
                    PATCH_SET((char*)(OIL_TOTAL + 32 + t[COM + 2] - 64), gval);
                COM += 3; break;
            case 12: //load val1 from global_val2
                if ((t[COM + 2] >= 0) && (t[COM + 2] < 32))
                    val[t[COM + 1]] = *(unsigned char*)(GOLD_TOTAL + 32 + t[COM + 2]);
                if ((t[COM + 2] >= 32) && (t[COM + 2] < 64))
                    val[t[COM + 1]] = *(unsigned char*)(LUMBER_TOTAL + 32 + t[COM + 2] - 32);
                if ((t[COM + 2] >= 64) && (t[COM + 2] < 96))
                    val[t[COM + 1]] = *(unsigned char*)(OIL_TOTAL + 32 + t[COM + 2] - 64);
                COM += 3; break;
            case 13: //val1+val2
                val[t[COM + 1]] += val[t[COM + 2]];
                if (val[t[COM + 1]] > 255)val[t[COM + 1]] = 255;
                COM += 3; break;
            case 14: //val1-val2
                if (val[t[COM + 1]] > val[t[COM + 2]])
                    val[t[COM + 1]] -= val[t[COM + 2]];
                else
                    val[t[COM + 1]] = 0;
                COM += 3; break;
            case 15: //val1*val2
                val[t[COM + 1]] *= val[t[COM + 2]];
                if (val[t[COM + 1]] > 255)val[t[COM + 1]] = 255;
                COM += 3; break;
            case 16: //val1 div val2 (if not 0)
                if (val[t[COM + 2]] != 0)
                    val[t[COM + 1]] = val[t[COM + 1]] / val[t[COM + 2]];
                COM += 3; break;
            case 17://val1 mod val2 (if not 0)
                if (val[t[COM + 2]] != 0)
                    val[t[COM + 1]] = val[t[COM + 1]] % val[t[COM + 2]];
                COM += 3; break;
            case 18: //plr unit to_who (all units on map)
                unit_convert(t[COM + 1], t[COM + 2], t[COM + 3], 0);
                COM += 4; break;
            case 19: //x y unit owner value
                unit_create(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4], val[t[COM + 5]] % 256);
                COM += 6; break;
            case 20: //x y unit owner amount
                unit_create(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4], t[COM + 5]);
                COM += 6; break;
            case 21:
                gflag[t[COM + 1]] = t[COM + 2] == 1;//set flag
                COM += 3; break;
            case 22://if
                if (!gflag[t[COM + 1]])
                    flag += 1;
                COM += 2; break;
            case 23://endif
                COM += 1; break;
            case 24://win
                awin = true;
                wasend = true;
                COM += 1; break;
            case 25://lose
                alose = true;
                wasend = true;
                COM += 1; break;
            case 26://resourses
                change_res(t[COM + 1], t[COM + 2], t[COM + 3], val[t[COM + 4]]);
                COM += 5; break;
            case 27://sheep farm activate
                sheep(true);
                COM += 1; break;
            case 28://3 build button activate
                build3(true);
                COM += 1; break;
            case 29://call default multiplayer trigger
                call_default();
                wasend = true;
                COM += 1; break;
            case 30://count units
                val[t[COM + 1]] = units;
                COM += 2; break;
            case 31://manacost spell mana
                manacost(t[COM + 1], t[COM + 2]);
                COM += 3; break;
            case 32://move player view
                center_view(t[COM + 1], t[COM + 2]);
                COM += 3; break;
            case 33://false if
                if (gflag[t[COM + 1]])
                    flag += 1;
                COM += 2; break;
            case 34://set resource (player, res , 4bytes)
                set_res(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4], t[COM + 5], t[COM + 6]);
                COM += 7; break;
            case 35://check resource (player, res , 4bytes, cmp)
                gflag[t[COM + 8]] = cmp_res(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4], t[COM + 5], t[COM + 6], t[COM + 7]);
                COM += 9; break;
            case 36://clear chat
                clear_chat();
                COM += 1; break;
            case 37://show_message
                if ((t[COM + 2] > 0) && (t[COM + 2] <= 60))
                {
                    for (int i = 0; i < t[COM + 2]; i++)
                    {
                        message[i] = t[COM + 3 + i];
                    }
                    char gmessage[64] = { '\x0' };
                    sprintf(gmessage, message, val[0]);
                    show_message(t[COM + 1], gmessage);
                    memset(message, 0, sizeof(message));
                    COM += t[COM + 2];
                }
                COM += 3; break;
            case 38://speedhack
                speedhack(t[COM + 1]);
                COM += 2; break;
            case 39://only unit1 can damage unit2 (only hadgar can damage portal)
                ua[t[COM + 1]] = t[COM + 2];
                ut[t[COM + 1]] = t[COM + 3];
                COM += 4; break;
            case 40: repair_all(t[COM + 1] == 1);//peon can rep all units
                COM += 2; break;
            case 41: A_runestone = true;//super runestone
                COM += 1; break;
            case 42: runes[t[COM + 1]] = t[COM + 2];//super runestone flags
                COM += 3; break;
            case 43: upgr(t[COM + 1], t[COM + 2]);//upgrades upgr_id power
                COM += 3; break;
            case 44://timer
                alarm[0] = *(byte*)(GB_HORSES + t[COM + 1]);
                if (alarm[0] == 0)
                {
                    alarm[0] = t[COM + 2];
                    PATCH_SET((char*)(GB_HORSES + t[COM + 1]), alarm);
                }
                gflag[t[COM + 3]] = alarm[0] == 1;
                COM += 4; break;
            case 45: //check comp opponents and players av
                if (t[COM + 1] == 8)
                    gflag[t[COM + 2]] = check_opponents(local);
                else
                    gflag[t[COM + 2]] = check_opponents(t[COM + 1]);
                COM += 3; break;
            case 46: //allied comp aggro
                agr = t[COM + 1] == 1;
                COM += 2; break;
            case 47: //check ally
                gflag[t[COM + 3]] = check_ally(t[COM + 1], t[COM + 2]);
                COM += 4; break;
            case 48: //capture low hp buildings
                cpt = t[COM + 1] == 1;
                COM += 2; break;
            case 49: //only peon can capture
                pcpt = t[COM + 1] == 1;
                COM += 2; break;
            case 50: //th captured, capture all
                thcpt = t[COM + 1] == 1;
                COM += 2; break;
            case 51: A_portal = true;//super portal
                COM += 1; break;
            case 52: repair_cat(t[COM + 1] == 1);//peon can rep cats
                COM += 2; break;
            case 53: wharf();//super wharf
                COM += 1; break;
            case 54: aport = t[COM + 1] == 1;//portal only tp allies
                COM += 2; break;
            case 55: mport = t[COM + 1] == 1;//portal only tp if mage nearby
                COM += 2; break;
            case 56:
                if (multi)
                    allow_cheats(true);//allow singleplayer cheats in mp
                COM += 1; break;
            case 57: demon(true);//can create demons from circle
                COM += 1; break;
            case 58: heros[t[COM + 1]] = t[COM + 2];//heros (slot_id hero_id)
                COM += 3; break;
            case 59: autoheal(true);//autoheal
                COM += 1; break;
            case 60://skip (this comand used in replace stage)
                COM += 1; break;
            case 61: fireball_dmg(t[COM + 1]);//fireball damage
                COM += 2; break;
            case 62: buff_time(t[COM + 1], t[COM + 2], t[COM + 3]);//buffs time
                COM += 4; break;
            case 63: A_transport = true;//allied transportation
                COM += 1; break;
            case 64: send_res = true;//exchange res with other players
                COM += 1; break;
            case 65: b3rune = t[COM + 1] == 1;//can build runestone in build3
                COM += 2; break;
            case 66: b3port = t[COM + 1] == 1;//can build portal in build3
                COM += 2; break;
            case 67: b3cirl = t[COM + 1] == 1;//can build circle in build3
                COM += 2; break;
            case 68: apn = t[COM + 1] == 1;//can build attack peons in th
                COM += 2; break;
            case 69: steal = t[COM + 1] == 1;//can steal resources
                COM += 2; break;
            case 70: manaburn = t[COM + 1] == 1;//can demon manaburn dmg
                COM += 2; break;
            case 71: peon_steal = true;//allow peons steal from other peons
                COM += 1; break;
            case 72: gflag[t[COM + 1]] = t[COM + 2] == local;//if player local color
                COM += 3; break;
            case 73: allow_table(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4]);//allowed table
                COM += 5; break;
            case 74: //arhon p1 controls p2
                if (multi)
                {
                    arhons[t[COM + 1] * 2] = t[COM + 2];
                    arhons[t[COM + 1] * 2 + 1] = t[COM + 3];
                }
                COM += 4; break;
            case 75: b3mine = t[COM + 1] == 1;//can build gold in build3
                COM += 2; break;
            case 76: b3forest = t[COM + 1] == 1;//can build forest in build3
                COM += 2; break;
            case 77: //rally point
                brclik(true);
                A_rally = true;
                A_tower = true;
                COM += 1; break;
            case 78: //always max damage
                no_random_dmg(true);
                COM += 1; break;
            case 79: //blood always x2
                blood_fix(true);
                COM += 1; break;
            case 80: //allow change game speed
                if (multi)
                    allow_speed(true);
                COM += 1; break;
            case 81: //slow aura
                m_slow_aura[t[COM + 1]] = t[COM + 2];
                COM += 3; break;
            case 82: //death aura
                m_death_aura[t[COM + 1]] = t[COM + 2];
                COM += 3; break;
            case 83: //devotion aura
                m_devotion[t[COM + 1]] = t[COM + 2];
                COM += 3; break;
            case 84: //vampire aura
                m_vampire[t[COM + 1]] = t[COM + 2];
                COM += 3; break;
            case 85: //prevent loss (survive 1 hp)
                m_prvnt[t[COM + 1]] = t[COM + 2];
                COM += 3; break;
            case 86: //sneak invis
                m_sneak[t[COM + 1]] = t[COM + 2];
                COM += 3; break;
            case 87: //skip (this comand used in replace stage)
                COM += 1; break;
            case 88: fireshield_flyers(t[COM + 1] == 1);//allow fireshield flyers
                COM += 2; break;
            case 89: more_res = true;//more resources adding from buildings
                COM += 1; break;
            case 90: //multicast
                multicast_fix(true);
                COM += 1; break;
            case 91: ally_one_sided(t[COM + 1], t[COM + 2], t[COM + 3]);//ally 1 pl
                COM += 4; break;
            case 92: viz_one_sided(t[COM + 1], t[COM + 2], t[COM + 3]);//viz 1 pl
                COM += 4; break;
            case 93: //skip (this comand used in replace stage)
                COM += 1; break;
            case 94: //skip (this comand used in replace stage)
                COM += 1; break;
            case 95: //remove tiles
                if (t[COM + 1] == 0)tile_remove_trees(t[COM + 2], t[COM + 3]);
                if (t[COM + 1] == 1)tile_remove_rocks(t[COM + 2], t[COM + 3]);
                if (t[COM + 1] == 2)tile_remove_walls(t[COM + 2], t[COM + 3]);
                COM += 4; break;
            case 96: viz_area_add(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4]);//viz area for players bits
                COM += 5; break;
            case 97: //skip (this comand used in replace stage)
                COM += 1; break;
            case 98: //capture low hp uits
                ucpt = t[COM + 1] == 1;
                COM += 2; break;
            case 99: //value get unit stat
                if (units != 0)
                {
                    val[t[COM + 1]] = get_stat(unit[0], t[COM + 2]);
                }
                COM += 3; break;
            case 100://skip (this comand used in replace stage)
                COM += 2; break;
            case 101://win video
                wasend = true;
                win2(table, t[COM + 1]);
                COM += 2; break;
            case 102://lose video
                wasend = true;
                lose2(table, t[COM + 1]);
                COM += 2; break;
            case 103://trigger speed
                trigger_time(t[COM + 1]);
                COM += 2; break;
            case 104://skip (this comand used in replace stage)
                COM += 1; break;
            case 105://skip (this comand used in replace stage)
                COM += 5; break;
            case 106://skip (this comand used in replace stage)
                COM += 3; break;
            case 107://create bullet
            {
                WORD bx = t[COM + 1] + 256 * t[COM + 2];
                WORD by = t[COM + 3] + 256 * t[COM + 4];
                bullet_create(bx, by, t[COM + 5]);
                COM += 6; break;
            }
            case 108://play sound x y
                gamesound_play(t[COM + 1] + 256 * t[COM + 2], t[COM + 3], t[COM + 4]);
                COM += 5; break;
            case 109:
                set_upgrade(t[COM + 1], t[COM + 2], val[t[COM + 3]] % 256);
                COM += 4; break;
            case 110:
                val[t[COM + 3]] = get_upgrade(t[COM + 1], t[COM + 2]);
                COM += 4; break;
            case 111:
                val[t[COM + 1]] = get_all_kills(t[COM + 2], t[COM + 3], t[COM + 4]);
                COM += 5; break;
            case 112:
            {
                WORD rgc = region_center();
                val[t[COM + 1]] = rgc % 256;//value1 = X center of region
                val[t[COM + 2]] = rgc / 256;//value2 = Y center of region
                COM += 3; break;
            }
            case 113: //remove tiles in region
                for (int i = reg[0]; i <= reg[2]; i++)
                {
                    for (int j = reg[1]; j <= reg[3]; j++)
                    {
                        if (t[COM + 1] == 0)tile_remove_trees(i, j);
                        if (t[COM + 1] == 1)tile_remove_rocks(i, j);
                        if (t[COM + 1] == 2)tile_remove_walls(i, j);
                    }
                }
                COM += 2; break;
            case 114:
                b3mine_anywhere = true;
                COM += 1; break;
            case 115:
                b3forest_anywhere = true;
                COM += 1; break;
            case 116:
                b3forest_price = t[COM + 1];
                COM += 2; break;
            case 117:
                gflag[t[COM + 1]] = t[COM + 2] == *(byte*)(CONTROLER_TYPE + t[COM + 3]);//player is hum/comp/nobody
                COM += 4; break;
            case 118: //x y unit owner value
                unit_create(val[t[COM + 1]] % 256, val[t[COM + 2]] % 256, t[COM + 3], t[COM + 4], val[t[COM + 5]] % 256);
                COM += 6; break;
            case 119:
                ships_speed_tower = true;
                COM += 1; break;
            case 120:
            {
                WORD osize = t[COM + 1] + 256 * t[COM + 2];
                if ((osize > 0) && (osize <= 512))
                {
                    mission_objective = (char*)((int)((char*)t) + (COM + 3));
                    COM += osize;
                }
                COM += 3; break;
            }
            case 121:
            {
                WORD osize = t[COM + 2] + 256 * t[COM + 3];
                if ((osize > 0) && (osize <= 48))
                {
                    unit_names[t[COM + 1]] = (char*)((int)((char*)t) + (COM + 4));
                    COM += osize;
                }
                COM += 4; break;
            }
            case 122:
            {
                WORD osize = t[COM + 1] + 256 * t[COM + 2];
                if ((osize > 0) && (osize <= 512))
                    COM += osize;
                COM += 3; break;
            }
            case 123:
            {
                WORD osize = t[COM + 1] + 256 * t[COM + 2];
                if ((osize > 0) && (osize <= 48))
                    COM += osize;
                COM += 3; break;
            }

            case 254: COM = MCOM; break;
            default: COM += 1; break;
            }
        }
        else
        {
            switch (t[COM])//skip commands (IF..END IF mode) 
            {
            case 0: COM = MCOM; break;
            case 1: COM += 2; break;
            case 2: COM += 2; break;
            case 3: COM += 4; break;
            case 4: COM += 4; break;
            case 5: COM += 3; break;
            case 6:
                add = 0;
                if (t[COM + 1] == 0)add = 1;
                if (t[COM + 1] == 2)add = 4;
                if (t[COM + 1] == 3)add = 3;
                if (t[COM + 1] == 6)add = 1;
                if (t[COM + 1] == 8)add = 2;
                if (t[COM + 1] == 9)add = 1;
                if (t[COM + 1] == 10)add = 2;
                if (t[COM + 1] == 12)add = 3;
                if (t[COM + 1] == 13)add = 2;
                if (t[COM + 1] == 15)add = 1;
                if (t[COM + 1] == 16)add = 1;
                if (t[COM + 1] == 17)add = 1;
                if (t[COM + 1] == 18)add = 2;
                if (t[COM + 1] == 19)add = 1;
                if (t[COM + 1] == 20)add = 1;
                if (t[COM + 1] == 21)add = 1;
                if (t[COM + 1] == 22)add = 3;
                if (t[COM + 1] == 23)add = 2;
                if (t[COM + 1] == 24)add = 2;
                if (t[COM + 1] == 25)add = 3;
                if (t[COM + 1] == 27)add = 2;
                COM += add;
                COM += 2; break;
            case 7: COM += 5; break;
            case 8: COM += 5; break;
            case 9: COM += 5; break;
            case 10: COM += 2; break;
            case 11: COM += 3; break;
            case 12: COM += 3; break;
            case 13: COM += 3; break;
            case 14: COM += 3; break;
            case 15: COM += 3; break;
            case 16: COM += 3; break;
            case 17: COM += 3; break;
            case 18: COM += 4; break;
            case 19: COM += 6; break;
            case 20: COM += 6; break;
            case 21: COM += 3; break;
            case 22: flag += 1;//new IF
                COM += 2; break;
            case 23: flag -= 1;//ENDIF
                COM += 1; break;
            case 24: wasend = true;
                COM += 1; break;
            case 25: wasend = true;
                COM += 1; break;
            case 26: COM += 5; break;
            case 27: COM += 1; break;
            case 28: COM += 1; break;
            case 29: wasend = true;
                COM += 1; break;
            case 30: COM += 2; break;
            case 31: COM += 3; break;
            case 32: COM += 3; break;
            case 33: flag += 1;//false if
                COM += 2; break;
            case 34: COM += 7; break;
            case 35: COM += 9; break;
            case 36: COM += 1; break;
            case 37:
                if ((t[COM + 2] > 0) && (t[COM + 2] <= 60))
                    COM += t[COM + 2];
                COM += 3; break;
            case 38: COM += 2; break;
            case 39: COM += 4; break;
            case 40: COM += 2; break;
            case 41: COM += 1; break;
            case 42: COM += 3; break;
            case 43: COM += 3; break;
            case 44: COM += 4; break;
            case 45: COM += 3; break;
            case 46: COM += 2; break;
            case 47: COM += 4; break;
            case 48: COM += 2; break;
            case 49: COM += 2; break;
            case 50: COM += 2; break;
            case 51: COM += 1; break;
            case 52: COM += 2; break;
            case 53: COM += 1; break;
            case 54: COM += 2; break;
            case 55: COM += 2; break;
            case 56: COM += 1; break;
            case 57: COM += 1; break;
            case 58: COM += 3; break;
            case 59: COM += 1; break;
            case 60: COM += 1; break;
            case 61: COM += 2; break;
            case 62: COM += 4; break;
            case 63: COM += 1; break;
            case 64: COM += 1; break;
            case 65: COM += 2; break;
            case 66: COM += 2; break;
            case 67: COM += 2; break;
            case 68: COM += 2; break;
            case 69: COM += 2; break;
            case 70: COM += 2; break;
            case 71: COM += 1; break;
            case 72: COM += 3; break;
            case 73: COM += 5; break;
            case 74: COM += 4; break;
            case 75: COM += 2; break;
            case 76: COM += 2; break;
            case 77: COM += 1; break;
            case 78: COM += 1; break;
            case 79: COM += 1; break;
            case 80: COM += 1; break;
            case 81: COM += 3; break;
            case 82: COM += 3; break;
            case 83: COM += 3; break;
            case 84: COM += 3; break;
            case 85: COM += 3; break;
            case 86: COM += 3; break;
            case 87: COM += 1; break;
            case 88: COM += 2; break;
            case 89: COM += 1; break;
            case 90: COM += 1; break;
            case 91: COM += 4; break;
            case 92: COM += 4; break;
            case 93: COM += 1; break;
            case 94: COM += 1; break;
            case 95: COM += 4; break;
            case 96: COM += 5; break;
            case 97: COM += 1; break;
            case 98: COM += 2; break;
            case 99: COM += 3; break;
            case 100:COM += 2; break;
            case 101: wasend = true;
                COM += 2; break;
            case 102: wasend = true;
                COM += 2; break;
            case 103:COM += 2; break;
            case 104:COM += 1; break;
            case 105:COM += 5; break;
            case 106:COM += 3; break;
            case 107:COM += 6; break;
            case 108:COM += 5; break;
            case 109:COM += 4; break;
            case 110:COM += 4; break;
            case 111:COM += 5; break;
            case 112:COM += 3; break;
            case 113:COM += 2; break;
            case 114:COM += 1; break;
            case 115:COM += 1; break;
            case 116:COM += 2; break;
            case 117:COM += 4; break;
            case 118:COM += 6; break;
            case 119:COM += 1; break;
            case 120:
            {
                WORD osize = t[COM + 1] + 256 * t[COM + 2];
                if ((osize > 0) && (osize <= 512))
                    COM += osize;
                COM += 3; break;
            }
            case 121:
            {
                WORD osize = t[COM + 2] + 256 * t[COM + 3];
                if ((osize > 0) && (osize <= 48))
                    COM += osize;
                COM += 4; break;
            }
            case 122:
            {
                WORD osize = t[COM + 1] + 256 * t[COM + 2];
                if ((osize > 0) && (osize <= 512))
                    COM += osize;
                COM += 3; break;
            }
            case 123:
            {
                WORD osize = t[COM + 1] + 256 * t[COM + 2];
                if ((osize > 0) && (osize <= 48))
                    COM += osize;
                COM += 3; break;
            }

            case 254: COM = MCOM; break;
            default: COM += 1; break;
            }
        }
    }
    th_change(true);
    if (!wasend)call_default();
    if (awin)
    {
        alose = false;
        win(table);
    }
    if (alose)lose(table);
    first_step = false;
    memset(all_kills, 0, sizeof(all_kills));
}

void war2_mod()
{
    units = 0;
    byte local = *(byte*)LOCAL_PLAYER;
    for (byte i = 0; i < 8; i++)
    {
        if ((*(byte*)(CONTROLER_TYPE + i) == C_COMP) && !slot_alive(i))
        {
            char sdd[] = "\x3"; sdd[0] = C_NOBODY;
            PATCH_SET((char*)(CONTROLER_TYPE + i), sdd);//place nobody (3) in dead comps slot
        }
    }

    trigger_time(100);
    upgr(ARROWS, 4);
    fireball_dmg(120);
    fireshield_flyers(true);
    manacost(RUNES, 100);
    manacost(POLYMORPH, 150);
    manacost(INVIS, 125);
    manacost(FLAME_SHIELD, 50);
    manacost(SLOW, 25);
    manacost(FIREBALL, 70);
    manacost(WHIRLWIND, 75);
    manacost(RAISE_DEAD, 25);
    b3rune = true;
    b3port = true;
    b3cirl = true;
    b3mine = true;
    b3mine_anywhere = true;
    b3forest = true;
    b3forest_anywhere = true;
    b3forest_price = 40;
    apn = true;
    manaburn = true;
    steal = true;
    aport = true;
    A_runestone = true;
    A_portal = true;
    A_transport = true;
    send_res = true;
    ships_speed_tower = true;
    m_slow_aura[0] = U_HADGAR;
    m_death_aura[0] = U_TERON;
    m_devotion[0] = U_TYRALYON;
    m_prvnt[0] = U_DENTARG;
    m_vampire[0] = U_LOTHAR;
    m_vampire[1] = U_KARGATH;
    repair_cat(true);
    sheep(true);
    build3(true);
    demon(true);
    autoheal(true);
    multicast_fix(true);

    brclik(true);
    A_rally = true;
    A_tower = true;

    find_all_alive_units(U_ZULJIN);
    heal_all(2, 0);
    find_all_alive_units(U_CHOGAL);
    set_stat_all(S_BLOOD, 1000);

    runes[3] = 1;//haste
    runes[5] = 25;//mana
    runes[6] = 1;//heal
    runes[8] = 1;//ally
    wharf();

    heros[0] = U_DANATH;
    heros[1] = U_ALLERIA;
    heros[2] = U_LOTHAR;
    heros[3] = U_TYRALYON;
    //4
    heros[5] = U_UTER;
    heros[6] = U_HADGAR;
    heros[7] = U_KURDRAN;

    heros[8] = U_GROM;
    heros[9] = U_ZULJIN;
    heros[10] = U_KARGATH;
    heros[11] = U_DENTARG;
    //12
    heros[13] = U_CHOGAL;
    heros[14] = U_TERON;
    heros[15] = U_DEATHWING;

    for (byte i = 0; i < 8; i++)
    {
        find_all_alive_units(U_CRITTER);
        sort_stat(S_OWNER, i, CMP_EQ);
        change_res(i, 0, 1, units);
        change_res(i, 1, 1, units);
    }

    th_change(true);
    call_default();
    first_step = false;
}

void war2_rebalance()
{
    A_transport = true;
    autoheal(true);
    multicast_fix(true);
    A_rally = true;
    brclik(true);
    call_default();
    first_step = false;
}

void war2_double_race()
{
    th_change(true);
    call_default();
    first_step = false;
}

bool in_lobby()
{
    //we in multiplayer lobby(or game) if lobby map name not empty
    return !(CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, MAP_LOBBY, -1, "", -1) == 2);
}

void replace_def()
{
    //set all vars to default
    memset(t, 0, sizeof(t));
    memset(crs, 0, sizeof(crs));
    memset(gflag, false, sizeof(gflag));
    memset(ua, 255, sizeof(ua));
    memset(ut, 255, sizeof(ut));
    memset(runes, 0, sizeof(runes));
    memset(hhero, 0, sizeof(hhero));
    memset(ohero, 0, sizeof(ohero));
    memset(heros, 0, sizeof(heros));
    memset(herosb, false, sizeof(herosb));
    memset(forced_race, 0, sizeof(forced_race));
    memset(m_slow_aura, 255, sizeof(m_slow_aura));
    memset(m_death_aura, 255, sizeof(m_death_aura));
    memset(m_sneak, 255, sizeof(m_sneak));
    memset(m_devotion, 255, sizeof(m_devotion));
    memset(m_vampire, 255, sizeof(m_vampire));
    memset(m_prvnt, 255, sizeof(m_prvnt));
    memset(arhons, 255, sizeof(arhons));
    memset(maxtir, 0, sizeof(maxtir));
    memset(vizs_areas, 0, sizeof(vizs_areas));
    memset(unit_names, 0, sizeof(unit_names));
    mission_objective = NULL;
    mission_briefing = NULL;
    mission_briefing_title = NULL;
    vizs_n = 0;
    first_step = true;
    game_started = false;
    campanign = false;
    prev = false;
    table = false;
    agr = false;
    cpt = false;
    pcpt = false;
    thcpt = false;
    ucpt = false;
    steal = false;
    aport = false;
    mport = false;
    b3rune = false;
    b3port = false;
    b3cirl = false;
    b3mine = false;
    b3mine_anywhere = false;
    b3forest = false;
    b3forest_anywhere = false;
    b3forest_price = 50;
    apn = false;
    manaburn = false;
    A_runestone = false;
    A_portal = false;
    A_transport = false;
    A_autoheal = false;
    A_rally = false;
    A_tower = false;
    blood_f = false;
    send_res = false;
    more_res = false;
    path_fixed = false;
    ai_fixed = false;
    saveload_fixed = false;
    peon_steal = false;
    ships_speed_tower = false;

    real_local = 255;
    fix_chat_full();
}

void replace_common()
{
    //peon can build any buildings
    char ballbuildings[] = "\x0\x0";//d1 05
    PATCH_SET((char*)BUILD_ALL_BUILDINGS1, ballbuildings);
    char ballbuildings2[] = "\x0";//0a
    PATCH_SET((char*)BUILD_ALL_BUILDINGS2, ballbuildings2);
    char ballbuildings3[] = "\x0";//68
    PATCH_SET((char*)BUILD_ALL_BUILDINGS3, ballbuildings3);

    //any building can train any unit
    char ballunits[] = "\xeb";//0x74
    PATCH_SET((char*)BUILD_ALL_UNITS1, ballunits);
    char ballunits2[] = "\xA1\xBC\x47\x49\x0\x90\x90";//8b 04 85 bc 47 49 00
    PATCH_SET((char*)BUILD_ALL_UNITS2, ballunits2);

    //any building can make any research
    char allres[] = "\xB1\x1\x90";
    PATCH_SET((char*)BUILD_ALL_RESEARCH1, allres);
    PATCH_SET((char*)BUILD_ALL_RESEARCH2, allres);

    //allow all units cast all spells
    char allsp[] = "\x90\x90";
    PATCH_SET((char*)CAST_ALL_SPELLS, allsp);

    //show kills
    byte d = S_KILLS;
    char sdmg[] = "\x8a\x90\x82\x0\x0\x0\x8b\xfa";//units
    sdmg[2] = d;
    PATCH_SET((char*)SPEED_STAT_UNITS, sdmg);
    char sdmg2[] = "\x8a\x82\x82\x0\x0\x0\x90\x90\x90";//catas
    sdmg2[2] = d;
    PATCH_SET((char*)SPEED_STAT_CATS, sdmg2);
    char sdmg3[] = "\x8a\x88\x82\x0\x0\x0\x90\x90\x90";//archers
    sdmg3[2] = d;
    PATCH_SET((char*)SPEED_STAT_ARCHERS, sdmg3);
    char sdmg4[] = "\x8a\x82\x82\x0\x0\x0\x90\x90\x90";//berserkers
    sdmg4[2] = d;
    PATCH_SET((char*)SPEED_STAT_BERSERKERS, sdmg4);
    char sdmg5[] = "\x8a\x88\x82\x0\x0\x0\x90\x90\x90";//ships
    sdmg5[2] = d;
    PATCH_SET((char*)SPEED_STAT_SHIPS, sdmg5);

    char dmg_fix[] = "\xeb";
    PATCH_SET((char*)DMG_FIX, dmg_fix);

    th_change(true);
    draw_stats_fix(true);
}

void replace_back()
{
    replace_def();

    set_mission('\xff');
    comps_vision(false);
    sheep(false);
    build3(false);
    demon(false);
    th_change(false);
    repair_all(false);
    repair_cat(false);
    trigger_time('\xc8');
    speedhack('\xd');
    allow_cheats(false);
    allow_speed(false);
    fireball_dmg(40);
    buff_time(0, 0xd0, 0x7);
    buff_time(1, 0xe8, 0x3);
    buff_time(2, 0xf4, 0x1);
    buff_time(3, 0xe8, 0x3);
    buff_time(4, 0xe8, 0x3);
    upgr(SWORDS, 2);
    upgr(ARMOR, 2);
    upgr(ARROWS, 1);
    upgr(SHIP_DMG, 5);
    upgr(SHIP_ARMOR, 5);
    upgr(CATA_DMG, 15);
    manacost(VISION, 70);
    manacost(HEAL, 6);
    manacost(GREATER_HEAL, 5);
    manacost(EXORCISM, 4);
    manacost(FIREBALL, 100);
    manacost(FLAME_SHIELD, 80);
    manacost(SLOW, 50);
    manacost(INVIS, 200);
    manacost(POLYMORPH, 200);
    manacost(BLIZZARD, 25);
    manacost(EYE_OF_KILROG, 70);
    manacost(BLOOD, 50);
    manacost(RAISE_DEAD, 50);
    manacost(COIL, 100);
    manacost(WHIRLWIND, 100);
    manacost(HASTE, 50);
    manacost(UNHOLY_ARMOR, 100);
    manacost(RUNES, 200);
    manacost(DEATH_AND_DECAY, 25);
    no_random_dmg(false);
    blood_fix(false);
    draw_stats_fix(false);
    ogre_init(false);
    brclik(false);
    autoheal(false);
    first_th(false);
    multicast_fix(false);
    pathfind_fix(false);
    ai_fix_plugin(false);
    comps_ffa(false);
    //replace all things back to default

    char ballbuildings[] = "\xd1\x5";
    PATCH_SET((char*)BUILD_ALL_BUILDINGS1, ballbuildings);
    char ballbuildings2[] = "\xa";
    PATCH_SET((char*)BUILD_ALL_BUILDINGS2, ballbuildings2);
    char ballbuildings3[] = "\x68";
    PATCH_SET((char*)BUILD_ALL_BUILDINGS3, ballbuildings3);

    char ballunits[] = "\x74";
    PATCH_SET((char*)BUILD_ALL_UNITS1, ballunits);
    char ballunits2[] = "\x8b\x4\x85\xbc\x47\x49\x0";
    PATCH_SET((char*)BUILD_ALL_UNITS2, ballunits2);

    char allres[] = "\xF\x94\xC1";
    PATCH_SET((char*)BUILD_ALL_RESEARCH1, allres);
    PATCH_SET((char*)BUILD_ALL_RESEARCH2, allres);

    char allsp[] = "\x74\x50";
    PATCH_SET((char*)CAST_ALL_SPELLS, allsp);

    char alwayssave[] = "\x74";
    PATCH_SET((char*)ALWAYS_SAVE, alwayssave);

    //briefing
    char buf1[] = "\x58";
    PATCH_SET((char*)BRIEFING_CHANGE1, buf1);
    char buf2[] = "\x46";
    PATCH_SET((char*)BRIEFING_CHANGE2, buf2);
    char buf3[] = "\x66\xa1\x0\xd3\x4a\x0";
    PATCH_SET((char*)BRIEFING_CHANGE3, buf3);

    char allyall[] = "\x0";
    PATCH_SET((char*)ALL_ALLIED, allyall);

    char mana[] = "\x46\x0\x6\x0\x5\x0\x4\x0\x50\x0\x64\x0\x32\x0\xc8\x0\xc8\x0\x19\x0\x46\x0\x32\x0\x32\x0\x64\x0\x64\x0\x32\x0\x64\x0\xc8\x0\x19\x0";
    PATCH_SET((char*)MANACOST, mana);

    char up[] = "\x1\x2\x2\x5\x5\xa\xf";
    PATCH_SET((char*)UPGRD, up);

    char dmg_fix[] = "\x75";
    PATCH_SET((char*)DMG_FIX, dmg_fix);
    
    char sdmg[] = "\x8a\x50\x27\x33\xc0\x8a\x82\x24";//units
    PATCH_SET((char*)SPEED_STAT_UNITS, sdmg);
    char sdmg2[] = "\x8a\x4a\x27\x8a\x81\x24\xb8\x4b\x0";//catas
    PATCH_SET((char*)SPEED_STAT_CATS, sdmg2);
    char sdmg3[] = "\x8a\x50\x27\x8a\x8a\x24\xb8\x4b\x0";//archers
    PATCH_SET((char*)SPEED_STAT_ARCHERS, sdmg3);
    char sdmg4[] = "\x8a\x4a\x27\x8a\x81\x24\xb8\x4b\x0";//berserkers
    PATCH_SET((char*)SPEED_STAT_BERSERKERS, sdmg4);
    char sdmg5[] = "\x8a\x50\x27\x8a\x8a\x24\xb8\x4b\x0";//ships
    PATCH_SET((char*)SPEED_STAT_SHIPS, sdmg5);

    char loset[] = "\x3b";
    PATCH_SET((char*)LOSE_SHOW_TABLE, loset);
    char wint[] = "\x74";
    PATCH_SET((char*)WIN_SHOW_TABLE, wint);

    //char back[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
    //PATCH_SET((char*)MULTIPLAYER_TRIGGER, back);

    //replace multiplayer trigger
    char rep[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
    void (*repf) () = trig_default;
    patch_setdword((DWORD*)(rep + 6), (DWORD)repf);
    PATCH_SET((char*)MULTIPLAYER_TRIGGER, rep);

    replaced = false;
    war2mod = false;
}

void replace_trigger()
{
    replace_def();

    bool bk = false;//if was some problems replace back
    char c[5] = { 0 };
    FILE* fp;//open map
    if ((fp = fopen(MAP_PATH, "rb")) == NULL)bk = true;
    else
    {
        fseek(fp, 20, 0);
        fread(crs, sizeof(char), 4, fp);//read crs
        while (true)
        {
            fread(c, sizeof(char), 4, fp);//read section name
            if (c[0] == 'O' && c[1] == 'I' && c[2] == 'L' && c[3] == 'M')break;//find OILM
            DWORD sk = 0;
            fread(&sk, 4, 1, fp);//read section size
            fseek(fp, sk, 1);//skip section
            if (feof(fp))
            {
                bk = true;//not found
                break;
            }
        }
        if (!bk)
        {
            DWORD sk = 0;
            fread(&sk, 4, 1, fp);//read section size
            if (sk < MAX_COMMANDS)
                fread(t, sizeof(byte), sk, fp);//read triggers
            else
                fread(t, sizeof(byte), MAX_COMMANDS, fp);//read triggers
        }
        fclose(fp);

        if (t[0] == 0)//if we not have some triggers
            bk = true;
        if (!bk)
        {
            MCOM = MAX_COMMANDS;
            int COM = 0;
            char buf1[] = "\x0";
            char buf2[] = "\x66\xb8\x0\x0\x90\x90";
            char allyall[] = "\x1";
            int add = 0;
            while ((t[COM] != 0) && (COM < MCOM))
            {
                switch (t[COM])
                {
                case 0: COM = MCOM; break;
                case 1: COM += 2; break;
                case 2: COM += 2; break;
                case 3: COM += 4; break;
                case 4: COM += 4; break;
                case 5: COM += 3; break;
                case 6:
                    add = 0;
                    if (t[COM + 1] == 0)add = 1;
                    if (t[COM + 1] == 2)add = 4;
                    if (t[COM + 1] == 3)add = 3;
                    if (t[COM + 1] == 6)add = 1;
                    if (t[COM + 1] == 8)add = 2;
                    if (t[COM + 1] == 9)add = 1;
                    if (t[COM + 1] == 10)add = 2;
                    if (t[COM + 1] == 12)add = 3;
                    if (t[COM + 1] == 13)add = 2;
                    if (t[COM + 1] == 15)add = 1;
                    if (t[COM + 1] == 16)add = 1;
                    if (t[COM + 1] == 17)add = 1;
                    if (t[COM + 1] == 18)add = 2;
                    if (t[COM + 1] == 19)add = 1;
                    if (t[COM + 1] == 20)add = 1;
                    if (t[COM + 1] == 21)add = 1;
                    if (t[COM + 1] == 22)add = 3;
                    if (t[COM + 1] == 23)add = 2;
                    if (t[COM + 1] == 24)add = 2;
                    if (t[COM + 1] == 25)add = 3;
                    if (t[COM + 1] == 27)add = 2;
                    COM += add;
                    COM += 2; break;
                case 7: COM += 5; break;
                case 8: COM += 5; break;
                case 9: COM += 5; break;
                case 10: COM += 2; break;
                case 11: COM += 3; break;
                case 12: COM += 3; break;
                case 13: COM += 3; break;
                case 14: COM += 3; break;
                case 15: COM += 3; break;
                case 16: COM += 3; break;
                case 17: COM += 3; break;
                case 18: COM += 4; break;
                case 19: COM += 6; break;
                case 20: COM += 6; break;
                case 21: COM += 3; break;
                case 22: COM += 2; break;
                case 23: COM += 1; break;
                case 24: COM += 1; break;
                case 25: COM += 1; break;
                case 26: COM += 5; break;
                case 27: COM += 1; break;
                case 28: COM += 1; break;
                case 29: COM += 1; break;
                case 30: COM += 2; break;
                case 31: COM += 3; break;
                case 32: COM += 3; break;
                case 33: COM += 2; break;
                case 34: COM += 7; break;
                case 35: COM += 9; break;
                case 36: COM += 1; break;
                case 37:
                    if ((t[COM + 2] > 0) && (t[COM + 2] <= 60))
                        COM += t[COM + 2];
                    COM += 3; break;
                case 38: COM += 2; break;
                case 39: COM += 4; break;
                case 40: COM += 2; break;
                case 41: COM += 1; break;
                case 42: COM += 3; break;
                case 43: COM += 3; break;
                case 44: COM += 4; break;
                case 45: COM += 3; break;
                case 46: COM += 2; break;
                case 47: COM += 4; break;
                case 48: COM += 2; break;
                case 49: COM += 2; break;
                case 50: COM += 2; break;
                case 51: COM += 1; break;
                case 52: COM += 2; break;
                case 53: COM += 1; break;
                case 54: COM += 2; break;
                case 55: COM += 2; break;
                case 56: COM += 1; break;
                case 57: COM += 1; break;
                case 58: COM += 3; break;
                case 59: COM += 1; break;
                case 60://all players allied from start
                    PATCH_SET((char*)ALL_ALLIED, allyall);
                    COM += 1; break;
                case 61: COM += 2; break;
                case 62: COM += 4; break;
                case 63: COM += 1; break;
                case 64: COM += 1; break;
                case 65: COM += 2; break;
                case 66: COM += 2; break;
                case 67: COM += 2; break;
                case 68: COM += 2; break;
                case 69: COM += 2; break;
                case 70: COM += 2; break;
                case 71: COM += 1; break;
                case 72: COM += 3; break;
                case 73: COM += 5; break;
                case 74: COM += 4; break;
                case 75: COM += 2; break;
                case 76: COM += 2; break;
                case 77: COM += 1; break;
                case 78: COM += 1; break;
                case 79: COM += 1; break;
                case 80: COM += 1; break;
                case 81: COM += 3; break;
                case 82: COM += 3; break;
                case 83: COM += 3; break;
                case 84: COM += 3; break;
                case 85: COM += 3; break;
                case 86: COM += 3; break;
                case 87: //first th fix
                    first_th(true);
                    COM += 1; break;
                case 88: COM += 2; break;
                case 89: COM += 1; break;
                case 90: COM += 1; break;
                case 91: COM += 4; break;
                case 92: COM += 4; break;
                case 93: pathfind_fix(true);
                    COM += 1; break;
                case 94: ai_fix_plugin(true);
                    COM += 1; break;
                case 95: COM += 4; break;
                case 96: COM += 5; break;
                case 97: saveload_fixed = true;
                    COM += 1; break;
                case 98: COM += 2; break;
                case 99: COM += 3; break;
                case 100://briefing
                    PATCH_SET((char*)BRIEFING_CHANGE1, buf1);
                    PATCH_SET((char*)BRIEFING_CHANGE2, buf1);
                    buf2[2] = t[COM + 1];//briefing id
                    PATCH_SET((char*)BRIEFING_CHANGE3, buf2);
                    COM += 2; break;
                case 101: COM += 2; break;
                case 102: COM += 2; break;
                case 103: COM += 2; break;
                case 104: campanign = true;//this is campaning map
                    COM += 1; break;
                case 105: prev = !check_campanign(t[COM + 1], t[COM + 2], t[COM + 3], t[COM + 4]);
                    COM += 5; break;//check if map with given CRC was completed before
                case 106: forced_race[t[COM + 1]] = t[COM + 2];//race for campanign
                    COM += 3; break;
                case 107:COM += 6; break;
                case 108:COM += 5; break;
                case 109:COM += 4; break;
                case 110:COM += 4; break;
                case 111:COM += 5; break;
                case 112:COM += 3; break;
                case 113:COM += 2; break;
                case 114:COM += 1; break;
                case 115:COM += 1; break;
                case 116:COM += 2; break;
                case 117:COM += 4; break;
                case 118:COM += 6; break;
                case 119:COM += 1; break;
                case 120:
                {
                    WORD osize = t[COM + 1] + 256 * t[COM + 2];
                    if ((osize > 0) && (osize <= 512))
                    {
                        mission_objective = (char*)((int)((char*)t) + (COM + 3));
                        COM += osize;
                    }
                    COM += 3; break;
                }
                case 121:
                {
                    WORD osize = t[COM + 2] + 256 * t[COM + 3];
                    if ((osize > 0) && (osize <= 48))
                    {
                        unit_names[t[COM + 1]] = (char*)((int)((char*)t) + (COM + 4));
                        COM += osize;
                    }
                    COM += 4; break;
                }
                case 122:
                {
                    WORD osize = t[COM + 1] + 256 * t[COM + 2];
                    if ((osize > 0) && (osize <= 512))
                    {
                        mission_briefing = (char*)((int)((char*)t) + (COM + 3));
                        COM += osize;
                    }
                    COM += 3; break;
                }
                case 123:
                {
                    WORD osize = t[COM + 1] + 256 * t[COM + 2];
                    if ((osize > 0) && (osize <= 48))
                    {
                        mission_briefing_title = (char*)((int)((char*)t) + (COM + 3));
                        COM += osize;
                    }
                    COM += 3; break;
                }

                case 254: if (t[COM + 1] == 255) MCOM = COM + 2; 
                    COM += 2;
                    break;
                default: COM += 1; break;
                }
            }

            replace_common();

            //can always save game in multiplayer
            char alwayssave[] = "\xeb";//0x74
            PATCH_SET((char*)ALWAYS_SAVE, alwayssave);

            //no draw in multiplayer, only win/lose
            char nodraw[] = "\xeb";//0x74
            PATCH_SET((char*)NO_DRAW, nodraw);

            //replace multiplayer trigger
            char rep[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
            void (*repf) () = trig;
            patch_setdword((DWORD*)(rep + 6), (DWORD)repf);
            PATCH_SET((char*)MULTIPLAYER_TRIGGER, rep);

            replaced = true;
        }
    }
    if (bk)
    {
        replace_back();//replace back if problems to be safe
    }
}

void replace_war2mod()
{
    replace_def();
    replace_common();

    saveload_fixed = true;
    ai_fix_plugin(true);

    //replace multiplayer trigger
    char rep[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
    void (*repf) () = war2_mod;
    patch_setdword((DWORD*)(rep + 6), (DWORD)repf);
    PATCH_SET((char*)MULTIPLAYER_TRIGGER, rep);

    replaced = true;
    war2mod = true;
}

void replace_war2rebalance()
{
    replace_def();
    replace_common();

    saveload_fixed = true;
    ai_fix_plugin(true);

    //replace multiplayer trigger
    char rep[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
    void (*repf) () = war2_rebalance;
    patch_setdword((DWORD*)(rep + 6), (DWORD)repf);
    PATCH_SET((char*)MULTIPLAYER_TRIGGER, rep);

    replaced = true;
}

void replace_war2double_race()
{
    replace_def();
    replace_common();

    //replace multiplayer trigger
    char rep[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
    void (*repf) () = war2_double_race;
    patch_setdword((DWORD*)(rep + 6), (DWORD)repf);
    PATCH_SET((char*)MULTIPLAYER_TRIGGER, rep);

    replaced = true;
}

PROC g_proc_0044A65C;
int status_get_tbl(void* tbl, WORD str_id)
{
    int* u = (int*)*(int*)UNIT_STATUS_TBL;
    if (u != NULL)
    {
        byte id = *((byte*)((uintptr_t)u + S_ID));
        if (id < U_EMPTY_BUTTONS)
        {
            if (str_id < U_EMPTY_BUTTONS)
            {
                if (unit_names[id] != NULL)
                {
                    return (int)unit_names[id];
                }
            }
        }
    }
    return ((int (*)(void*, int))g_proc_0044A65C)(tbl, str_id);//original
}

int* hover_unit;

PROC g_proc_0044AC83;
void unit_hover_get_id(int a, int* b)
{
    if (b != NULL)
    {
        byte id = *((byte*)((uintptr_t)b + 0x20));
        hover_unit = (int*)*(int*)(LOCAL_UNIT_SELECTED_PANEL + 4 * id);
    }
    else
        hover_unit = NULL;
    ((void (*)(int, int*))g_proc_0044AC83)(a, b);//original
}

PROC g_proc_0044AE27;
int unit_hover_get_tbl(void* tbl, WORD str_id)
{
    int* u = hover_unit;
    if (u != NULL)
    {
        byte id = *((byte*)((uintptr_t)u + S_ID));
        if (id < U_EMPTY_BUTTONS)
        {
            if (str_id < U_EMPTY_BUTTONS)
            {
                if (unit_names[id] != NULL)
                {
                    return (int)unit_names[id];
                }
            }
        }
    }
    return ((int (*)(void*, int))g_proc_0044AE27)(tbl, str_id);//original
}

PROC g_proc_004354C8;
int objct_get_tbl_custom(void* tbl, WORD str_id)
{
    if (mission_objective != NULL)
    {
        return (int)mission_objective;
    }
    return ((int (*)(void*, int))g_proc_004354C8)(tbl, str_id);//original
}

PROC g_proc_004354FA;
int objct_get_tbl_campanign(void* tbl, WORD str_id)
{
    if (mission_objective != NULL)
    {
        return (int)mission_objective;
    }
    return ((int (*)(void*, int))g_proc_004354FA)(tbl, str_id);//original
}

PROC g_proc_004300A5;
int objct_get_tbl_briefing_task(void* tbl, WORD str_id)
{
    if (mission_objective != NULL)
    {
        return (int)mission_objective;
    }
    return ((int (*)(void*, int))g_proc_004300A5)(tbl, str_id);//original
}

PROC g_proc_004300CA;
int objct_get_tbl_briefing_title(void* tbl, WORD str_id)
{
    if (mission_briefing_title != NULL)
    {
        return (int)mission_briefing_title;
    }
    return ((int (*)(void*, int))g_proc_004300CA)(tbl, str_id);//original
}

PROC g_proc_004301CA;
int objct_get_tbl_briefing_text(void* tbl, WORD str_id)
{
    if (mission_briefing != NULL)
    {
        return (int)mission_briefing;
    }
    return ((int (*)(void*, int))g_proc_004301CA)(tbl, str_id);//original
}

PROC g_proc_00430113;
int objct_get_briefing_speech(char* speech)
{
    if (mission_briefing != NULL)
    {
        return 0;
    }
    return ((int (*)(char*))g_proc_00430113)(speech);//original
}

byte selection_color = 0;

bool selected_by_player(int* u, byte p)
{
    for (int i = 0; i < 9; i++)
    {
        if ((DWORD)u == *(DWORD*)(UNITS_SELECTED + 4 * i + 9 * 4 * p))return true;
    }
    return false;
}

byte ogre_mode_get_color(int* u)
{
    byte c = 0;
    if (ogre_mode)
    {
        byte l = *(byte*)LOCAL_PLAYER;
        bool sf = (*((byte*)((uintptr_t)u + S_FLAGS4)) & SF_SELECTED) != 0;
        if (sf)
        {
            byte o = *((byte*)((uintptr_t)u + S_OWNER));
            if (o == l)c = 250;
            else if (check_ally(o, l))c = 251;
            else c = 249;
        }
        for (int i = 0; i < 8; i++)
        {
            if (real_local != i)
            {
                if (ogre_find_player(real_local) == ogre_find_player(i))
                {
                    if (selected_by_player(u, i))
                    {
                        c = 251;
                        if (sf)
                            c = 253;
                    }
                }
            }
        }
    }
    return c;
}

PROC g_proc_0045456B;
void draw_selection_unit(int* u)
{
    selection_color = ogre_mode_get_color(u);
    if (selection_color != 0)
    {
        char buf[] = "\x90\x90";
        PATCH_SET((char*)ARCHON_SELECT_DRAW_UNIT2, buf);
        ((void (*)(int*))g_proc_0045456B)(u);//original
        char buf2[] = "\x74\x12";
        PATCH_SET((char*)ARCHON_SELECT_DRAW_UNIT2, buf2);
        selection_color = 0;
    }
    else
        ((void (*)(int*))g_proc_0045456B)(u);//original
}

PROC g_proc_00454C02;
void draw_selection_building(int* u)
{
    selection_color = ogre_mode_get_color(u);
    if (selection_color != 0)
    {
        char buf[] = "\x90\x90";
        PATCH_SET((char*)ARCHON_SELECT_DRAW_UNIT2, buf);
        ((void (*)(int*))g_proc_00454C02)(u);//original
        char buf2[] = "\x74\x12";
        PATCH_SET((char*)ARCHON_SELECT_DRAW_UNIT2, buf2);
        selection_color = 0;
    }
    else
        ((void (*)(int*))g_proc_00454C02)(u);//original
}

PROC g_proc_00454852;
void draw_selection_side1(int x, int y,int sz)
{
    if (selection_color != 0)
    {
        char c[] = "\x0";
        c[0] = selection_color;
        PATCH_SET((char*)DRAW_COLOR, c);
    }
    ((void (*)(int, int, int))g_proc_00454852)(x, y, sz);//original
}

PROC g_proc_00454882;
void draw_selection_side2(int x, int y, int sz)
{
    if (selection_color != 0)
    {
        char c[] = "\x0";
        c[0] = selection_color;
        PATCH_SET((char*)DRAW_COLOR, c);
    }
    ((void (*)(int, int, int))g_proc_00454882)(x, y, sz);//original
}

PROC g_proc_004548B9;
void draw_selection_side3(int x, int y, int sz)
{
    if (selection_color != 0)
    {
        char c[] = "\x0";
        c[0] = selection_color;
        PATCH_SET((char*)DRAW_COLOR, c);
    }
    ((void (*)(int, int, int))g_proc_004548B9)(x, y, sz);//original
}

PROC g_proc_004548E8;
void draw_selection_side4(int x, int y, int sz)
{
    if (selection_color != 0)
    {
        char c[] = "\x0";
        c[0] = selection_color;
        PATCH_SET((char*)DRAW_COLOR, c);
    }
    ((void (*)(int, int, int))g_proc_004548E8)(x, y, sz);//original
}

#pragma pack (1)
struct Player
{
    char unknown1[8];
    char nickname[10];
    char unknown2[62];
    int id;
    char unknown3[64];
    int ping;
    char unknown4[8];
};
struct Player* g_players = (struct Player*)FOIS_PLAYERS_STRUCT;
int g_numplayers = 0, msg_rec = 0;

void count_num_players()
{
    g_numplayers = 0;
    for (int i = 0; i < 10; i++)
    {
        if (g_players[i].nickname[0] && g_players[i].id != -1)
        {
            g_numplayers++;
        }
    }
}

PROC g_proc_00433899;
void viz_set(int a)
{
    ((void (*)(int))g_proc_00433899)(a);
    char buf[] = "\xFF";
    PATCH_SET((char*)AVED, buf);//set AV all
    for (int i = 0; i < 8; i++)
    {
        if ((get_player_team(i) != 0) && (get_player_team(i) != 9))
        {
            for (int j = 0; j < 8; j++)
            {
                if (i != j)
                {
                    if (get_player_team(i) == get_player_team(j))
                    {
                        ally(i, j, 1);
                        viz(i, j, 1);
                    }
                    else
                    {
                        ally(i, j, 0);
                        viz(i, j, 0);
                    }
                }
            }
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if ((get_player_team(i) == 9))
        {
            for (int j = 0; j < 8; j++)
            {
                ally(i, j, 1);
                viz_obs(i, j);
            }
        }
    }
}

PROC g_proc_0042BB04;
int* map_create_unit(int x,int y,byte id,byte o)
{
    if (get_player_team(*(byte*)LOCAL_PLAYER) == 9)
    {
        char buf[] = "\x0\x0\x0\x0";
        PATCH_SET((char*)CAMERA_Y, buf);
    }
    bool f = true;
    if (o < 8)
    {
        if (get_player_team(o) == 9)
        {
            find_all_alive_units(ANY_UNITS);
            sort_stat(S_OWNER, o, CMP_EQ);
            if (units != 0)f = false;
        }
    }
    if (f)
    {
        int* u = NULL;
        if (get_player_team(o) == 9)
        {
            u = ((int* (*)(int, int, byte, byte))g_proc_0042BB04)(x, y, U_PEASANT, o);
            if (u == NULL)u = ((int* (*)(int, int, byte, byte))g_proc_0042BB04)(x, y, U_FLYER, o);
            unit_hide(u);
            ((void (*)(int*))F_UNIT_UNPLACE)(u);//unplace
            set_res(o, 0, 0, 0, 0, 0);
            set_res(o, 1, 0, 0, 0, 0);
            set_res(o, 2, 0, 0, 0, 0);
        }
        else
        {
            if ((o < 8) && ogre_mode && (get_player_controler(o) != C_COMP))
            {
                bool f2 = true;
                if (ogre_find_real_player(o) != o)
                {
                    find_all_alive_units(ANY_UNITS);
                    sort_stat(S_OWNER, o, CMP_EQ);
                    if (units != 0)f2 = false;
                }
                if (f2)
                {
                    if (ogre_find_real_player(o) != o)
                    {
                        u = ((int* (*)(int, int, byte, byte))g_proc_0042BB04)(x, y, U_PEASANT, o);
                        if (u == NULL)u = ((int* (*)(int, int, byte, byte))g_proc_0042BB04)(x, y, U_FLYER, o);
                        unit_hide(u);
                        ((void (*)(int*))F_UNIT_UNPLACE)(u);//unplace
                        set_res(o, 0, 0, 0, 0, 0);
                        set_res(o, 1, 0, 0, 0, 0);
                        set_res(o, 2, 0, 0, 0, 0);

                        char bufc[] = "\x0\x0\x0\x0";
                        PATCH_SET((char*)CAMERA_Y, bufc);
                    }
                    else
                        u = ((int* (*)(int, int, byte, byte))g_proc_0042BB04)(x, y, id, o);
                }
                else
                    return NULL;
            }
            else
                u = ((int* (*)(int, int, byte, byte))g_proc_0042BB04)(x, y, id, o);
        }
        return u;
    }
    else
        return NULL;
}

void send_chat_msg(char* msg)
{
    ((int (*)(char*))F_LOBBY_CHAT_SEND)(msg);
}

DWORD trig_ver = DLL_VERSION;
char trig_msg[] = TRIG_MESSAGE;
char join_msg[] = JOIN_MESSAGE;
char trig_name[] = TRIG_GAME_NAME;

DWORD CALLBACK Thread_msg(LPVOID lpParam)
{
    if (*(byte*)ME_HOST == 1)
    {
        Sleep(3500);
        if (msg_rec != g_numplayers)
        {
            char buf[] = "\x5Not all players loaded triggers!                 Triggers NOT activated!";
            if (in_lobby())
                send_chat_msg(buf);
        }
    }
    ExitThread(0);
}

void send_join_msg()
{
    Sleep(500);
    if (!jmsend)
    {
        bool f = true;
        for (int i = 0; i < 7; i++)
        {
            char c = *(char*)(GAME_NAME + i);
            if (c != trig_name[i])f = false;
        }
        if (f)
        {
            if (in_lobby())
            {
                send_chat_msg(join_msg);
            }
        }
        if (in_lobby())
        {
            if (*(byte*)ME_HOST == 0)
            {
                char join_msg_t[] = "-getinfo\x3\x5";
                send_chat_msg(join_msg_t);
            }
        }
        jmsend = true;
    }
}

void send_team_msg()
{
    byte local = *(byte*)LOCAL_PLAYER;
    byte tm = get_player_team(local);
    char team_msg[] = "                   ";
    if (tm == 9)
        sprintf(team_msg, "\x6\xed\x3Team: \x5OBS");
    else
    {
        if (tm == 0)
            sprintf(team_msg, "\x6\xed\x3Team: \x5NO");
        else
            sprintf(team_msg, "\x6\xed\x3Team: \x5%d", tm);
    }
    send_chat_msg(team_msg);
}

void send_comp_team_msg(int p, byte t)
{
    char cmsg[] = "                                                       ";
    if (t == 9)
        sprintf(cmsg, "\x6\xed\x3Team for COMP in slot \x4%d: \x5OBS", p);
    else
    {
        if (t == 0)
            sprintf(cmsg, "\x6\xed\x3Team for COMP in slot \x4%d: \x5NO", p);
        else
            sprintf(cmsg, "\x6\xed\x3Team for COMP in slot \x4%d: \x5%d", p, t);
    }
    send_chat_msg(cmsg);
}

void send_teams_hidden()
{
    if (*(byte*)ME_HOST == 1)
    {
        teams_lag[*(byte*)LOCAL_PLAYER]++;
        char msg[] = "                                            ";
        sprintf(msg, "-team\x3\xed %d%d%d%d%d%d%d%d%d%d %d", get_player_team(0), get_player_team(1),
            get_player_team(2), get_player_team(3), get_player_team(4), get_player_team(5),
            get_player_team(6), get_player_team(7), war2_game_mode, (int)ogre_mode, teams_lag[*(byte*)LOCAL_PLAYER]);
        send_chat_msg(msg);
    }
}

void send_setinfo(DWORD k)
{
    char msg[] = "                                            ";
    sprintf(msg, "-setinfo\x3\x5 %d", k);
    send_chat_msg(msg);
    teams_lag[*(byte*)LOCAL_PLAYER] = k;
}

void command_only_for_host()
{
    char msg[] = "This command only for host!";
    ((void (*)(char*))F_SHOW_DIALOG_OK)(msg);//show dialog
}

char msg_written_text[256];

PROC g_proc_00475A7A;
int msg_written_fois(int a)
{
    return ((int (*)(int))F_MSG_WRITTEN_ORIGINAL)(a);
}

PROC g_proc_00475AA2;
void msg_written(int a, char* msg)
{
    bool orig = false;
    if (*(byte*)DLL_STATUS == 1)
    {
        bool fc = false;
        byte writer = 0;
        for (int i = 0; i < 10; i++)
        {
            if (g_players[i].nickname[0] && (g_players[i].id == a))
            {
                writer = g_players[i].unknown2[58];
            }
        }
        byte local = *(byte*)LOCAL_PLAYER;
        byte host = *(byte*)ME_HOST;
        byte phost = *(byte*)PLAYER_HOST;

        for (int i = 0; i < 60; i++)
        {
            msg_written_text[i] = msg[i];
            if (msg_written_text[i] == '\x0')break;
        }
        char* token = strtok(msg_written_text, " ");
        if (token != NULL)
        {
            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "-team", -1) == CSTR_EQUAL)
            if (lstrcmpi(token, "-team") == 0)
            {
                if (writer == phost)
                {
                    token = strtok(NULL, " ");
                    if (token != NULL)
                    {
                        int pl = -1;
                        for (int i = 0; i < 8; i++)
                        {
                            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1,
                            //    (char*)(PLAYERS_INFO + 4 + 5 + 38 * i), -1) == CSTR_EQUAL)
                            if (lstrcmpi(token, (char*)(PLAYERS_INFO + 4 + 5 + 38 * i)) == 0)
                            {
                                pl = i;
                            }
                        }
                        if (pl == -1)
                        {
                            char c1 = token[0];
                            if ((c1 >= '0') && (c1 <= '9'))
                            {
                                pl = (c1 - 48);
                            }
                        }
                        if (pl != -1)
                        {
                            byte pc = get_player_controler(pl);
                            if ((pc == C_PLAYER) || (pc == C_PREFER_COMP))
                            {
                                token = strtok(NULL, " ");
                                if (token != NULL)
                                {
                                    byte tm = 255;
                                    //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "obs", -1) == CSTR_EQUAL)
                                    if (lstrcmpi(token, "obs") == 0)
                                    {
                                        tm = 9;
                                    }
                                    else
                                    {
                                        char c = token[0];
                                        if ((c >= '0') && (c <= '9'))
                                        {
                                            tm = (c - 48);
                                        }
                                    }
                                    if (tm != 255)
                                    {
                                        if (pc == C_PLAYER)
                                        {
                                            teams_host[pl] = tm;
                                            set_player_team(pl, tm);
                                            if (pl == local)
                                                send_team_msg();
                                        }
                                        if (pc == C_PREFER_COMP)
                                        {
                                            if (tm != 9)
                                            {
                                                teams_host[pl] = tm;
                                                set_player_team(pl, tm);
                                                if (phost == local)
                                                {
                                                    send_comp_team_msg(pl, tm);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    fc = true;
                }
                else
                {
                    if (writer == local)
                    {
                        command_only_for_host();
                    }
                }
            }
            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "-team\x3\xed", -1) == CSTR_EQUAL)
            if (lstrcmpi(token, "-team\x3\xed") == 0)
            {
                if (writer == phost)
                {
                    token = strtok(NULL, " ");
                    if (token != NULL)
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            byte tm = 255;
                            char c = token[i];
                            if ((c >= '0') && (c <= '9'))
                            {
                                tm = (c - 48);
                            }
                            if (tm != 255)
                            {
                                if ((get_player_controler(i) == C_PLAYER)
                                    || ((get_player_controler(i) == C_PREFER_COMP) && (tm != 9)))
                                {
                                    teams_host[i] = tm;
                                    set_player_team(i, tm);
                                }
                            }
                        }
                        byte w2m = 0;
                        char w2c = token[8];
                        if ((w2c >= '0') && (w2c <= '9'))
                        {
                            w2m = (w2c - 48);
                        }
                        war2_game_mode = w2m;
                        w2m = 0;
                        w2c = token[9];
                        if ((w2c >= '0') && (w2c <= '9'))
                        {
                            w2m = (w2c - 48);
                        }
                        ogre_mode = w2m == 1;
                        token = strtok(NULL, " ");
                        if (token != NULL)
                        {
                            char k[] = "0000000000";
                            for (int i = 0; i < 10; i++)
                            {
                                k[i] = token[i];
                            }
                            DWORD kol = 0;
                            for (int i = 0; (i < 10) && (k[i] != 0); i++)
                            {
                                char c = k[i];
                                if ((c >= '0') && (c <= '9'))
                                {
                                    kol *= 10;
                                    kol += (c - 48);
                                }
                            }
                            send_setinfo(kol);
                        }
                    }
                    fc = true;
                }
            }
            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "-teams", -1) == CSTR_EQUAL)
            if (lstrcmpi(token, "-teams") == 0)
            {
                if (writer == phost)
                {
                    send_team_msg();
                    if (phost == local)
                    {
                        for (int i = 0; i < 8; i++)
                        {
                            if (get_player_controler(i) == C_PREFER_COMP)
                                send_comp_team_msg(i, get_player_team(i));
                        }
                    }
                    fc = true;
                }
                else
                {
                    if (writer == local)
                    {
                        command_only_for_host();
                    }
                }
            }
            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "-getinfo\x3\x5", -1) == CSTR_EQUAL)
            if (lstrcmpi(token, "-getinfo\x3\x5") == 0)
            {
                if (phost == local)send_teams_hidden();
                fc = true;
            }
            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "-setinfo\x3\x5", -1) == CSTR_EQUAL)
            if (lstrcmpi(token, "-setinfo\x3\x5") == 0)
            {
                token = strtok(NULL, " ");
                if (token != NULL)
                {
                    char k[] = "0000000000";
                    for (int i = 0; i < 5; i++)
                    {
                        k[i] = token[i];
                    }
                    DWORD kol = 0;
                    for (int i = 0; (i < 10) && (k[i] != 0); i++)
                    {
                        char c = k[i];
                        if ((c >= '0') && (c <= '9'))
                        {
                            kol *= 10;
                            kol += (c - 48);
                        }
                    }
                    teams_lag[writer] = kol;
                }
                fc = true;
            }
            //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "-check", -1) == CSTR_EQUAL)
            if (lstrcmpi(token, "-check") == 0)
            {
                if (writer == phost)
                {
                    send_chat_msg(join_msg);
                    fc = true;
                }
                else
                {
                    if (writer == local)
                    {
                        command_only_for_host();
                    }
                }
            }
        }

        bool f = true;
        for (int i = 0; i < 7; i++)
        {
            char c = *(char*)(GAME_NAME + i);
            if (c != trig_name[i])f = false;
        }
        if (f || (war2_game_mode != 0) || ogre_mode)
        {
            if (!strcmp(msg, trig_msg))
            {
                if (g_numplayers == 0)
                {
                    count_num_players();
                    msg_rec = 0;
                    if (host == 1)
                    {
                        DWORD dw = 0;
                        CreateThread(NULL, 0, Thread_msg, &dw, 0, NULL);
                    }
                }
                msg_rec += 1;
                if (msg_rec == g_numplayers)
                {
                    replace_back();
                    if (f || (war2_game_mode == 1))
                    {
                        replace_trigger();
                        if (replaced)
                        {
                            if (host == 1)
                            {
                                char buf[] = "\x5Triggers from MAP\x3 Activated!!!";
                                send_chat_msg(buf);
                            }
                        }
                        else
                        {
                            if (host == 1)
                            {
                                char buf2[] = "\x5War2Mod\x3 Activated!!!";
                                send_chat_msg(buf2);
                            }
                            replace_war2mod();
                        }
                    }
                    if (war2_game_mode == 2)replace_war2rebalance();
                    if (war2_game_mode == 3)replace_war2double_race();
                    if (ogre_mode)ogre_init(true);
                }
            }
            else
                orig = true;
        }
        else
            orig = true;
        if (fc)
        {
            orig = false;
        }
    }
    else orig = true;
    if (orig)
    {
        ((void (*)(int, char*))g_proc_00475AA2)(a, msg);//orig
        ((int (*)(int))g_proc_00475A7A)(0);//fois plugin sound
    }
}

PROC g_proc_0047A7F6;
int game_starting(int a)
{
    int original = ((int (*)(int))g_proc_0047A7F6)(a);
    if (*(byte*)DLL_STATUS == 1)
    {
        menu_1 = false;
        m_ticks1 = 0;
        m_ticks = 0;
        apm_clear();
        memset(obs_lose, 0, sizeof(obs_lose));
        if (*(byte*)ME_HOST == 1)send_teams_hidden();
        bool f = true;
        for (int i = 0; i < 7; i++)
        {
            char c = *(char*)(GAME_NAME + i);
            if (c != trig_name[i])f = false;
        }
        if (f || (war2_game_mode != 0) || ogre_mode)
        {
            send_chat_msg(trig_msg);
            char buf[] = "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0";
            PATCH_SET((char*)GB_HORSES,buf);
            char buf2[] = "\x0\x0\x0\x0";
            PATCH_SET((char*)CHEATBITS, buf2);
        }
    }
    return original;
}

PROC g_proc_00468798;
int game_create(int a1, char* name, char* pass, int game_type, int a2, byte speed, int a3)
{
    memset(teams_host_r, 0, sizeof(teams_host_r));
    //a1 pointer?
    //a2 unknown - always = 0
    //a3 pointer?
    char* p = name;
    if (*(byte*)DLL_STATUS == 1)
    {
        char d1[] = "battle.snp";
        int ad = (int)GetModuleHandleA(d1);
        if (ad != 0)
        {
            int chan_addr = ad + 0x30094;
            char d[] = "War2Mod";
            if (!strcmp((char*)chan_addr, d))
            {
                for (int i = 0; i < 7; i++)
                {
                for (int j = 23; j > i; j--)
                {
                    p[j] = p[j - 1];
                }
                p[i] = trig_name[i];
                }
            }
        }
    }
    return ((int(*)(int, char*, char*, int, int, byte, int))g_proc_00468798)(a1, p, pass, game_type, a2, speed, a3);
}

PROC g_proc_00479516;
void report_send_bne(int a, int b)
{
    ((void (*)(int, int))g_proc_00479516)(a, b);
    for (int i = 0; i < 8; i++)
    {
        DWORD rep = *(DWORD*)(a + 4 * i);//report result
        DWORD net_node = *(DWORD*)(PLAYERS_NET_NODES + 4 * i);
        if (net_node < 8)
        {
            if (get_player_team(net_node) == 9)
            {
                if (obs_lose[net_node] == 1)
                {
                    patch_setdword((DWORD*)(a + 4 * i), 2);//set lose
                }
                else
                {
                    //patch_setdword((DWORD*)(a + 4 * i), 5);//set observer
                }
            }
        }
    }
}

void def_stat(byte u, WORD hp, byte str, byte prc, byte arm, byte rng, byte gold, byte lum, byte oil, byte time)
{
    char buf2[] = "\x0\x0";
    buf2[0] = hp % 256;
    buf2[1] = hp / 256;
    PATCH_SET((char*)(UNIT_HP_TABLE + 2 * u), buf2);
    char buf[] = "\x0";
    buf[0] = str;
    PATCH_SET((char*)(UNIT_STRENGTH_TABLE + u), buf);
    buf[0] = prc;
    PATCH_SET((char*)(UNIT_PIERCE_TABLE + u), buf);
    buf[0] = arm;
    PATCH_SET((char*)(UNIT_ARMOR_TABLE + u), buf);
    buf[0] = rng;
    PATCH_SET((char*)(UNIT_RANGE_TABLE + u), buf);
    buf[0] = gold;
    PATCH_SET((char*)(UNIT_GOLD_TABLE + u), buf);
    buf[0] = lum;
    PATCH_SET((char*)(UNIT_LUMBER_TABLE + u), buf);
    buf[0] = oil;
    PATCH_SET((char*)(UNIT_OIL_TABLE + u), buf);
    buf[0] = time;
    PATCH_SET((char*)(UNIT_TIME_TABLE + u), buf);
}

void def_upgr(byte u, WORD gold, WORD lum, WORD oil)
{
    char buf2[] = "\x0\x0";
    buf2[0] = gold % 256;
    buf2[1] = gold / 256;
    PATCH_SET((char*)(UPGR_GOLD_TABLE + 2 * u), buf2);
    buf2[0] = lum % 256;
    buf2[1] = lum / 256;
    PATCH_SET((char*)(UPGR_LUMBER_TABLE + 2 * u), buf2);
    buf2[0] = oil % 256;
    buf2[1] = oil / 256;
    PATCH_SET((char*)(UPGR_OIL_TABLE + 2 * u), buf2);
}

PROC g_proc_0041F915;
int map_load(int a,int b)
{
    int original = ((int (*)(int, int))g_proc_0041F915)(a, b);
    if (war2mod)
    {
        char buf[] = "\x0\x0\x0\x0";//fix vision
        patch_setdword((DWORD*)buf, (DWORD)F_VISION6);
        PATCH_SET((char*)(UNIT_VISION_FUNCTIONS_TABLE + 4 * U_RUNESTONE), buf);
        PATCH_SET((char*)(UNIT_VISION_FUNCTIONS_TABLE + 4 * U_PORTAL), buf);
        PATCH_SET((char*)(UNIT_VISION_FUNCTIONS_TABLE + 4 * U_CIRCLE), buf);
        char buf2[] = "\x1";
        PATCH_SET((char*)(UNIT_MULTISELECTABLE + U_DEMON), buf2);

        def_stat(U_MINE, 25500, 0, 0, 20, 0, 0, 100, 0, 64);

        def_stat(U_RUNESTONE, 1000, 0, 0, 20, 0, 100, 60, 0, 150);
        def_stat(U_PORTAL, 3000, 0, 0, 20, 0, 150, 50, 0, 150);
        def_stat(U_CIRCLE, 1, 0, 0, 0, 0, 120, 0, 0, 0);
        def_stat(U_DEMON, 66, 0, 3, 0, 4, 150, 0, 0, 150);
        def_stat(U_CRITTER, 5, 0, 0, 0, 0, 10, 0, 0, 200);

        def_stat(U_ATTACK_PEASANT, 36, 4, 3, 1, 1, 15, 0, 0, 25);
        def_stat(U_ATTACK_PEON, 36, 4, 3, 1, 1, 15, 0, 0, 25);

        def_stat(U_DANATH, 121, 9, 4, 6, 1, 140, 0, 0, 60);
        def_stat(U_GROM, 121, 12, 4, 3, 1, 140, 0, 0, 60);

        def_stat(U_ALLERIA, 83, 4, 13, 1, 6, 70, 15, 0, 70);
        def_stat(U_ZULJIN, 99, 4, 7, 3, 6, 70, 15, 0, 70);

        def_stat(U_LOTHAR, 205, 10, 4, 8, 1, 170, 20, 0, 90);
        def_stat(U_KARGATH, 207, 8, 3, 12, 1, 170, 20, 0, 90);

        def_stat(U_TYRALYON, 162, 10, 4, 5, 1, 180, 30, 0, 100);
        def_stat(U_DENTARG, 168, 10, 4, 5, 1, 180, 30, 0, 100);

        def_stat(U_UTER, 144, 4, 12, 6, 1, 200, 30, 0, 140);
        def_stat(U_CHOGAL, 111, 6, 20, 4, 1, 200, 30, 0, 140);

        def_stat(U_HADGAR, 101, 0, 20, 2, 8, 220, 0, 0, 200);
        def_stat(U_TERON, 103, 0, 20, 2, 8, 220, 0, 0, 200);

        def_stat(U_KURDRAN, 333, 10, 25, 10, 6, 255, 55, 0, 255);
        def_stat(U_DEATHWING, 333, 10, 25, 10, 6, 255, 55, 0, 255);
    }
    return original;
}

PROC g_proc_0043373E;
void map_load2()
{
    if (campanign)
    {
        byte b1 = *(byte*)MAP_LOAD_SETTINGS;
        b1 &= ~0xF0;
        byte b2 = *(byte*)(MAP_LOAD_SETTINGS + 1);
        b2 &= ~0x03;
        char buf[] = "\x0\x0";
        buf[0] = b1;
        buf[1] = b2;
        PATCH_SET((char*)MAP_LOAD_SETTINGS, buf);
        for (int i = 0; i < 8; i++)
        {
            if (forced_race[i] != 0)
            {
                char r[] = "\x0";
                r[0] = forced_race[i] - 1;
                PATCH_SET((char*)(MAP_LOAD_RACE + i), r);
            }
        }
    }
    ((void (*)())g_proc_0043373E)();
}

PROC g_proc_00436B8C;//build func
PROC g_proc_0043B0AF;//placebox set
PROC g_proc_00418F46;//start build
int check_build_price(int* creator, byte bid)
{
    int original = 0;
    byte cid = (*(byte*)((uintptr_t)creator + S_ID));
    if (((bid == U_FARM) && (cid == U_PEON)) || ((bid == U_PIGFARM) && (cid == U_PEASANT)))
    {
        byte g = *(byte*)(UNIT_GOLD_TABLE + bid);
        byte l = *(byte*)(UNIT_LUMBER_TABLE + bid);
        byte o = *(byte*)(UNIT_OIL_TABLE + bid);
        char buf[] = "\x0";
        buf[0] = b3forest_price;
        PATCH_SET((char*)(UNIT_GOLD_TABLE + bid), buf);
        buf[0] = 0;
        PATCH_SET((char*)(UNIT_LUMBER_TABLE + bid), buf);
        buf[0] = 0;
        PATCH_SET((char*)(UNIT_OIL_TABLE + bid), buf);
        original = ((int (*)(int*, byte))g_proc_00436B8C)(creator, bid);//original
        buf[0] = g;
        PATCH_SET((char*)(UNIT_GOLD_TABLE + bid), buf);
        buf[0] = l;
        PATCH_SET((char*)(UNIT_LUMBER_TABLE + bid), buf);
        buf[0] = o;
        PATCH_SET((char*)(UNIT_OIL_TABLE + bid), buf);
    }
    else
        original = ((int (*)(int*, byte))g_proc_00436B8C)(creator, bid);//original
    return original;
}

PROC g_proc_00455F3C;//singleplayer
PROC g_proc_004762B1;//multiplayer
void cancel_building_check_price(int* bld, int a, int b)
{
    byte bid = (*(byte*)((uintptr_t)bld + S_ID));
    byte cid = (*(byte*)((uintptr_t)bld + S_RESOURCES));
    if (((bid == U_FARM) && (cid == U_PEON)) || ((bid == U_PIGFARM) && (cid == U_PEASANT)))
    {
        byte g = *(byte*)(UNIT_GOLD_TABLE + bid);
        byte l = *(byte*)(UNIT_LUMBER_TABLE + bid);
        byte o = *(byte*)(UNIT_OIL_TABLE + bid);
        char buf[] = "\x0";
        buf[0] = b3forest_price;
        PATCH_SET((char*)(UNIT_GOLD_TABLE + bid), buf);
        buf[0] = 0;
        PATCH_SET((char*)(UNIT_LUMBER_TABLE + bid), buf);
        buf[0] = 0;
        PATCH_SET((char*)(UNIT_OIL_TABLE + bid), buf);
        ((void (*)(int*, int, int))g_proc_00455F3C)(bld, a, b);//original
        buf[0] = g;
        PATCH_SET((char*)(UNIT_GOLD_TABLE + bid), buf);
        buf[0] = l;
        PATCH_SET((char*)(UNIT_LUMBER_TABLE + bid), buf);
        buf[0] = o;
        PATCH_SET((char*)(UNIT_OIL_TABLE + bid), buf);
    }
    else
        ((void (*)(int*, int, int))g_proc_00455F3C)(bld, a, b);//original
}

PROC g_proc_004526FE;
void grow_structure(int* p)
{
    ((void (*)(int*))g_proc_004526FE)(p);//original
    if (replaced)
    {
        if (p)
        {
            byte id = (*(byte*)((uintptr_t)p + S_ID));
            if ((id == U_FARM) || (id == U_PIGFARM))
            {
                if (check_unit_complete(p))
                {
                    byte cid = (*(byte*)((uintptr_t)p + S_RESOURCES));
                    if (((id == U_FARM) && (cid == U_PEON)) || ((id == U_PIGFARM) && (cid == U_PEASANT)))
                    {
                        byte x = (*(byte*)((uintptr_t)p + S_X));
                        byte y = (*(byte*)((uintptr_t)p + S_Y));
                        byte mxs = *(byte*)MAP_SIZE;//map size
                        byte era = *(byte*)MAP_ERA;//map era
                        char* cel = (char*)*(int*)(MAP_CELLS_POINTER);//map cells
                        char* sq = (char*)*(int*)(MAP_SQ_POINTER);//map cells
                        char* reg = (char*)*(int*)(MAP_REG_POINTER);//map reg
                        char bufte[] = "\x88\x84\x82\x81\x81\x83\x7F\x85\x81\x83\x7F\x86\x81\x83\x7F\x85";//4 era tiles
                        char buft[] = "\x7E\x0";
                        for (int xx = x; (xx < x + 2) && (xx < mxs); xx++)
                        {
                            for (int yy = y; (yy < y + 2) && (yy < mxs); yy++)
                            {
                                if ((xx == x) && (yy == y))
                                    buft[0] = bufte[era * 4];
                                if ((xx == x + 1) && (yy == y))
                                    buft[0] = bufte[era * 4 + 1];
                                if ((xx == x) && (yy == y + 1))
                                    buft[0] = bufte[era * 4 + 2];
                                if ((xx == x + 1) && (yy == y + 1))
                                    buft[0] = bufte[era * 4 + 3];
                                PATCH_SET((char*)(cel + 2 * xx + 2 * yy * mxs), buft);
                                char buf[] = "\x81\x0";//unpassable land
                                PATCH_SET((char*)(sq + 2 * xx + 2 * yy * mxs), buf);
                                buf[0] = '\xFE';//tree
                                buf[1] = '\xFF';//tree
                                PATCH_SET((char*)(reg + 2 * xx + 2 * yy * mxs), buf);
                            }
                        }
                        byte flg = (*(byte*)((uintptr_t)p + S_FLAGS3));
                        flg |= SF_HIDDEN;//hide
                        set_stat(p, flg, S_FLAGS3);
                        ((void (*)(int*))F_DEAD_BUILDING_REDRAW)(p);//unitdraw insert dead bldg
                        unit_kill(p);
                    }
                }
            }
            if (id == U_MINE)
            {
                if (check_unit_complete(p))
                {
                    give(p, P_NEUTRAL);
                    set_stat(p, P_NEUTRAL, S_COLOR);
                    WORD mhp = *(WORD*)(UNIT_HP_TABLE + 2 * id);//max hp
                    if (mhp > 300)
                    {
                        set_stat(p, 300, S_HP);
                    }
                    int r = ((int (*)())F_NET_RANDOM)();
                    r %= 50;
                    r += 50;
                    set_stat(p, r, S_RESOURCES);

                    if (!b3mine_anywhere)
                    {
                        byte x = (*(byte*)((uintptr_t)p + S_X));
                        byte y = (*(byte*)((uintptr_t)p + S_Y));
                        byte mxs = *(byte*)MAP_SIZE;//map size
                        byte era = *(byte*)MAP_ERA;//map era
                        char* cel = (char*)*(int*)(MAP_CELLS_POINTER);//map cells
                        for (int xx = x; (xx < x + 3) && (xx < mxs); xx++)
                        {
                            for (int yy = y; (yy < y + 3) && (yy < mxs); yy++)
                            {
                                char buf[] = "\x4b\x1";
                                if (era == 0)buf[0] = 0x4e;//forest
                                if (era == 1)buf[0] = 0x4b;//winter
                                if (era == 2)buf[0] = 0x4a;//wast
                                if (era == 3)buf[0] = 0x50;//swamp
                                PATCH_SET((char*)(cel + 2 * xx + 2 * yy * mxs), buf);
                            }
                        }
                    }
                }
            }
        }
    }
}

PROC g_proc_00418FFE;
int* create_building(int a, int b, int id, int c)
{
    int* cr = (int*)*(int*)UNIT_RUN_UNIT_POINTER;
    int* original = ((int* (*)(int, int, int, int))g_proc_00418FFE)(a, b, id, c);//original
    if (replaced)
    {
        if (original)
        {
            if (cr)
            {
                if ((id == U_FARM) || (id == U_PIGFARM))
                {
                    byte cid = *((byte*)((uintptr_t)cr + S_ID));
                    set_stat(original, cid, S_RESOURCES);
                }
                if (id == U_CIRCLE)
                {
                    WORD xx = *((WORD*)((uintptr_t)cr + S_DRAW_X));
                    WORD yy = *((WORD*)((uintptr_t)cr + S_DRAW_Y));
                    bullet_create(xx + 16, yy + 16, B_CAT_HIT);
                    unit_kill(cr);
                }
            }
        }
    }
    return original;
}

PROC g_proc_0043A974;//placing
PROC g_proc_0043ABAB;//draw
WORD placebox_query(int* p, WORD x, WORD y, byte id)
{
    WORD original = ((WORD (*)(int*, WORD, WORD, byte))g_proc_0043A974)(p, x, y, id);//original
    if (replaced)
    {
        byte mxs = *(byte*)MAP_SIZE;//map size
        byte era = *(byte*)MAP_ERA;//map era
        char* cel = (char*)*(int*)(MAP_CELLS_POINTER);//map cells
        byte pid = *((byte*)((uintptr_t)p + S_ID));//builder
        if (((id == U_FARM) && (pid == U_PEON)) || ((id == U_PIGFARM) && (pid == U_PEASANT)))
        {
            if (!b3forest_anywhere)
            {
                WORD dr = 0x7E;//destroyed forest
                for (int xx = x; (xx < x + 2) && (xx < mxs); xx++)
                {
                    for (int yy = y; (yy < y + 2) && (yy < mxs); yy++)
                    {
                        WORD c = *(WORD*)(cel + 2 * xx + 2 * yy * mxs);
                        if (c != dr)
                        {
                            char buf[] = "\x7";
                            PATCH_SET((char*)(CAN_PLACE_TBL + (xx - x) + 4 * (yy - y)), buf);//can place tbl
                            original = 7;
                        }
                    }
                }
            }
        }
        if (id == U_MINE)
        {
            if (!b3mine_anywhere)
            {
                WORD dr = 0xA1;//destroyed rock
                if (era == 0)dr = 0xA6;//forest
                if (era == 1)dr = 0xA1;//winter
                if (era == 2)dr = 0xA3;//wast
                if (era == 3)dr = 0xA1;//swamp
                for (int xx = x; (xx < x + 3) && (xx < mxs); xx++)
                {
                    for (int yy = y; (yy < y + 3) && (yy < mxs); yy++)
                    {
                        WORD c = *(WORD*)(cel + 2 * xx + 2 * yy * mxs);
                        if (c != dr)
                        {
                            char buf[] = "\x7";
                            PATCH_SET((char*)(CAN_PLACE_TBL + (xx - x) + 4 * (yy - y)), buf);//can place tbl
                            original = 7;
                        }
                    }
                }
            }
        }
    }
    return original;
}

PROC g_proc_00424745;//entering
PROC g_proc_004529C0;//grow struct
int goods_into_inventory(int* p)
{
    if (replaced)
    {
        int tr = (*(int*)((uintptr_t)p + S_ORDER_UNIT_POINTER));
        if (tr != 0)
        {
            bool f = false;
            int* trg = (int*)tr;
            byte o = *((byte*)((uintptr_t)p + S_OWNER));
            byte id = *((byte*)((uintptr_t)p + S_ID));
            byte tid = *((byte*)((uintptr_t)trg + S_ID));
            byte pf = *((byte*)((uintptr_t)p + S_PEON_FLAGS));
            int pflag = *(int*)(UNIT_GLOBAL_FLAGS + id * 4);
            int tflag = *(int*)(UNIT_GLOBAL_FLAGS + tid * 4);
            int res = 100;
            if (pf & PEON_LOADED)
            {
                if (((pflag & IS_SHIP) != 0) && ((tflag & IS_OILRIG) == 0))
                {
                    int r = get_val(REFINERY, o);
                    if (more_res)
                    {
                        res = 100 + 25 * r;
                    }
                    else
                    {
                        if (r != 0)
                        {
                            res = 125;
                        }
                        else
                        {
                            res = 100;
                        }
                    }
                    change_res(o, 2, 1, res);
                    add_total_res(o, 2, 1, res);
                    f = true;
                }
                else
                {
                    if (((tflag & IS_TOWNHALL) != 0) || ((tflag & IS_LUMBER) != 0))
                    {
                        if (((tflag & IS_TOWNHALL) != 0))
                        {
                            pf |= PEON_IN_CASTLE;
                            set_stat(p, pf, S_PEON_FLAGS);
                        }
                        if (((pf & PEON_HARVEST_GOLD) != 0) && ((tflag & IS_TOWNHALL) != 0))
                        {
                            int r2 = get_val(TH2, o);
                            int r3 = get_val(TH3, o);
                            if (more_res)
                            {
                                res = 100 + 10 * r2 + 20 * r3;
                            }
                            else
                            {
                                if (r3 != 0)
                                {
                                    res = 120;
                                }
                                else
                                {
                                    if (r2 != 0)
                                    {
                                        res = 110;
                                    }
                                    else
                                    {
                                        res = 100;
                                    }
                                }
                            }
                            pf &= ~PEON_HARVEST_GOLD;
                            change_res(o, 0, 1, res);
                            add_total_res(o, 0, 1, res);
                            f = true;
                        }
                        else
                        {
                            if (((pf & PEON_HARVEST_LUMBER) != 0))
                            {
                                int r = get_val(LUMBERMILL, o);
                                if (more_res)
                                {
                                    res = 100 + 25 * r;
                                }
                                else
                                {
                                    if (r != 0)
                                    {
                                        res = 125;
                                    }
                                    else
                                    {
                                        res = 100;
                                    }
                                }
                                pf &= ~PEON_HARVEST_LUMBER;
                                change_res(o, 1, 1, res);
                                add_total_res(o, 1, 1, res);
                                f = true;
                            }
                        }
                    }
                }
            }
            if (f)
            {
                pf &= ~PEON_LOADED;
                set_stat(p, pf, S_PEON_FLAGS);
                ((void (*)(int*))F_GROUP_SET)(p);
                return 1;
            }
        }
        return 0;
    }
    else
        return ((int(*)(int*))g_proc_00424745)(p);//original
}

const char VICTORY_OBS[] = "OBS!";
byte victory_local = 0;
PROC g_proc_00431329;
int victory_result(int a, int b)
{
    int original = ((int (*)(int, int))g_proc_00431329)(a, b);
    if (get_player_team(victory_local) == 9)return (int)VICTORY_OBS;
    else return original;
}

PROC g_proc_0042A4E1;
void victory_save_local()
{
    victory_local = *(byte*)LOCAL_PLAYER;
    ((void (*)())g_proc_0042A4E1)();
}

DWORD victory_comp_id = 0;
int victory_comps_stat1()
{
    if (check_ally(victory_comp_id % 256, victory_local))
        return ((int (*)(int))F_COMP_VICTORY_STATUS_TBL)(0x24);
    else
        return ((int (*)(int))F_COMP_VICTORY_STATUS_TBL)(0x23);
}

PROC g_proc_0043147C;
void victory_comps_stat(int a)
{
    *(byte*)LOCAL_PLAYER = 0;
    *(byte*)PACKET_PLAYER = 1;
}

void victory_comps_stat_init()
{
    char buf[] = "\x89\x3D\x0\x0\x0\x0\xE8\x0\x0\x0\x0\xC3";
    patch_setdword((DWORD*)(buf + 2), (DWORD)&victory_comp_id);
    PATCH_SET((char*)victory_comps_stat, buf);
    patch_call((char*)victory_comps_stat + 6, (char*)victory_comps_stat1);
}

int H_act = 0;
PROC g_proc_00455D65;
void ah_rec_act()
{
    H_act = *(byte*)GW_ACTION_TYPE;
    ((void (*)())g_proc_00455D65)();//original
}

PROC g_proc_0047610B;
void ah_rec_act2()
{
    H_act = *(byte*)GW_ACTION_TYPE;
    ((void (*)())g_proc_0047610B)();//original
}

bool ah_mage_check(int* u)
{
    byte p = (*(byte*)((uintptr_t)u + S_OWNER));
    byte id = (*(byte*)((uintptr_t)u + S_ID));
    bool o = true, f = false;
    int l = 0, a = 0;
    switch (H_act)
    {

    case ORDER_SPELL_VISION:
        o = false;
        l = L_VISION;
        a = A_VISION;
        break;
    case ORDER_SPELL_HEAL:
        o = false;
        l = L_HEAL;
        a = A_HEAL;
        break;
    case ORDER_SPELL_EXORCISM:
        o = false;
        l = L_EXORCISM;
        a = A_EXORCISM;
        break;

    case ORDER_SPELL_FIREBALL:
        o = false;
        l = L_FIREBALL;
        a = A_FIREBALL;
        break;
    case ORDER_SPELL_FIRESHIELD:
        o = false;
        l = L_FLAME_SHIELD;
        a = A_FLAME_SHIELD;
        break;
    case ORDER_SPELL_INVIS:
        o = false;
        l = L_INVIS;
        a = A_INVIS;
        break;
    case ORDER_SPELL_POLYMORPH:
        o = false;
        l = L_POLYMORF;
        a = A_POLYMORF;
        break;
    case ORDER_SPELL_SLOW:
        o = false;
        l = L_SLOW;
        a = A_SLOW;
        break;
    case ORDER_SPELL_BLIZZARD:
        o = false;
        l = L_BLIZZARD;
        a = A_BLIZZARD;
        break;

    case ORDER_SPELL_EYE:
        o = false;
        l = L_EYE;
        a = A_EYE;
        break;
    case ORDER_SPELL_BLOODLUST:
        o = false;
        l = L_BLOOD;
        a = A_BLOOD;
        break;
    case ORDER_SPELL_RUNES:
        o = false;
        l = L_RUNES;
        a = A_RUNES;
        break;

    case ORDER_SPELL_DRAINLIFE:
        o = false;
        l = L_COIL;
        a = A_COIL;
        break;
    case ORDER_SPELL_RAISEDEAD:
        o = false;
        l = L_RAISE;
        a = A_RAISE;
        break;
    case ORDER_SPELL_ARMOR:
        o = false;
        l = L_UNHOLY;
        a = A_UNHOLY;
        break;
    case ORDER_SPELL_WHIRLWIND:
        o = false;
        l = L_WIND;
        a = A_WIND;
        break;
    case ORDER_SPELL_HASTE:
        o = false;
        l = L_HASTE;
        a = A_HASTE;
        break;
    case ORDER_SPELL_ROT:
        o = false;
        l = L_DD;
        a = A_DD;
        break;
    }
    if (!o)
    {
        if (((*(int*)(SPELLS_LEARNED + 4 * p) & (1 << l)) == 0) ||
            ((*(int*)(ALLOWED_SPELLS + 4 * p) & (1 << a)) == 0))
            f = true;
        if (id == U_CHOGAL)f = false;
    }
    if (f)
    {
        byte x = (*(byte*)((uintptr_t)u + S_X)) + 1;
        byte y = *(byte*)((uintptr_t)u + S_Y);
        WORD xx = *((WORD*)((uintptr_t)u + S_DRAW_X));
        WORD yy = *((WORD*)((uintptr_t)u + S_DRAW_Y));
        set_stat(u, SF_HIDDEN, S_FLAGS3);
        unit_kill(u);
        unit_create(x, y, U_CRITTER, P_NEUTRAL, 1);
        ((void (*)(WORD, WORD, byte))F_SPELL_SOUND_XY)(xx + 16, yy + 16, SS_POLYMORPH);//poly sound
        return false;
    }
    else
        return true;
}

PROC g_proc_00455EA2;
void ah_rec_act_set(int* u, int b, int c, int d, int e)
{
    if (ah_mage_check(u))
        ((void (*)(int*, int, int, int, int))g_proc_00455EA2)(u, b, c, d, e);//original
}

PROC g_proc_0047623F;
void ah_rec_act_set2(int* u, int b, int c, int d, int e)
{
    if (ah_mage_check(u))
        ((void (*)(int*, int, int, int, int))g_proc_0047623F)(u, b, c, d, e);//original
}

bool ah_place_check(int* u)
{
    byte p = (*(byte*)((uintptr_t)u + S_OWNER));
    if (p < 8)
    {
        byte b = (*(byte*)((uintptr_t)u + S_PEON_BUILD));
        int t = -1;
        if ((b == U_INVENTOR) || (b == U_ALCHEMIST) || (b == U_OGREMOUND) || (b == U_STABLES))t = 0;
        if ((b == U_ALTAR) || (b == U_CHURCH) || (b == U_MAGE_TOWER) || (b == U_TEMPLE)
            || (b == U_AVIARY) || (b == U_DRAGONROOST))t = 1;
        if ((b == U_SHIPYARD) || (b == U_WHARF))t = 2;
        if ((b == U_HREFINERY) || (b == U_OREFINERY) || (b == U_HFOUNDRY) || (b == U_OFOUNDRY))t = 3;
        if (t != -1)
        {
            if ((maxtir[p] & (1 << t)) == 0)
            {
                unit_kill(u);
                return false;
            }
        }
    }
    return true;
}

PROC g_proc_0043B116;
void ah_placebox(int* u, int b, int c, int d, int e)
{
    if (ah_place_check(u))
        ((void (*)(int*, int, int, int, int))g_proc_0043B116)(u, b, c, d, e);//original
}

void send_cheat_res(byte pl, byte res, WORD kol)
{
    byte local = *(byte*)LOCAL_PLAYER;
    if (get_player_team(local) != 9)
    {
        int b = *(int*)CHEATBITS;
        b &= ~(262143 << 12);
        b |= (1 << 7);//7 is auto loss cheat
        b |= (pl << 12);
        b |= (res << 15);
        b |= (kol << 17);
        ((void (*)(int))F_SEND_CHEAT_PACKET)(b);
    }
    else
    {
        char msg[] = "OBS cannot send resources";
        show_message(10, msg);
    }
}

void send_cheat_kill(byte pl)
{
    int b = *(int*)CHEATBITS;
    b &= ~(255 << 12);
    b |= (1 << 6);//ss
    b |= (1 << 12);//kill
    b |= (pl << 14);//player id
    ((void (*)(int))F_SEND_CHEAT_PACKET)(b);
}

void send_cheat_obs(byte pl)
{
    int b = *(int*)CHEATBITS;
    b &= ~(255 << 12);
    b |= (1 << 6);//ss
    b |= (1 << 13);//obs
    b |= (pl << 14);//player id
    ((void (*)(int))F_SEND_CHEAT_PACKET)(b);
}

void song_change(int id)
{
    if ((id >= 0) && (id <= 25))
    {
        *(byte*)SONG_ID = id % 256;
        if (((int (*)())F_SONG_LOAD)())
        {
            ((void (*)())F_SONG_STOP)();
            ((void (*)(int))F_SONG_PLAY)(*(byte*)SONG_ID);
        }
    }
}

PROC g_proc_0044CEBB;
void get_msg(int a, char* text)
{
    //this function called when you write in game chat and press enter
    ((void (*)(int, char*))g_proc_0044CEBB)(a, text);//original
    for (int i = 0; i < 60; i++)
    {
        msg_written_text[i] = text[i];
    }
    char* token = strtok(msg_written_text, " ");
    if (token != NULL)
    {
        //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "/song", -1) == CSTR_EQUAL)
        if (lstrcmpi(token, "/song") == 0)
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                char k[] = "00";
                for (int i = 0; i < 2; i++)
                {
                    k[i] = token[i];
                }
                int s_id = 0;
                for (int i = 0; (i < 5) && (k[i] != 0); i++)
                {
                    char c = k[i];
                    if ((c >= '0') && (c <= '9'))
                    {
                        s_id *= 10;
                        s_id += (c - 48);
                    }
                }
                song_change(s_id);
                text[0] = 0;
            }
        }
        //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "/remove", -1) == CSTR_EQUAL)
        if (lstrcmpi(token, "/remove") == 0)
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                char name[60];
                memset(name, 0, sizeof(name));
                for (int i = 0; i < 6; i++)
                {
                    name[i] = tolower(token[i]);
                }
                int pl = -1;
                for (int ip = 0; ip < 19; ip++)
                {
                    //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, name, -1, str_colors[ip], -1) == CSTR_EQUAL)
                    if (lstrcmpi(token, str_colors[ip]) == 0)
                        pl = ip;
                }
                if (pl == 8)pl = 2;
                if (pl == 9)pl = 3;
                if ((pl >= 10) && (pl <= 17))pl -= 10;
                if (pl == 18)pl = *(byte*)(PLAYERS_COLORS + *(byte*)LOCAL_PLAYER);
                if (pl != -1)send_cheat_kill(pl);
                text[0] = 0;
            }
        }
        //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "/obs", -1) == CSTR_EQUAL)
        if (lstrcmpi(token, "/obs") == 0)
        {
            token = strtok(NULL, " ");
            if (token != NULL)
            {
                char name[60];
                memset(name, 0, sizeof(name));
                for (int i = 0; i < 6; i++)
                {
                    name[i] = tolower(token[i]);
                }
                int pl = -1;
                for (int ip = 0; ip < 19; ip++)
                {
                    //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, name, -1, str_colors[ip], -1) == CSTR_EQUAL)
                    if (lstrcmpi(token, str_colors[ip]) == 0)
                        pl = ip;
                }
                if (pl == 8)pl = 2;
                if (pl == 9)pl = 3;
                if ((pl >= 10) && (pl <= 17))pl -= 10;
                if (pl == 18)pl = *(byte*)(PLAYERS_COLORS + *(byte*)LOCAL_PLAYER);
                if (pl != -1)send_cheat_obs(pl);
                text[0] = 0;
            }
        }
        if (replaced)
        {
            if (send_res)
            {
                //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, token, -1, "/send", -1) == CSTR_EQUAL)
                if (lstrcmpi(token, "/send") == 0)
                {
                    token = strtok(NULL, " ");
                    if (token != NULL)
                    {
                        char k[] = "00000";
                        for (int i = 0; i < 5; i++)
                        {
                            k[i] = token[i];
                        }
                        token = strtok(NULL, " ");
                        if (token != NULL)
                        {
                            char res[] = "000000";
                            for (int i = 0; i < 6; i++)
                            {
                                res[i] = tolower(token[i]);
                            }
                            token = strtok(NULL, " ");
                            if (token != NULL)
                            {
                                char name[60];
                                memset(name, 0, sizeof(name));
                                for (int i = 0; i < 6; i++)
                                {
                                    name[i] = tolower(token[i]);
                                }

                                int kol = 0;
                                for (int i = 0; (i < 5) && (k[i] != 0); i++)
                                {
                                    char c = k[i];
                                    if ((c >= '0') && (c <= '9'))
                                    {
                                        kol *= 10;
                                        kol += (c - 48);
                                    }
                                }

                                int ress = -1;
                                for (int ir = 0; ir < 3; ir++)
                                {
                                    //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, res, -1, str_res[ir], -1) == CSTR_EQUAL)
                                    if (lstrcmpi(res, str_res[ir]) == 0)
                                        ress = ir;
                                }

                                int pl = -1;
                                for (int ip = 0; ip < 19; ip++)
                                {
                                    //if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, name, -1, str_colors[ip], -1) == CSTR_EQUAL)
                                    if (lstrcmpi(name, str_colors[ip]) == 0)
                                        pl = ip;
                                }
                                if (pl == 8)pl = 2;
                                if (pl == 9)pl = 3;
                                if ((pl >= 10) && (pl <= 17))pl -= 10;
                                if (pl == 18)pl = *(byte*)(PLAYERS_COLORS + *(byte*)LOCAL_PLAYER);
                                if (kol > 8000)
                                {
                                    char msg[] = "Too many!\x3 MAX\x5 resource send is\x3 8000";
                                    show_message(20, msg);
                                }
                                else
                                {
                                    if ((pl != -1) && (ress != -1))
                                        send_cheat_res(pl, ress, kol);
                                }
                                text[0] = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}

PROC g_proc_004425C1;
void cell_set_pos(WORD x, WORD y)
{
    if (m_holy_cam)
        ((void (*)(WORD, WORD))g_proc_004425C1)(x, y);//original
}

PROC g_proc_00457D4A;
void select_many_units(int n, int a,int b,int c)
{
    DWORD ctrl = *(DWORD*)CTRL_STATUS;
    if (ctrl == 0)
    {
        if (!m_double_click)
            n = 1;
    }
    ((void (*)(int, int, int, int))g_proc_00457D4A)(n, a, b, c);//original
}

PROC g_proc_00420EFD;
void status_res()
{
    byte local = *(byte*)LOCAL_PLAYER;
    if (get_player_team(local) == 9)
    {
        char buf[] = "\x0";
        buf[0] = local;
        int* p = NULL;
        p = (int*)LOCAL_UNITS_SELECTED;
        if (p)
        {
            p = (int*)(*p);
            if (p)buf[0] = *((byte*)((uintptr_t)p + S_OWNER));
        }
        if ((buf[0] >= 0) && (buf[0] <= 7))
            PATCH_SET((char*)LOCAL_PLAYER, buf);
        ((void (*)())g_proc_00420EFD)();//original
        buf[0] = local;
        PATCH_SET((char*)LOCAL_PLAYER, buf);
    }
    else
        ((void (*)())g_proc_00420EFD)();//original
}

PROC g_proc_00420F02;
void status_main()
{
    byte local = *(byte*)LOCAL_PLAYER;
    if (get_player_team(local) == 9)
    {
        char buf[] = "\x0";
        buf[0] = local;
        int* p = NULL;
        p = (int*)LOCAL_UNITS_SELECTED;
        if (p)
        {
            p = (int*)(*p);
            if (p)buf[0] = *((byte*)((uintptr_t)p + S_OWNER));
        }
        if ((buf[0] >= 0) && (buf[0] <= 7))
            PATCH_SET((char*)LOCAL_PLAYER, buf);
        ((void (*)())g_proc_00420F02)();//original
        buf[0] = local;
        PATCH_SET((char*)LOCAL_PLAYER, buf);
    }
    else
        ((void (*)())g_proc_00420F02)();//original
}

PROC g_proc_0044528C;
void status_buttons_draw_pal(int col)
{
    if (get_player_team(*(byte*)LOCAL_PLAYER) == 9)
    {
        int buf = 0;
        int* p = (int*)LOCAL_UNITS_SELECTED;
        if (p)
        {
            p = (int*)(*p);
            if (p)buf = *((byte*)((uintptr_t)p + S_OWNER));
        }
        if ((buf >= 0) && (buf <= 7))
            col = buf;
    }
    ((void (*)(int))g_proc_0044528C)(col);//original
}

PROC g_proc_0041F780;
void players_info_before_reset()
{
    for (int i = 0; i < 8; i++)
        set_player_team(i, teams_host[i]);
    ((void (*)())g_proc_0041F780)();//original
}

void sounds_ready_table_set(byte id, WORD snd)
{
    char buf[] = "\x0\x0";
    buf[0] = snd % 256;
    buf[1] = snd / 256;
    PATCH_SET((char*)(UNIT_SOUNDS_READY_TABLE + 2 * id), buf);
}

void sounds_tables()
{
    sounds_ready_table_set(U_CRITTER, 247);
    sounds_ready_table_set(U_DEMON, 301);

    sounds_ready_table_set(U_ALLERIA, 254);
    sounds_ready_table_set(U_DANATH, 263);
    sounds_ready_table_set(U_HADGAR, 272);
    sounds_ready_table_set(U_KURDRAN, 281);
    sounds_ready_table_set(U_TYRALYON, 290);
    sounds_ready_table_set(U_UTER, 154);
    sounds_ready_table_set(U_LOTHAR, 155);

    sounds_ready_table_set(U_DEATHWING, 300);
    sounds_ready_table_set(U_DENTARG, 310);
    sounds_ready_table_set(U_GROM, 318);
    sounds_ready_table_set(U_KARGATH, 327);
    sounds_ready_table_set(U_TERON, 336);
    sounds_ready_table_set(U_ZULJIN, 204);
    sounds_ready_table_set(U_CHOGAL, 179);
    sounds_ready_table_set(U_GULDAN, 88);

}

PROC g_proc_00478441;
void apm_stop_peons()
{
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)())g_proc_00478441)();//original
}

PROC g_proc_0047842F;
void apm_move_peons()
{
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)())g_proc_0047842F)();//original
}

PROC g_proc_004765C0;
void apm_right_click(WORD x,WORD y,int a)
{
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
    {
        bool f = false;
        if ((apm_rx == x) && (apm_ry == y))
        {
            f = apm_rt == 0;
        }
        else
            f = true;
        if (f)
        {
            byte spd = *(byte*)CURRENT_SPEED;
            spd = *(byte*)(SLOWEST_SPEED + spd);
            if (spd == 0)spd = 1;
            int tik_per_sec = 1000 / spd;
            apm_rt = 2 * tik_per_sec;
            apm_rx = x;
            apm_ry = y;
            apm_add(1);
        }
    }
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)(WORD, WORD, int))g_proc_004765C0)(x, y, a);//original
}

PROC g_proc_00478470;
void apm_action(int a)
{
    byte plr = *(byte*)PACKET_PLAYER;
    if ((*(char*)MULTICAST_FIX == '\xeb') && (*(byte*)(a + 7) == 0x31))//0x31 = bloodlust
    {
        if (get_player_team(plr) != 9)
        {
            byte selected[9 * 4] = { 0 };
            for (int i = 0; i < 9 * 4; i++)
            {
                selected[i] = *(byte*)(UNITS_SELECTED + 9 * 4 * plr + i);
                *(byte*)(UNITS_SELECTED + 9 * 4 * plr + i) = 0;
            }
            bool trgs = false;
            for (int k = 0; k < 9; k++)
            {
                int kk = k * 4;
                int next = selected[kk] + 256 * selected[kk + 1] + 256 * 256 * selected[kk + 2] + 256 * 256 * 256 * selected[kk + 3];
                if (*(WORD*)(a + 5) == ((WORD(*)(int, int))F_UNIT_FIXUP)(next, 1))//unit_fixup
                    trgs = true;
            }
            if (*(WORD*)(a + 5) == 0xFFFF)
                trgs = true;
            for (int k = 0; k < 9; k++)
            {
                for (int i = k * 4; i < k * 4 + 4; i++)
                    *(byte*)(UNITS_SELECTED + 9 * 4 * plr + i) = selected[i];
                if (trgs || (!trgs && (k != 0)))
                {
                    int kc = k;
                    *(WORD*)(a + 5) = 0xFFFF;
                    while ((*(WORD*)(a + 5) == 0xFFFF) && (kc < 9))
                    {
                        int kk = kc == 8 ? 0 : (kc + 1) * 4;
                        int next = selected[kk] + 256 * selected[kk + 1] + 256 * 256 * selected[kk + 2] + 256 * 256 * 256 * selected[kk + 3];
                        *(WORD*)(a + 5) = ((WORD(*)(int, int))F_UNIT_FIXUP)(next, 1);//unit_fixup
                        kc++;
                    }
                }
                ((void (*)(int))g_proc_00478470)(a);//original
                for (int i = k * 4; i < k * 4 + 4; i++)
                    *(byte*)(UNITS_SELECTED + 9 * 4 * plr + i) = 0;
            }
            for (int i = 0; i < 9 * 4; i++)
            {
                *(byte*)(UNITS_SELECTED + 9 * 4 * plr + i) = selected[i];
            }
        }
        if (plr == *(byte*)LOCAL_PLAYER)
            apm_add(1);
    }
    else
    {
        if (plr == *(byte*)LOCAL_PLAYER)
            apm_add(1);
        if (get_player_team(plr) != 9)
            ((void (*)(int))g_proc_00478470)(a);//original
    }
}

PROC g_proc_00475DB1;
int apm_select(int a, int b, int c)
{
    int original = ((int (*)(int, int, int))g_proc_00475DB1)(a, b, c);//original
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
    {
        bool f = false;
        for (int i = 0; i < 9 * 4; i++)
        {
            if (apm_s[i] != *(byte*)(UNITS_SELECTED + *(byte*)LOCAL_PLAYER * 9 * 4 + i))
            {
                apm_s[i] = *(byte*)(UNITS_SELECTED + *(byte*)LOCAL_PLAYER * 9 * 4 + i);
                f = true;
            }
        }
        if (f)
            apm_add(1);
    }
    return original;
}

PROC g_proc_004766D2;
void apm_group_select(int a)
{
    ((void (*)(int))g_proc_004766D2)(a);//original
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
    {
        bool f = false;
        for (int i = 0; i < 9 * 4; i++)
        {
            if (apm_s[i] != *(byte*)(UNITS_SELECTED + *(byte*)LOCAL_PLAYER * 9 * 4 + i))
            {
                apm_s[i] = *(byte*)(UNITS_SELECTED + *(byte*)LOCAL_PLAYER * 9 * 4 + i);
                f = true;
            }
        }
        if (f)
            apm_add(1);
    }
}

PROC g_proc_004766C4;
void apm_group_create(int a)
{
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
    {
        if (apm_gct == 0)
        {
            byte spd = *(byte*)CURRENT_SPEED;
            spd = *(byte*)(SLOWEST_SPEED + spd);
            if (spd == 0)spd = 1;
            int tik_per_sec = 1000 / spd;
            apm_gct = tik_per_sec;
            apm_add(1);
        }
    }
    ((void (*)(int))g_proc_004766C4)(a);//original
}

PROC g_proc_00478456;
void apm_placebox(int a)
{
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
        apm_add(1);
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)(int))g_proc_00478456)(a);//original
}

PROC g_proc_00478487;
void apm_cancel_building()
{
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
        apm_add(1);
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)())g_proc_00478487)();//original
}

PROC g_proc_0047849C;
void apm_train_unit(int a)
{
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
        apm_add(1);
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)(int))g_proc_0047849C)(a);//original
}

PROC g_proc_004784B3;
void apm_cancel_unit()
{
    if (*(byte*)PACKET_PLAYER == *(byte*)LOCAL_PLAYER)
        apm_add(1);
    if (get_player_team(*(byte*)PACKET_PLAYER) != 9)
        ((void (*)())g_proc_004784B3)();//original
}

PROC g_proc_0044AC1F;
void apm_space(WORD x, WORD y)
{
    if (apm_st == 0)
    {
        byte spd = *(byte*)CURRENT_SPEED;
        spd = *(byte*)(SLOWEST_SPEED + spd);
        if (spd == 0)spd = 1;
        int tik_per_sec = 1000 / spd;
        apm_st = tik_per_sec;
        apm_add(1);
    }
    ((void (*)(WORD, WORD))g_proc_0044AC1F)(x, y);//original
}

PROC g_proc_0041FF92;
void apm_f2f3f4(WORD x, WORD y)
{
    if (apm_ft == 0)
    {
        byte spd = *(byte*)CURRENT_SPEED;
        spd = *(byte*)(SLOWEST_SPEED + spd);
        if (spd == 0)spd = 1;
        int tik_per_sec = 1000 / spd;
        apm_ft = tik_per_sec;
        apm_add(1);
    }
    ((void (*)(WORD, WORD))g_proc_0041FF92)(x, y);//original
}

PROC g_proc_0042E105;
void apm_map_click(WORD x, WORD y)
{
    if (apm_mt == 0)
    {
        byte spd = *(byte*)CURRENT_SPEED;
        spd = *(byte*)(SLOWEST_SPEED + spd);
        if (spd == 0)spd = 1;
        int tik_per_sec = 1000 / spd;
        apm_mt = tik_per_sec;
        apm_add(1);
    }
    ((void (*)(WORD, WORD))g_proc_0042E105)(x, y);//original
}

bool game_start_lag = false;
PROC g_proc_0046CDA2;
int game_starting_players_number()
{
    int original = ((int (*)())g_proc_0046CDA2)();//original
    if (original > 1)
    {
        game_start_lag = false;
        for (int i = 0; i < 8; i++)
        {
            if ((teams_lag[i] != 0) && (teams_lag[i] != teams_lag[*(byte*)LOCAL_PLAYER]))
            {
                game_start_lag = true;
                original = 1;
            }
        }
    }
    return original;
}

PROC g_proc_0046CDCE;
int game_starting_players_message(int a)
{
    if (game_start_lag)
    {
        char msg[] = "       Can't start game\n\x4Possible reasons:\n\n1. Someone \x3lagging \x4hard\n\n2. Someone using \x3/ignore\x4 for host or vice versa\nto fix \"ignoring\" reason use \x3/unignore\x5 PlayerName\x4";
        ((void (*)(char*, int))F_SHOW_BIGOK_DIALOG)(msg, MENU_MODAL_DIALOG);
        return 1;
    }
    else
        return ((int (*)(int))g_proc_0046CDCE)(a);//original
}

struct combobox
{
    bool clik = false;
    int x = 0;
    int y = 0;
    byte val = 0;
};
combobox* COMB[8 + 6 + 1];//8 lobby 6 info colors 1 apm type
bool gm_lobby_click = false;

void create_comboboxes()
{
    for (int i = 0; i < 8 + 6 + 1; i++)
    {
        COMB[i] = new combobox;
    }
}

char help_txt[] = "\x34\x00\x6a\x00\x84\x00\x85\x00\x9a\x00\xb5\x00\xb6\x00\xcc\x00\xe7\x00\xe8\x00\xfb\x00\x16\x01\x1e\x01\x1f\x01\x20\x01\x3a\x01\x3b\x01\x55\x01\x73\x01\x8e\x01\x8f\x01\x90\x01\x91\x01\x92\x01\x93\x01\x94\x01\x95\x01\x96\x01\xae\x01\xaf\x01\xcd\x01\xf2\x01\xf3\x01\xf4\x01\xf5\x01\xf6\x01\xf7\x01\xf8\x01\xf9\x01\xfa\x01\xfb\x01\x10\x02\x11\x02\x2a\x02\x2b\x02\x40\x02\x4a\x02\x54\x02\x55\x02\x56\x02\x57\x02\x58\x02\x59\x02\x09\x09\x09\x31\x2e\x20\x45\x78\x74\x72\x61\x20\x69\x6e\x66\x6f\x20\x6f\x70\x74\x69\x6f\x6e\x73\x2e\x00\x00\x09\x04\x54\x69\x6d\x65\x72\x01\x09\x09\x09\x09\x09\x09\x63\x6f\x6c\x6f\x72\x3a\x00\x09\x09\x70\x6f\x73\x69\x74\x69\x6f\x6e\x3a\x09\x09\x09\x09\x62\x61\x63\x6b\x67\x72\x6f\x75\x6e\x64\x3a\x00\x00\x09\x04\x54\x69\x63\x6b\x65\x72\x01\x09\x09\x09\x09\x09\x09\x63\x6f\x6c\x6f\x72\x3a\x00\x09\x09\x70\x6f\x73\x69\x74\x69\x6f\x6e\x3a\x09\x09\x09\x09\x62\x61\x63\x6b\x67\x72\x6f\x75\x6e\x64\x3a\x00\x00\x09\x04\x41\x50\x4d\x01\x09\x09\x09\x09\x09\x09\x63\x6f\x6c\x6f\x72\x3a\x00\x09\x09\x70\x6f\x73\x69\x74\x69\x6f\x6e\x3a\x09\x09\x09\x09\x62\x61\x63\x6b\x67\x72\x6f\x75\x6e\x64\x3a\x00\x09\x09\x74\x79\x70\x65\x3a\x00\x00\x00\x09\x09\x09\x05\x32\x2e\x20\x4f\x62\x73\x65\x72\x76\x65\x72\x20\x6f\x70\x74\x69\x6f\x6e\x73\x2e\x01\x00\x00\x09\x05\x4f\x42\x53\x20\x73\x74\x61\x74\x73\x01\x09\x09\x09\x09\x09\x05\x63\x6f\x6c\x6f\x72\x3a\x01\x00\x09\x09\x05\x70\x6f\x73\x69\x74\x69\x6f\x6e\x3a\x09\x09\x09\x09\x05\x62\x61\x63\x6b\x67\x72\x6f\x75\x6e\x64\x3a\x01\x00\x09\x09\x05\x74\x79\x70\x65\x3a\x09\x09\x09\x09\x09\x05\x61\x75\x78\x2e\x20\x63\x6f\x6c\x6f\x72\x3a\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09\x09\x09\x33\x2e\x20\x47\x61\x6d\x65\x70\x6c\x61\x79\x20\x6f\x70\x74\x69\x6f\x6e\x73\x2e\x00\x00\x09\x48\x6f\x6c\x79\x20\x56\x69\x73\x69\x6f\x6e\x20\x77\x69\x6c\x6c\x20\x6d\x6f\x76\x65\x20\x63\x61\x6d\x65\x72\x61\x00\x09\x44\x6f\x75\x62\x6c\x65\x63\x6c\x69\x63\x6b\x20\x77\x69\x6c\x6c\x20\x70\x69\x63\x6b\x20\x73\x69\x6d\x69\x6c\x61\x72\x20\x75\x6e\x69\x74\x73\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x09\x09\x09\x34\x2e\x20\x4f\x74\x68\x65\x72\x20\x6f\x70\x74\x69\x6f\x6e\x73\x2e\x00\x00\x09\x53\x68\x6f\x77\x20\x6d\x6f\x64\x20\x6f\x70\x74\x69\x6f\x6e\x73\x20\x62\x75\x74\x74\x6f\x6e\x00\x00\x09\x4c\x6f\x62\x62\x79\x20\x54\x65\x61\x6d\x20\x6d\x61\x72\x6b\x65\x72\x73\x3a\x00\x09\x09\x43\x6f\x6c\x6f\x72\x65\x64\x00\x09\x09\x4e\x75\x6d\x65\x72\x69\x63\x00\x00\x00\x00\x00\x00\x00";
char help_bin[] = "War2Mod Settings";
bool HELP_TXT = false;
int HELP_POS = 1;

PROC g_proc_0042ECC6;
int dialogs_load()
{
    int original = ((int (*)())g_proc_0042ECC6)();//original
    if (HELP_TXT)
    {
        int dialog = *(int*)MENU_HELP_DIALOG;
        char buf[] = "\x0\x0\x0\x0";
        patch_setdword((DWORD*)buf, (DWORD)help_bin);
        PATCH_SET((char*)(dialog + 0x17C), buf);
    }
    return original;
}

PROC g_proc_004249C9;
int help_txt_load(int a, int b, int c, int d)
{
    int original = ((int (*)(int, int, int, int))g_proc_004249C9)(a, b, c, d);//original
    if (HELP_TXT)return (int)help_txt;
    return original;
}

PROC g_proc_004248BC;
int help_txt_change_pos(int a, int b)
{
    int original = ((int (*)(int, int))g_proc_004248BC)(a, b);//original
    HELP_POS = *(int*)MENU_HELP_POS;
    return original;
}

void mod_menu_closed()
{
    if (*(byte*)DLL_STATUS == 1)
    {
        if (menu_1)
        {
            ini_entry_list_t list;
            list = NULL;
            char buf[] = "0";
            char buf2[] = "000";

            buf[0] = m_show_timer + '0';
            AddEntryToList(&list, INI_TXT_TIMER, INI_TXT_ENABLED, buf);
            buf2[2] = (m_timer_col % 10) + '0';
            buf2[1] = ((m_timer_col / 10) % 10) + '0';
            buf2[0] = (m_timer_col / 100) + '0';
            AddEntryToList(&list, INI_TXT_TIMER, INI_TXT_COL, buf2);
            buf2[2] = (m_timer_bcol % 10) + '0';
            buf2[1] = ((m_timer_bcol / 10) % 10) + '0';
            buf2[0] = (m_timer_bcol / 100) + '0';
            AddEntryToList(&list, INI_TXT_TIMER, INI_TXT_BCOL, buf2);
            buf2[2] = (m_timer_x % 10) + '0';
            buf2[1] = ((m_timer_x / 10) % 10) + '0';
            buf2[0] = (m_timer_x / 100) + '0';
            AddEntryToList(&list, INI_TXT_TIMER, INI_TXT_POSX, buf2);
            buf2[2] = (m_timer_y % 10) + '0';
            buf2[1] = ((m_timer_y / 10) % 10) + '0';
            buf2[0] = (m_timer_y / 100) + '0';
            AddEntryToList(&list, INI_TXT_TIMER, INI_TXT_POSY, buf2);

            buf[0] = m_show_ticker + '0';
            AddEntryToList(&list, INI_TXT_TICKER, INI_TXT_ENABLED, buf);
            buf2[2] = (m_ticker_col % 10) + '0';
            buf2[1] = ((m_ticker_col / 10) % 10) + '0';
            buf2[0] = (m_ticker_col / 100) + '0';
            AddEntryToList(&list, INI_TXT_TICKER, INI_TXT_COL, buf2);
            buf2[2] = (m_ticker_bcol % 10) + '0';
            buf2[1] = ((m_ticker_bcol / 10) % 10) + '0';
            buf2[0] = (m_ticker_bcol / 100) + '0';
            AddEntryToList(&list, INI_TXT_TICKER, INI_TXT_BCOL, buf2);
            buf2[2] = (m_ticker_x % 10) + '0';
            buf2[1] = ((m_ticker_x / 10) % 10) + '0';
            buf2[0] = (m_ticker_x / 100) + '0';
            AddEntryToList(&list, INI_TXT_TICKER, INI_TXT_POSX, buf2);
            buf2[2] = (m_ticker_y % 10) + '0';
            buf2[1] = ((m_ticker_y / 10) % 10) + '0';
            buf2[0] = (m_ticker_y / 100) + '0';
            AddEntryToList(&list, INI_TXT_TICKER, INI_TXT_POSY, buf2);

            buf[0] = m_show_apm + '0';
            AddEntryToList(&list, INI_TXT_APM, INI_TXT_ENABLED, buf);
            buf2[2] = (m_apm_col % 10) + '0';
            buf2[1] = ((m_apm_col / 10) % 10) + '0';
            buf2[0] = (m_apm_col / 100) + '0';
            AddEntryToList(&list, INI_TXT_APM, INI_TXT_COL, buf2);
            buf2[2] = (m_apm_bcol % 10) + '0';
            buf2[1] = ((m_apm_bcol / 10) % 10) + '0';
            buf2[0] = (m_apm_bcol / 100) + '0';
            AddEntryToList(&list, INI_TXT_APM, INI_TXT_BCOL, buf2);
            buf2[2] = (m_apm_x % 10) + '0';
            buf2[1] = ((m_apm_x / 10) % 10) + '0';
            buf2[0] = (m_apm_x / 100) + '0';
            AddEntryToList(&list, INI_TXT_APM, INI_TXT_POSX, buf2);
            buf2[2] = (m_apm_y % 10) + '0';
            buf2[1] = ((m_apm_y / 10) % 10) + '0';
            buf2[0] = (m_apm_y / 100) + '0';
            AddEntryToList(&list, INI_TXT_APM, INI_TXT_POSY, buf2);
            buf[0] = m_apm_type + '0';
            AddEntryToList(&list, INI_TXT_APM, INI_TXT_TYPE, buf);

            buf[0] = m_holy_cam + '0';
            AddEntryToList(&list, INI_TXT_GAMEPLAY, INI_TXT_HOLY, buf);
            buf[0] = m_double_click + '0';
            AddEntryToList(&list, INI_TXT_GAMEPLAY, INI_TXT_DCLICK, buf);

            buf[0] = m_lobby_comb + '0';
            AddEntryToList(&list, INI_TXT_LOBBY, INI_TXT_TYPE, buf);
            buf[0] = m_lobby_interface + '0';
            AddEntryToList(&list, INI_TXT_LOBBY, INI_TXT_INTERFACE, buf);

            buf[0] = m_show_button + '0';
            AddEntryToList(&list, INI_TXT_WAR2MOD, INI_TXT_BUTTON, buf);

            MakeINIFile(INI_PATH, list);
            FreeList(list);
        }
        menu_1 = false;
    }
}

PROC g_proc_00402761;
void menu_closed(int a, int b)
{
    ((void (*)(int, int))g_proc_00402761)(a, b);//original
    mod_menu_closed();
}

int* menu_pointer = NULL;
void mod_options_menu_open()
{
    byte chat_open = *(byte*)MENU_CHAT_OPEN;
    int menu_open = *(int*)MENU_OPEN1;
    int menu_open2 = *(int*)MENU_OPEN2;
    if ((menu_open == 0) && (menu_open2 == 0) && (chat_open == 0))
    {
        HELP_POS = 1;
        HELP_TXT = true;
        ((void (*)())F_SHOW_DIALOG_HELP)();//call help.bin
        HELP_TXT = false;
        menu_1 = true;
        COMB[0]->val = 3;
        COMB[1]->val = 2;
        menu_pointer = (int*)*(int*)MENU_OPEN1;
    }
}

void mod_options_menu_close()
{
    DWORD a = 0x0002000B;
    patch_setdword((DWORD*)MENU_STATUS, 0xFFFFFFFE);
    ((void (*)(int*, DWORD*))F_MENU_CLOSE)(menu_pointer, &a);
    mod_menu_closed();
}

bool mouse_inside(WORD mx, WORD my, WORD x1, WORD y1, WORD x2, WORD y2)
{
    return (mx >= x1) && (my >= y1) && (mx <= x2) && (my <= y2);
}

void ev_change_pos(WORD mx, WORD my, WORD px, WORD py, WORD* xx, WORD* yy)
{
    bool inval = false;
    if (mouse_inside(mx, my, px, py, px + 20, py + 12))
    {
        *xx = m_minx;
        *yy = m_miny;
        inval = true;
    }
    if (mouse_inside(mx, my, px, py + 14, px + 20, py + 14 + 12))
    {
        *xx = m_minx;
        *yy = m_maxy;
        inval = true;
    }
    if (mouse_inside(mx, my, px + 44, py, px + 44 + 20, py + 12))
    {
        *xx = m_maxx;
        *yy = m_miny;
        inval = true;
    }
    if (mouse_inside(mx, my, px + 44, py + 14, px + 44 + 20, py + 14 + 12))
    {
        *xx = m_maxx;
        *yy = m_maxy;
        inval = true;
    }
    if (mouse_inside(mx, my, px + 22, py, px + 22 + 20, py + 12))
    {
        *xx = m_minx + (m_screen_w - 200) / 2;
        *yy = m_miny;
        inval = true;
    }
    if (mouse_inside(mx, my, px + 22, py + 14, px + 22 + 20, py + 14 + 12))
    {
        *xx = m_minx + (m_screen_w - 200) / 2;
        *yy = m_maxy;
        inval = true;
    }
    if (inval)inval_game_view();
}

WORD EV_PREV = EVENT_NOTHING;
bool EV_CALL = false;
void events_main(WORD mx, WORD my, WORD ev, byte key)
{
    EV_PREV = ev;
    if (*(byte*)DLL_STATUS == 1)
    {
        if (*(byte*)GAME_MODE == 3)
        {
            if (ev == EVENT_MOUSE_MOVE)
            {
                menu_button_hover = mouse_inside(mx, my, 168, 0, 200, 15);
            }
            if (!menu_1)
            {
                if (ev == EVENT_KEY_PRESSED)
                {
                    if (key == 117)//F6
                    {
                        mod_options_menu_open();
                    }
                }
                if (ev == EVENT_LEFT_PRESSED)
                {
                    if (mouse_inside(mx, my, 168, 0, 200, 15))
                    {
                        mod_options_menu_open();
                    }
                }
            }
            else
            {
                if (ev == EVENT_KEY_PRESSED)
                {
                    if (key == 117)//F6
                    {
                        mod_options_menu_close();
                    }
                }
                if (ev == EVENT_LEFT_PRESSED)
                {
                    if (mouse_inside(mx, my, 168, 0, 200, 15))
                    {
                        mod_options_menu_close();
                    }
                }
                if (ev == EVENT_MOUSE_MOVE)
                {
                    if (m_drag == 1)
                    {
                        //m_timer_x = ((mx - m_drag_x) / 8) * 8;
                        m_timer_x = mx - m_drag_x;
                        if ((my - m_drag_y) >= 0)
                            m_timer_y = ((my - m_drag_y) / 8) * 8;
                        else
                            m_timer_y = 0;
                        if (m_timer_x < m_minx)m_timer_x = m_minx;
                        if (m_timer_x > m_maxx)m_timer_x = m_maxx;
                        if (m_timer_y < m_miny)m_timer_y = m_miny;
                        if (m_timer_y > m_maxy)m_timer_y = m_maxy;
                        inval_game_view();
                    }
                    if (m_drag == 2)
                    {
                        //m_ticker_x = ((mx - m_drag_x) / 8) * 8;
                        m_ticker_x = mx - m_drag_x;
                        if ((my - m_drag_y) >= 0)
                            m_ticker_y = ((my - m_drag_y) / 8) * 8;
                        else
                            m_ticker_y = 0;
                        if (m_ticker_x < m_minx)m_ticker_x = m_minx;
                        if (m_ticker_x > m_maxx)m_ticker_x = m_maxx;
                        if (m_ticker_y < m_miny)m_ticker_y = m_miny;
                        if (m_ticker_y > m_maxy)m_ticker_y = m_maxy;
                        inval_game_view();
                    }
                    if (m_drag == 3)
                    {
                        //m_apm_x = ((mx - m_drag_x) / 8) * 8;
                        m_apm_x = mx - m_drag_x;
                        if ((my - m_drag_y) >= 0)
                            m_apm_y = ((my - m_drag_y) / 8) * 8;
                        else
                            m_apm_y = 0;
                        if (m_apm_x < m_minx)m_apm_x = m_minx;
                        if (m_apm_x > m_maxx)m_apm_x = m_maxx;
                        if (m_apm_y < m_miny)m_apm_y = m_miny;
                        if (m_apm_y > m_maxy)m_apm_y = m_maxy;
                        inval_game_view();
                    }
                }
                if (ev == EVENT_LEFT_PRESSED)
                {
                    if (m_drag == 0)
                    {
                        WORD xx = 0;
                        xx = m_timer_x;
                        if (xx >= (m_maxx + 8 - m_timer_size))xx = m_maxx + 8 - m_timer_size;
                        if (mouse_inside(mx, my, xx, m_timer_y, m_timer_x + 80, m_timer_y + 8))
                        {
                            if (m_show_timer)
                            {
                                m_drag = 1;
                                m_drag_x = mx - m_timer_x;
                                m_drag_y = my - m_timer_y;
                            }
                        }
                        xx = m_ticker_x;
                        if (xx >= (m_maxx + 8 - m_ticker_size))xx = m_maxx + 8 - m_ticker_size;
                        if (mouse_inside(mx, my, xx, m_ticker_y, m_ticker_x + 80, m_ticker_y + 8))
                        {
                            if (m_show_ticker)
                            {
                                m_drag = 2;
                                m_drag_x = mx - m_ticker_x;
                                m_drag_y = my - m_ticker_y;
                            }
                        }
                        xx = m_apm_x;
                        if (xx >= (m_maxx + 8 - m_apm_size))xx = m_maxx + 8 - m_apm_size;
                        if (mouse_inside(mx, my, xx, m_apm_y, m_apm_x + 80, m_apm_y + 8))
                        {
                            if (m_show_apm)
                            {
                                m_drag = 3;
                                m_drag_x = mx - m_apm_x;
                                m_drag_y = my - m_apm_y;
                            }
                        }
                    }
                    if (HELP_POS == 1)
                    {
                        for (int i = 8; i < 8 + 6; i++)
                        {
                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                            {
                                COMB[i]->clik = true;
                            }
                        }
                        if (mouse_inside(mx, my, COMB[14]->x, COMB[14]->y, COMB[14]->x + 66, COMB[14]->y + 13))
                        {
                            COMB[14]->clik = true;
                        }
                    }
                    if (HELP_POS == 40)
                    {
                        for (int i = 0; i < 2; i++)
                        {
                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                            {
                                COMB[i]->clik = true;
                            }
                        }
                    }
                }
                if (ev == EVENT_LEFT_RELEASED)
                {
                    if (m_drag == 0)
                    {
                        if (HELP_POS == 1)
                        {
                            if (mouse_inside(mx, my, 241 + m_align_x, 141 + m_align_y,
                                241 + 17 + 90 + m_align_x, 141 + 17 + m_align_y))
                            {
                                m_show_timer = !m_show_timer;
                                inval_game_view();
                            }
                            if (mouse_inside(mx, my, 241 + m_align_x, 201 + m_align_y,
                                241 + 17 + 90 + m_align_x, 201 + 17 + m_align_y))
                            {
                                m_show_ticker = !m_show_ticker;
                                inval_game_view();
                            }
                            if (mouse_inside(mx, my, 241 + m_align_x, 261 + m_align_y,
                                241 + 17 + 90 + m_align_x, 261 + 17 + m_align_y))
                            {
                                m_show_apm = !m_show_apm;
                                inval_game_view();
                            }
                            ev_change_pos(mx, my, 354 + m_align_x, 148 + m_align_y, &m_timer_x, &m_timer_y);
                            ev_change_pos(mx, my, 354 + m_align_x, 208 + m_align_y, &m_ticker_x, &m_ticker_y);
                            ev_change_pos(mx, my, 354 + m_align_x, 268 + m_align_y, &m_apm_x, &m_apm_y);
                            for (int i = 8; i < 8 + 6; i++)
                            {
                                if (COMB[i]->clik)
                                {
                                    if ((i % 2) == 0)
                                    {
                                        for (int j = 0; j < C_COLORS_SIZE; j++)
                                        {
                                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y + 16 * j,
                                                COMB[i]->x + 24, COMB[i]->y + 16 * j + 16 - 1))
                                            {
                                                if (i == 8)
                                                {
                                                    if (!((colors[j] == 0) || (colors[j] == m_timer_bcol)))
                                                        m_timer_col = colors[j];
                                                    else
                                                        m_timer_col = 200;
                                                }
                                                if (i == 10)
                                                {
                                                    if (!((colors[j] == 0) || (colors[j] == m_ticker_bcol)))
                                                        m_ticker_col = colors[j];
                                                    else
                                                        m_ticker_col = 113;
                                                }
                                                if (i == 12)
                                                {
                                                    if (!((colors[j] == 0) || (colors[j] == m_apm_bcol)))
                                                        m_apm_col = colors[j];
                                                    else
                                                        m_apm_col = 246;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        for (int j = 0; j < C_BCOLORS_SIZE; j++)
                                        {
                                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y + 16 * j,
                                                COMB[i]->x + 24, COMB[i]->y + 16 * j + 16 - 1))
                                            {
                                                if (i == 9)
                                                {
                                                    if (bcolors[j] != m_timer_col)
                                                        m_timer_bcol = bcolors[j];
                                                    else
                                                        m_timer_bcol = 0;
                                                }
                                                if (i == 11)
                                                {
                                                    if (bcolors[j] != m_ticker_col)
                                                        m_ticker_bcol = bcolors[j];
                                                    else
                                                        m_ticker_bcol = 0;
                                                }
                                                if (i == 13)
                                                {
                                                    if (bcolors[j] != m_apm_col)
                                                        m_apm_bcol = bcolors[j];
                                                    else
                                                        m_apm_bcol = 0;
                                                }
                                            }
                                        }
                                    }
                                }
                                COMB[i]->clik = false;
                            }
                            if (COMB[14]->clik)
                            {
                                for (int j = 0; j < 3; j++)
                                {
                                    if (mouse_inside(mx, my, COMB[14]->x, COMB[14]->y + 13 * j,
                                        COMB[14]->x + 66, COMB[14]->y + 13 * j + 13 - 1))
                                    {
                                        m_apm_type = j;
                                    }
                                }
                            }
                            COMB[14]->clik = false;
                        }
                        if (HELP_POS == 14)
                        {
                            //obs
                        }
                        if (HELP_POS == 27)
                        {
                            if (mouse_inside(mx, my, 241 + m_align_x, 141 + m_align_y,
                                241 + 17 + 256 + m_align_x, 141 + 17 + m_align_y))
                            {
                                m_holy_cam = !m_holy_cam;
                            }
                            if (mouse_inside(mx, my, 241 + m_align_x, 161 + m_align_y,
                                241 + 17 + 256 + m_align_x, 161 + 17 + m_align_y))
                            {
                                m_double_click = !m_double_click;
                            }
                        }
                        if (HELP_POS == 40)
                        {
                            if (mouse_inside(mx, my, 241 + m_align_x, 141 + m_align_y,
                                241 + 17 + 300 + m_align_x, 141 + 17 + m_align_y))
                            {
                                m_show_button = !m_show_button;
                                m_show_button_last = true;
                            }
                            if (mouse_inside(mx, my, 241 - 12 + m_align_x, 200 + m_align_y,
                                241 + 17 + m_align_x, 200 + 20 + m_align_y)
                                || mouse_inside(mx, my, 241 + 50 + m_align_x, 200 + m_align_y,
                                    241 + 50 + 200 + m_align_x, 200 + 20 + m_align_y))
                            {
                                m_lobby_comb = 0;
                            }
                            if (mouse_inside(mx, my, 241 - 12 + m_align_x, 220 + m_align_y,
                                241 + 17 + m_align_x, 220 + 20 + m_align_y)
                                || mouse_inside(mx, my, 241 + 50 + m_align_x, 220 + m_align_y,
                                    241 + 50 + 200 + m_align_x, 220 + 20 + m_align_y))
                            {
                                m_lobby_comb = 1;
                            }
                            for (int i = 0; i < 2; i++)
                            {
                                if (COMB[i]->clik)
                                {
                                    for (int j = 0; j < 10; j++)
                                    {
                                        if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y + 16 * j, 
                                            COMB[i]->x + 24, COMB[i]->y + 16 * j + 16 - 1))
                                            COMB[i]->val = j;
                                    }
                                }
                                COMB[i]->clik = false;
                            }
                        }
                    }
                    m_drag = 0;
                }
                if (ev == EVENT_RIGHT_PRESSED)
                {
                    if (HELP_POS == 1)
                    {
                        for (int i = 8; i < 8 + 6; i++)
                        {
                            if ((i % 2) == 0)
                            {
                                if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                                {
                                    if (i == 8)
                                    {
                                        byte temp = m_timer_col;
                                        m_timer_col = m_timer_bcol;
                                        m_timer_bcol = temp;
                                    }
                                    if (i == 10)
                                    {
                                        byte temp = m_ticker_col;
                                        m_ticker_col = m_ticker_bcol;
                                        m_ticker_bcol = temp;
                                    }
                                    if (i == 12)
                                    {
                                        byte temp = m_apm_col;
                                        m_apm_col = m_apm_bcol;
                                        m_apm_bcol = temp;
                                    }
                                }
                            }
                            else
                            {
                                if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                                {
                                    if (i == 9)
                                    {
                                        byte temp = m_timer_col;
                                        m_timer_col = m_timer_bcol;
                                        m_timer_bcol = temp;
                                    }
                                    if (i == 11)
                                    {
                                        byte temp = m_ticker_col;
                                        m_ticker_col = m_ticker_bcol;
                                        m_ticker_bcol = temp;
                                    }
                                    if (i == 13)
                                    {
                                        byte temp = m_apm_col;
                                        m_apm_col = m_apm_bcol;
                                        m_apm_bcol = temp;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if ((*(byte*)GAME_MODE == 13) && in_lobby() && (*(byte*)IS_GAME_STARTED == 0))
        {
            ((void (*)(int, int, int, int))F_INVALIDATE)(m_align_x, m_align_y,
                m_align_x + 640, m_align_y + 480);
            if (*(byte*)ME_HOST == 1)
            {
                if (ev == EVENT_KEY_PRESSED)
                {
                    if (key == 117)//F6
                    {
                        m_lobby_interface++;
                        if (m_lobby_interface > 1)m_lobby_interface = 0;
                        menu_1 = true;
                        mod_menu_closed();//save options
                    }
                }
                int gm_x = 28 + m_align_x;
                int gm_y = 292 + m_align_y;
                int o_x = 160 + m_align_x;
                int o_y = 280 + m_align_y;
                bool Gf = true;
                for (int i = 0; i < 7; i++)
                {
                    if (*(char*)(GAME_NAME + i) != trig_name[i])Gf = false;
                }
                if (ev == EVENT_LEFT_PRESSED)
                {
                    if (!Gf)
                    {
                        if ((mx >= (gm_x + 32)) && (my >= (gm_y - 4 - 10))
                            && (mx <= (gm_x + 32 + 96)) && (my <= (gm_y + 4)))
                        {
                            gm_lobby_click = true;
                        }
                    }
                    for (int i = 0; i < 8; i++)
                    {
                        if ((get_player_controler(i) == C_PLAYER) || (get_player_controler(i) == C_PREFER_COMP))
                        {
                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                            {
                                COMB[i]->clik = true;
                            }
                        }
                    }
                }
                if (ev == EVENT_LEFT_RELEASED)
                {
                    if (!Gf)
                    {
                        if (gm_lobby_click)
                        {
                            for (int i = 0; i < 4; i++)
                            {
                                int ii = (i - 1);
                                if ((mx >= (gm_x + 32)) && (my >= (gm_y + 4 + 12 * ii + 6))
                                    && (mx <= (gm_x + 32 + 96)) && (my <= (gm_y + 4 + 12 * ii + 12 - 1 + 6)))
                                {
                                    war2_game_mode = i;
                                    send_teams_hidden();
                                }
                            }
                        }
                        gm_lobby_click = false;
                        if ((mx >= (o_x)) && (my >= (o_y)) && (mx <= (o_x + 75)) && (my <= (o_y + 21)))
                        {
                            ogre_mode = !ogre_mode;
                            send_teams_hidden();
                        }
                    }
                    for (int i = 0; i < 8; i++)
                    {
                        if ((get_player_controler(i) == C_PLAYER) || (get_player_controler(i) == C_PREFER_COMP))
                        {
                            if (COMB[i]->clik)
                            {
                                if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 162))
                                {
                                    byte tm = 255;
                                    for (int j = 0; j < 10; j++)
                                    {
                                        if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y + 16 * j,
                                            COMB[i]->x + 24, COMB[i]->y + 16 * j + 16 - 1))
                                            tm = j;
                                    }
                                    if ((get_player_controler(i) != C_PLAYER) && (tm == 9))tm = 255;
                                    if (tm != 255)
                                    {
                                        set_player_team(i, tm);
                                        send_teams_hidden();
                                    }
                                }
                            }
                        }
                        COMB[i]->clik = false;
                    }
                }
                if (ev == EVENT_RIGHT_PRESSED)
                {
                    for (int i = 0; i < 8; i++)
                    {
                        if (get_player_controler(i) == C_PLAYER)
                        {
                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                            {
                                if (get_player_team(i) != 9)
                                {
                                    teams_host_r[i] = get_player_team(i);
                                    set_player_team(i, 9);
                                    send_teams_hidden();
                                }
                                else
                                {
                                    set_player_team(i, teams_host_r[i]);
                                    send_teams_hidden();
                                }
                            }
                        }
                        if (get_player_controler(i) == C_PREFER_COMP)
                        {
                            if (mouse_inside(mx, my, COMB[i]->x, COMB[i]->y, COMB[i]->x + 24, COMB[i]->y + 13))
                            {
                                if (get_player_team(i) != 0)
                                {
                                    teams_host_r[i] = get_player_team(i);
                                    set_player_team(i, 0);
                                    send_teams_hidden();
                                }
                                else
                                {
                                    set_player_team(i, teams_host_r[i]);
                                    send_teams_hidden();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

PROC g_proc_0040585E;
int events_menu(int a, int b)
{
    int original = ((int (*)(int, int))g_proc_0040585E)(a, b);//original
    WORD mx = *(WORD*)(EVENTS_INFO);
    WORD my = *(WORD*)(EVENTS_INFO + 2);
    WORD ev = *(WORD*)a;
    byte key = *(byte*)(a + 12);
    if ((ev < EVENT_NOTHING))
    {
        if ((!((EV_PREV == EVENT_LEFT_PRESSED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_LEFT_RELEASED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_RIGHT_PRESSED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_RIGHT_RELEASED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_LEFT_DOUBLE) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_RIGHT_DOUBLE) && (EV_PREV == ev))))
            events_main(mx, my, ev, key);
        if (ev != EVENT_MOUSE_MOVE)
            EV_CALL = true;
    }
    else
    {
            EV_CALL = false;
    }
    return original;
}

PROC g_proc_0041C763;
int events_game(int a, int b)
{
    int original = ((int (*)(int, int))g_proc_0041C763)(a, b);//original
    WORD mx = *(WORD*)(EVENTS_MOUSE_STATUS);
    WORD my = *(WORD*)(EVENTS_MOUSE_STATUS + 2);
    WORD ev = *(WORD*)a;
    byte key = *(byte*)(a + 12);
    if ((ev < EVENT_NOTHING))
    {
        if ((!((EV_PREV == EVENT_LEFT_PRESSED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_LEFT_RELEASED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_RIGHT_PRESSED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_RIGHT_RELEASED) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_LEFT_DOUBLE) && (EV_PREV == ev))) &&
            (!((EV_PREV == EVENT_RIGHT_DOUBLE) && (EV_PREV == ev))))
            events_main(mx, my, ev, key);
        if (ev != EVENT_MOUSE_MOVE)
            EV_CALL = true;
    }
    else
    {
        EV_CALL = false;
    }
    return original;
}

byte* ScreenPTR = NULL;
byte* getScreenPtr() {
    DWORD* r;
    r = (DWORD*)SCREEN_POINTER;
    if (r) { return (byte*)*r; }
    return NULL;
}

void draw_pixel(int x, int y, byte c, bool inval)
{
    if (ScreenPTR)
    {
        *(byte*)(ScreenPTR + x + m_screen_w * y) = c;
        if (inval)
            ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x, y);
    }
}

void draw_line(int x1, int y1, int x2, int y2, byte c, bool inval)
{
    int x, y, xdelta, ydelta, xdiff, ydiff, accum, sign;

    xdelta = x2 - x1;
    ydelta = y2 - y1;
    accum = 0;
    sign = 0;

    if (!xdelta && ydelta)
    { /* Special case: straight vertical line */
        x = x1;
        for (y = y1; y < (y1 + ydelta); ++y)
        {
            draw_pixel(x, y, c, true);
        }
    }
    else if (xdelta && !ydelta)
    { /* Special case: straight horisontal line */
        y = y1;
        for (x = x1; x <= x1 + xdelta; ++x)
        {
            draw_pixel(x, y, c, true);
        }
    }
    else
    {
        xdiff = (xdelta << 16) / ydelta;
        ydiff = (ydelta << 16) / xdelta;

        if (abs(xdiff) > abs(ydiff)) { /* horizontal-major */
            y = y1;
            if (xdelta < 0) { /* traversing negative x */
                for (x = x1; x >= x2; --x) {
                    draw_pixel(x, y, c, true);
                    accum += abs(ydiff);
                    while (accum >= (1 << 16)) {
                        ++y;
                        accum -= (1 << 16);
                    }
                }
            }
            else { /* traversing positive x */
                for (x = x1; x <= x2; ++x) {
                    draw_pixel(x, y, c, true);
                    accum += abs(ydiff);
                    while (accum >= (1 << 16)) {
                        ++y;
                        accum -= (1 << 16);
                    }
                }
            }
        }
        else if (abs(ydiff) > abs(xdiff)) { /* vertical major */
            sign = (xdelta > 0 ? 1 : -1);
            x = x1;
            for (y = y1; y <= y2; ++y) {
                draw_pixel(x, y, c, true);
                accum += abs(xdiff);
                while (accum >= (1 << 16)) {
                    x += sign;
                    accum -= (1 << 16);
                }
            }
        }
        else if (abs(ydiff) == abs(xdiff)) { /* 45 degrees */
            sign = (xdelta > 0 ? 1 : -1);
            x = x1;
            for (y = y1; y <= y2; ++y) {
                draw_pixel(x, y, c, true);
                x += sign;
            }
        }
    }
    if (inval)
        ((void (*)(int, int, int, int))F_INVALIDATE)(x1, y1, x2, y2);
}

#define my_min(x,y) (((x)<(y))?(x):(y))
#define my_max(x,y) (((x)>(y))?(x):(y))
void draw_rect(int x1, int y1, int x2, int y2, byte c, bool o, bool inval)
{
    if (!ScreenPTR)return;
    int xmin = my_min(x1, x2);
    int xmax = my_max(x1, x2);
    int ymin = my_min(y1, y2);
    int ymax = my_max(y1, y2);
    byte* p = ScreenPTR + ymin * m_screen_w + xmin;
    if (o)
    {
        for (int i = xmax - xmin + 1; i > 0; i--)
        {
            *p++ = c;
        }
        p = ScreenPTR + ymin * m_screen_w + xmin;
        for (int i = ymax - ymin + 1; i > 0; i--)
        {
            *p = c;
            p += m_screen_w;
        }
        p = ScreenPTR + ymax * m_screen_w + xmin;
        for (int i = xmax - xmin + 1; i > 0; i--)
        {
            *p++ = c;
        }
        p = ScreenPTR + ymin * m_screen_w + xmax;
        for (int i = ymax - ymin + 1; i > 0; i--)
        {
            *p = c;
            p += m_screen_w;
        }
    }
    else
    {
        for (int j = ymax - ymin + 1; j > 0; j--)
        {
            for (int i = xmax - xmin + 1; i > 0; i--)
                *p++ = c;
            p += m_screen_w - xmax + xmin - 1;
        }
    }
    if (inval)
        ((void (*)(int, int, int, int))F_INVALIDATE)(x1, y1, x2, y2);
}

int draw_char(int x, int y, byte c, unsigned char ch, byte bc, bool inval)
{
    if (!ScreenPTR)return 0;
    byte* p = ScreenPTR + y * m_screen_w + x;
    if (ch < ' ')ch = ' ';
    ch -= ' ';
    int chMfontwidthplus1 = ch * (1 + FONT_6PX_PROP_CHAR_WIDTH);
    int w = 1 + font_6px_prop[chMfontwidthplus1];
    if (!((c == 0) && (bc == 0)))
    {
        if (bc)
            for (int i = 1; i <= w; i++)
            {
                byte font_line = font_6px_prop[chMfontwidthplus1 + i];
                for (int j = 0; j < 8; j++)
                {
                    if (font_line & (1 << j))*p = c;
                    else *p = bc;
                    p += m_screen_w;
                    //                p++;
                }
                p -= m_screen_w * 8;
                p++;
                //            p+=m_screen_w-8;
            }
        else
            for (int i = 0; i < w; i++)
            {
                byte font_line = font_6px_prop[ch * (1 + FONT_6PX_PROP_CHAR_WIDTH) + 1 + i];
                for (int j = 0; j < 8; j++)
                {
                    if (font_line & (1 << j))*p = c;
                    p += m_screen_w;
                    //                p++;
                }
                p -= m_screen_w * 8;
                p++;
                //            p+=m_screen_w-8;
            }
    }
    if (inval)
        ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + w, y + 8);
    return w;
}

int draw_text(int x, int y, byte c, unsigned char* ch, byte bc, byte cond, bool inval)
{
    int w = 0;
    int C = 0;
    while (ch[C] != 0)
    {
        w += draw_char(x + w, y, c, ch[C], bc, false);
        w -= cond;
        if (w < 0)w = 0;
        C++;
    }
    if (inval)
        ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + w, y + 8);
    return w;
}

int draw_char_get_width(unsigned char ch)
{
    if (ch < ' ')ch = ' ';
    return 1 + font_6px_prop[(ch - ' ') * (1 + FONT_6PX_PROP_CHAR_WIDTH)];
}

int draw_text_get_width(unsigned char* ch, int s, byte cond)
{
    int w = 0;
    int C = 0;
    while (ch[C] != 0)
    {
        w += draw_char_get_width(ch[C]);
        w -= cond;
        if (w < 0)w = 0;
        C++;
    }
    return w * s;
}

void draw_sprite(int x, int y, byte* s, int w, int h, bool a, bool inval)
{
    if (!ScreenPTR)return;
    if (s != NULL)
    {
        byte* p = ScreenPTR + y * m_screen_w + x;
        byte* ps = s;
        if (!a)
            for (int j = h - 1; j >= 0; j--)
            {
                for (int i = w - 1; i >= 0; i--)
                    *p++ = *ps++;
                p += m_screen_w - w;
            }
        else
            for (int j = h - 1; j >= 0; j--)
            {
                for (int i = w - 1; i >= 0; i--)
                {
                    if (*ps) *p = *ps;
                    p++;
                    ps++;
                }
                p += m_screen_w - w;
            }
    }
    if (inval)
        ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + w, y + h);
}

void draw_combobox(combobox* box, byte type)
{
    unsigned char msg_no[] = "No";
    unsigned char msg_obs[] = "OBS";
    unsigned char msg_obs1[] = "Obs";
    unsigned char msg_x[] = "0";
    if (type == 0)//lobby
    {
        if (!box->clik)
        {
            if ((box->val == 10) && (!box->clik))
            {
                draw_sprite(box->x + 13, box->y - 1, spr_hourglass, 10, 14, true, false);
            }
            else
            {
                if (box->val == 9)
                {
                    draw_text(box->x + 3, box->y + 3, 200, msg_obs, 0, 0, false);
                }
                else
                {
                    if ((box->val >= 1) && (box->val <= 8))
                    {
                        if (*(byte*)GAME_MODE != 3)
                        {
                            if (get_player_team(*(byte*)LOCAL_PLAYER) == box->val)
                            {
                                draw_line(box->x + 11 + 13, box->y + 1, box->x + 11 + 13, box->y + 14, 107, false);
                                draw_line(box->x + 1 + 13, box->y + 14, box->x + 11 + 13, box->y + 14, 107, false);
                            }
                        }
                        draw_rect(box->x + 13, box->y, box->x + 10 + 13, box->y + 13, 104, true, false);
                        draw_rect(box->x + 1 + 13, box->y + 1, box->x + 9 + 13, box->y + 12, 0, true, false);
                        byte col = 208 + 4 * (box->val - 1);
                        if (col == 236)col = 200;
                        if (m_lobby_comb == 1)col = 161;
                        draw_rect(box->x + 2 + 13, box->y + 2, box->x + 8 + 13, box->y + 11, col, false, false);
                        if (m_lobby_comb == 1)
                        {
                            msg_x[0] = '0' + box->val;
                            draw_text(box->x + 2 + 13, box->y + 3, 200, msg_x, 0, 0, false);
                        }
                    }
                    if (*(byte*)ME_HOST == 1)
                    {
                        if ((m_lobby_comb == 0) || ((m_lobby_comb == 1) && (box->val == 0)))
                            draw_sprite(box->x + 13, box->y, spr_host_triangle, 11, 7, true, false);
                    }
                }
            }
        }
        if (box->clik)
        {
            draw_rect(box->x + 1, box->y, box->x + 23, box->y + 16 * 10 - (box->val == 9 ? 2 : 0), m_lobby_comb == 0 ? 161 : 162, false, false);
            draw_rect(box->x + 1, box->y, box->x + 23, box->y + 160 - (box->val == 9 ? 2 : 0), 104, true, false);
            for (int i = 1; i <= 8; i++)
            {
                byte col = 208 + 4 * (i - 1);
                if (col == 236)col = 200;
                if (m_lobby_comb == 1)col = 161;
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, col, false, false);
                if (m_lobby_comb == 1)
                {
                    msg_x[0] = '0' + i;
                    draw_text(box->x + 9, box->y + 3 + 16 * i, 200, msg_x, 0, 0, false);
                }
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, 0, true, false);
            }
            draw_text(box->x + 6, box->y + 3 + ((box->val == 0) ? 0 : 1), 200, msg_no, 0, 0, false);
            draw_text(box->x + 4, box->y - 1 + 160 - 12, 200, msg_obs1, 0, 1, false);
            if ((box->val >= 0) && (box->val <= 9))
            {
                if (((box->val != 0) && (box->val != 9)) && (m_lobby_comb == 0))
                    draw_sprite(box->x + 1, box->y - 1 + 16 * box->val, spr_team_selection, 23, 16, true, false);
                else
                    draw_sprite(box->x + 1, box->y - 1 + 16 * box->val, spr_team_selection_, 23, 16, true, false);
            }
        }
        ((void (*)(int, int, int, int))F_INVALIDATE)(box->x, box->y - 1, box->x + 28, box->y + 180);
    }
    if (type == 10)//lobby comp
    {
        if (!box->clik)
        {
            if ((box->val >= 1) && (box->val <= 8))
            {
                if (*(byte*)GAME_MODE != 3)
                {
                    if (get_player_team(*(byte*)LOCAL_PLAYER) == box->val)
                    {
                        draw_line(box->x + 11 + 13, box->y + 1, box->x + 11 + 13, box->y + 14, 107, false);
                        draw_line(box->x + 1 + 13, box->y + 14, box->x + 11 + 13, box->y + 14, 107, false);
                    }
                }
                draw_rect(box->x + 13, box->y, box->x + 10 + 13, box->y + 13, 104, true, false);
                draw_rect(box->x + 1 + 13, box->y + 1, box->x + 9 + 13, box->y + 12, 0, true, false);
                byte col = 208 + 4 * (box->val - 1);
                if (col == 236)col = 200;
                if (m_lobby_comb == 1)col = 161;
                draw_rect(box->x + 2 + 13, box->y + 2, box->x + 8 + 13, box->y + 11, col, false, false);
                if (m_lobby_comb == 1)
                {
                    msg_x[0] = '0' + box->val;
                    draw_text(box->x + 2 + 13, box->y + 3, 200, msg_x, 0, 0, false);
                }
            }
            if (*(byte*)ME_HOST == 1)
            {
                if ((m_lobby_comb == 0) || ((m_lobby_comb == 1) && (box->val == 0)))
                    draw_sprite(box->x + 13, box->y, spr_host_triangle, 11, 7, true, false);
            }
        }
        if (box->clik)
        {
            draw_rect(box->x + 1, box->y, box->x + 23, box->y + 16 * 9 - (box->val == 8 ? 2 : 0), m_lobby_comb == 0 ? 161 : 162, false, false);
            draw_rect(box->x + 1, box->y, box->x + 23, box->y + 16 * 9 - (box->val == 8 ? 2 : 0), 104, true, false);
            for (int i = 1; i <= 8; i++)
            {
                byte col = 208 + 4 * (i - 1);
                if (col == 236)col = 200;
                if (m_lobby_comb == 1)col = 161;
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, col, false, false);
                if (m_lobby_comb == 1)
                {
                    msg_x[0] = '0' + i;
                    draw_text(box->x + 9, box->y + 3 + 16 * i, 200, msg_x, 0, 0, false);
                }
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, 0, true, false);
            }
            draw_text(box->x + 6, box->y + 3 + ((box->val == 0) ? 0 : 1), 200, msg_no, 0, 0, false);
            if ((box->val >= 0) && (box->val <= 8))
            {
                if ((box->val != 0) && (m_lobby_comb == 0))
                    draw_sprite(box->x + 1, box->y - 1 + 16 * box->val, spr_team_selection, 23, 16, true, false);
                else
                    draw_sprite(box->x + 1, box->y - 1 + 16 * box->val, spr_team_selection_, 23, 16, true, false);
            }
        }
        ((void (*)(int, int, int, int))F_INVALIDATE)(box->x, box->y - 1, box->x + 28, box->y + 180);
    }
    if (type == 1)//color
    {
        unsigned char msg_def[] = "Def";
        if (!box->clik)
        {
            if (box->val != 0)
            {
                draw_rect(box->x + 13, box->y, box->x + 10 + 13, box->y + 13, 104, true, false);
                draw_rect(box->x + 1 + 13, box->y + 1, box->x + 9 + 13, box->y + 12, 0, true, false);
                draw_rect(box->x + 2 + 13, box->y + 2, box->x + 8 + 13, box->y + 11, box->val, false, false);
            }
            draw_sprite(box->x + 13, box->y, spr_host_triangle, 11, 7, true, false);
        }
        if (box->clik)
        {
            draw_rect(box->x + 1, box->y - 1, box->x + 23, box->y + 16 * C_COLORS_SIZE, *(byte*)PLAYER_RACE ? 132 : 99, false, false);
            draw_rect(box->x + 1, box->y - 2, box->x + 23, box->y + 16 * C_COLORS_SIZE, 104, true, false);
            for (int i = 1; i < C_COLORS_SIZE; i++)
            {
                byte col = colors[i];
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, col, false, false);
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, 0, true, false);
            }
            draw_text(box->x + 4, box->y + 4, 200, msg_def, 0, 1, false);
            int p = 0;
            for (int i = 1; i < C_COLORS_SIZE; i++)
            {
                if (box->val == colors[i])p = i;
            }
            if (p != 0)
                draw_sprite(box->x + 1, box->y - 1 + 16 * p, spr_team_selection, 23, 16, true, false);
        }
        ((void (*)(int, int, int, int))F_INVALIDATE)(box->x, box->y - 1, box->x + 28, box->y + 20 + 16 * C_COLORS_SIZE);
    }
    if (type == 2)//back color
    {
        if (!box->clik)
        {
            if (box->val != 0)
            {
                draw_rect(box->x + 13, box->y, box->x + 10 + 13, box->y + 13, 104, true, false);
                draw_rect(box->x + 1 + 13, box->y + 1, box->x + 9 + 13, box->y + 12, 0, true, false);
                draw_rect(box->x + 2 + 13, box->y + 2, box->x + 8 + 13, box->y + 11, box->val, false, false);
            }
            draw_sprite(box->x + 13, box->y, spr_host_triangle, 11, 7, true, false);
        }
        if (box->clik)
        {
            draw_rect(box->x + 1, box->y - 1, box->x + 23, box->y + 16 * C_BCOLORS_SIZE, *(byte*)PLAYER_RACE ? 132 : 99, false, false);
            draw_rect(box->x + 1, box->y, box->x + 23, box->y + 16 * C_BCOLORS_SIZE, 104, true, false);
            for (int i = 1; i < C_BCOLORS_SIZE; i++)
            {
                byte col = bcolors[i];
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, col, false, false);
                draw_rect(box->x + 3, box->y + 16 * i, box->x + 21, box->y + 16 * i + 13, 0, true, false);
            }
            draw_text(box->x + 6, box->y + 3 + ((box->val == 0) ? 0 : 1), 200, msg_no, 0, 0, false);
            int p = 0;
            for (int i = 1; i < C_BCOLORS_SIZE; i++)
            {
                if (box->val == bcolors[i])p = i;
            }
            if ((box->val != 0))
                draw_sprite(box->x + 1, box->y - 1 + 16 * p, spr_team_selection, 23, 16, true, false);
            else
                draw_sprite(box->x + 1, box->y - 1 + 16 * p, spr_team_selection_, 23, 16, true, false);
        }
        ((void (*)(int, int, int, int))F_INVALIDATE)(box->x, box->y - 1, box->x + 28, box->y + 20 + 16 * C_BCOLORS_SIZE);
    }
    if (type == 3)//apm type
    {
        unsigned char msg_1[] = "APM:100";
        unsigned char msg_2[] = "100 APM";
        unsigned char msg_3[] = "100";
        unsigned char* msg_APM[] = { msg_1 ,msg_2,msg_3 };
        if (!box->clik)
        {
            draw_rect(box->x, box->y, box->x + 66, box->y + 13, 104, true, false);
            draw_rect(box->x + 1, box->y + 1, box->x + 66 - 1, box->y + 12, 0, true, false);
            draw_rect(box->x + 2, box->y + 2, box->x + 66 - 2, box->y + 11, *(byte*)PLAYER_RACE ? 162 : 179, false, false);
            draw_text(box->x + 3, box->y + 3, 200, msg_APM[box->val], 0, 0, false);
            draw_sprite(box->x + 56, box->y, spr_host_triangle, 11, 7, true, false);
        }
        if (box->clik)
        {
            draw_rect(box->x, box->y, box->x + 66, box->y + 12 * 3 + 3, *(byte*)PLAYER_RACE ? 162 : 179, false, false);
            draw_rect(box->x, box->y, box->x + 66, box->y + 12 * 3 + 3, 104, true, false);
            for (int i = 0; i < 3; i++)
            {
                draw_text(box->x + 3, box->y + 3 + 13 * i, 200, msg_APM[i], 0, 0, false);
            }
            for (int i = 1; i < 3; i++)
            {
                draw_line(box->x + 2, box->y + 13 * i, box->x + 64, box->y + 13 * i, *(byte*)PLAYER_RACE ? 161 : 180, false);
            }
        }
        ((void (*)(int, int, int, int))F_INVALIDATE)(box->x, box->y - 1, box->x + 80, box->y + 32);
    }
}

void draw_checkbox(int x, int y, bool a)
{
    if (*(byte*)PLAYER_RACE)
    {
        if (a)
            draw_sprite(x, y, spr_cb_orc_on, 17, 20, true, false);
        else
            draw_sprite(x, y, spr_cb_orc_off, 17, 17, true, false);
    }
    else
    {
        if (a)
            draw_sprite(x, y, spr_cb_hum_on, 17, 20, true, false);
        else
            draw_sprite(x, y, spr_cb_hum_off, 17, 17, true, false);
    }
    ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + 17, y + 20);
}

void draw_checkbox2(int x, int y, bool a)
{
    if (*(byte*)PLAYER_RACE)
    {
        if (a)
            draw_sprite(x, y, spr_cb_orc2_on, 17, 20, true, false);
        else
            draw_sprite(x, y, spr_cb_orc2_off, 17, 17, true, false);
    }
    else
    {
        if (a)
            draw_sprite(x, y, spr_cb_hum2_on, 17, 20, true, false);
        else
            draw_sprite(x, y, spr_cb_hum2_off, 17, 17, true, false);
    }
    ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + 17, y + 20);
}

void draw_radio(int x, int y, bool a)
{
    if (*(byte*)PLAYER_RACE)
    {
        if (a)
            draw_sprite(x, y, spr_rb_orc_on, 19, 19, true, false);
        else
            draw_sprite(x, y, spr_rb_orc_off, 19, 19, true, false);
    }
    else
    {
        if (a)
            draw_sprite(x, y, spr_rb_hum_on, 19, 19, true, false);
        else
            draw_sprite(x, y, spr_rb_hum_off, 19, 19, true, false);
    }
    ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + 19, y + 19);
}

void draw_pos_selector(int x, int y, WORD xx, WORD yy, WORD sz)
{
    byte mc = 196, sc = 200, bc = 131, sbc = 239;
    if (*(byte*)PLAYER_RACE)
    {
        mc = 196;
        sc = 200;
        bc = 131;
        sbc = 239;
    }
    else
    {
        mc = 104;
        sc = 246;
        bc = 97;
        sbc = 239;
    }
    byte c = mc;
    draw_rect(x - 1, y - 1, x + 65, y + 27, bc, false, false);
    byte s = 255;
    if ((xx == m_minx) && (yy == m_miny))s = 0;
    if ((xx == m_minx) && (yy >= (m_maxy - 1)))s = 1;
    if ((xx >= (m_maxx - 1 - sz)) && (yy == m_miny))s = 2;
    if ((xx >= (m_maxx - 1 - sz)) && (yy >= (m_maxy - 1)))s = 3;
    if ((xx >= m_minx + ((m_screen_w - 200) / 2) - 10) && (xx <= m_minx + ((m_screen_w - 200) / 2) + 10) && (yy == m_miny))s = 4;
    if ((xx >= m_minx + ((m_screen_w - 200) / 2) - 10) && (xx <= m_minx + ((m_screen_w - 200) / 2) + 10) && (yy >= (m_maxy - 1)))s = 5;
    if (s == 0)c = sc;
    else c = mc;
    if (c == sc)
        draw_rect(x - 1, y - 1, x + 21, y + 13, sbc, false, false);
    draw_rect(x, y, x + 20, y + 12, c, true, false);
    draw_rect(x, y, x + 4, y + 4, c, false, false);
    if (s == 1)c = sc;
    else c = mc;
    if (c == sc)
        draw_rect(x - 1, y + 13, x + 21, y + 15 + 12, sbc, false, false);
    draw_rect(x, y + 14, x + 20, y + 14 + 12, c, true, false);
    draw_rect(x, y + 14 + 8, x + 4, y + 14 + 12, c, false, false);
    if (s == 2)c = sc;
    else c = mc;
    if (c == sc)
        draw_rect(x + 43, y - 1, x + 21 + 44, y + 13, sbc, false, false);
    draw_rect(x + 44, y, x + 44 + 20, y + 12, c, true, false);
    draw_rect(x + 44 + 16, y, x + 44 + 20, y + 4, c, false, false);
    if (s == 3)c = sc;
    else c = mc;
    if (c == sc)
        draw_rect(x + 43, y + 13, x + 21 + 44, y + 15 + 12, sbc, false, false);
    draw_rect(x + 44, y + 14, x + 44 + 20, y + 14 + 12, c, true, false);
    draw_rect(x + 44 + 16, y + 14 + 8, x + 44 + 20, y + 14 + 12, c, false, false);
    if (s == 4)c = sc;
    else c = mc;
    if (c == sc)
        draw_rect(x + 21, y - 1, x + 21 + 22, y + 13, sbc, false, false);
    draw_rect(x + 22, y, x + 22 + 20, y + 12, c, true, false);
    draw_rect(x + 22 + 8, y, x + 22 + 12, y + 4, c, false, false);
    if (s == 5)c = sc;
    else c = mc;
    if (c == sc)
        draw_rect(x + 21, y + 13, x + 21 + 22, y + 15 + 12, sbc, false, false);
    draw_rect(x + 22, y + 14, x + 22 + 20, y + 14 + 12, c, true, false);
    draw_rect(x + 22 + 8, y + 14 + 8, x + 22 + 12, y + 14 + 12, c, false, false);
    ((void (*)(int, int, int, int))F_INVALIDATE)(x, y, x + 64, y + 32);
}

void draw_gamemode_selector(int gm_x, int gm_y)
{
    unsigned char text_gm1[] = "Game";
    unsigned char text_gm2[] = "Mode";
    draw_text(gm_x, gm_y - 8 - 2, 251, text_gm1, 0, 0, false);
    draw_text(gm_x + 1, gm_y - 2, 251, text_gm2, 0, 0, false);
    unsigned char text_gm_0[] = "Classic";
    unsigned char text_gm_1[] = "Triggers";
    unsigned char text_gm_2[] = "Rebalanced";
    unsigned char text_gm_3[] = "Double Race";
    unsigned char* text_gm_[] = { text_gm_0,text_gm_1,text_gm_2,text_gm_3 };
    byte gm_col[] = { 251,254,250,253 };
    int boxx = gm_x + 32;
    int boxy = gm_y - 8;
    if (*(byte*)ME_HOST == 1)
    {
        if (!gm_lobby_click)
        {
            draw_rect(boxx, boxy, boxx + 96, boxy + 13, 104, true, false);
            draw_rect(boxx + 1, boxy + 1, boxx + 96 - 1, boxy + 12, 0, true, false);
            draw_rect(boxx + 2, boxy + 2, boxx + 96 - 2, boxy + 11, 162, false, false);
            draw_text(boxx + 3, boxy + 3, gm_col[war2_game_mode], text_gm_[war2_game_mode], 0, 0, false);
            draw_sprite(boxx + 86, boxy, spr_host_triangle, 11, 7, true, false);
        }
        else
        {
            draw_rect(boxx, boxy, boxx + 96, boxy + 12 * 4 + 3, 162, false, false);
            draw_rect(boxx, boxy, boxx + 96, boxy + 12 * 4 + 3, 104, true, false);
            for (int i = 0; i < 4; i++)
            {
                draw_text(boxx + 3, boxy + 3 + 13 * i, gm_col[i], text_gm_[i], 0, 0, false);
            }
            for (int i = 1; i < 4; i++)
            {
                draw_line(boxx + 2, boxy + 13 * i, boxx + 94, boxy + 13 * i, 161, false);
            }
        }
    }
    else
    {
        draw_text(boxx + 3, boxy + 3, gm_col[war2_game_mode], text_gm_[war2_game_mode], 0, 0, false);
    }
    ((void (*)(int, int, int, int))F_INVALIDATE)(gm_x, gm_y, gm_x + 96, gm_y + 13 * 3);
}

void draw_ogre_box(int o_x, int o_y)
{
    unsigned char text1[] = "Two-head";
    unsigned char text2[] = "Ogre mode";
    draw_checkbox2(o_x, o_y, ogre_mode);
    draw_text(o_x + 20, o_y, 251, text1, 0, 0, true);
    draw_text(o_x + 20 - 1, o_y + 8, 251, text2, 0, 0, true);
}

void drawing()
{
    if (*(byte*)DLL_STATUS == 1)
    {
        ScreenPTR = getScreenPtr();
        if ((*(byte*)GAME_MODE == 3) && (game_started))
        {
            if (m_show_button)
            {
                byte* spr = NULL;
                if (menu_1)spr = (*(byte*)PLAYER_RACE) ? spr_menu_orc2a : spr_menu_hum2a;
                else
                {
                    byte chat_open = *(byte*)MENU_CHAT_OPEN;
                    int menu_open = *(int*)MENU_OPEN1;
                    int menu_open2 = *(int*)MENU_OPEN2;
                    if ((menu_open == 0) && (menu_open2 == 0) && (chat_open == 0))
                    {
                        if (menu_button_hover)spr = (*(byte*)PLAYER_RACE) ? spr_menu_orc2 : spr_menu_hum2;
                        else spr = (*(byte*)PLAYER_RACE) ? spr_menu_orc1 : spr_menu_hum1;
                    }
                    else spr = (*(byte*)PLAYER_RACE) ? spr_menu_orc1 : spr_menu_hum1;
                }
                draw_sprite(168, 0, spr, 32, 14, true, true);
            }
            else
            {
                if (m_show_button_last)
                {
                    m_show_button_last = false;
                    draw_sprite(168, 0, (*(byte*)PLAYER_RACE) ? spr_menu_orc0 : spr_menu_hum0, 32, 14, true, true);
                }
            }
            if (menu_1)
            {
                if (HELP_POS == 1)
                {
                    draw_checkbox(241 + m_align_x, 141 + m_align_y, m_show_timer!=0);
                    draw_checkbox(241 + m_align_x, 201 + m_align_y, m_show_ticker!=0);
                    draw_checkbox(241 + m_align_x, 261 + m_align_y, m_show_apm!=0);
                    draw_pos_selector(354 + m_align_x, 148 + m_align_y, m_timer_x, m_timer_y, m_timer_size - 8);
                    draw_pos_selector(354 + m_align_x, 208 + m_align_y, m_ticker_x, m_ticker_y, m_ticker_size - 8);
                    draw_pos_selector(354 + m_align_x, 268 + m_align_y, m_apm_x, m_apm_y, m_apm_size - 8);
                    COMB[8]->x = 538 + m_align_x;
                    COMB[8]->y = 144 + m_align_y;
                    COMB[8]->val = m_timer_col;
                    COMB[9]->x = 538 + m_align_x;
                    COMB[9]->y = 164 + m_align_y;
                    COMB[9]->val = m_timer_bcol;
                    COMB[10]->x = 538 + m_align_x;
                    COMB[10]->y = 204 + m_align_y;
                    COMB[10]->val = m_ticker_col;
                    COMB[11]->x = 538 + m_align_x;
                    COMB[11]->y = 224 + m_align_y;
                    COMB[11]->val = m_ticker_bcol;
                    COMB[12]->x = 538 + m_align_x;
                    COMB[12]->y = 264 + m_align_y;
                    COMB[12]->val = m_apm_col;
                    COMB[13]->x = 538 + m_align_x;
                    COMB[13]->y = 284 + m_align_y;
                    COMB[13]->val = m_apm_bcol;
                    COMB[14]->x = 353 + m_align_x;
                    COMB[14]->y = 304 + m_align_y;
                    COMB[14]->val = m_apm_type;
                    for (int i = 8; i < 8 + 6; i++)
                    {
                        if (!COMB[i]->clik)
                            draw_combobox(COMB[i], i % 2 == 0 ? 1 : 2);
                    }
                    if (!COMB[14]->clik)
                        draw_combobox(COMB[14], 3);
                    for (int i = 8; i < 8 + 6; i++)
                    {
                        if (COMB[i]->clik)
                            draw_combobox(COMB[i], i % 2 == 0 ? 1 : 2);
                    }
                    if (COMB[14]->clik)
                        draw_combobox(COMB[14], 3);
                }
                if (HELP_POS == 14)
                {
                    draw_sprite(241 + m_align_x, 141 + m_align_y, spr_cb_dis, 17, 17, true, true);
                }
                if (HELP_POS == 27)
                {
                    draw_checkbox(241 + m_align_x, 141 + m_align_y, m_holy_cam != 0);
                    draw_checkbox(241 + m_align_x, 161 + m_align_y, m_double_click != 0);
                }
                if (HELP_POS == 40)
                {
                    draw_checkbox(241 + m_align_x, 141 + m_align_y, m_show_button != 0);
                    draw_radio(241 + m_align_x, 200 + m_align_y, m_lobby_comb == 0);
                    draw_radio(241 + m_align_x, 220 + m_align_y, m_lobby_comb == 1);
                    byte ltemp = m_lobby_comb;
                    COMB[0]->x = 261 + m_align_x;
                    COMB[0]->y = 202 + m_align_y;
                    COMB[1]->x = 261 + m_align_x;
                    COMB[1]->y = 222 + m_align_y;
                    m_lobby_comb = 0;
                    if (!COMB[0]->clik)
                        draw_combobox(COMB[0], 0);
                    m_lobby_comb = 1;
                    if (!COMB[1]->clik)
                        draw_combobox(COMB[1], 0);
                    m_lobby_comb = 0;
                    if (COMB[0]->clik)
                        draw_combobox(COMB[0], 0);
                    m_lobby_comb = 1;
                    if (COMB[1]->clik)
                        draw_combobox(COMB[1], 0);
                    m_lobby_comb = ltemp;
                }
            }
            if (m_show_timer != 0)
            {
                unsigned char tiks[] = "                        ";
                byte spd = *(byte*)CURRENT_SPEED;
                spd = *(byte*)(SLOWEST_SPEED + spd);
                if (spd == 0)spd = 1;
                int tik_per_sec = 1000 / spd;
                long long sec = ((long long)m_ticks * 100 + m_ticks1) / tik_per_sec;
                int hr = (sec / 3600) % 3600;
                int mn = (sec / 60) % 60;
                int sc = sec % 60;
                if (hr != 0)
                    sprintf((char*)tiks, "%02d:%02d:%02d", hr, mn, sc);
                else
                    sprintf((char*)tiks, "%02d:%02d", mn, sc);
                m_timer_size = draw_text_get_width(tiks, 1, 0);
                if (m_timer_y < m_miny)m_timer_y = m_miny;
                if (m_timer_y >= m_maxy)m_timer_y = m_maxy;
                if (m_timer_x < m_minx)m_timer_x = m_minx;
                int xx = m_timer_x;
                if (m_timer_x >= (m_maxx + 8))m_timer_x = m_maxx + 8;
                if (xx >= (m_maxx + 8 - m_timer_size - 1))xx = m_maxx + 8 - m_timer_size;
                draw_text(xx, m_timer_y, m_timer_col, tiks, m_timer_bcol, 0, true);
            }
            if (m_show_ticker != 0)
            {
                unsigned char tiks[] = "                        ";
                sprintf((char*)tiks, "%d.%02d", m_ticks, m_ticks1);
                m_ticker_size = draw_text_get_width(tiks, 1, 0);
                if (m_ticker_y < m_miny)m_ticker_y = m_miny;
                if (m_ticker_y >= m_maxy)m_ticker_y = m_maxy;
                if (m_ticker_x < m_minx)m_ticker_x = m_minx;
                int xx = m_ticker_x;
                if (m_ticker_x >= (m_maxx + 8))m_ticker_x = m_maxx + 8;
                if (xx >= (m_maxx + 8 - m_ticker_size - 1))xx = m_maxx + 8 - m_ticker_size;
                draw_text(xx, m_ticker_y, m_ticker_col, tiks, m_ticker_bcol, 0, true);
            }
            if (m_show_apm != 0)
            {
                int apm = 0;
                for (int i = 0; i < 1000; i++)
                    apm += apm_list[i].val;
                if (apm > 999)apm = 999;
                unsigned char apm_txt[] = "                        ";
                if (m_apm_type == 0)
                    sprintf((char*)apm_txt, "APM:%3d", apm);
                if (m_apm_type == 1)
                    sprintf((char*)apm_txt, "%3d APM", apm);
                if (m_apm_type == 2)
                    sprintf((char*)apm_txt, "%3d", apm);
                int i = 0;
                while (apm_txt[i] != 0)
                {
                    if (apm_txt[i] == ' ')apm_txt[i] = (unsigned char)160;
                    i++;
                }
                m_apm_size = draw_text_get_width(apm_txt, 1, 0);
                if (m_apm_y < m_miny)m_apm_y = m_miny;
                if (m_apm_y >= m_maxy)m_apm_y = m_maxy;
                if (m_apm_x < m_minx)m_apm_x = m_minx;
                int xx = m_apm_x;
                if (m_apm_x >= (m_maxx + 8))m_apm_x = m_maxx + 8;
                if (xx >= (m_maxx + 8 - m_apm_size - 1))xx = m_maxx + 8 - m_apm_size;
                draw_text(xx, m_apm_y, m_apm_col, apm_txt, m_apm_bcol, 0, true);
            }
        }
        if ((*(byte*)GAME_MODE == 13) && in_lobby())
        {
            bool Gf = true;
            for (int i = 0; i < 7; i++)
            {
                if (*(char*)(GAME_NAME + i) != trig_name[i])Gf = false;
            }
            if (!Gf)
            {
                if (((*(byte*)ME_HOST == 1) && (m_lobby_interface == 0))
                    || (war2_game_mode != 0))draw_gamemode_selector(28 + m_align_x, 292 + m_align_y);
                if (((*(byte*)ME_HOST == 1) && (m_lobby_interface == 0))
                    || ogre_mode)draw_ogre_box(160 + m_align_x, 280 + m_align_y);
            }
            int yy = 0, k3 = 0;
            bool v1 = false, v2 = false;
            for (int i = 0; i < 8; i++)
            {
                if (*(byte*)(PLAYERS_INFO + 4 + 2 + 38 * i) == 1)v1 = true;
                if (*(byte*)(PLAYERS_INFO + 4 + 2 + 38 * i) == 2)v2 = true;
                if ((get_player_controler(i) == C_NOBODY) || (get_player_controler(i) == C_ALLY_ACTIVE) ||
                    (get_player_controler(i) == C_ALLY_PASSIVE) || (get_player_controler(i) == C_PASSIVE_COMP))
                    k3 += 1;
                yy = 0;
                if (v1)yy = 24;
                if (v2)yy = 48;
                yy -= 24 * k3;
                if ((teams_lag[i] == 0) || ((teams_lag[i] != 0) && (teams_lag[i] == teams_lag[*(byte*)LOCAL_PLAYER])))
                {
                    if (*(byte*)PLAYER_HOST == *(byte*)LOCAL_PLAYER)
                        COMB[i]->val = get_player_team(i);
                    else
                        COMB[i]->val = teams_host[i];
                }
                else
                    COMB[i]->val = 10;
                COMB[i]->x = 2 + m_align_x;
                COMB[i]->y = 39 + 24 * i + yy + m_align_y;
                if (get_player_controler(i) == C_PLAYER)
                {
                    if (!COMB[i]->clik)
                        draw_combobox(COMB[i], 0);
                }
                if (get_player_controler(i) == C_PREFER_COMP)
                {
                    if (!COMB[i]->clik)
                        draw_combobox(COMB[i], 10);
                }
            }
            for (int i = 0; i < 8; i++)
            {
                if (get_player_controler(i) == C_PLAYER)
                {
                    if (COMB[i]->clik)
                        draw_combobox(COMB[i], 0);
                }
                if (get_player_controler(i) == C_PREFER_COMP)
                {
                    if (COMB[i]->clik)
                        draw_combobox(COMB[i], 10);
                }
            }
        }
    }
}

PROC g_proc_00405463;
void draw_hook1(int a, int b, int c)
{
    ((void (*)(int, int, int))g_proc_00405463)(a, b, c);//original
    drawing();
}

PROC g_proc_00405483;
void draw_hook2(int a, int b, int c)
{
    ((void (*)(int, int, int))g_proc_00405483)(a, b, c);//original
    drawing();
}

PROC g_proc_00421F57;
void draw_hook3()
{
    ((void (*)())g_proc_00421F57)();//original
    drawing();
}

void read_mod_options()
{
    FILE* fp = NULL;
    if ((fp = fopen(INI_PATH, "r")) != NULL)
    {
        ini_entry_t entry;
        entry.section = NULL;
        entry.key = NULL;
        entry.value = NULL;
        int result;
        while ((result = GetEntryFromFile(fp, &entry)) > 0)
        {
            byte r = 0;
            WORD rx = 0;
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_TIMER, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_ENABLED, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_show_timer = r - '0';
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_COL, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    if ((rx >= 0) && (rx <= 255))
                        m_timer_col = rx%256;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_BCOL, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    if ((rx >= 0) && (rx <= 255))
                        m_timer_bcol = rx % 256;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_POSX, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    m_timer_x = rx;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_POSY, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    m_timer_y = rx;
                }
            }
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_TICKER, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_ENABLED, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_show_ticker = r - '0';
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_COL, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    if ((rx >= 0) && (rx <= 255))
                        m_ticker_col = rx%256;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_BCOL, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    if ((rx >= 0) && (rx <= 255))
                        m_ticker_bcol = rx % 256;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_POSX, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    m_ticker_x = rx;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_POSY, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    m_ticker_y = rx;
                }
            }
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_APM, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_ENABLED, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_show_apm = r - '0';
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_COL, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    if ((rx >= 0) && (rx <= 255))
                        m_apm_col = rx%256;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_BCOL, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    if ((rx >= 0) && (rx <= 255))
                        m_apm_bcol = rx % 256;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_POSX, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    m_apm_x = rx;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_POSY, -1) == CSTR_EQUAL)
                {
                    rx = 100 * (entry.value[0] - '0') + 10 * (entry.value[1] - '0') + (entry.value[2] - '0');
                    m_apm_y = rx;
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_TYPE, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '2'))
                        m_apm_type = r - '0';
                }
            }
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_GAMEPLAY, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_HOLY, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_holy_cam = r - '0';
                }
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_DCLICK, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_double_click = r - '0';
                }
            }
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_LOBBY, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_TYPE, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_lobby_comb = r - '0';
                }
            }
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_LOBBY, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_INTERFACE, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_lobby_interface = r - '0';
                }
            }
            if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.section, -1, INI_TXT_WAR2MOD, -1) == CSTR_EQUAL)
            {
                if (CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, entry.key, -1, INI_TXT_BUTTON, -1) == CSTR_EQUAL)
                {
                    r = entry.value[0];
                    if ((r >= '0') && (r <= '1'))
                        m_show_button = r - '0';
                }
            }
        }
        fclose(fp);
    }
}

//------------------------color chat
DWORD64 cchat_orc_default = 0x0000EFC0C5C7C800;
DWORD64 cchat_hum_default = 0x0000EF686CF6F600;

DWORD64 cchat_red_player = 0x0000EFD3D2D0D100;
DWORD64 cchat_blue_player = 0x0000EFD7D6D4D500;
DWORD64 cchat_green_player = 0x0000EFDBDAD8D900;
DWORD64 cchat_violet_player = 0x0000EFDFDEDCDD00;
DWORD64 cchat_orange_player = 0x0000EFE3E2E0E100;
DWORD64 cchat_black_player = 0x0000EFE7E6E4E500;
DWORD64 cchat_white_player = 0x0000EFEBEAE8E900;
DWORD64 cchat_yellow_player = 0x0000EF0F0E0C0D00;

DWORD64 cchat_orc_skin = 0x0000EF90979A9B00;
DWORD64 cchat_water = 0x0000EF28272A2B00;

DWORD cchat_g_jmp = 0;
char cchat_tbuf[1024];
byte cchat_tsz = 0;
byte cchat_cl = 0;

void cchat_text_start()
{
    memset(cchat_tbuf, 0, sizeof(cchat_tbuf));
    cchat_tsz = 0;
}

void cchat_text_start1()
{
    //placeholder empty function
    *(DWORD*)0x00412345 = 0;
    *(DWORD*)0x00422345 = 1;
    *(DWORD*)0x00432345 = 2;
    *(DWORD*)0x00442345 = 3;
}

void cchat_text_mid()
{
    cchat_tbuf[cchat_tsz] = cchat_cl;
    cchat_tsz++;
}

void cchat_text_mid1()
{
    //placeholder empty function
    *(DWORD*)0x00412345 = 0;
    *(DWORD*)0x00422345 = 1;
    *(DWORD*)0x00432345 = 2;
    *(DWORD*)0x00442345 = 3;
    *(DWORD*)0x00452345 = 4;
    *(DWORD*)0x00462345 = 5;
}

void cchat_text_color()
{
    bool def = true;
    if (cchat_tsz > 1)
    {
        byte prev = cchat_tbuf[cchat_tsz - 2];
        if (prev == 3)
        {
            *(DWORD*)0x004D4FCC = (DWORD)&cchat_orc_default;
            def = false;
        }
        if (prev == 5)
        {
            *(DWORD*)0x004D4FCC = (DWORD)&cchat_hum_default;
            def = false;
        }
        if (prev == 6)
        {
            *(DWORD*)0x004D4FCC = (DWORD)&cchat_orc_skin;
            def = false;
        }
        if (prev == 13)
        {
            *(DWORD*)0x004D4FCC = (DWORD)&cchat_water;
            def = false;
        }
        if (cchat_tsz > 2)
        {
            byte pprev = cchat_tbuf[cchat_tsz - 3];
            if ((pprev == 3) && (prev == 3))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_red_player;
                def = false;
            }
            if ((pprev == 3) && (prev == 5))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_blue_player;
                def = false;
            }
            if ((pprev == 3) && (prev == 6))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_green_player;
                def = false;
            }
            if ((pprev == 3) && (prev == 13))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_violet_player;
                def = false;
            }
            if ((pprev == 5) && (prev == 3))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_orange_player;
                def = false;
            }
            if ((pprev == 5) && (prev == 5))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_black_player;
                def = false;
            }
            if ((pprev == 5) && (prev == 6))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_white_player;
                def = false;
            }
            if ((pprev == 5) && (prev == 13))
            {
                *(DWORD*)0x004D4FCC = (DWORD)&cchat_yellow_player;
                def = false;
            }
        }

    }
    if (def)*(DWORD*)0x004D4FCC = 0x004D4FC0;
}

void cchat_load()
{
    char code1[] = "\x60\x0\x0\x0\x0\x0\x61\x89\xD\x84\x4F\x4D\x0\xC3";
    PATCH_SET((char*)cchat_text_start1, code1);
    patch_call((char*)cchat_text_start1 + 1, (char*)cchat_text_start);
    patch_call((char*)CCHAT_CODE1, (char*)cchat_text_start1);
    char nop[] = "\x90";
    PATCH_SET((char*)(CCHAT_CODE1 + 5), nop);

    char code2[] = "\x60\x88\xD\x0\x0\x0\x0\x0\x0\x0\x0\x0\x61\x8A\xC1\x43\x3C\x7\xC3";
    patch_setdword((DWORD*)(code2 + 3), (DWORD)&cchat_cl);
    PATCH_SET((char*)cchat_text_mid1, code2);
    patch_call((char*)cchat_text_mid1 + 7, (char*)cchat_text_mid);
    patch_call((char*)CCHAT_CODE2, (char*)cchat_text_mid1);

    char code3[] = "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90";
    PATCH_SET((char*)CCHAT_CODE3, code3);
    patch_call((char*)CCHAT_CODE3, (char*)cchat_text_color);
}

void cchat_unload()
{
    char buf1[] = "\x89\xD\x84\x4F\x4D\x0";
    PATCH_SET((char*)CCHAT_CODE1, buf1);
    char buf2[] = "\x8A\xC1\x43\x3C\x7";
    PATCH_SET((char*)CCHAT_CODE2, buf2);
    char buf3[] = "\xF\x85\xC\x2\x0\x0\xC7\x5\xCC\x4F\x4D\x0\xC0\x4F\x4D\x0";
    PATCH_SET((char*)CCHAT_CODE3, buf3);
}
//------------------------color chat

struct g_proc
{
    char old[5];
    int a;
};
g_proc procs[200];
int procn = 0;

void hook(int adr, PROC* p, char* func)
{
    procs[procn].a = adr;
    for (int i = 0; i < 5; i++)
        procs[procn].old[i] = *(char*)(adr + i);
    *p = patch_call((char*)adr, func);
    procn++;
}

void unhook(int p)
{
    patch_setbytes((char*)procs[p].a, procs[p].old, 5);
}

void files_hooks()
{
    hook(0x0044A65C, &g_proc_0044A65C, (char*)status_get_tbl);
    hook(0x0044AC83, &g_proc_0044AC83, (char*)unit_hover_get_id);
    hook(0x0044AE27, &g_proc_0044AE27, (char*)unit_hover_get_tbl);
    hook(0x004354C8, &g_proc_004354C8, (char*)objct_get_tbl_custom);
    hook(0x004354FA, &g_proc_004354FA, (char*)objct_get_tbl_campanign);
    hook(0x004300A5, &g_proc_004300A5, (char*)objct_get_tbl_briefing_task);
    hook(0x004300CA, &g_proc_004300CA, (char*)objct_get_tbl_briefing_title);
    hook(0x004301CA, &g_proc_004301CA, (char*)objct_get_tbl_briefing_text);
    hook(0x00430113, &g_proc_00430113, (char*)objct_get_briefing_speech);
}

void common_hooks()
{
    hook(0x0045271B, &g_proc_0045271B, (char*)update_spells);

    hook(0x004522B9, &g_proc_004522B9, (char*)seq_run);

    hook(0x0041038E, &g_proc_0041038E, (char*)damage1);
    hook(0x00409F3B, &g_proc_00409F3B, (char*)damage2);
    hook(0x0040AF70, &g_proc_0040AF70, (char*)damage3);
    hook(0x0040AF99, &g_proc_0040AF99, (char*)damage4);
    hook(0x00410762, &g_proc_00410762, (char*)damage5);
    hook(0x004428AD, &g_proc_004428AD, (char*)damage6);

    hook(0x0043BAE1, &g_proc_0043BAE1, (char*)rc_snd);
    hook(0x0043B943, &g_proc_0043B943, (char*)rc_build_click);
    hook(0x0040DF71, &g_proc_0040DF71, (char*)bld_unit_create);
    hook(0x0040AFBF, &g_proc_0040AFBF, (char*)tower_find_attacker);
    hook(0x00451728, &g_proc_00451728, (char*)unit_kill_deselect);

    hook(0x0044FF20, &g_proc_0044FF20, (char*)pathfind_mov);
    hook(0x0044FFE6, &g_proc_0044FFE6, (char*)pathfind_mov);

    hook(0x00436B8C, &g_proc_00436B8C, (char*)check_build_price);
    hook(0x0043B0AF, &g_proc_0043B0AF, (char*)check_build_price);
    hook(0x00418F46, &g_proc_00418F46, (char*)check_build_price);
    hook(0x00455F3C, &g_proc_00455F3C, (char*)cancel_building_check_price);
    hook(0x004762B1, &g_proc_004762B1, (char*)cancel_building_check_price);

    hook(0x004526FE, &g_proc_004526FE, (char*)grow_structure);
    hook(0x00418FFE, &g_proc_00418FFE, (char*)create_building);
    hook(0x0043A974, &g_proc_0043A974, (char*)placebox_query);
    hook(0x0043ABAB, &g_proc_0043ABAB, (char*)placebox_query);
    hook(0x00424745, &g_proc_00424745, (char*)goods_into_inventory);
    hook(0x004529C0, &g_proc_004529C0, (char*)goods_into_inventory);

    hook(0x00451054, &g_proc_00451054, (char*)count_add_to_tables_load_game);
    hook(0x00438A5C, &g_proc_00438A5C, (char*)unset_peon_ai_flags);
    hook(0x00438985, &g_proc_00438985, (char*)unset_peon_ai_flags);

    hook(0x0040EEDD, &g_proc_0040EEDD, (char*)upgrade_tower);
    hook(0x00442E25, &g_proc_00442E25, (char*)create_skeleton);
    hook(0x00425D1C, &g_proc_00425D1C, (char*)cast_raise);
    hook(0x00424F94, &g_proc_00424F94, (char*)cast_runes);
    hook(0x00424FD7, &g_proc_00424FD7, (char*)cast_runes);
    hook(0x0042757E, &g_proc_0042757E, (char*)ai_spell);
    hook(0x00427FAE, &g_proc_00427FAE, (char*)ai_attack);

    hook(0x0044CEBB, &g_proc_0044CEBB, (char*)get_msg);

    files_hooks();
}

void ogre_mode_hooks()
{
    hook(0x0044CFAF, &g_proc_0044CFAF, (char*)fix_chat1);
    hook(0x0045456B, &g_proc_0045456B, (char*)draw_selection_unit);
    hook(0x00454C02, &g_proc_00454C02, (char*)draw_selection_building);
    hook(0x00454852, &g_proc_00454852, (char*)draw_selection_side1);
    hook(0x00454882, &g_proc_00454882, (char*)draw_selection_side2);
    hook(0x004548B9, &g_proc_004548B9, (char*)draw_selection_side3);
    hook(0x004548E8, &g_proc_004548E8, (char*)draw_selection_side4);
}

void server_hooks()
{
    common_hooks();

    hook(0x00405463, &g_proc_00405463, (char*)draw_hook1);
    hook(0x00405483, &g_proc_00405483, (char*)draw_hook2);
    hook(0x00421F57, &g_proc_00421F57, (char*)draw_hook3);

    hook(0x00431329, &g_proc_00431329, (char*)victory_result);
    hook(0x0042A4E1, &g_proc_0042A4E1, (char*)victory_save_local);
    hook(0x0043147C, &g_proc_0043147C, (char*)victory_comps_stat);
    victory_comps_stat_init();

    hook(0x004425C1, &g_proc_004425C1, (char*)cell_set_pos);
    hook(0x00457D4A, &g_proc_00457D4A, (char*)select_many_units);
 
    hook(0x00478441, &g_proc_00478441, (char*)apm_stop_peons);
    hook(0x0047842F, &g_proc_0047842F, (char*)apm_move_peons);
    hook(0x004765C0, &g_proc_004765C0, (char*)apm_right_click);
    hook(0x00478470, &g_proc_00478470, (char*)apm_action);
    hook(0x00475DB1, &g_proc_00475DB1, (char*)apm_select);
    hook(0x004766D2, &g_proc_004766D2, (char*)apm_group_select);
    hook(0x004766C4, &g_proc_004766C4, (char*)apm_group_create);
    hook(0x00478456, &g_proc_00478456, (char*)apm_placebox);
    hook(0x00478487, &g_proc_00478487, (char*)apm_cancel_building);
    hook(0x0047849C, &g_proc_0047849C, (char*)apm_train_unit);
    hook(0x004784B3, &g_proc_004784B3, (char*)apm_cancel_unit);
    hook(0x0041FF92, &g_proc_0041FF92, (char*)apm_space);
    hook(0x0044AC1F, &g_proc_0044AC1F, (char*)apm_f2f3f4);
    hook(0x0042E105, &g_proc_0042E105, (char*)apm_map_click);

    hook(0x004763FA, &g_proc_004763FA, (char*)receive_cheat);

    hook(0x00433899, &g_proc_00433899, (char*)viz_set);
    hook(0x0042BB04, &g_proc_0042BB04, (char*)map_create_unit);

    hook(0x00475A7A, &g_proc_00475A7A, (char*)msg_written_fois);
    hook(0x00475AA2, &g_proc_00475AA2, (char*)msg_written);
    hook(0x0046CDA2, &g_proc_0046CDA2, (char*)game_starting_players_number);
    hook(0x0046CDCE, &g_proc_0046CDCE, (char*)game_starting_players_message);
    hook(0x0047A7F6, &g_proc_0047A7F6, (char*)game_starting);
    hook(0x00468798, &g_proc_00468798, (char*)game_create);
    hook(0x00479516, &g_proc_00479516, (char*)report_send_bne);

    hook(0x0041F915, &g_proc_0041F915, (char*)map_load);
    hook(0x0043373E, &g_proc_0043373E, (char*)map_load2);

    hook(0x00455D65, &g_proc_00455D65, (char*)ah_rec_act);
    hook(0x0047610B, &g_proc_0047610B, (char*)ah_rec_act2);
    hook(0x00455EA2, &g_proc_00455EA2, (char*)ah_rec_act_set);
    hook(0x0047623F, &g_proc_0047623F, (char*)ah_rec_act_set2);
    hook(0x0043B116, &g_proc_0043B116, (char*)ah_placebox);

    hook(0x00420EFD, &g_proc_00420EFD, (char*)status_res);
    hook(0x00420F02, &g_proc_00420F02, (char*)status_main);
    hook(0x0044528C, &g_proc_0044528C, (char*)status_buttons_draw_pal);

    hook(0x0041F780, &g_proc_0041F780, (char*)players_info_before_reset);

    hook(0x0042ECC6, &g_proc_0042ECC6, (char*)dialogs_load);
    hook(0x004249C9, &g_proc_004249C9, (char*)help_txt_load);
    hook(0x004248BC, &g_proc_004248BC, (char*)help_txt_change_pos);
    hook(0x00402761, &g_proc_00402761, (char*)menu_closed);
    hook(0x0040585E, &g_proc_0040585E, (char*)events_menu);
    hook(0x0041C763, &g_proc_0041C763, (char*)events_game);

    ogre_mode_hooks();
}

void capture_fix()
{
    char buf[] = "\xB0\x01\xF6\xC1\x02\x74\x02\xB0\x02\x50\x66\x8B\x7B\x18\x66\x8B\x6B\x1A\x8B\xD7\x8B\xF5\x29\xC2\x29\xC6\x8D\x43\x27\x31\xC9\x89\x44\x24\x24\x8A\x08\xC1\xE1\x02\x66\x8B\x81\x1C\xEE\x4C\x00\x66\x8B\x89\x1E\xEE\x4C\x00\x66\x01\xF8\x66\x01\xE9\x5D\x01\xE8\x01\xE9\x90\x90";
    PATCH_SET((char*)CAPTURE_BUG, buf);
}

void common_init()
{
    sounds_tables();//fix hero sounds
    capture_fix();//fix capture big units from right side
    upgr_check_replace(true);//fix upgrades big lvl draw buttons
}

void server_init()
{
    m_screen_w = *(WORD*)SCREEN_SIZE_W;
    m_screen_h = *(WORD*)SCREEN_SIZE_H;
    m_added_width = m_screen_w - 640;
    m_added_height = m_screen_h - 480;
    m_align_x = m_added_width > 0 ? m_added_width / 2 : 0;
    m_align_y = m_added_height > 0 ? m_added_height / 2 : 0;
    m_maxx = m_minx + m_screen_w - 200;
    m_maxy = m_miny + m_screen_h - 40;

    server_hooks();
    create_comboboxes();
    read_mod_options();
    cchat_load();

    common_init();
    
    replace_back();

    char r[] = "\xF";
    PATCH_SET((char*)DROP_TIMER, r);//15 sec drop
    char res_bug[] = "\x8B\xE8\xFF\x15\xB4\x3\x49\x0\x50\xFF\xD7\x6A\x0\x6A\x1\x6A\xB\x56\xFF\xD7\xEB\x8\x68\x50\x1\x0\x0\x56\xFF\xD7\x8B\x54\x24\x8\x8B\xC\xAD";
    PATCH_SET((char*)RES_BUG, res_bug);
}

DWORD CALLBACK Thread(LPVOID lpParam)
{
    while (TRUE)
    {
        Sleep(1);
        byte GM = *(byte*)GAME_MODE;
        if ((GM == 13) && in_lobby())
        {
            if (*(byte*)ME_HOST == 1)
            {
                for (int i = 0; i < 8; i++)
                {
                    if (get_player_controler(i) != C_PLAYER)
                    {
                        teams_lag[i] = 0;
                    }
                    if ((i == *(byte*)PLAYER_HOST) && (*(byte*)ME_HOST == 0))
                    {
                        teams_lag[i] = 0;
                    }
                }
            }
        }
        if ((GM == 13) && in_lobby() && !jmsend)
        {
            int numplayers = 0;
            for (int i = 0; i < 10; i++)
            {
                if (g_players[i].nickname[0] && g_players[i].id != -1)
                {
                    numplayers++;
                }
            }
            if (numplayers > 1)send_join_msg();
        }
        if (!replaced)//check if was not replaced to replace only once
        {
            if (GM == 13)//if we online (13 = BNE)
            {
                w13 = true;
                if (!in_lobby())
                {
                    war2_game_mode = 0;
                    ogre_mode = false;
                    memset(teams_host, 0, sizeof(teams_host));
                    memset(teams_lag, 0, sizeof(teams_lag));
                    jmsend = false;
                    g_numplayers = 0;
                    if (*(char*)NO_DRAW != '\x74')
                    {
                        char nodraw[] = "\x74";//need replace back nodraw only after records table
                        PATCH_SET((char*)NO_DRAW, nodraw);
                    }
                }
            }
        }
        else
        {
            if ((GM == 13) && !in_lobby())//if we in BNE chat but map not hosted
            {
                w13 = true;
                jmsend = false;
                Sleep(100);//safe time check
                if ((GM == 13) && !in_lobby())
                {
                    replace_back();
                }
            }
            if (GM == 6)
            {
                //if player win actions
                if (campanign)//if this was campanign map
                {
                    if (!check_campanign(crs[0], crs[1], crs[2], crs[3]))//if not already have
                    {
                        FILE* fp;
                        if ((fp = fopen(RECORD_PATH, "a+b")) != NULL)//file opened
                        {
                            fwrite(crs, sizeof(char), 4, fp);//write CRC of completed map
                            fclose(fp);
                        }
                    }
                }
                replace_back();
            }
            if (GM == 7)
            {
                //if player lose actions
                replace_back();
            }
        }
        if (GM == 4)
        {
            //back in main menu
            if ((w13) || (EV_CALL))
            {
                char buf[] = "\x0";
                PATCH_SET((char*)DLL_STATUS, buf);
                w13 = false;

                replace_back();
                upgr_check_replace(false);
                cchat_unload();
                for (int i = 0; i < procn; i++)
                    unhook(i);
                char back[] = "\xc7\x05\x38\x0d\x4c\x0\x60\x86\x45\x0";
                PATCH_SET((char*)MULTIPLAYER_TRIGGER, back);
                Sleep(2);
                HMODULE me = *(HMODULE*)DLL_POINTER;
                FreeLibraryAndExitThread(me, 0);
            }
        }
    }
}

extern "C" __declspec(dllexport) void _()
{
    *(DWORD*)(DLL_STATUS + 1) = trig_ver;

    server_init();

    DWORD dw = 0;
    CreateThread(NULL, 0, Thread, &dw, 0, NULL);
}

struct Player* g_players2021 = (struct Player*)FOIS_PLAYERS_STRUCT_GOG;//gog

bool in_lobby2021()
{
    //we in multiplayer lobby(or game) if lobby map name not empty
    return !(CompareString(LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, MAP_LOBBY2021, -1, "", -1) == 2);
}

void send_chat_msg2021(char* msg)
{
    ((int (*)(char*))F_LOBBY_CHAT_SEND2021)(msg);
}

void send_join_msg2021()
{
    Sleep(1000);
    if (in_lobby2021())
    {
        char msg[] = "\x5is using \x3unsafe \x5game \x5version! \x4Warning!";
        send_chat_msg2021(msg);
        jmsend = true;
    }
}

DWORD CALLBACK Thread2021(LPVOID lpParam)
{
    while (TRUE)
    {
        Sleep(1);
        byte GM = *(byte*)GAME_MODE2021;
        if ((GM == 13) && in_lobby2021() && !jmsend)
        {
            int numplayers = 0;
            for (int i = 0; i < 10; i++)
            {
                if (g_players2021[i].nickname[0] && g_players2021[i].id != -1)
                {
                    numplayers++;
                }
            }
            if (numplayers > 1)send_join_msg2021();
        }
        if (GM == 13)//if we online (13 = BNE)
        {
            w13 = true;
            if (!in_lobby2021())
            {
                jmsend = false;
            }
        }
        if (GM == 4)
        {
            //back in main menu
            if (w13)
            {
                char buf[] = "\x0";
                PATCH_SET((char*)DLL_STATUS, buf);
                w13 = false;

                HMODULE me = *(HMODULE*)DLL_POINTER;
                FreeLibraryAndExitThread(me, 0);
            }
        }
    }
}

extern "C" __declspec(dllexport) void _g()
{
    *(DWORD*)(DLL_STATUS + 1) = trig_ver;
    DWORD dw = 0;
    CreateThread(NULL, 0, Thread2021, &dw, 0, NULL);
}

PROC g_proc_0045614E;
void receive_cheat_single(int c, int a1)
{
    char buf[] = "\x0";
    buf[0] = *(byte*)LOCAL_PLAYER;
    PATCH_SET((char*)PACKET_PLAYER, buf);
    bool f = true;
    if ((c & (1 << 8)) != 0)//8 - autoheal
    {
        rec_autoheal();
        f = false;
    }
    if ((c & (1 << 9)) != 0)//9 - attack peons
    {
        rec_peons();
        f = false;
    }
    if (f)
    {
        c &= 141375;//0 on screen;1 fast build;2 god;3 magic;4 upgrades;5 money;11 hatchet;17 disco
        //13 tigerlily
        ((void (*)(int, int))g_proc_0045614E)(c, a1);//orig
    }
    else
    {
        char buf[] = "\x0";
        PATCH_SET((char*)PLAYER_CHEATED, buf);
    }
}

void replace_single_back()
{
    replace_back();
    char trig_jmp[] = "\x74\x0F";
    PATCH_SET((char*)SINGLEPLAYER_JMP, trig_jmp);
    char back[] = "\xc7\x05\x38\x0d\x4c\x0\x30\x8C\x45\x0";
    PATCH_SET((char*)SINGLEPLAYER_TRIGGER, back);
}

const char TRIG_PATH[] = ".\\plugin\\Triggers\\";
const char S_W2TR[] = ".w2tr";
const char S_CUSTOM[] = "custom";
const char S_HUMAN[] = "human";
const char S_ORC[] = "orc";
byte a_custom = 0;

void replace_single()
{
    char trig_file[40] = { 0 };
    if (a_custom != 0)
    {
        sprintf(trig_file, "%s%s%s", TRIG_PATH, S_CUSTOM, S_W2TR);
    }
    else
    {
        char lvln[] = "01";
        byte lvl = *(byte*)LEVEL_OBJ;
        bool r = lvl % 2 == 0;
        if (lvl < 28)
        {
            lvl /= 2;
            lvl += 1;
            lvln[0] = lvl / 10 + 48;
            lvln[1] = lvl % 10 + 48;
            if (r)
                sprintf(trig_file, "%s%s%s%s", TRIG_PATH, S_HUMAN, lvln, S_W2TR);
            else
                sprintf(trig_file, "%s%s%s%s", TRIG_PATH, S_ORC, lvln, S_W2TR);
        }
        else
        {
            lvl -= 28;
            lvl /= 2;
            lvl += 1;
            lvln[0] = lvl / 10 + 48;
            lvln[1] = lvl % 10 + 48;
            if (r)
                sprintf(trig_file, "%sx%s%s%s", TRIG_PATH, S_HUMAN, lvln, S_W2TR);
            else
                sprintf(trig_file, "%sx%s%s%s", TRIG_PATH, S_ORC, lvln, S_W2TR);

        }
    }
    replace_def();
    bool bk = false;//if was some problems replace back
    FILE* fp;//open file
    if ((fp = fopen(trig_file, "rb")) == NULL)bk = true;
    else
    {
        fread(t, sizeof(byte), MAX_COMMANDS, fp);//read triggers
        fclose(fp);

        if (t[0] == 0)//if we not have some triggers
            bk = true;
        if (!bk)
        {
            MCOM = MAX_COMMANDS;
            int COM = 0;
            int add = 0;
            while ((t[COM] != 0) && (COM < MCOM))
            {
                switch (t[COM])
                {
                case 0: COM = MCOM; break;
                case 1: COM += 2; break;
                case 2: COM += 2; break;
                case 3: COM += 4; break;
                case 4: COM += 4; break;
                case 5: COM += 3; break;
                case 6:
                    add = 0;
                    if (t[COM + 1] == 0)add = 1;
                    if (t[COM + 1] == 2)add = 4;
                    if (t[COM + 1] == 3)add = 3;
                    if (t[COM + 1] == 6)add = 1;
                    if (t[COM + 1] == 8)add = 2;
                    if (t[COM + 1] == 9)add = 1;
                    if (t[COM + 1] == 10)add = 2;
                    if (t[COM + 1] == 12)add = 3;
                    if (t[COM + 1] == 13)add = 2;
                    if (t[COM + 1] == 15)add = 1;
                    if (t[COM + 1] == 16)add = 1;
                    if (t[COM + 1] == 17)add = 1;
                    if (t[COM + 1] == 18)add = 2;
                    if (t[COM + 1] == 19)add = 1;
                    if (t[COM + 1] == 20)add = 1;
                    if (t[COM + 1] == 21)add = 1;
                    if (t[COM + 1] == 22)add = 3;
                    if (t[COM + 1] == 23)add = 2;
                    if (t[COM + 1] == 24)add = 2;
                    if (t[COM + 1] == 25)add = 3;
                    if (t[COM + 1] == 27)add = 2;
                    COM += add;
                    COM += 2; break;
                case 7: COM += 5; break;
                case 8: COM += 5; break;
                case 9: COM += 5; break;
                case 10: COM += 2; break;
                case 11: COM += 3; break;
                case 12: COM += 3; break;
                case 13: COM += 3; break;
                case 14: COM += 3; break;
                case 15: COM += 3; break;
                case 16: COM += 3; break;
                case 17: COM += 3; break;
                case 18: COM += 4; break;
                case 19: COM += 6; break;
                case 20: COM += 6; break;
                case 21: COM += 3; break;
                case 22: COM += 2; break;
                case 23: COM += 1; break;
                case 24: COM += 1; break;
                case 25: COM += 1; break;
                case 26: COM += 5; break;
                case 27: COM += 1; break;
                case 28: COM += 1; break;
                case 29: COM += 1; break;
                case 30: COM += 2; break;
                case 31: COM += 3; break;
                case 32: COM += 3; break;
                case 33: COM += 2; break;
                case 34: COM += 7; break;
                case 35: COM += 9; break;
                case 36: COM += 1; break;
                case 37:
                    if ((t[COM + 2] > 0) && (t[COM + 2] <= 60))
                        COM += t[COM + 2];
                    COM += 3; break;
                case 38: COM += 2; break;
                case 39: COM += 4; break;
                case 40: COM += 2; break;
                case 41: COM += 1; break;
                case 42: COM += 3; break;
                case 43: COM += 3; break;
                case 44: COM += 4; break;
                case 45: COM += 3; break;
                case 46: COM += 2; break;
                case 47: COM += 4; break;
                case 48: COM += 2; break;
                case 49: COM += 2; break;
                case 50: COM += 2; break;
                case 51: COM += 1; break;
                case 52: COM += 2; break;
                case 53: COM += 1; break;
                case 54: COM += 2; break;
                case 55: COM += 2; break;
                case 56: COM += 1; break;
                case 57: COM += 1; break;
                case 58: COM += 3; break;
                case 59: COM += 1; break;
                case 60://ignore - multiplayer only
                    COM += 1; break;
                case 61: COM += 2; break;
                case 62: COM += 4; break;
                case 63: COM += 1; break;
                case 64: COM += 1; break;
                case 65: COM += 2; break;
                case 66: COM += 2; break;
                case 67: COM += 2; break;
                case 68: COM += 2; break;
                case 69: COM += 2; break;
                case 70: COM += 2; break;
                case 71: COM += 1; break;
                case 72: COM += 3; break;
                case 73: COM += 5; break;
                case 74: COM += 4; break;
                case 75: COM += 2; break;
                case 76: COM += 2; break;
                case 77: COM += 1; break;
                case 78: COM += 1; break;
                case 79: COM += 1; break;
                case 80: COM += 1; break;
                case 81: COM += 3; break;
                case 82: COM += 3; break;
                case 83: COM += 3; break;
                case 84: COM += 3; break;
                case 85: COM += 3; break;
                case 86: COM += 3; break;
                case 87: //ignore - multiplayer only
                    COM += 1; break;
                case 88: COM += 2; break;
                case 89: COM += 1; break;
                case 90: COM += 1; break;
                case 91: COM += 4; break;
                case 92: COM += 4; break;
                case 93: pathfind_fix(true);
                    COM += 1; break;
                case 94: ai_fix_plugin(true);
                    COM += 1; break;
                case 95: COM += 4; break;
                case 96: COM += 5; break;
                case 97: saveload_fixed = true;
                    COM += 1; break;
                case 98: COM += 2; break;
                case 99: COM += 3; break;
                case 100: //ignore - multiplayer only
                    COM += 2; break;
                case 101: COM += 2; break;
                case 102: COM += 2; break;
                case 103: COM += 2; break;
                case 104: //ignore - multiplayer only
                    COM += 1; break;
                case 105: //ignore - multiplayer only
                    COM += 5; break;
                case 106: //ignore - multiplayer only
                    COM += 3; break;
                case 107:COM += 6; break;
                case 108:COM += 5; break;
                case 109:COM += 4; break;
                case 110:COM += 4; break;
                case 111:COM += 5; break;
                case 112:COM += 3; break;
                case 113:COM += 2; break;
                case 114:COM += 1; break;
                case 115:COM += 1; break;
                case 116:COM += 2; break;
                case 117:COM += 4; break;
                case 118:COM += 6; break;
                case 119:COM += 1; break;
                case 120:
                {
                    WORD osize = t[COM + 1] + 256 * t[COM + 2];
                    if ((osize > 0) && (osize <= 512))
                    {
                        mission_objective = (char*)((int)((char*)t) + (COM + 3));
                        COM += osize;
                    }
                    COM += 3; break;
                }
                case 121:
                {
                    WORD osize = t[COM + 2] + 256 * t[COM + 3];
                    if ((osize > 0) && (osize <= 48))
                    {
                        unit_names[t[COM + 1]] = (char*)((int)((char*)t) + (COM + 4));
                        COM += osize;
                    }
                    COM += 4; break;
                }
                case 122:
                {
                    WORD osize = t[COM + 1] + 256 * t[COM + 2];
                    if ((osize > 0) && (osize <= 512))
                    {
                        mission_briefing = (char*)((int)((char*)t) + (COM + 3));
                        COM += osize;
                    }
                    COM += 3; break;
                }
                case 123:
                {
                    WORD osize = t[COM + 1] + 256 * t[COM + 2];
                    if ((osize > 0) && (osize <= 48))
                    {
                        mission_briefing_title = (char*)((int)((char*)t) + (COM + 3));
                        COM += osize;
                    }
                    COM += 3; break;
                }

                case 254: if (t[COM + 1] == 255) MCOM = COM + 2;
                    COM += 2;
                    break;
                default: COM += 1; break;
                }
            }

            replace_common();

            //replace singleplayer trigger
            char trig_jmp[] = "\x74\x1A";//74 0F
            PATCH_SET((char*)SINGLEPLAYER_JMP, trig_jmp);
            char rep[] = "\xc7\x05\x38\x0d\x4c\x0\x30\x8C\x45\x0";
            void (*repf) () = trig;
            patch_setdword((DWORD*)(rep + 6), (DWORD)repf);
            PATCH_SET((char*)SINGLEPLAYER_TRIGGER, rep);

            replaced = true;
        }
    }
    if (bk)
    {
        replace_single_back();//replace back if problems to be safe
    }
}

PROC g_proc_0042A4A1;
void new_game(int a, int b, long c)
{
    a_custom = b % 256;
    if (*(byte*)GB_MULTIPLAYER == 0)
    {
        if (a_custom)*(byte*)LEVEL_OBJ = 53;//remember custom obj
        else
        {
            if (*(byte*)LEVEL_OBJ == 53)a_custom = 1;//fix for when saved game loads custom get broken
        }
        replace_single_back();
        replace_single();
    }
    ((void (*)(int, int, long))g_proc_0042A4A1)(a, b, c);//original
}

PROC g_proc_0041F7E4;
int load_game(int a)
{
    int original = ((int (*)(int))g_proc_0041F7E4)(a);//original
    if (*(byte*)GB_MULTIPLAYER == 0)
    {
        replace_single_back();
        replace_single();
    }
    return original;
}

void single_hooks()
{
    common_hooks();

    hook(0x0042A4A1, &g_proc_0042A4A1, (char*)new_game);
    hook(0x0041F7E4, &g_proc_0041F7E4, (char*)load_game);
    hook(0x0045614E, &g_proc_0045614E, (char*)receive_cheat_single);
}

DWORD CALLBACK Thread_single(LPVOID lpParam)
{
    while (TRUE)
    {
        Sleep(1);
        byte GM = *(byte*)GAME_MODE;
        if (GM == 6)
        {
            //if player win actions
            replace_single_back();
        }
        if (GM == 7)
        {
            //if player lose actions
            replace_single_back();
        }
        if (GM == 4)
        {
            //back in main menu
            replace_single_back();
        }
        if (GM == 12)
        {
            //load custom mission
            replace_single_back();
        }
    }
}

extern "C" __declspec(dllexport) void w2p_init()
{
    single_hooks();
    common_init();

    DWORD dw = 0;
    CreateThread(NULL, 0, Thread_single, &dw, 0, NULL);
}