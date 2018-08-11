#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:8002"
#define CLIENTID    "1111"
#define TOPIC       "$SYS/brokers/+/clients/#"
#define QOS         1
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    cJSON *json,*json_value;
    
    printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: ");

    payloadptr = message->payload;
    json = cJSON_Parse(payloadptr);
    if(json)
    {
        json_value = cJSON_GetObjectItem( json , "clientid");
        if(strstr(topicName,"/connected"))
        {
            if(json_value->type==cJSON_String)
            {
                printf("%s connected\n",json_value->valuestring);
            }
        }
        else if(strstr(topicName,"/disconnected"))
        {
            if(json_value->type==cJSON_String)
            {
                printf("%s disconnected\n",json_value->valuestring);
            }
        }
    }
    cJSON_Delete(json);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int main(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    int rc;
    int ch;

    MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username="malongwei";
    conn_opts.password="malongwei123";
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
    MQTTClient_subscribe(client, TOPIC, QOS);

    do 
    {
        ch = getchar();
    } while(ch!='Q' && ch != 'q');

    MQTTClient_unsubscribe(client, TOPIC);
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return rc;
}
