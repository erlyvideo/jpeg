
all: priv/jpeg.so ebin/jpeg.beam


priv/jpeg.so: c_src/jpeg.c
	gcc c_src/jpeg.c /opt/libjpeg-turbo/lib64/libturbojpeg.a -lswscale -shared -fPIC -o priv/jpeg.so -I /usr/local/lib/erlang/erts-5.8.4/include/ -I /opt/libjpeg-turbo/include/

ebin/jpeg.beam: src/jpeg.erl
	erlc -o ebin src/jpeg.erl

compile:
	rm -f ._rebar.config
	rebar compile

