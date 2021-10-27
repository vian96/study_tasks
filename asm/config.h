#ifndef CONFIG_H
#define CONFIG_H

// TODO cmd args for debug mode, i am tired of recompiling...
#define NDEBUG

#ifndef NDEBUG
#define DEB(...) printf (__VA_ARGS__);
#else // debug
#define DEB(...) 
#endif // not debug

#endif // CONFIG_H
