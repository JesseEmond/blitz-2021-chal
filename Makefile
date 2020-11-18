.PHONY: all clean zip python

all: zip

clean:
	rm -f app.zip
	$(MAKE) -C python -f Makefile $@

zip: app.zip

app.zip: python
	zip -r $@ python/

python:
	$(MAKE) -C python -f Makefile all
