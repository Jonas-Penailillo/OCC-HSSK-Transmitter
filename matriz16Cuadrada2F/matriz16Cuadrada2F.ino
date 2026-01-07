#include <Adafruit_NeoPixel.h>
#include <math.h>

#define PIN              14       // 14 13
#define PIN2             13       // 14 13
#define NUM_LEDS         16      // Cambia este valor para el número de LEDs que deseas controlar:1/4/9/16/25/36/49/64
#define TOTAL_LEDS       16      // Total de LEDs disponibles en tu tira
#define SYNC_SLOT_TIME   1000//1000     // Tiempo de cada slot para la señal de sincronización en milisegundos
#define CSK_SLOT_TIME    50     // Tiempo de cada slot para la constelación 4CSK en milisegundos
#define OFF_TIME         0//10000    // Tiempo durante el cual los LEDs estarán apagados al final del ciclo en milisegundos
#define Ini_OFF_Time     1000*5//1000*60*1//10000//60000   // Tiempo durante el cual los LEDs estarán apagados al inicio del ciclo en milisegundos
#define GRID_ROWS 16 // Tamaño de la cuadrícula de colores
#define brilloLED  255  // Variable para controlar el valor (brillo) en HSV
#define SaturationD 64  //Desfase de la saturacion 
#define ValueD 0 //Desfase del Value
#define Canales 2 //Cantidad de canales Usados

Adafruit_NeoPixel strip(TOTAL_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(TOTAL_LEDS, PIN2, NEO_GRB + NEO_KHZ800);

// Función para apagar todos los LEDs
void apagarTodosLosLEDs() {
  for (int i = 0; i < TOTAL_LEDS; i++) {
    strip.setPixelColor(i, 0); // Apagar el LED
    strip2.setPixelColor(i, 0); // Apagar el LED
  }
  strip.show();
  strip2.show();
}
// Orden d e LEDs desde la esquina superior izquierda hacia afuera en una matriz de 8x8
// Orden de LEDs en cuadros crecientes desde la esquina superior izquierda (4x4)
int ledOrder[] = {
  // Cuadro 1 (1x1)
  0,

  // Cuadro 2 (2x2)
  1, 7, 6,

  // Cuadro 3 (3x3)
  2, 8, 5, 9, 10,

  // Cuadro 4 (4x4, toda la matriz)
  3, 15, 4, 14, 13, 11, 12
};



// Función para mostrar un color en todos los LEDs durante un cierto tiempo
void showColor(uint32_t color[Canales], unsigned long duration) {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(ledOrder[i], color[0]);
    strip2.setPixelColor(ledOrder[i], color[1]);
  }
  strip.show();
  strip2.show();
  delay(duration);
}



// Función para convertir los bits en un valor decimal utilizando la codificación Gray
int bitsToGrayDecimal(int bits, int numBits) {
  int gray = bits & (1 << (numBits - 1));
  for (int i = numBits - 2; i >= 0; i--) {
    gray |= ((bits >> i) ^ (bits >> (i + 1))) << i;
  }
  return gray;
}

// Función para convertir los bits de la fila en un valor decimal utilizando la codificación Gray
int rowBitsToGrayDecimal(bool* bits, int numBits) {
  int gray = bits[0];
  for (int i = 1; i < numBits; i++) {
    gray = (gray << 1) | (bits[i] ^ bits[i - 1]);
  }
  return gray;
}

// Definir la función para convertir texto a bits
void textToBits(String text, bool* bits) {
  int textLength = text.length();
  for (int i = 0; i < textLength; i++) {
    char c = text.charAt(i);
    for (int j = 0; j < 8; j++) {
      bits[i * 8 + j] = (c >> (7 - j)) & 1;
    }
  }
}

  // Función para calcular el logaritmo base 2 manualmente
  int log2(int x) {
    int res = 0;
    while (x >>= 1) res++;
    return res;
  }

// Función para modular bits a colores usando la constelación definida
void modulateToColors(bool* bits, int length) {
  int totalColors = 0;
  int saturation = 255; // Saturación fija al 100%
  int saturationA;
  int brilloLEDA;

  // Calcular el número de bits necesarios para representar GRID_ROWS
  int bitsForRow = log2(GRID_ROWS);

  uint32_t colorT[Canales]; // Arreglo unidimensional para almacenar un color por canal

  // Variables para almacenar los bits de fila
  bool row[bitsForRow];

  
  int symbolIdx = 0; // Nuevo contador de símbolos

  for (int i = 0; i < length; i += bitsForRow) {
    // Actualizar los bits de fila según el índice actual
    memset(row, 0, bitsForRow);
    for (int j = i; j < i + bitsForRow && j < length; j++) {
      row[j - i] = bits[j];
    }

    // Convertir los bits de la fila en un valor decimal utilizando la codificación Gray
    int grayRow = rowBitsToGrayDecimal(row, bitsForRow);

    // Determinar el valor de saturación y brillo
    if (grayRow % 2 == 0) {
        saturationA = saturation - SaturationD;
        brilloLEDA = brilloLED - ValueD;
    } else {
        saturationA = saturation;
        brilloLEDA = brilloLED;
    }

    // Calcular el matiz (hue) basado en el valor de fila (usando código Gray)
    long hue = (long)(grayRow * 360 / GRID_ROWS);
    long hueR = (long)(hue * 65535 / 360);

    uint32_t color = strip.ColorHSV(hueR, saturationA, brilloLEDA);

    // Mostrar por Serial lo que se va a generar
    //Serial.print("Símbolo: "); Serial.print(grayRow);
    //Serial.print(" | Hue: "); Serial.print(hue);
    //Serial.print(" | Saturation: "); Serial.print(saturationA);
    //Serial.print(" | Brillo: "); Serial.print(brilloLEDA);
    //Serial.print(" | Color (hex): 0x"); Serial.println(color, HEX);


    // Calcular el índice de canal para distribuir el color
    //int Resto = i % Canales;
    int Resto = symbolIdx % Canales;

    // Asignar el color al canal correspondiente
    colorT[Resto] = color;

    // Si ya hemos llenado todos los canales, mostrar los colores de todos los canales
    if (Resto == Canales - 1 || i + bitsForRow >= length) {
      showColor(colorT, CSK_SLOT_TIME);  // Mostrar todos los colores de los canales
    }

    
    symbolIdx++; // Aumentamos el número de símbolos modulados
  }
}


// Función para convertir un número decimal a su representación binaria
void decimalToBinary(int num, bool* bits, int numBits) {
  for (int i = 0; i < numBits; i++) {
    bits[i] = (num >> (numBits - 1 - i)) & 1; // Llenar el arreglo de bits en orden directo
  }
}

// Función para calcular los colores posibles basados en GRID_ROWS
void generateColors(uint32_t* colors, int numColors) {
    int saturation = 255; // Saturación al 100%
    int saturationA;
    int brilloLEDA;
    int bitsForRow = (int)log2(GRID_ROWS);
    bool binaryBits[bitsForRow]; // Arreglo para almacenar los bits binarios

    for (int i = 0; i < numColors; i++) {
        // Convertir 'i' a su representación binaria
        decimalToBinary(i, binaryBits, bitsForRow);

        // Convertir los bits binarios a Gray
        int grayValue = rowBitsToGrayDecimal(binaryBits, bitsForRow);

        if (grayValue % 2 == 0) {
            saturationA=saturation-SaturationD;
            brilloLEDA=brilloLED-ValueD;
        } else {
            saturationA=saturation;
            brilloLEDA=brilloLED;
        }
        // Calcular el color basado en el valor Gray
        long hue = (long)(grayValue * 360 / GRID_ROWS);
        long hueR = (long)(hue * 65535 / 360);
        colors[i] = strip.ColorHSV(hueR, saturationA, brilloLEDA);
    }
}


// Función para mostrar todos los símbolos posibles basados en GRID_ROWS
void showAllSymbols() {
  int numColors = GRID_ROWS;
  uint32_t colors[numColors];
  generateColors(colors, numColors); // Generamos los colores

  uint32_t colorT[Canales]; // Arreglo unidimensional para almacenar un color por canal

  // Asignamos los colores generados a cada canal
  for (int i = 0; i < numColors; i++) {
    for (int j = 0; j < Canales; j++) {
      colorT[j] = colors[i]; // Asignamos el color a cada canal
    }
    showColor(colorT, CSK_SLOT_TIME); // Mostrar colores para todos los canales a la vez
  }
}

void showTransmitterID() {
  int numColors = GRID_ROWS;
  uint32_t colors[numColors];
  generateColors(colors, numColors); // Generar los colores de la constelación

  uint32_t colorT[Canales];

  // Asignar los primeros 'Canales' colores a cada canal
  for (int j = 0; j < Canales; j++) {
    colorT[j] = colors[j];
  }

  // Mostrar los colores para identificar este transmisor
  showColor(colorT, CSK_SLOT_TIME); 
}


void showSyncSignal() {
  int saturation = 255; // Saturación fija al 100%
  uint32_t color[Canales];

  // Verde
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(21845, saturation, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Verde 50%
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(21845, saturation / 2, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Rojo
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(0, saturation, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Rojo 50%
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(0, saturation / 2, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Azul
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(43690, saturation, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Azul 50%
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(43690, saturation / 2, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Amarillo
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(10923, saturation, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Amarillo 50%
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(10923, saturation / 2, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Cian
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(32768, saturation, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Cian 50%
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(32768, saturation / 2, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Magenta
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(54613, saturation, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Magenta 50%
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(54613, saturation / 2, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Blanco
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(0, 0, brilloLED);
  showColor(color, SYNC_SLOT_TIME);

  // Blanco (repetido)
  for (int i = 0; i < Canales; i++) color[i] = strip.ColorHSV(0, 0, brilloLED);
  showColor(color, SYNC_SLOT_TIME);
}


void setup() {
  Serial.begin(115200);
  strip.begin();
  strip2.begin();
  strip.show();
  strip2.show();
}

void loop() {
  //String text = "Hola, mundo!";
  //String text = "Hola, mundo! Hola, mundo! Hola, mundo! Hola, mundo! Hola, mundo! Hola, mundo! Hola, mundo! Hola, mundo! Hola, mundo!";
  //String text = "Once upon a time,in a land far,far away,there lived a young prince who dreamed of exploring the world beyond his kingdom.";
  //String text = "Bold knights guard castles as clever foxes roam the woods, seeking treasure, wisdom, and old tales!!. ";
  //String text = "Majestic mountains stand tall while brave climbers ascend, chasing the thrill of reaching the summit!.";


  //String text = "Majestic mountains stand tall while brave climbers ascend, chasing the thrill of reaching the summit!.";
  //String text = "Silent waves crash on the shore as seagulls dance in the golden sunset, painting the sky with dreams.";
  //String text = "A fearless knight rides into the unknown, seeking honor and adventure beyond the misty hills of time.";
  //String text = ">e3zvshd](b.S43brt#:3*p|@`(RsV.z0k`SHzjrHdbMcJI:x5~W'fMa)B=<K,o{85[tBatcMzW>KkLJq\y`^?9:>l'~vkXMy>_*";
  String text = "SXF\OLZj)@I<>hEMDjfFp\SQiEam-$9Ng K^rdO$?7dSMW7bdbzUu u$y/Ws,H@CFB~1%6~?T0$'GsMQ)<lx<)>M m!S>.#v=JHO";
  //String text = "Ancient ruins whisper secrets of a forgotten civilization, buried beneath the sands of endless time.";
  //String text = "Beneath the ocean's surface, dazzling creatures glide through coral gardens, unseen by human eyes.";
  //String text = "The midnight train rushes through foggy valleys, carrying untold stories bound for distant lands.";
  //String text = "Echoes of laughter fill the bustling city streets as neon lights flicker against the darkened sky.";
  //String text = "A lone astronaut gazes at Earth from the silent void, humbled by the vastness of the endless cosmos.";
  //String text = "Golden leaves dance in the autumn breeze, swirling like tiny embers in the crisp afternoon air.";
  //String text = "A hidden waterfall roars deep in the forest, its shimmering cascade untouched by time or man.";
  //String text = "Wandering fireflies create glowing trails, weaving silent symphonies in the warm summer night.";
  //String text = "Snow blankets the quiet village, muffling every sound as chimneys puff warm smoke into the sky.";
  //String text = "The desert wind carves stories into the dunes, shifting the landscape in an eternal waltz of sand.";
  //String text = "Storm clouds gather on the horizon, rumbling like ancient giants waking from a restless slumber.";
  //String text = "A forgotten melody lingers in the abandoned hall, its notes fading into dust and distant echoes.";
  //String text = "Lanterns float gently down the river, carrying whispered wishes toward the endless ocean beyond.";
  //String text = "The castle gates creak open, revealing hidden chambers filled with ancient relics and mystery.";
  //String text = "A painter stands before a blank canvas, colors swirling in their mind, waiting to be set free.";
  //String text = "Thunder rolls across the sky as raindrops drum against the rooftops in a soothing evening lullaby.";
  //String text = "Lost in a vast labyrinth, the explorer traces faded symbols, searching for a path to the light.";
  //String text = "The clock strikes midnight, and shadows dance on the walls, whispering secrets of the unseen.";
  //String text = "The lone lighthouse stands firm against raging waves, guiding lost ships home through the storm.";
  //String text = "A gentle breeze rustles the cherry blossoms, sending pink petals swirling in a poetic farewell.";
  //String text = "Deep in the jungle, vibrant parrots call to one another, their voices echoing through the canopy.";



  int totalBits = text.length() * 8;
  int bitsPerSlot = Canales * log2(GRID_ROWS);
  int paddingBits = (bitsPerSlot - (totalBits % bitsPerSlot)) % bitsPerSlot;
  //int paddingBits = ((log2(GRID_ROWS)) - (totalBits % (log2(GRID_ROWS))))%log2(GRID_ROWS);

  
  // Imprimir los bits de padding
  //Serial.print("Bits de padding: ");
  //Serial.println(paddingBits);

  int bitsArraySize = totalBits + paddingBits;
  bool bits[bitsArraySize];
  memset(bits, 0, bitsArraySize); // Inicializar todos los bits a 0
  textToBits(text, bits);

  // Imprimir el array de bits
  //Serial.print("bitTexto: ");
  //for (int i = 0; i < bitsArraySize; i++) {
  //  Serial.print(bits[i]);
  //}
  //Serial.print('\n');

  // Apagar todos los LEDs antes de comenzar
  apagarTodosLosLEDs();
  delay(Ini_OFF_Time);

  showSyncSignal();

  

  showAllSymbols(); // Mostrar todos los símbolos posibles
  
  showTransmitterID(); // Muestra la id del canal  
 
  // Repetir indefinidamente la modulación a colores
  while (true) {
    modulateToColors(bits, bitsArraySize);
  }

  // Apagar los LEDs por un tiempo definido
  apagarTodosLosLEDs();
  delay(OFF_TIME);
}


