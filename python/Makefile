.PHONY: all clean scrub run

all clean scrub:
	$(MAKE) -C cpp -f Makefile $@

run: all
	python3 application.py -e 1
