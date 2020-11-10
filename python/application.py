import argparse
import ctypes
lib = ctypes.cdll.LoadLibrary('./cpp/libseqsum.so')

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Microchallenge Starter Pack')
    parser.add_argument('-p', metavar='p', type=int, default=27178)

    args = parser.parse_args()
    lib.launch(args.p)