CFLAGS= -I../paho.mqtt.c/src -I./cJSON
LDFLAGS= ../paho.mqtt.c/build/output/libpaho-mqtt3c.so.1.0

.PHONY: all clean

all : mqtt_client

mqtt_client : ./build/mqtt_client.o ./build/cJSON.o
	${CC} $^ -g -o $@ ${LDFLAGS} -lm

./build/mqtt_client.o : ./src/mqtt_client.c
	${CC} -g -c $^ -o $@ ${CFLAGS} 

./build/cJSON.o : ./cJSON/cJSON.c
	${CC} -g -c $^ -o $@ 
clean : 
	-rm -f ./build/*.o mqtt_client 
