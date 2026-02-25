#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ------------------------------------------------------------
// Keyboard Layout Configuration
// ------------------------------------------------------------
// Must be defined BEFORE including HID-Project headers.
// Ensures correct key mapping for Norwegian keyboard layout.
#define LAYOUT_NORWEGIAN
#include <HID-Project.h>
#include <HID-Settings.h>

// ------------------------------------------------------------
// LCD Configuration (16x2 I2C Display)
// ------------------------------------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------------------------------------------------
// Physical Keypad Wiring
// ------------------------------------------------------------
// Row and column pins correspond to physical keypad matrix.
byte rowPins[4] = {8, 9, 10, A2};
byte colPins[5] = {4, 5, 6, 7, A1};

// ------------------------------------------------------------
// Keymap Definition
// ------------------------------------------------------------
const byte ROWS = 4, COLS = 5;

char keys[ROWS][COLS] = {
  { '7','4','1','P','n' },
  { '8','5','2','0','i' },
  { '9','6','3','.', 'g' },
  { 'B','C','M','E','h' }
};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ------------------------------------------------------------
// Operating Modes and State Management
// ------------------------------------------------------------
enum Bank { BANK_IP, BANK_WEB };
Bank bank = BANK_IP;

enum Edit { EDIT_IP, EDIT_PORT };
Edit editMode = EDIT_IP;

String ipDraft = "", ipSaved = "";
String portDraft = "";
uint16_t portSaved = 0;

// Tool selection flags
bool selNmap=false, selNikto=false, selGobuster=false;
bool selSqlmap=false, selWhatweb=false, selHarvester=false;

// ------------------------------------------------------------
// Input Validation Utilities
// ------------------------------------------------------------
static inline bool isDigitChar(char c){
  return c>='0' && c<='9';
}

// Validates IPv4 address format and value range
bool validIPv4(const String& ip){
  int n=ip.length();
  if(n<7||n>15) return false;

  int parts=0, i=0;

  while(i<n){
    if(ip[i]=='.') return false;

    int v=0, d=0;
    while(i<n && isDigitChar(ip[i])){
      v=v*10+(ip[i]-'0');
      if(++d>3) return false;
      i++;
    }

    if(d==0 || v>255) return false;

    parts++;
    if(parts==4) break;

    if(i>=n || ip[i]!='.') return false;
    i++;
  }

  return parts==4 && i==n;
}

// Validates TCP port range (1–65535)
bool validPort(const String& ps){
  if(ps.length()==0 || ps.length()>5) return false;

  for(char c: ps)
    if(!isDigitChar(c)) return false;

  long v = ps.toInt();
  return v>=1 && v<=65535;
}

// ------------------------------------------------------------
// LCD User Interface
// ------------------------------------------------------------
void draw(){
  lcd.clear();
  lcd.setCursor(0,0);

  if(editMode==EDIT_IP){
    lcd.print("IP:");
    lcd.print(ipDraft.length()? ipDraft :
              (ipSaved.length()? ipSaved : "-"));
  } else {
    lcd.print("PORT:");
    if(portDraft.length()) lcd.print(portDraft);
    else if(portSaved>0) lcd.print(portSaved);
    else lcd.print("-");
  }

  lcd.setCursor(0,1);

  if(bank==BANK_IP){
    if(selNmap) lcd.print("NM ");
    if(selNikto) lcd.print("NI ");
    if(selGobuster) lcd.print("GB ");
    if(!selNmap && !selNikto && !selGobuster)
      lcd.print("IP tools");
  } else {
    if(selSqlmap) lcd.print("SQ ");
    if(selWhatweb) lcd.print("WW ");
    if(selHarvester) lcd.print("TH ");
    if(!selSqlmap && !selWhatweb && !selHarvester)
      lcd.print("Web tools");
  }
}

// Temporary status message display
void flashMsg(const char* s, uint16_t ms=750){
  lcd.setCursor(0,1);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print(s);
  delay(ms);
  draw();
}

// Toggle tool selection within current bank
void toggleTool(char k){
  if(bank==BANK_IP){
    if(k=='n') selNmap = !selNmap;
    if(k=='i') selNikto = !selNikto;
    if(k=='g') selGobuster = !selGobuster;
  } else {
    if(k=='n') selSqlmap = !selSqlmap;
    if(k=='i') selWhatweb = !selWhatweb;
    if(k=='g') selHarvester = !selHarvester;
  }
  draw();
}

// ------------------------------------------------------------
// Keyboard Output Helpers (Norwegian Layout)
// ------------------------------------------------------------

// Hyphen (-)
static inline void sendHyphen_NO(){
  BootKeyboard.press(KEY_SLASH);
  delay(2);
  BootKeyboard.release(KEY_SLASH);
  delay(2);
}

// Forward slash (/)
static inline void sendSlash_NO(){
  BootKeyboard.press(KEY_LEFT_SHIFT);
  BootKeyboard.press('7');
  delay(2);
  BootKeyboard.release('7');
  BootKeyboard.release(KEY_LEFT_SHIFT);
  delay(2);
}

// Colon (:)
static inline void sendColon_NO(){
  BootKeyboard.press(KEY_LEFT_SHIFT);
  BootKeyboard.press('.');
  delay(2);
  BootKeyboard.release('.');
  BootKeyboard.release(KEY_LEFT_SHIFT);
  delay(2);
}

// Generic key tap utility
static inline void tap(uint8_t key,
                       uint16_t down_ms=45,
                       uint16_t up_gap_ms=30) {
  BootKeyboard.press(key);
  delay(down_ms);
  BootKeyboard.release(key);
  delay(up_gap_ms);
}

// Opens terminal using Super + Enter (Hyprland)
void openTerminal(){
  BootKeyboard.press(KEY_LEFT_GUI);
  delay(40);
  BootKeyboard.write(KEY_ENTER);
  BootKeyboard.release(KEY_LEFT_GUI);
  delay(800);
}
