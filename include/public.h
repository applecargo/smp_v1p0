//
// things that is 'public'
//
// --> collection of 'externs' from the modules
//
//     --> make all variables start with double underscores + module's id
//     --> for example, '__xx_yy' (xx module's yy variable)
//

#include <Arduino.h>

//main (smp_v1p0.ino)
extern int __main_count;
extern String __main_filename;

//oled
extern int __oled_start();
extern void __oled_stop();

//fs
extern int __fs_start();
extern void __fs_stop();
