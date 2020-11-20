#! /usr/bin/env python3

MAX = 9999

with open('cpp/numbers.h', 'w') as f:
    f.write('#ifndef NUMBERS_H\n#define NUMBERS_H\n\n#include <stdint.h>\n\n#define NUMBERS_MAX ' + str(MAX) + '\n\nconst uint32_t NUMBERS[] = {\n')
    l = len(str(MAX))
    for i in range(MAX + 1):
        s = ''.join('{:02x}'.format(ord(c)) for c in str(i).rjust(l)[::-1])
        f.write('    0x{}{}\n'.format(s, '' if i == MAX else ','))
    f.write('};\n\n#endif\n')
