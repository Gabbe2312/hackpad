#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---- Norsk layout for NicoHood HID (må være før include) ----
#define LAYOUT_NORWEGIAN
#include <HID-Project.h>
#include <HID-Settings.h>

// ----------------- LCD -----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ----------------- DIN FYSISKE KOBLING -----------------
byte rowPins[4] = {8, 9, 10, A2};   // C1..C4
byte colPins[5] = {4, 5, 6, 7, A1}; // R1..R5

// ----------------- KEYMAP -----------------
const byte ROWS = 4, COLS = 5;
char keys[ROWS][COLS] = {
  { '7','4','1','P','n' },   // C1
  { '8','5','2','0','i' },   // C2
  { '9','6','3','.', 'g' },  // C3
  { 'B','C','M','E','h' }    // C4
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ----------------- MODUSER / TILSTAND -----------------
enum Bank { BANK_IP, BANK_WEB };  Bank bank = BANK_IP;
enum Edit { EDIT_IP, EDIT_PORT }; Edit editMode = EDIT_IP;

String ipDraft = "", ipSaved = "";
String portDraft = "";
uint16_t portSaved = 0;

// IP-bank verktøy
bool selNmap=false, selNikto=false, selGobuster=false;
// WEB-bank verktøy
bool selSqlmap=false, selWhatweb=false, selHarvester=false;

// ----------------- VALIDERING -----------------
static inline bool isDigitChar(char c){ return c>='0' && c<='9'; }

bool validIPv4(const String& ip){
  int n=ip.length(); if(n<7||n>15) return false;
  int parts=0, i=0;
  while(i<n){
    if(ip[i]=='.') return false;
    int v=0, d=0;
    while(i<n && isDigitChar(ip[i])){
      v=v*10+(ip[i]-'0'); if(++d>3) return false; i++;
    }
    if(d==0 || v>255) return false;
    parts++; if(parts==4) break;
    if(i>=n || ip[i]!='.') return false; i++;
  }
  return parts==4 && i==n;
}

bool validPort(const String& ps){
  if(ps.length()==0 || ps.length()>5) return false;
  for(char c: ps) if(!isDigitChar(c)) return false;
  long v = ps.toInt();
  return v>=1 && v<=65535;
}

// ----------------- LCD UI -----------------
void draw(){
  lcd.clear();
  lcd.setCursor(0,0);
  if(editMode==EDIT_IP){
    lcd.print("IP:");
    lcd.print(ipDraft.length()? ipDraft : (ipSaved.length()? ipSaved : "-"));
  } else {
    lcd.print("PORT:");
    if(portDraft.length()) lcd.print(portDraft);
    else if(portSaved>0)   lcd.print(portSaved);
    else                   lcd.print("-");
  }

  lcd.setCursor(0,1);
  if(bank==BANK_IP){
    if(selNmap)     lcd.print("NM ");
    if(selNikto)    lcd.print("NI ");
    if(selGobuster) lcd.print("GB ");
    if(!selNmap && !selNikto && !selGobuster)
      lcd.print("IP tools");
  } else {
    if(selSqlmap)    lcd.print("SQ ");
    if(selWhatweb)   lcd.print("WW ");
    if(selHarvester) lcd.print("TH ");
    if(!selSqlmap && !selWhatweb && !selHarvester)
      lcd.print("Web tools");
  }
}

void flashMsg(const char* s, uint16_t ms=750){
  lcd.setCursor(0,1); lcd.print("                ");
  lcd.setCursor(0,1); lcd.print(s);
  delay(ms);
  draw();
}

void toggleTool(char k){
  if(bank==BANK_IP){
    if(k=='n') selNmap     = !selNmap;
    if(k=='i') selNikto    = !selNikto;
    if(k=='g') selGobuster = !selGobuster;
  } else {
    if(k=='n') selSqlmap    = !selSqlmap;
    if(k=='i') selWhatweb   = !selWhatweb;
    if(k=='g') selHarvester = !selHarvester;
  }
  draw();
}

// ----------------- NO-LAYOUT: spesialtegn helpers -----------------
// Norsk '-' = fysisk tast ved US '/', dvs HID KEY_SLASH uten Shift.
static inline void sendHyphen_NO(){
  BootKeyboard.press(KEY_SLASH);
  delay(2);
  BootKeyboard.release(KEY_SLASH);
  delay(2);
}
// Norsk '/' = Shift + '7'
static inline void sendSlash_NO(){
  BootKeyboard.press(KEY_LEFT_SHIFT);
  BootKeyboard.press('7');
  delay(2);
  BootKeyboard.release('7');
  BootKeyboard.release(KEY_LEFT_SHIFT);
  delay(2);
}
// Norsk ':' = Shift + '.'
static inline void sendColon_NO(){
  BootKeyboard.press(KEY_LEFT_SHIFT);
  BootKeyboard.press('.');
  delay(2);
  BootKeyboard.release('.');
  BootKeyboard.release(KEY_LEFT_SHIFT);
  delay(2);
}

// ---- Hyprland: Super (Left GUI) + Return (KEY_ENTER) ----
static inline void tap(uint8_t key, uint16_t down_ms=45, uint16_t up_gap_ms=30) {
  BootKeyboard.press(key);
  delay(down_ms);
  BootKeyboard.release(key);
  delay(up_gap_ms);
}

void openTerminal(){
  BootKeyboard.press(KEY_LEFT_GUI);  // Super_L / Mod4
  delay(40);
  BootKeyboard.write(KEY_ENTER);                   // Return (wev: sym=Return)
  BootKeyboard.release(KEY_LEFT_GUI);
  delay(800);
}



// ----------------- IP-bank kommandoer -----------------
void typeNmap(const String& ip, uint16_t port){
  BootKeyboard.print(F("sudo nmap "));
  sendHyphen_NO(); BootKeyboard.print(F("sV "));

  if(port>0){
    // Vanlig portscan
    sendHyphen_NO(); BootKeyboard.print(F("p ")); 
    BootKeyboard.print(port); 
    BootKeyboard.print(F(" "));
    BootKeyboard.print(ip);
  } else {
    // Full portscan på alle porter
    sendHyphen_NO(); BootKeyboard.print(F("O "));
    sendHyphen_NO(); BootKeyboard.print(F("p"));   // -p
    sendHyphen_NO(); BootKeyboard.print(F(" "));   // -
    sendHyphen_NO(); BootKeyboard.print(F("T4 ")); // -T4
    BootKeyboard.print(ip);
  }

  BootKeyboard.write(KEY_ENTER);
}



void typeNikto(const String& ip, uint16_t port){
  BootKeyboard.print(F("nikto "));
  sendHyphen_NO(); BootKeyboard.print(F("host ")); BootKeyboard.print(ip);
  if(port>0){ BootKeyboard.print(F(" ")); sendHyphen_NO(); BootKeyboard.print(F("port ")); BootKeyboard.print(port); }
  BootKeyboard.write(KEY_ENTER);
}

void typeGobuster(const String& ip, uint16_t port){
  BootKeyboard.print(F("gobuster dir "));
  sendHyphen_NO(); BootKeyboard.print(F("u http"));
  sendColon_NO();  // :
  sendSlash_NO();  // /
  sendSlash_NO();  // /
  BootKeyboard.print(ip);
  if(port>0){ sendColon_NO(); BootKeyboard.print(port); }
  BootKeyboard.print(F(" "));
  sendHyphen_NO(); BootKeyboard.print(F("w "));
  // /usr/share/wordlists/dirb/common.txt
  sendSlash_NO(); BootKeyboard.print(F("usr"));
  sendSlash_NO(); BootKeyboard.print(F("share"));
  sendSlash_NO(); BootKeyboard.print(F("wordlists"));
  sendSlash_NO(); BootKeyboard.print(F("dirb"));
  sendSlash_NO(); BootKeyboard.print(F("common"));
  BootKeyboard.print(F(".txt"));
  BootKeyboard.write(KEY_ENTER);
}

// ----------------- WEB-bank kommandoer -----------------
void typeSqlmap(const String& ip, uint16_t port){
  // sqlmap -u http://IP[:port] --batch
  BootKeyboard.print(F("sqlmap "));
  sendHyphen_NO(); BootKeyboard.print(F("u http"));
  sendColon_NO(); sendSlash_NO(); sendSlash_NO();
  BootKeyboard.print(ip);
  if(port>0){ sendColon_NO(); BootKeyboard.print(port); }  // <-- inkluder PORT
  BootKeyboard.print(F(" "));
  // "--batch"
  sendHyphen_NO(); sendHyphen_NO(); BootKeyboard.print(F("batch"));
  BootKeyboard.write(KEY_ENTER);
}

void typeWhatweb(const String& ip, uint16_t port){
  // whatweb http://IP[:port]
  BootKeyboard.print(F("whatweb http"));
  sendColon_NO(); sendSlash_NO(); sendSlash_NO();
  BootKeyboard.print(ip);
  if(port>0){ sendColon_NO(); BootKeyboard.print(port); }  // <-- inkluder PORT
  BootKeyboard.write(KEY_ENTER);
}

void typeHarvester(const String& target, uint16_t /*port*/){
  // theHarvester -d <target> -b all
  // NB: theHarvester har ingen port-opsjon; port blir ikke brukt her.
  BootKeyboard.print(F("theHarvester "));
  sendHyphen_NO(); BootKeyboard.print(F("d ")); BootKeyboard.print(target);
  BootKeyboard.print(F(" "));
  sendHyphen_NO(); BootKeyboard.print(F("b all"));
  BootKeyboard.write(KEY_ENTER);
}


// ----------------- Kjør valgte -----------------
void runSelectedTools(){
  if(!validIPv4(ipSaved)){
    if(validIPv4(ipDraft)) ipSaved=ipDraft;
  }
  if(!validIPv4(ipSaved)){
    flashMsg("Set valid IP",900);
    return;
  }

  BootKeyboard.begin();

  if(bank==BANK_IP){
    if(selNmap){ openTerminal(); typeNmap(ipSaved,portSaved); }
    if(selNikto){ openTerminal(); typeNikto(ipSaved,portSaved); }
    if(selGobuster){ openTerminal(); typeGobuster(ipSaved,portSaved); }
  } else {
    if(selSqlmap){ openTerminal(); typeSqlmap(ipSaved,portSaved); }
    if(selWhatweb){ openTerminal(); typeWhatweb(ipSaved,portSaved); }
    if(selHarvester){ openTerminal(); typeHarvester(ipSaved,portSaved); }
  }

  BootKeyboard.end();
  flashMsg("Launched!",800);
}

// ----------------- TASTEHÅNDTERING -----------------
void handleKey(char k){
  if(k=='M'){ bank=(bank==BANK_IP?BANK_WEB:BANK_IP); draw(); return; }
  if(k=='n'||k=='i'||k=='g'){ toggleTool(k); return; }
  if(k=='h'){ runSelectedTools(); return; }

  if(editMode==EDIT_IP){
    if(isDigitChar(k)||k=='.'){ if(ipDraft.length()<15){ ipDraft+=k; draw(); } }
    else if(k=='B'){ if(ipDraft.length()){ ipDraft.remove(ipDraft.length()-1); draw(); } }
    else if(k=='C'){ ipDraft=""; draw(); }
    else if(k=='E'){ if(validIPv4(ipDraft)){ ipSaved=ipDraft; flashMsg("IP Saved"); } else flashMsg("Invalid IP",900); }
    else if(k=='P'){ editMode=EDIT_PORT; portDraft=""; draw(); }
  } else {
    if(isDigitChar(k)){ if(portDraft.length()<5){ portDraft+=k; draw(); } }
    else if(k=='B'){ if(portDraft.length()){ portDraft.remove(portDraft.length()-1); draw(); } }
    else if(k=='C'){ portDraft=""; portSaved=0; draw(); }
    else if(k=='E'){
      if(validPort(portDraft)){ portSaved=portDraft.toInt(); flashMsg("Port Saved"); }
      else flashMsg("Invalid Port",900);
    }
    else if(k=='P'){ editMode=EDIT_IP; draw(); }
  }
}

// ----------------- SETUP / LOOP -----------------
void setup(){
  lcd.init(); lcd.backlight();
  keypad.setDebounceTime(25);
  draw();
}

void loop(){
  char k = keypad.getKey();
  if(k) handleKey(k);
}
