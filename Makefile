include version.mk

all: priv/jpeg.so ebin/jpeg.beam


priv/jpeg.so: c_src/jpeg.c
	mkdir -p priv/
	gcc c_src/jpeg.c /opt/libjpeg-turbo/lib64/libturbojpeg.a -g -shared -fPIC -o priv/jpeg.so -I /usr/local/lib/erlang/erts-5.8.4/include/ -I /opt/libjpeg-turbo/include/

ebin/jpeg.beam: src/jpeg.erl
	erlc -o ebin src/jpeg.erl

compile:
	rm -f ._rebar.config
	rebar compile

package: compile
	rm -rf tmproot
	mkdir -p tmproot/opt/erlyvideo/lib/jpeg-$(VERSION)/
	cp -r priv ebin src tmproot/opt/erlyvideo/lib/jpeg-$(VERSION)/
	cd tmproot && \
	fpm -s dir -t deb -n erly-jpeg -v $(VERSION) -m "Max Lapshin <max@maxidoors.ru>" opt 
	mv tmproot/*.deb .

upload_package: 
	scp *$(VERSION)* erlyhub@git.erlyvideo.org:/apps/erlyvideo/debian/public/transcoding
	ssh erlyhub@git.erlyvideo.org "cd /apps/erlyvideo/debian ; ./update transcoding"


.PHONY: package upload_package


