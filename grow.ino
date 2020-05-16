#include <NTPClient.h>
#include <WiFiUdp.h>

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

// digital pin 5
#define LED_PIN D5

String horaAtual, uniSegundoStr, dezSegundoStr, uniHoraStr, dezHoraStr;

int uniHoraAnterior = -1;
int uniSegundoAtual, uniSegundoAnterior, dezSegundoAtual, dezSegundoAnterior, horaLiga, dezHoraLiga, uniHoraLiga, horaDesliga, dezHoraDesliga, uniHoraDesliga, uniHoraAtual, dezHoraAtual;

const int intervalo = 10; // Tempo de atualização do relógio (Segundos)

byte lamp = 0;

int timeZone = -3;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.br.pool.ntp.org", timeZone * 3600);

AdafruitIO_Feed *lampadas = io.feed("lampadas");
AdafruitIO_Feed *ligar = io.feed("ligar");
AdafruitIO_Feed *desligar = io.feed("desligar");
AdafruitIO_Feed *hora = io.feed("hora");

void setup()
{

    // set button pin as an input
    pinMode(LED_PIN, OUTPUT);

    // start the serial connection
    Serial.begin(115200);

    // wait for serial monitor to open
    while (!Serial)
        ;

    // connect to io.adafruit.com
    Serial.print("Connecting to Adafruit IO");
    io.connect();

    // wait for a connection
    while (io.status() < AIO_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    // we are connected
    Serial.println();
    Serial.println(io.statusText());

    ligar->onMessage(ligarLampada);
    desligar->onMessage(desligarLampada);

    ligar->get();
    desligar->get();

    lampadas->save(1);

    digitalWrite(LED_PIN, HIGH);

    timeClient.begin();
}

void loop()
{
    io.run();

    salvaHora();

    handleLampadas();
}

void salvaHora()
{
    timeClient.update();

    horaAtual = timeClient.getFormattedTime();

    uniHoraStr = horaAtual[1];
    uniHoraAtual = uniHoraStr.toInt();

    dezHoraStr = horaAtual[0];
    dezHoraAtual = dezHoraStr.toInt();

    uniSegundoStr = horaAtual[7];
    uniSegundoAtual = uniSegundoStr.toInt();

    dezSegundoStr = horaAtual[6];
    dezSegundoAtual = dezSegundoStr.toInt();

    if (uniSegundoAtual != uniSegundoAnterior)
    {
        Serial.println(timeClient.getFormattedTime());
        uniSegundoAnterior = uniSegundoAtual;
    }

    if (dezSegundoAtual != dezSegundoAnterior)
    {
        hora->save(horaAtual);
        dezSegundoAnterior = dezSegundoAtual;
    }
}

void handleLampadas()
{
    Serial.print("dezHoraAtual: ");
    Serial.println(dezHoraAtual);
    Serial.print("dezHoraLiga: ");
    Serial.println(dezHoraLiga);
    Serial.print("dezHoraDesliga: ");
    Serial.println(dezHoraDesliga);
    Serial.print("uniHoraAtual: ");
    Serial.println(uniHoraAtual);
    Serial.print("uniHoraLiga: ");
    Serial.println(uniHoraLiga);
    Serial.print("uniHoraDesliga: ");
    Serial.println(uniHoraDesliga);
    Serial.print("uniHoraAnterior: ");
    Serial.println(uniHoraAnterior);
    
    if (dezHoraAtual == dezHoraLiga && uniHoraAtual == uniHoraLiga && uniHoraAtual != uniHoraAnterior)
    {
        lamp = 1;
        lampadas->save(lamp);
        digitalWrite(LED_PIN, HIGH);
        uniHoraAnterior = uniHoraAtual;
    }
    else
    {
        if (dezHoraAtual == dezHoraDesliga && uniHoraAtual == uniHoraDesliga && uniHoraAtual != uniHoraAnterior)
        {
            lamp = 0;
            lampadas->save(lamp);
            digitalWrite(LED_PIN, LOW);
            uniHoraAnterior = uniHoraAtual;
        }
    }
}

void ligarLampada(AdafruitIO_Data *data)
{ // Recebe o valor no slider e define o tempo de troca das lâmpadas
    horaLiga = data->toInt();
    dezHoraLiga = horaLiga / 10;
    uniHoraLiga = horaLiga - (dezHoraLiga * 10);
}

void desligarLampada(AdafruitIO_Data *data)
{ // Recebe o valor no slider e define o tempo de troca das lâmpadas
    horaDesliga = data->toInt();
    dezHoraDesliga = horaDesliga / 10;
    uniHoraDesliga = horaDesliga - (dezHoraDesliga * 10);
    Serial.print("Desliga dezena: ");
    Serial.println(dezHoraDesliga);
    Serial.print("Desliga unidade: ");
    Serial.println(uniHoraDesliga);
}
