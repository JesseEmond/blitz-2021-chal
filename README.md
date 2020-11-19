# Challenge

See https://2021.blitz.codes/.

Really, see the doc on [python/cpp/main.c](./python/cpp/main.c), it explains
the challenge, what we do, and where we should improve.

## Benchmarking

Launch the server:

```sh
$ cd python
$ make && python3 application.py
```

Launch the benchmark tool:

```sh
$ cd benchmark
$ python3 bench.py --verify
```

If you are just testing potential speeds without fully implementing everything
(and don't want to check answers), remove `--verify`.

NOTE: This does end-to-end tests from an external client. If you want benchmark
timers from the program itself, switch the `PROFILE_ENABLED` define in [config.h](./python/cpp/config.h)
to `1`.

## Submitting

You have a good candidate?

Remove prints. Have `PROFILE_ENABLED` set to `0`.

Go in [`Makefile`](./python/cpp/Makefile), uncomment the line to generate the
[PGO](https://en.wikipedia.org/wiki/Profile-guided_optimization) profile
(`-fprofile-generate`). Run the server with the benchmark tool for a full
challenge set.

Recompile using `make clean && make`, but now using the profile generated
(`-fprofile-use`).

Zip your `python` folder and submit on the website. Wait until the build is
done. Click on `Launch`, and wait!

**RUN MULTIPLE TIMES.** There is a lot of variation on the server. Up to ~0.05
pts even. Just rerun a couple of times to be sure.
