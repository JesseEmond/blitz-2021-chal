#! /usr/bin/env python3

MAX = 9999

with open('cpp/numbers.h', 'w') as f:
    f.write('#ifndef NUMBERS_H\n#define NUMBERS_H\n\n#define NUMBERS_MAX ' + str(MAX) + '\n\nconst char* const NUMBERS[] = {\n')
    l = len(str(MAX))
    for i in range(MAX + 1):
        s = str(i)
        f.write('    "{}"{}\n'.format(s.rjust(l), '' if i == MAX else ','))
    f.write('};\n\n#endif\n')
