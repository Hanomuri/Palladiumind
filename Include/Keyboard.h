#define KEY_ESC      256
#define KEY_UP       325
#define KEY_DOWN     326
#define KEY_RIGHT    327
#define KEY_LEFT     328

typedef struct Keyboard 
{
  int bytesToRead;              // Bytes in stdin buffer
  char keysBuffer[3];   // Max keys to be read at a time
} Keyboard;

short KeyboardParser(Keyboard* keyboard);