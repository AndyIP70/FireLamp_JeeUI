/*
Copyright © 2020 Dmytro Korniienko (kDn)
JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov

    This file is part of FireLamp_JeeUI.

    FireLamp_JeeUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireLamp_JeeUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireLamp_JeeUI.  If not, see <https://www.gnu.org/licenses/>.

  (Этот файл — часть FireLamp_JeeUI.

   FireLamp_JeeUI - свободная программа: вы можете перераспространять ее и/или
   изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
   в каком она была опубликована Фондом свободного программного обеспечения;
   либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
   версии.

   FireLamp_JeeUI распространяется в надежде, что она будет полезной,
   но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
   или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
   общественной лицензии GNU.

   Вы должны были получить копию Стандартной общественной лицензии GNU
   вместе с этой программой. Если это не так, см.
   <https://www.gnu.org/licenses/>.)
*/

#include "main.h"

extern const TProgmemRGBPalette16 WaterfallColors_p FL_PROGMEM = {
  CRGB::Black,
  CRGB::DarkSlateGray,
  CRGB::DimGray,
  CRGB::LightSlateGray,

  CRGB::DimGray,
  CRGB::DarkSlateGray,
  CRGB::Silver,
  CRGB::DarkCyan,

  CRGB::Lavender,
  CRGB::Silver,
  CRGB::Azure,
  CRGB::LightGrey,

  CRGB::GhostWhite,
  CRGB::Silver,
  CRGB::White,
  CRGB::RoyalBlue
};

//----------------------------------------------------
void fadePixel(uint8_t i, uint8_t j, uint8_t step)          // новый фейдер
{
    int32_t pixelNum = myLamp.getPixelNumber(i, j);
    if (myLamp.getPixColor(pixelNum) == 0U) return;

    CRGB led = myLamp.getLeds(pixelNum);
    if (led.r >= 30U ||
        led.g >= 30U ||
        led.b >= 30U){
        myLamp.setLedsfadeToBlackBy(pixelNum,step);
    }
    else{
        myLamp.setLeds(pixelNum, 0U);
    }
}

// функция плавного угасания цвета для всех пикселей
void fader(uint8_t step)
{
  for (uint8_t i = 0U; i < WIDTH; i++)
  {
    for (uint8_t j = 0U; j < HEIGHT; j++)
    {
      fadePixel(i, j, step);
    }
  }
}
// ============= ЭФФЕКТЫ ===============

// ------------- конфетти --------------
#define EFF_FADE_OUT_SPEED        (25U)                         // скорость затухания
void sparklesRoutine(CRGB *leds, const char *param)
{
  for (uint8_t i = 0; i < myLamp.effects.getScale()/10+1; i++)
  {
    uint8_t x = random(0U, WIDTH);
    uint8_t y = random(0U, HEIGHT);
    if (myLamp.getPixColorXY(x, y) == 0U)
    {
      myLamp.setLeds(myLamp.getPixelNumber(x, y),CHSV(random(1U, 255U), random(127U, 255U), random(127U, 255U)));
    }
  }
  fader((uint8_t)(EFF_FADE_OUT_SPEED*((float)myLamp.effects.getSpeed())/255)+1);
}

// ------------- белый свет -------------
void whiteColorRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    FastLED.clear();

    for (uint16_t i = 0U; i < NUM_LEDS; i++)
    {
      myLamp.setLeds(i, CHSV(0U, 0U, 255U));
    }
  }
}

// ------------- белый свет (светится горизонтальная полоса по центру лампы; масштаб - высота центральной горизонтальной полосы; скорость - регулировка от холодного к тёплому; яркость - общая яркость) -------------
void whiteColorStripeRoutine(CRGB *leds, const char *param)
{
    // if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    //   return;
    // } else {
    //   myLamp.setEffDelay(millis());
    // }

    if(myLamp.effects.getScale()<127){
        uint8_t centerY = max((uint8_t)round(HEIGHT / 2.0F) - 1, 0);
        uint8_t bottomOffset = (uint8_t)(!(HEIGHT & 1) && (HEIGHT > 1));                      // если высота матрицы чётная, линий с максимальной яркостью две, а линии с минимальной яркостью снизу будут смещены на один ряд
        //Serial.printf_P(PSTR("%d %d\n"), centerY, bottomOffset);  
        for (int16_t y = centerY; y >= 0; y--)
        {
          int br = BRIGHTNESS-(13*(WIDTH-y)); if((br-(255-myLamp.getLampBrightness()))<0) br=0;
          
          CRGB color = CHSV(
            45U,                                                                              // определяем тон
            map(myLamp.effects.getSpeed(), 0U, 255U, 0U, 170U),                            // определяем насыщенность
            y == centerY                                                                                                    // определяем яркость
              ? BRIGHTNESS                                                                                                  // для центральной горизонтальной полосы (или двух) яркость всегда равна BRIGHTNESS
              : ((myLamp.effects.getScale()) / 143.0F) > ((centerY + 1.0F) - (y + 1.0F)) / (centerY + 1.0F) ? BRIGHTNESS : br);  // для остальных горизонтальных полос яркость равна либо BRIGHTNESS, либо вычисляется по br

          for (int16_t x = 0U; x < (int16_t)WIDTH; x++)
          {
            myLamp.drawPixelXY(x, y, color);                                                         // при чётной высоте матрицы максимально яркими отрисуются 2 центральных горизонтальных полосы
            myLamp.drawPixelXY(x, max((uint8_t)(HEIGHT - 1U) - (y + 1U) + bottomOffset, 0U), color); // при нечётной - одна, но дважды
          }
        }
    } else {
        uint8_t centerX = max((uint8_t)round(WIDTH / 2.0F) - 1, 0);
        uint8_t leftOffset = (uint8_t)(!(WIDTH & 1) && (WIDTH > 1));                      // если ширина матрицы чётная, линий с максимальной яркостью две, а линии с минимальной яркостью слева будут смещены на один ряд
        //Serial.printf_P(PSTR("%d %d\n"), centerX, leftOffset); 
        for (int16_t y = 0U; y < (int16_t)HEIGHT; y++)
        {
          for (int16_t x = centerX; x >= 0; x--)
          {
            int br = BRIGHTNESS-(13*(WIDTH-x)); if((br-(255-myLamp.getLampBrightness()))<0) br=0;
            
            CRGB color = CHSV(
              45U,                                                                              // определяем тон
              map(myLamp.effects.getSpeed(), 0U, 255U, 0U, 170U),                            // определяем насыщенность
              x == centerX                                                                                                    // определяем яркость
                ? BRIGHTNESS                                                                                                  // для центральной вертикальной полосы (или двух) яркость всегда равна BRIGHTNESS
                : ((255-myLamp.effects.getScale()) / 143.0F) > ((centerX + 1.0F) - (x + 1.0F)) / (centerX + 1.0F) ? BRIGHTNESS : br);  // для остальных вертикальных полос яркость равна либо BRIGHTNESS, либо вычисляется по br

            myLamp.drawPixelXY(x, y, color);                                                      // при чётной ширине матрицы максимально яркими отрисуются 2 центральных вертикальных полосы
            myLamp.drawPixelXY(max((uint8_t)(WIDTH - 1U) - (x + 1U) + leftOffset, 0U), y, color); // при нечётной - одна, но дважды
          }
        }
    }
}

// ============= водо/огне/лава/радуга/хренопад ===============
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKINGNEW 80U // 50 // 30 // 120 // 90 // 60
void fire2012WithPalette(CRGB*leds, const char *param) {
  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }


  uint8_t scale = myLamp.effects.getScale();
  uint8_t COOLINGNEW = constrain((uint16_t)(scale % 16) * 32 / HEIGHT + 16, 1, 255) ;
  // Array of temperature readings at each simulation cell
  // static byte GSHMEM.heat[WIDTH][HEIGHT];

  for (uint8_t x = 0; x < WIDTH; x++) {
    // Step 1.  Cool down every cell a little
    for (unsigned int i = 0; i < HEIGHT; i++) {
      //GSHMEM.heat[x][i] = qsub8(GSHMEM.heat[x][i], random8(0, ((COOLINGNEW * 10) / HEIGHT) + 2));
      GSHMEM.heat[x][i] = qsub8(GSHMEM.heat[x][i], random8(0, COOLINGNEW));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for (int k = HEIGHT - 1; k >= 2; k--) {
      GSHMEM.heat[x][k] = (GSHMEM.heat[x][k - 1] + GSHMEM.heat[x][k - 2] + GSHMEM.heat[x][k - 2]) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of GSHMEM.heat near the bottom
    if (random8() < SPARKINGNEW) {
      int y = random8(2);
      GSHMEM.heat[x][y] = qadd8(GSHMEM.heat[x][y], random8(160, 255));
    }

    // Step 4.  Map from GSHMEM.heat cells to LED colors
    for (unsigned int j = 0; j < HEIGHT; j++) {
      // Scale the GSHMEM.heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8(GSHMEM.heat[x][j], 240);
      if  (scale < 16) {            // Lavafall
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(LavaColors_p, colorindex));
      } else if (scale < 32) {      // Firefall
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(HeatColors_p, colorindex));
      } else if (scale < 48) {      // Waterfall
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(WaterfallColors_p, colorindex)); // PartyColors_p
      } else if (scale < 64) {      // Skyfall
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(CloudColors_p, colorindex));
      } else if (scale < 80) {      // Forestfall
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(ForestColors_p, colorindex));
      } else if (scale < 96) {      // Rainbowfall
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(RainbowColors_p, colorindex));        
      } else {                      // Aurora
        myLamp.setLeds(myLamp.getPixelNumber(x, (HEIGHT - 1) - j), ColorFromPalette(RainbowStripeColors_p, colorindex));
      }
    }
  }
}

// --------------------------- эффект пульс ----------------------
// Stefan Petrick's PULSE Effect mod by PalPalych for GyverLamp 

void drawCircle(int16_t x0, int16_t y0, uint16_t radius, const CRGB & color){
  int a = radius, b = 0;
  int radiusError = 1 - a;

  if (radius == 0) {
    myLamp.drawPixelXY(x0, y0, color);
    return;
  }

  while (a >= b)  {
    myLamp.drawPixelXY(a + x0, b + y0, color);
    myLamp.drawPixelXY(b + x0, a + y0, color);
    myLamp.drawPixelXY(-a + x0, b + y0, color);
    myLamp.drawPixelXY(-b + x0, a + y0, color);
    myLamp.drawPixelXY(-a + x0, -b + y0, color);
    myLamp.drawPixelXY(-b + x0, -a + y0, color);
    myLamp.drawPixelXY(a + x0, -b + y0, color);
    myLamp.drawPixelXY(b + x0, -a + y0, color);
    b++;
    if (radiusError < 0)
      radiusError += 2 * b + 1;
    else
    {
      a--;
      radiusError += 2 * (b - a + 1);
    }
  }
}

// uint8_t GSHMEM.pulse_hue;
// uint8_t GSHMEM.pulse_step = 0;
void pulseRoutine(CRGB *leds, const char *param) {
    if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
      return;
    } else {
      myLamp.setEffDelay(millis());
    }

  if(myLamp.isLoading()){
	GSHMEM.pulse_step = 0;
  }
  
  CRGBPalette16 palette;
  CRGB _pulse_color;
  uint8_t currentRadius = 4;
  uint8_t centerX = random8(WIDTH - 5U) + 3U;
  uint8_t centerY = random8(HEIGHT - 5U) + 3U;
  //uint16_t _rc;
  uint8_t _pulse_hue = 0;
  uint8_t _pulse_hueall = 0;
  uint8_t _pulse_delta = 0;

  palette = RainbowColors_p;
  uint8_t _scale = myLamp.effects.getScale();
  //const uint8_t limitsteps = 6U;
  //static const float fadeRate = 0.8;

  myLamp.dimAll(248U); // если эффект устанавливается с другими эффектами от Stefan Petrick, тогда  процедура должна называться dimAll (без двоечки)
  if (GSHMEM.pulse_step <= currentRadius) {
    for (uint8_t i = 0; i < GSHMEM.pulse_step; i++ ) {
      uint8_t _dark = qmul8( 2U, cos8 (128U / (GSHMEM.pulse_step + 1U) * (i + 1U))) ;
      if (_scale == 1) {            // 1 - случайные диски
        _pulse_hue = GSHMEM.pulse_hue;
        _pulse_color = CHSV(_pulse_hue, 255U, _dark);
      
      } else if (_scale <= 17) {    // 2...17 - перелив цвета дисков 
        _pulse_delta = (17U - _scale) ;
        _pulse_color = CHSV(_pulse_hueall, 255U, _dark);
     
      } else if (_scale <= 33) {    // 18...33 - выбор цвета дисков 
        _pulse_hue = (_scale - 18U) * 16U ;
        _pulse_color = CHSV(_pulse_hue, 255U, _dark);
      
      } else if (_scale <= 50) {    // 34...50 - дискоцветы
        _pulse_hue += (_scale - 33U) * 5U ;
        _pulse_color = CHSV(_pulse_hue, 255U, _dark);
      
      } else if (_scale <= 67) {    // 51...67 - пузыри цветы
        uint8_t _sat =  qsub8( 255U, cos8 (128U / (GSHMEM.pulse_step + 1U) * (i + 1U))) ;
         _pulse_hue += (68U - _scale) * 7U ;
        _pulse_color = CHSV(_pulse_hue, _sat, _dark);
      
      } else if (_scale < 83) {     // 68...83 - выбор цвета пузырей
        uint8_t _sat =  qsub8( 255U, cos8 (128U / (GSHMEM.pulse_step + 1U) * (i + 1U))) ;
        _pulse_hue = (_scale - 68U) * 16U ;
        _pulse_color = CHSV(_pulse_hue, _sat, _dark);
      
      } else if (_scale < 100) {    // 84...99 - перелив цвета пузырей
        uint8_t _sat =  qsub8( 255U, cos8 (128U / (GSHMEM.pulse_step + 1U) * (i + 1U))) ;
        _pulse_delta = (_scale - 85U)  ;
        _pulse_color = CHSV(_pulse_hueall, _sat, _dark);
      
      } else { // 100 - случайные пузыри
        uint8_t _sat =  qsub8( 255U, cos8 (128U / (GSHMEM.pulse_step + 1U) * (i + 1U))) ;
        _pulse_hue = GSHMEM.pulse_hue;
        _pulse_color = CHSV(_pulse_hue, _sat, _dark);
      }
      drawCircle(centerX, centerY, i, _pulse_color  );
    }
  } else {
    centerX = random8(WIDTH - 5U) + 3U;
    centerY = random8(HEIGHT - 5U) + 3U;
    _pulse_hueall += _pulse_delta;
    GSHMEM.pulse_hue = random8(0U, 255U);
    currentRadius = random8(3U, 9U);
    GSHMEM.pulse_step = 0;
  }
  GSHMEM.pulse_step++;
}

// радуги 2D
// ------------- радуга вертикальная/горизонтальная ----------------
//uint8_t GSHMEM.hue;
void rainbowHorVertRoutine(bool isVertical)
{
  GSHMEM.hue += 4;
  for (uint8_t i = 0U; i < (isVertical?WIDTH:HEIGHT); i++)
  {
    CHSV thisColor = CHSV((uint8_t)(GSHMEM.hue + i * myLamp.effects.getScale()%86), 255, 255); // 1/3 без центральной между 1...255, т.е.: 1...84, 170...255
    for (uint8_t j = 0U; j < (isVertical?HEIGHT:WIDTH); j++)
    {
      myLamp.drawPixelXY((isVertical?i:j), (isVertical?j:i), thisColor);
    }
  }
}

// ------------- радуга диагональная -------------
void rainbowDiagonalRoutine(CRGB *leds, const char *param)
{
  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }

  if(myLamp.effects.getScale()<85){
    rainbowHorVertRoutine(false);
    return;
  } else if (myLamp.effects.getScale()>170){
    rainbowHorVertRoutine(true);
    return;
  }

  GSHMEM.hue += 4;
  for (uint8_t i = 0U; i < WIDTH; i++)
  {
    for (uint8_t j = 0U; j < HEIGHT; j++)
    {
      float twirlFactor = 3.0F * (myLamp.effects.getScale() / 30.0F);      // на сколько оборотов будет закручена матрица, [0..3]
      CRGB thisColor = CHSV((uint8_t)(GSHMEM.hue + ((float)WIDTH / (float)HEIGHT * i + j * twirlFactor) * ((float)255 / (float)myLamp.getmaxDim())), 255, 255);
      myLamp.drawPixelXY(i, j, thisColor);
    }
  }
}

// ------------- цвета -----------------
void colorsRoutine(CRGB *leds, const char *param)
{
  static unsigned int step = 0; // доп. задержка
  unsigned int delay = (myLamp.effects.getSpeed()==1)?4294967294:255-myLamp.effects.getSpeed(); // на скорости 1 будет очень долгое ожидание)))
  
  if (myLamp.isLoading()){ // начальная установка цвета
    GSHMEM.hue = myLamp.effects.getScale();
  } else {
    step=(step+1)%(delay+1);
    if(step!=delay) {
      myLamp.fillAll(CHSV(GSHMEM.hue, 255U, 255U)); // еще не наступила смена цвета, поэтому выводим текущий
    }
    else {  
      GSHMEM.hue += myLamp.effects.getScale(); // смещаемся на следущий
      myLamp.fillAll(CHSV(GSHMEM.hue, 255U, 255U)); // и выводим
    }
  }
}

// ------------- матрица ---------------
void matrixRoutine(CRGB *leds, const char *param)
{
  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }

  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // обрабатываем нашу матрицу снизу вверх до второй сверху строчки
    for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
    {
      uint32_t thisColor = myLamp.getPixColorXY(x, y);                                              // берём цвет нашего пикселя
      uint32_t upperColor = myLamp.getPixColorXY(x, y + 1U);                                        // берём цвет пикселя над нашим
      if (upperColor >= 0x900000 && random(7 * HEIGHT) != 0U)                  // если выше нас максимальная яркость, игнорим этот факт с некой вероятностью или опускаем цепочку ниже
        myLamp.drawPixelXY(x, y, upperColor);
      else if (thisColor == 0U && random((255 - myLamp.effects.getScale()) * HEIGHT) == 0U)  // если наш пиксель ещё не горит, иногда зажигаем новые цепочки
      //else if (thisColor == 0U && random((255 - myLamp.effects.getScale()) * HEIGHT*3) == 0U)  // для длинных хвостов
        myLamp.drawPixelXY(x, y, 0x9bf800);
      else if (thisColor <= 0x050800)                                                        // если наш пиксель почти погас, стараемся сделать затухание медленней
      {
        if (thisColor >= 0x030000)
          myLamp.drawPixelXY(x, y, 0x020300);
        else if (thisColor != 0U)
          myLamp.drawPixelXY(x, y, 0U);
      }
      else if (thisColor >= 0x900000)                                                        // если наш пиксель максимальной яркости, резко снижаем яркость
        myLamp.drawPixelXY(x, y, 0x558800);
      else 
        myLamp.drawPixelXY(x, y, thisColor - 0x0a1000);                                             // в остальных случаях снижаем яркость на 1 уровень
        //myLamp.drawPixelXY(x, y, thisColor - 0x050800);                                             // для длинных хвостов
    }
    // аналогично обрабатываем верхний ряд пикселей матрицы
    uint32_t thisColor = myLamp.getPixColorXY(x, HEIGHT - 1U);
    if (thisColor == 0U)                                                                     // если наш верхний пиксель не горит, заполняем его с вероятностью .Scale
    {
      if (random(255 - myLamp.effects.getScale()) == 0U)
        myLamp.drawPixelXY(x, HEIGHT - 1U, 0x9bf800);
    }  
    else if (thisColor <= 0x050800)                                                          // если наш верхний пиксель почти погас, стараемся сделать затухание медленней
    {
      if (thisColor >= 0x030000)
        myLamp.drawPixelXY(x, HEIGHT - 1U, 0x020300);
      else
        myLamp.drawPixelXY(x, HEIGHT - 1U, 0U);
    }
    else if (thisColor >= 0x900000)                                                          // если наш верхний пиксель максимальной яркости, резко снижаем яркость
      myLamp.drawPixelXY(x, HEIGHT - 1U, 0x558800);
    else 
      myLamp.drawPixelXY(x, HEIGHT - 1U, thisColor - 0x0a1000);                                     // в остальных случаях снижаем яркость на 1 уровень
      //myLamp.drawPixelXY(x, HEIGHT - 1U, thisColor - 0x050800);                                     // для длинных хвостов
  }
}

// ------------- снегопад ----------
void snowRoutine(CRGB *leds, const char *param)
{
  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }

  // сдвигаем всё вниз
  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    for (uint8_t y = 0U; y < HEIGHT - 1; y++)
    {
      myLamp.drawPixelXY(x, y, myLamp.getPixColorXY(x, y + 1U));
    }
  }

  for (uint8_t x = 0U; x < WIDTH; x++)
  {
    // заполняем случайно верхнюю строку
    // а также не даём двум блокам по вертикали вместе быть
    if (myLamp.getPixColorXY(x, HEIGHT - 2U) == 0U && (random(0, 255 - myLamp.effects.getScale()) == 0U))
      myLamp.drawPixelXY(x, HEIGHT - 1U, 0xE0FFFF - 0x101010 * random(0, 4));
    else
      myLamp.drawPixelXY(x, HEIGHT - 1U, 0x000000);
  }
}

// ------------- метель -------------
#define SNOW_DENSE            (60U)                         // плотность снега
#define SNOW_TAIL_STEP        (100U)                        // длина хвоста
#define SNOW_SATURATION       (0U)                          // насыщенность (от 0 до 255)
// ------------- звездопад -------------
#define STAR_DENSE            (60U)                         // плотность комет
#define STAR_TAIL_STEP        (100U)                        // длина хвоста кометы
#define STAR_SATURATION       (150U)                        // насыщенность кометы (от 0 до 255)

// ------------- звездопад/метель -------------
void snowStormStarfallRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading()){
    FastLED.clear();
  }
  
  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }

  // заполняем головами комет левую и верхнюю линию
  for (uint8_t i = HEIGHT / 2U; i < HEIGHT; i++)
  {
    if (myLamp.getPixColorXY(0U, i) == 0U &&
       (random(0, (myLamp.effects.getScale()<127?SNOW_DENSE:STAR_DENSE)) == 0) &&
        myLamp.getPixColorXY(0U, i + 1U) == 0U &&
        myLamp.getPixColorXY(0U, i - 1U) == 0U)
    {
      myLamp.setLeds(myLamp.getPixelNumber(0U, i), CHSV(random(0, 200), (myLamp.effects.getScale()<127?SNOW_SATURATION:STAR_SATURATION), 255U));
    }
  }
  
  for (uint8_t i = 0U; i < WIDTH / 2U; i++)
  {
    if (myLamp.getPixColorXY(i, HEIGHT - 1U) == 0U &&
       (random(0, map((myLamp.effects.getScale()%128)*2, 0U, 255U, 10U, 120U)) == 0U) &&
        myLamp.getPixColorXY(i + 1U, HEIGHT - 1U) == 0U &&
        myLamp.getPixColorXY(i - 1U, HEIGHT - 1U) == 0U)
    {
      myLamp.setLeds(myLamp.getPixelNumber(i, HEIGHT - 1U), CHSV(random(0, 200),  (myLamp.effects.getScale()<127?SNOW_SATURATION:STAR_SATURATION), 255U));
    }
  }

  // сдвигаем по диагонали
  for (uint8_t y = 0U; y < HEIGHT - 1U; y++)
  {
    for (uint8_t x = WIDTH - 1U; x > 0U; x--)
    {
      myLamp.drawPixelXY(x, y, myLamp.getPixColorXY(x - 1U, y + 1U));
    }
  }

  // уменьшаем яркость левой и верхней линии, формируем "хвосты"
  for (uint8_t i = HEIGHT / 2U; i < HEIGHT; i++)
  {
    fadePixel(0U, i,  (myLamp.effects.getScale()<127?SNOW_TAIL_STEP:STAR_TAIL_STEP));
  }
  for (uint8_t i = 0U; i < WIDTH / 2U; i++)
  {
    fadePixel(i, HEIGHT - 1U, (myLamp.effects.getScale()<127?SNOW_TAIL_STEP:STAR_TAIL_STEP));
  }
}

// ------------- светлячки --------------
//#define LIGHTERS_AM           (100U)
void lightersRoutine(CRGB *leds, const char *param)
{
  // static int32_t GSHMEM.lightersPos[2U][LIGHTERS_AM];
  // static int8_t GSHMEM.lightersSpeed[2U][LIGHTERS_AM];
  // static uint16_t GSHMEM.lightersColor[LIGHTERS_AM];
  // static uint8_t GSHMEM.loopCounter;
  //int32_t angle[LIGHTERS_AM];
  //int32_t speedV[LIGHTERS_AM];
  //int8_t angleSpeed[LIGHTERS_AM];

  if (myLamp.isLoading())
  {
    randomSeed(millis());
	GSHMEM.loopCounter = 0U;
    for (uint8_t i = 0U; i < LIGHTERS_AM; i++)
    {
      GSHMEM.lightersPos[0U][i] = random(0, WIDTH * 10);
      GSHMEM.lightersPos[1U][i] = random(0, HEIGHT * 10);
      GSHMEM.lightersSpeed[0U][i] = random(-10, 10);
      GSHMEM.lightersSpeed[1U][i] = random(-10, 10);
      GSHMEM.lightersColor[i] = random(0U, 255U);
    }
  }

  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }

  FastLED.clear();
  if (++GSHMEM.loopCounter > 20U) GSHMEM.loopCounter = 0U;
  for (uint8_t i = 0U; i < (uint8_t)((LIGHTERS_AM/255.0)*myLamp.effects.getScale())+1; i++) // масштабируем на LIGHTERS_AM, чтобі не было выхода за диапазон
  {
    if (GSHMEM.loopCounter == 0U)                                  // меняем скорость каждые 255 отрисовок
    {
      GSHMEM.lightersSpeed[0U][i] += random(-3, 4);
      GSHMEM.lightersSpeed[1U][i] += random(-3, 4);
      GSHMEM.lightersSpeed[0U][i] = constrain(GSHMEM.lightersSpeed[0U][i], -20, 20);
      GSHMEM.lightersSpeed[1U][i] = constrain(GSHMEM.lightersSpeed[1U][i], -20, 20);
    }

    GSHMEM.lightersPos[0U][i] += GSHMEM.lightersSpeed[0U][i];
    GSHMEM.lightersPos[1U][i] += GSHMEM.lightersSpeed[1U][i];

    if (GSHMEM.lightersPos[0U][i] < 0) GSHMEM.lightersPos[0U][i] = (WIDTH - 1) * 10;
    if (GSHMEM.lightersPos[0U][i] >= (int32_t)(WIDTH * 10)) GSHMEM.lightersPos[0U][i] = 0;

    if (GSHMEM.lightersPos[1U][i] < 0)
    {
      GSHMEM.lightersPos[1U][i] = 0;
      GSHMEM.lightersSpeed[1U][i] = -GSHMEM.lightersSpeed[1U][i];
    }
    if (GSHMEM.lightersPos[1U][i] >= (int32_t)(HEIGHT - 1) * 10)
    {
      GSHMEM.lightersPos[1U][i] = (HEIGHT - 1U) * 10;
      GSHMEM.lightersSpeed[1U][i] = -GSHMEM.lightersSpeed[1U][i];
    }
    myLamp.drawPixelXY(GSHMEM.lightersPos[0U][i] / 10, GSHMEM.lightersPos[1U][i] / 10, CHSV(GSHMEM.lightersColor[i], 255U, 255U));
  }
}

// ------------- светлячки со шлейфом -------------
//#define BALLS_AMOUNT          (7U)                          // максимальное количество "шариков"
#define CLEAR_PATH            (1U)                          // очищать путь
#define BALL_TRACK            (1U)                          // (0 / 1) - вкл/выкл следы шариков
#define TRACK_STEP            (70U)                         // длина хвоста шарика (чем больше цифра, тем хвост короче)
void ballsRoutine(CRGB *leds, const char *param)
{
  // static int16_t GSHMEM.coord[BALLS_AMOUNT][2U];
  // static int8_t GSHMEM.vector[BALLS_AMOUNT][2U];
  int16_t ballColors[BALLS_AMOUNT];

  if (myLamp.isLoading())
  {
    for (uint8_t j = 0U; j < BALLS_AMOUNT; j++)
    {
      int8_t sign;
      // забиваем случайными данными
      GSHMEM.coord[j][0U] = WIDTH / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      GSHMEM.vector[j][0U] = random(4, 15) * sign;
      GSHMEM.coord[j][1U] = HEIGHT / 2 * 10;
      random(0, 2) ? sign = 1 : sign = -1;
      GSHMEM.vector[j][1U] = random(4, 15) * sign;
      //ballColors[j] = random(0, 9) * 28;
    }
  }

  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) < (unsigned)(255-myLamp.effects.getSpeed())){
    return;
  } else {
    myLamp.setEffDelay(millis());
  }

  if (!BALL_TRACK)                                          // режим без следов шариков
  {
    FastLED.clear();
  }
  else                                                      // режим со следами
  {
    fader(TRACK_STEP);
  }

  // движение шариков
  int maxBalls = (uint8_t)((BALLS_AMOUNT/255.0)*myLamp.effects.getScale()+0.99);
  for (uint8_t j = 0U; j < maxBalls; j++)
  {
    // цвет зависит от масштаба
    ballColors[j] = myLamp.effects.getScale() * (maxBalls-j) * BALLS_AMOUNT + j;
          
    // движение шариков
    for (uint8_t i = 0U; i < 2U; i++)
    {
      GSHMEM.coord[j][i] += GSHMEM.vector[j][i];
      if (GSHMEM.coord[j][i] < 0)
      {
        GSHMEM.coord[j][i] = 0;
        GSHMEM.vector[j][i] = -GSHMEM.vector[j][i];
      }
    }

    if (GSHMEM.coord[j][0U] > (int16_t)((WIDTH - 1) * 10))
    {
      GSHMEM.coord[j][0U] = (WIDTH - 1) * 10;
      GSHMEM.vector[j][0U] = -GSHMEM.vector[j][0U];
    }
    if (GSHMEM.coord[j][1U] > (int16_t)((HEIGHT - 1) * 10))
    {
      GSHMEM.coord[j][1U] = (HEIGHT - 1) * 10;
      GSHMEM.vector[j][1U] = -GSHMEM.vector[j][1U];
    }
    myLamp.setLeds(myLamp.getPixelNumber(GSHMEM.coord[j][0U] / 10, GSHMEM.coord[j][1U] / 10), CHSV(ballColors[j], 255U, 255U));
  }
}

// ------------- пейнтбол -------------
#define BORDERTHICKNESS       (1U)                          // глубина бордюра для размытия яркой частицы: 0U - без границы (резкие края); 1U - 1 пиксель (среднее размытие) ; 2U - 2 пикселя (глубокое размытие)
void lightBallsRoutine(CRGB *leds, const char *param)
{
  const uint8_t paintWidth = WIDTH - BORDERTHICKNESS * 2;
  const uint8_t paintHeight = HEIGHT - BORDERTHICKNESS * 2;

  // Apply some blurring to whatever's already on the matrix
  // Note that we never actually clear the matrix, we just constantly
  // blur it repeatedly.  Since the blurring is 'lossy', there's
  // an automatic trend toward black -- by design.
  uint8_t blurAmount = dim8_raw(beatsin8(3,64,100));
  blur2d(leds, WIDTH, HEIGHT, blurAmount);

  float speedScale = (((float)myLamp.effects.getSpeed())/255)+(5.0/255.0);

  // Use two out-of-sync sine waves
  uint16_t  i = beatsin16( 79*speedScale, 0, 255); //91
  uint16_t  j = beatsin16( 67*speedScale, 0, 255); //109
  uint16_t  k = beatsin16( 53*speedScale, 0, 255); //73
  uint16_t  m = beatsin16( 97*speedScale, 0, 255); //123

  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis() / (myLamp.effects.getScale()/16 + 1);
  leds[myLamp.getPixelNumber( highByte(i * paintWidth) + BORDERTHICKNESS, highByte(j * paintHeight) + BORDERTHICKNESS)] += CHSV( ms / 29, 200U, 255U);
  leds[myLamp.getPixelNumber( highByte(j * paintWidth) + BORDERTHICKNESS, highByte(k * paintHeight) + BORDERTHICKNESS)] += CHSV( ms / 41, 200U, 255U);
  leds[myLamp.getPixelNumber( highByte(k * paintWidth) + BORDERTHICKNESS, highByte(m * paintHeight) + BORDERTHICKNESS)] += CHSV( ms / 37, 200U, 255U);
  leds[myLamp.getPixelNumber( highByte(m * paintWidth) + BORDERTHICKNESS, highByte(i * paintHeight) + BORDERTHICKNESS)] += CHSV( ms / 53, 200U, 255U);
}

// ------------- блуждающий кубик -------------
#define RANDOM_COLOR          (1U)                          // случайный цвет при отскоке
void ballRoutine(CRGB *leds, const char *param)
{
  // static float GSHMEM.coordB[2U];
  // static int8_t GSHMEM.vectorB[2U];
  // static int16_t GSHMEM.ballColor;
  int8_t ballSize;

  ballSize = map(myLamp.effects.getScale(), 0U, 255U, 2U, max((uint8_t)min(WIDTH,HEIGHT) / 3, 2));

  if (myLamp.isLoading())
  {
    //FastLED.clear();
    myLamp.setEffDelay(millis());
    for (uint8_t i = 0U; i < 2U; i++)
    {
      GSHMEM.coordB[i] = i? (WIDTH - ballSize) / 2 : (HEIGHT - ballSize) / 2;
      GSHMEM.vectorB[i] = random(8, 24) - 12;
      GSHMEM.ballColor = random(1, 255) * myLamp.effects.getScale();
    }
  }

  if((millis() - myLamp.getEffDelay() - EFFECTS_RUN_TIMER) > 10000){ // каждые 10 секунд коррекция направления
    myLamp.setEffDelay(millis());
    for (uint8_t i = 0U; i < 2U; i++)
    {
      if(abs(GSHMEM.vectorB[i])<12)
        GSHMEM.vectorB[i] += (random(0, 8) - 4);
      else if (GSHMEM.vectorB[i]>12)
        GSHMEM.vectorB[i] -= random(1, 6);
      else
        GSHMEM.vectorB[i] += random(1, 6);
      if(!GSHMEM.vectorB[i]) GSHMEM.vectorB[i]++;
      GSHMEM.ballColor = random(1, 255) * myLamp.effects.getScale();
    }
  }

  for (uint8_t i = 0U; i < 2U; i++)
  {
    GSHMEM.coordB[i] += GSHMEM.vectorB[i]*((0.1*myLamp.effects.getSpeed())/255.0);
    if ((int8_t)GSHMEM.coordB[i] < 0)
    {
      GSHMEM.coordB[i] = 0;
      GSHMEM.vectorB[i] = -GSHMEM.vectorB[i];
      if (RANDOM_COLOR) GSHMEM.ballColor = random(1, 255) * myLamp.effects.getScale();
    }
  }
  if ((int8_t)GSHMEM.coordB[0U] > (int16_t)(WIDTH - ballSize))
  {
    GSHMEM.coordB[0U] = (WIDTH - ballSize);
    GSHMEM.vectorB[0U] = -GSHMEM.vectorB[0U];
    if (RANDOM_COLOR) GSHMEM.ballColor = random(1, 255) * myLamp.effects.getScale();
  }
  if ((int8_t)GSHMEM.coordB[1U] > (int16_t)(HEIGHT - ballSize))
  {
    GSHMEM.coordB[1U] = (HEIGHT - ballSize);
    GSHMEM.vectorB[1U] = -GSHMEM.vectorB[1U];
    if (RANDOM_COLOR) GSHMEM.ballColor = random(1, 255) * myLamp.effects.getScale();
  }

  //FastLED.clear();
  fader((uint8_t)(10*((float)myLamp.effects.getSpeed())/255)+50); // фейдер, 10 - коэф. масштабирование (вся шкала 0...10), 50 - смещение (мин. уровень фейдера)
  for (uint8_t i = 0U; i < ballSize; i++)
  {
    for (uint8_t j = 0U; j < ballSize; j++)
    {
      myLamp.setLeds(myLamp.getPixelNumber((int8_t)GSHMEM.coordB[0U] + i, (int8_t)GSHMEM.coordB[1U] + j), CHSV(GSHMEM.ballColor, 255U, 255U));
    }
  }
}

// Trivial XY function for the SmartMatrix; use a different XY
// function for different matrix grids. See XYMatrix example for code.
uint16_t XY(uint8_t x, uint8_t y)
{
  uint16_t i;
  if (y & 0x01)
  {
    // Odd rows run backwards
    uint8_t reverseX = (WIDTH - 1) - x;
    i = (y * WIDTH) + reverseX;
  }
  else
  {
    // Even rows run forwards
    i = (y * WIDTH) + x;
  }
  return i%(WIDTH*HEIGHT+1);
}

//-- 3D Noise эффектцы --------------
// uint16_t GSHMEM.speed = 20;                                        // speed is set dynamically once we've started up
// uint16_t GSHMEM.scale = 30;                                        // scale is set dynamically once we've started up
// uint8_t GSHMEM.ihue = 0;
// static uint16_t GSHMEM.x;
// static uint16_t GSHMEM.y;
// static uint16_t GSHMEM.z;
// #if (WIDTH > HEIGHT)
// uint8_t GSHMEM.noise[WIDTH][WIDTH];
// #else
// uint8_t GSHMEM.noise[HEIGHT][HEIGHT];
// #endif
// CRGBPalette16 GSHMEM.currentPalette(PartyColors_p);
// uint8_t GSHMEM.colorLoop = 1;

// ************* СЛУЖЕБНЫЕ *************
void fillNoiseLED()
{
  uint8_t dataSmoothing = 0;
  if (GSHMEM.speed < 50)
  {
    dataSmoothing = 200 - (GSHMEM.speed * 4);
  }
  for (uint8_t i = 0; i < myLamp.getmaxDim(); i++)
  {
    int32_t ioffset = GSHMEM.scale * i;
    for (uint8_t j = 0; j < myLamp.getmaxDim(); j++)
    {
      int32_t joffset = GSHMEM.scale * j;

      uint8_t data = inoise8(GSHMEM.x + ioffset, GSHMEM.y + joffset, GSHMEM.z);

      data = qsub8(data, 16);
      data = qadd8(data, scale8(data, 39));

      if (dataSmoothing)
      {
        uint8_t olddata = GSHMEM.noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }

      GSHMEM.noise[i][j] = data;
    }
  }
  GSHMEM.z += GSHMEM.speed;

  // apply slow drift to X and Y, just for visual variation.
  GSHMEM.x += GSHMEM.speed / 8;
  GSHMEM.y -= GSHMEM.speed / 16;

  for (uint8_t i = 0; i < WIDTH; i++)
  {
    for (uint8_t j = 0; j < HEIGHT; j++)
    {
      uint8_t index = GSHMEM.noise[j][i];
      uint8_t bri =   GSHMEM.noise[i][j];
      // if this palette is a 'loop', add a slowly-changing base value
      if ( GSHMEM.colorLoop)
      {
        index += GSHMEM.ihue;
      }
      // brighten up, as the color palette itself often contains the
      // light/dark dynamic range desired
      if ( bri > 127 )
      {
        bri = 255;
      }
      else
      {
        bri = dim8_raw( bri * 2);
      }
      CRGB color = ColorFromPalette( GSHMEM.currentPalette, index, bri);      
      myLamp.drawPixelXY(i, j, color);                             //leds[getPixelNumber(i, j)] = color;
    }
  }
  GSHMEM.ihue += 1;
}

void fillnoise8()
{
  for (uint8_t i = 0; i < myLamp.getmaxDim(); i++)
  {
    int32_t ioffset = GSHMEM.scale * i;
    for (uint8_t j = 0; j < myLamp.getmaxDim(); j++)
    {
      int32_t joffset = GSHMEM.scale * j;
      GSHMEM.noise[i][j] = inoise8(GSHMEM.x + ioffset, GSHMEM.y + joffset, GSHMEM.z);
    }
  }
  GSHMEM.z += GSHMEM.speed;
}

void madnessNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
  }
  GSHMEM.scale = 127UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillnoise8();
  for (uint8_t i = 0; i < WIDTH; i++)
  {
    for (uint8_t j = 0; j < HEIGHT; j++)
    {
      CRGB thisColor = CHSV(GSHMEM.noise[j][i], 255, GSHMEM.noise[i][j]);
      myLamp.drawPixelXY(i, j, thisColor);                         //leds[getPixelNumber(i, j)] = CHSV(GSHMEM.noise[j][i], 255, GSHMEM.noise[i][j]);
    }
  }
  GSHMEM.ihue += 1;
}

void rainbowNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.colorLoop = 1;
  }
  GSHMEM.scale = 127UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void rainbowStripeNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.currentPalette = RainbowStripeColors_p;
    GSHMEM.colorLoop = 1;
  }
  GSHMEM.scale = 64UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void zebraNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    // 'black out' all 16 palette entries...
    fill_solid(GSHMEM.currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    GSHMEM.currentPalette[0] = CRGB::White;
    GSHMEM.currentPalette[4] = CRGB::White;
    GSHMEM.currentPalette[8] = CRGB::White;
    GSHMEM.currentPalette[12] = CRGB::White;
    GSHMEM.colorLoop = 1;
  }
  GSHMEM.scale = 64UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void forestNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.currentPalette = ForestColors_p;
    GSHMEM.colorLoop = 0;
  }
  GSHMEM.scale = 96UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void oceanNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.currentPalette = OceanColors_p;
    GSHMEM.colorLoop = 0;
  }
  GSHMEM.scale = 127UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void plasmaNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.currentPalette = PartyColors_p;
    GSHMEM.colorLoop = 1;
  }
  GSHMEM.scale = 64UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void cloudsNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.currentPalette = CloudColors_p;
    GSHMEM.colorLoop = 0;
  }
  GSHMEM.scale = 64UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

void lavaNoiseRoutine(CRGB *leds, const char *param)
{
  if (myLamp.isLoading())
  {
    GSHMEM.currentPalette = LavaColors_p;
    GSHMEM.colorLoop = 0;
  }
  GSHMEM.scale = 64UL*myLamp.effects.getScale()/255;
  GSHMEM.speed = 64UL*myLamp.effects.getSpeed()/255;
  fillNoiseLED();
}

// --------------------------- эффект мячики ----------------------
//  BouncingBalls2014 is a program that lets you animate an LED strip
//  to look like a group of bouncing balls
//  Daniel Wilson, 2014
//  https://github.com/githubcdr/Arduino/blob/master/bouncingballs/bouncingballs.ino
//  With BIG thanks to the FastLED community!
//  адаптация от SottNick
#define bballsGRAVITY           (-9.81)              // Downward (negative) acceleration of gravity in m/s^2
#define bballsH0                (1)                  // Starting height, in meters, of the ball (strip length)
//#define bballsMaxNUM_BALLS      (16U)                // максимальное количество мячиков прикручено при адаптации для бегунка Масштаб
void BBallsRoutine(CRGB *leds, const char *param)
{
  uint8_t bballsNUM_BALLS;                             // Number of bouncing balls you want (recommend < 7, but 20 is fun in its own way) ... количество мячиков теперь задаётся бегунком, а не константой

  // static uint8_t GSHMEM.bballsCOLOR[bballsMaxNUM_BALLS] ;                   // прикручено при адаптации для разноцветных мячиков
  // static uint8_t GSHMEM.bballsX[bballsMaxNUM_BALLS] ;                       // прикручено при адаптации для распределения мячиков по радиусу лампы
  // static int   GSHMEM.bballsPos[bballsMaxNUM_BALLS] ;                       // The integer position of the dot on the strip (LED index)
  // static float GSHMEM.bballsH[bballsMaxNUM_BALLS] ;                         // An array of heights
  // static float GSHMEM.bballsVImpact[bballsMaxNUM_BALLS] ;                   // As time goes on the impact velocity will change, so make an array to store those values
  // static float GSHMEM.bballsTCycle[bballsMaxNUM_BALLS] ;                    // The time since the last time the ball struck the ground
  // static float GSHMEM.bballsCOR[bballsMaxNUM_BALLS] ;                       // Coefficient of Restitution (bounce damping)
  // static long  GSHMEM.bballsTLast[bballsMaxNUM_BALLS] ;                     // The clock time of the last ground strike
  float bballsVImpact0 = sqrt( -2 * bballsGRAVITY * bballsH0 );      // Impact velocity of the ball when it hits the ground if "dropped" from the top of the strip

  bballsNUM_BALLS = (uint8_t)((bballsMaxNUM_BALLS * myLamp.effects.getScale())/256+1);
  for (int i = 0 ; i < bballsNUM_BALLS ; i++)
    GSHMEM.bballsCOR[i] = 0.90 - float(i)/(255-myLamp.effects.getSpeed()); // это, видимо, прыгучесть. для каждого мячика уникальная изначально

  if (myLamp.isLoading()){
    FastLED.clear();

    for (int i = 0 ; i < bballsNUM_BALLS ; i++) {          // Initialize variables
      GSHMEM.bballsCOLOR[i] = random(0U, 256U);
      GSHMEM.bballsX[i] = random(0U, WIDTH);
      GSHMEM.bballsTLast[i] = millis();
      GSHMEM.bballsH[i] = bballsH0;
      GSHMEM.bballsPos[i] = 0;                                    // Balls start on the ground
      GSHMEM.bballsVImpact[i] = bballsVImpact0;                   // And "pop" up at vImpact0
      GSHMEM.bballsTCycle[i] = 0;
    }
  }
  

  for (int i = 0 ; i < bballsNUM_BALLS ; i++) {
    myLamp.setLeds(myLamp.getPixelNumber(GSHMEM.bballsX[i], GSHMEM.bballsPos[i]), CRGB::Black); // off for the next loop around

    GSHMEM.bballsTCycle[i] =  millis() - GSHMEM.bballsTLast[i] ;     // Calculate the time since the last time the ball was on the ground

    // A little kinematics equation calculates positon as a function of time, acceleration (gravity) and intial velocity
    GSHMEM.bballsH[i] = 0.5 * bballsGRAVITY * pow( GSHMEM.bballsTCycle[i]/1000 , 2.0 ) + GSHMEM.bballsVImpact[i] * GSHMEM.bballsTCycle[i]/1000;

    if ( GSHMEM.bballsH[i] < 0 ) {                      
      GSHMEM.bballsH[i] = 0;                            // If the ball crossed the threshold of the "ground," put it back on the ground
      GSHMEM.bballsVImpact[i] = GSHMEM.bballsCOR[i] * GSHMEM.bballsVImpact[i] ;   // and recalculate its new upward velocity as it's old velocity * COR
      GSHMEM.bballsTLast[i] = millis();

//      if ( GSHMEM.bballsVImpact[i] < 0.01 ) GSHMEM.bballsVImpact[i] = GSHMEM.bballsVImpact0;  // If the ball is barely moving, "pop" it back up at vImpact0
      if ( GSHMEM.bballsVImpact[i] < 0.01 ) // сделал, чтобы мячики меняли свою прыгучесть и положение каждый цикл
        {
          switch (random(3U)) // этот свитч двигает мячики влево-вправо иногда
            {
              case 0U:
              {
                if (GSHMEM.bballsX[i] == 0U) GSHMEM.bballsX[i] = WIDTH - 1U;
                  else --GSHMEM.bballsX[i];
                break;
              }
              case 2U:
              {
                if (GSHMEM.bballsX[i] == WIDTH - 1U) GSHMEM.bballsX[i] = 0U;
                  else ++GSHMEM.bballsX[i];
                break;
              }
            }
          GSHMEM.bballsCOR[i] = 0.90 - float(random(0U,5U))/pow(random(1U,6U),2); // а это прыгучесть меняется. кажется, не очень удачно сделано
          GSHMEM.bballsVImpact[i] = bballsVImpact0;  // If the ball is barely moving, "pop" it back up at vImpact0
        }
    }
    GSHMEM.bballsPos[i] = round( GSHMEM.bballsH[i] * (HEIGHT - 1) / bballsH0);       // Map "h" to a "pos" integer index position on the LED strip

    myLamp.setLeds(myLamp.getPixelNumber(GSHMEM.bballsX[i], GSHMEM.bballsPos[i]), CHSV(GSHMEM.bballsCOLOR[i], 255, 255));
  }
}

// ***** SINUSOID3 / СИНУСОИД3 *****
// v1.7.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
/*
  Sinusoid3 by Stefan Petrick (mod by Palpalych for GyverLamp 27/02/2020)
  read more about the concept: https://www.youtube.com/watch?v=mubH-w_gwdA
*/
void Sinusoid3Routine (CRGB *leds, const char *param)
{
  const uint8_t semiHeightMajor =  HEIGHT / 2 + (HEIGHT % 2);
  const uint8_t semiWidthMajor =  WIDTH / 2  + (WIDTH % 2) ;
  float e_s3_speed = 0.004 * myLamp.effects.getSpeed() + 0.015; // speed of the movement along the Lissajous curves
  float e_s3_size = 3 * (float)myLamp.effects.getScale()/255.0 + 2;    // amplitude of the curves

  float time_shift = float(millis()%(uint32_t)(30000*(1.0/((float)myLamp.effects.getSpeed()/255))));

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {
      CRGB color;

      float cx = y + float(e_s3_size * (sinf (float(e_s3_speed * 0.003 * time_shift)))) - semiHeightMajor;  // the 8 centers the middle on a 16x16
      float cy = x + float(e_s3_size * (cosf (float(e_s3_speed * 0.0022 * time_shift)))) - semiWidthMajor;
      float v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
      color.r = v;

      cx = x + float(e_s3_size * (sinf (e_s3_speed * float(0.0021 * time_shift)))) - semiWidthMajor;
      cy = y + float(e_s3_size * (cosf (e_s3_speed * float(0.002 * time_shift)))) - semiHeightMajor;
      v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
      color.b = v;

      cx = x + float(e_s3_size * (sinf (e_s3_speed * float(0.0041 * time_shift)))) - semiWidthMajor;
      cy = y + float(e_s3_size * (cosf (e_s3_speed * float(0.0052 * time_shift)))) - semiHeightMajor;
      v = 127 * (1 + sinf ( sqrtf ( ((cx * cx) + (cy * cy)) ) ));
      color.g = v;
      myLamp.drawPixelXY(x, y, color);
    }
  }
}

/*
  Metaballs proof of concept by Stefan Petrick (mod by Palpalych for GyverLamp 27/02/2020)
  ...very rough 8bit math here...
  read more about the concept of isosurfaces and metaballs:
  https://www.gamedev.net/articles/programming/graphics/exploring-metaballs-and-isosurfaces-in-2d-r2556
*/

// ***** METABALLS / МЕТАШАРИКИ *****
// v1.7.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
void metaBallsRoutine(CRGB *leds, const char *param)
{
  float speed = myLamp.effects.getSpeed()/127.0;

  // get some 2 random moving points
  uint8_t x2 = inoise8(millis() * speed, 25355, 685 ) / WIDTH;
  uint8_t y2 = inoise8(millis() * speed, 355, 11685 ) / HEIGHT;

  uint8_t x3 = inoise8(millis() * speed, 55355, 6685 ) / WIDTH;
  uint8_t y3 = inoise8(millis() * speed, 25355, 22685 ) / HEIGHT;

  // and one Lissajou function
  uint8_t x1 = beatsin8(23 * speed, 0, 15);
  uint8_t y1 = beatsin8(28 * speed, 0, 15);

  for (uint8_t y = 0; y < HEIGHT; y++) {
    for (uint8_t x = 0; x < WIDTH; x++) {

      // calculate distances of the 3 points from actual pixel
      // and add them together with weightening
      uint8_t  dx =  abs(x - x1);
      uint8_t  dy =  abs(y - y1);
      uint8_t dist = 2 * sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x2);
      dy =  abs(y - y2);
      dist += sqrt((dx * dx) + (dy * dy));

      dx =  abs(x - x3);
      dy =  abs(y - y3);
      dist += sqrt((dx * dx) + (dy * dy));

      // inverse result
      byte color = myLamp.effects.getScale()*4 / dist;

      // map color between thresholds
      if (color > 0 and color < 60) {
        myLamp.drawPixelXY(x, y, CHSV(color * 9, 255, 255));
      } else {
        myLamp.drawPixelXY(x, y, CHSV(0, 255, 255));
      }
      // show the 3 points, too
      myLamp.drawPixelXY(x1, y1, CRGB(255, 255, 255));
      myLamp.drawPixelXY(x2, y2, CRGB(255, 255, 255));
      myLamp.drawPixelXY(x3, y3, CRGB(255, 255, 255));
    }
  }
}

// --------------------------- эффект спирали ----------------------
/*
 * Aurora: https://github.com/pixelmatix/aurora
 * https://github.com/pixelmatix/aurora/blob/sm3.0-64x64/PatternSpiro.h
 * Copyright (c) 2014 Jason Coon
 * Неполная адаптация SottNick
 */
uint8_t mapsin8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatsin = sin8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatsin, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

uint8_t mapcos8(uint8_t theta, uint8_t lowest = 0, uint8_t highest = 255) {
  uint8_t beatcos = cos8(theta);
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8(beatcos, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

void spiroRoutine(CRGB *leds, const char *param)
{
    //static float GSHMEM.spirotheta1 = 0;
    //static float GSHMEM.spirotheta2 = 0;
    //static byte GSHMEM.spirohueoffset = 0;
    //static uint8_t GSHMEM.spirocount = 1;
    //static boolean GSHMEM.spiroincrement = false;
    //static boolean GSHMEM.spirohandledChange = false;
    if(myLamp.isLoading()){
      GSHMEM.spirotheta1 = 0;
      GSHMEM.spirotheta2 = 0;
      GSHMEM.spirohueoffset = 0;
      GSHMEM.spirocount = 1;
      GSHMEM.spiroincrement = false;
      GSHMEM.spirohandledChange = false;
    }

    const uint8_t spiroradiusx = WIDTH / 4;
    const uint8_t spiroradiusy = HEIGHT / 4;
    
    const uint8_t spirocenterX = WIDTH / 2;
    const uint8_t spirocenterY = HEIGHT / 2;
    
    const uint8_t spirominx = spirocenterX - spiroradiusx;
    const uint8_t spiromaxx = spirocenterX + spiroradiusx + 1;
    const uint8_t spirominy = spirocenterY - spiroradiusy;
    const uint8_t spiromaxy = spirocenterY + spiroradiusy + 1;
  
    const TProgmemRGBPalette16 *palette_arr[] = {&PartyColors_p, &OceanColors_p, &LavaColors_p, &HeatColors_p, &WaterfallColors_p, &CloudColors_p, &ForestColors_p, &RainbowColors_p, &RainbowStripeColors_p};
    const TProgmemRGBPalette16 *curPalette = palette_arr[(int)((float)myLamp.effects.getScale()/255*((sizeof(palette_arr)/sizeof(TProgmemRGBPalette16 *))-1))];
    const float speed_factor = (float)myLamp.effects.getSpeed()/127+1;
    uint8_t spirooffset = 256 / GSHMEM.spirocount;
    boolean change = false;

    myLamp.dimAll(250-speed_factor*7);

    
    for (int i = 0; i < GSHMEM.spirocount; i++) {
      uint8_t x = mapsin8(GSHMEM.spirotheta1 + i * spirooffset, spirominx, spiromaxx);
      uint8_t y = mapcos8(GSHMEM.spirotheta1 + i * spirooffset, spirominy, spiromaxy);

      uint8_t x2 = mapsin8(GSHMEM.spirotheta2 + i * spirooffset, x - spiroradiusx, x + spiroradiusx);
      uint8_t y2 = mapcos8(GSHMEM.spirotheta2 + i * spirooffset, y - spiroradiusy, y + spiroradiusy);

      CRGB color = ColorFromPalette(*curPalette, (GSHMEM.spirohueoffset + i * spirooffset), 128U);

      if (x2<WIDTH && y2<HEIGHT){ // добавил проверки. не знаю, почему эффект подвисает без них
        CRGB tmpColor = myLamp.getPixColorXY(x2, y2);
        tmpColor += color;
        myLamp.setLeds(myLamp.getPixelNumber(x2, y2), tmpColor); // += color
      }
      
      if(x2 == spirocenterX && y2 == spirocenterY) change = true;
    }

    GSHMEM.spirotheta2 += 2*speed_factor;

    EVERY_N_MILLIS(12) {
      GSHMEM.spirotheta1 += 1*speed_factor;
    }

    EVERY_N_MILLIS(50) {
      if (change && !GSHMEM.spirohandledChange) {
        GSHMEM.spirohandledChange = true;
        
        if (GSHMEM.spirocount >= WIDTH || GSHMEM.spirocount == 1) GSHMEM.spiroincrement = !GSHMEM.spiroincrement;

        if (GSHMEM.spiroincrement) {
          if(GSHMEM.spirocount >= 4)
            GSHMEM.spirocount *= 2;
          else
            GSHMEM.spirocount += 1;
        }
        else {
          if(GSHMEM.spirocount > 4)
            GSHMEM.spirocount /= 2;
          else
            GSHMEM.spirocount -= 1;
        }

        spirooffset = 256 / GSHMEM.spirocount;
      }
      
      if(!change) GSHMEM.spirohandledChange = false;
    }

    EVERY_N_MILLIS(33) {
      GSHMEM.spirohueoffset += 1;
    }
}

// ***** RAINBOW COMET / РАДУЖНАЯ КОМЕТА *****
// v1.5.0 - Follow the Rainbow Comet / first release by PalPalych 29.02.2020
// v1.7.0 - Updating for GuverLamp v1.7 by PalPalych 12.03.2020
// v1.7.1 - Scale & Speed correction, dither & blur 23.03.2020
// v1.7.2 - 0 Scale for Random 3d color 24.03.2020

#define e_com_TAILSPEED             (500)         // скорость смещения хвоста 
#define e_com_BLUR                  (24U)         // размытие хвоста 
#define e_com_3DCOLORSPEED          (5U)          // скорость случайного изменения цвета (0й - режим)

// // The coordinates for 3 16-bit noise spaces.
// #define NUM_LAYERS 1
// uint32_t GSHMEM.e_x[NUM_LAYERS];
// uint32_t GSHMEM.e_y[NUM_LAYERS];
// uint32_t GSHMEM.e_z[NUM_LAYERS];
// uint32_t GSHMEM.e_scaleX[NUM_LAYERS];
// uint32_t GSHMEM.e_scaleY[NUM_LAYERS];
// uint8_t GSHMEM.noise3d[NUM_LAYERS][WIDTH][HEIGHT];
// uint8_t GSHMEM.eNs_noisesmooth;

void FillNoise(int8_t layer) {
  const uint8_t e_centerX =  (WIDTH / 2) - 1;
  const uint8_t e_centerY = (HEIGHT / 2) - 1;

  for (uint8_t i = 0; i < WIDTH; i++) {
    int32_t ioffset = GSHMEM.e_scaleX[layer] * (i - e_centerX);
    for (uint8_t j = 0; j < HEIGHT; j++) {
      int32_t joffset = GSHMEM.e_scaleY[layer] * (j - e_centerY);
      int8_t data = inoise16(GSHMEM.e_x[layer] + ioffset, GSHMEM.e_y[layer] + joffset, GSHMEM.e_z[layer]) >> 8;
      int8_t olddata = GSHMEM.noise3d[layer][i][j];
      int8_t newdata = scale8( olddata, GSHMEM.eNs_noisesmooth ) + scale8( data, 255 - GSHMEM.eNs_noisesmooth );
      data = newdata;
      GSHMEM.noise3d[layer][i][j] = data;
    }
  }
}

void MoveFractionalNoiseX(int8_t amplitude = 1, float shift = 0) {
  uint8_t zD;
  uint8_t zF;
  CRGB *leds = myLamp.getLeds(); // unsafe
  CRGB ledsbuff[NUM_LEDS];

  for(uint8_t i=0; i<NUM_LAYERS; i++)
    for (uint8_t y = 0; y < HEIGHT; y++) {
      int16_t amount = ((int16_t)(GSHMEM.noise3d[i][0][y] - 128) * 2 * amplitude + shift * 256);
      int8_t delta = ((uint16_t)abs(amount) >> 8) ;
      int8_t fraction = ((uint16_t)abs(amount) & 255);
      for (uint8_t x = 0 ; x < WIDTH; x++) {
        if (amount < 0) {
          zD = x - delta; zF = zD - 1;
        } else {
          zD = x + delta; zF = zD + 1;
        }
        CRGB PixelA = CRGB::Black  ;
        if ((zD >= 0) && (zD < WIDTH)) PixelA = myLamp.getLeds(myLamp.getPixelNumber(zD, y));
        CRGB PixelB = CRGB::Black ;
        if ((zF >= 0) && (zF < WIDTH)) PixelB = myLamp.getLeds(myLamp.getPixelNumber(zF, y));
        //myLamp.setLeds(myLamp.getPixelNumber(x, y), (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction))));   // lerp8by8(PixelA, PixelB, fraction );
        //((CRGB *)GSHMEM.ledsbuff)[myLamp.getPixelNumber(x, y)] = (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction)));   // lerp8by8(PixelA, PixelB, fraction );
        ledsbuff[myLamp.getPixelNumber(x, y)] = (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction)));   // lerp8by8(PixelA, PixelB, fraction );
      }
    }
  // memcpy(leds, GSHMEM.ledsbuff, sizeof(CRGB)* NUM_LEDS);
  memcpy(leds, ledsbuff, sizeof(CRGB)* NUM_LEDS);
}

void MoveFractionalNoiseY(int8_t amplitude = 1, float shift = 0) {
  uint8_t zD;
  uint8_t zF;
  CRGB *leds = myLamp.getLeds(); // unsafe
  CRGB ledsbuff[NUM_LEDS];

  for(uint8_t i=0; i<NUM_LAYERS; i++)
    for (uint8_t x = 0; x < WIDTH; x++) {
      int16_t amount = ((int16_t)(GSHMEM.noise3d[i][x][0] - 128) * 2 * amplitude + shift * 256);
      int8_t delta = (uint16_t)abs(amount) >> 8 ;
      int8_t fraction = (uint16_t)abs(amount) & 255;
      for (uint8_t y = 0 ; y < WIDTH; y++) {
        if (amount < 0) {
          zD = y - delta; zF = zD - 1;
        } else {
          zD = y + delta; zF = zD + 1;
        }
        CRGB PixelA = CRGB::Black ;
        if ((zD >= 0) && (zD < WIDTH)) PixelA = myLamp.getLeds(myLamp.getPixelNumber(x, zD));
        CRGB PixelB = CRGB::Black ;
        if ((zF >= 0) && (zF < WIDTH)) PixelB = myLamp.getLeds(myLamp.getPixelNumber(x, zF));
        //myLamp.setLeds(myLamp.getPixelNumber(x, y), (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction))));
        //((CRGB *)GSHMEM.ledsbuff)[myLamp.getPixelNumber(x, y)] = (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction)));
        ledsbuff[myLamp.getPixelNumber(x, y)] = (PixelA.nscale8(ease8InOutApprox(255 - fraction))) + (PixelB.nscale8(ease8InOutApprox(fraction)));
      }
    }
  // memcpy(leds, GSHMEM.ledsbuff, sizeof(CRGB)* NUM_LEDS);
  memcpy(leds, ledsbuff, sizeof(CRGB)* NUM_LEDS);
}

void drawFillRect2_fast(int8_t x1, int8_t y1, int8_t x2, int8_t y2, CRGB color)
{ // Fine if: 0<x1<x2<WIDTH && 0<y1<y2<HEIGHT
  for (int8_t xP = x1; xP <= x2; xP++)
  {
    for (int8_t yP = y1; yP <= y2; yP++)
    {
      myLamp.setLeds(myLamp.getPixelNumber(xP, yP), color);
    }
  }
}

void rainbowCometRoutine(CRGB *leds, const char *param)
{ // Rainbow Comet by PalPalych
/*
  Follow the Rainbow Comet Efect by PalPalych
  Speed = tail dispersing
  Scale = 0 - Random 3d color
          1...127 - time depending color
          128...254 - selected color
          255 - white
*/
  const uint8_t e_centerX =  (WIDTH / 2) - 1;
  const uint8_t e_centerY = (HEIGHT / 2) - 1;
  uint8_t Scale = myLamp.effects.getScale();


  if(myLamp.isLoading()){
    //randomSeed(millis());
    GSHMEM.eNs_noisesmooth = random(0, 200*(uint_fast16_t)myLamp.effects.getSpeed()/255); // степень сглаженности шума 0...200
    //for(uint8_t i=0; i<NUM_LAYERS;i++){
        // GSHMEM.e_x[i] = random16();
        // GSHMEM.e_y[i] = random16();
        // GSHMEM.e_z[i] = random16();
        // GSHMEM.e_scaleX[i] = 6000;
        // GSHMEM.e_scaleY[i] = 6000;
    //}
    // memset(GSHMEM.e_x,0,sizeof(GSHMEM.e_x));
    // memset(GSHMEM.e_y,0,sizeof(GSHMEM.e_y));
    // memset(GSHMEM.e_z,0,sizeof(GSHMEM.e_z));
    // memset(GSHMEM.e_scaleX,0,sizeof(GSHMEM.e_scaleX));
    // memset(GSHMEM.e_scaleY,0,sizeof(GSHMEM.e_scaleY));
    // memset(GSHMEM.noise3d,0,sizeof(GSHMEM.noise3d));
  }

  myLamp.blur2d(e_com_BLUR);    // < -- размытие хвоста
  myLamp.dimAll(254);            // < -- затухание эффекта для последующего кадра
  CRGB _eNs_color = CRGB::White;
  if (Scale <= 1) {
    _eNs_color = CHSV(GSHMEM.noise3d[0][0][4] * e_com_3DCOLORSPEED , 255, 255);
  } else if (Scale < 128) {
    _eNs_color = CHSV(millis() / (Scale + 1U) * 4 + 10, 255, 255);
  } else if (Scale < 255) {
    _eNs_color = CHSV((Scale - 128) * 2, 255, 255);
  }
  drawFillRect2_fast(e_centerX - 1, e_centerY - 1, e_centerX + 1, e_centerY + 1, _eNs_color);
  // Noise
  uint16_t sc = (uint16_t)myLamp.effects.getSpeed() * 30 + 500; //64 + 1000;
  uint16_t sc2 = (float)myLamp.effects.getSpeed()/127.0+1.5; //1.5...3.5;
  for(uint8_t i=0; i<NUM_LAYERS; i++){
    GSHMEM.e_x[i] += e_com_TAILSPEED*sc2;
    GSHMEM.e_y[i] += e_com_TAILSPEED*sc2;
    GSHMEM.e_z[i] += e_com_TAILSPEED*sc2;
    GSHMEM.e_scaleX[i] = sc; // 8000;
    GSHMEM.e_scaleY[i] = sc; // 8000;
    FillNoise(i);
  }
  MoveFractionalNoiseX(WIDTH / 2U - 1U);
  MoveFractionalNoiseY(HEIGHT / 2U - 1U);
}

void rainbowComet3Routine(CRGB *leds, const char *param)
{ // Rainbow Comet by PalPalych
/*
  Follow the Rainbow Comet Efect by PalPalych
  Speed = tail dispersing
  Scale = 0 - Random 3d color
          1...127 - time depending color
          128...254 - selected color
          255 - white
*/
  const uint8_t e_centerX =  (WIDTH / 2) - 1;
  const uint8_t e_centerY = (HEIGHT / 2) - 1;
  uint8_t Scale = myLamp.effects.getScale();


  if(myLamp.isLoading()){
    //randomSeed(millis());
    GSHMEM.eNs_noisesmooth = random(0, 200*(uint_fast16_t)myLamp.effects.getSpeed()/255); // степень сглаженности шума 0...200
    //for(uint8_t i=0; i<NUM_LAYERS;i++){
        // GSHMEM.e_x[i] = random16();
        // GSHMEM.e_y[i] = random16();
        // GSHMEM.e_z[i] = random16();
        // GSHMEM.e_scaleX[i] = 6000;
        // GSHMEM.e_scaleY[i] = 6000;
    //}
    // memset(GSHMEM.e_x,0,sizeof(GSHMEM.e_x));
    // memset(GSHMEM.e_y,0,sizeof(GSHMEM.e_y));
    // memset(GSHMEM.e_z,0,sizeof(GSHMEM.e_z));
    // memset(GSHMEM.e_scaleX,0,sizeof(GSHMEM.e_scaleX));
    // memset(GSHMEM.e_scaleY,0,sizeof(GSHMEM.e_scaleY));
    // memset(GSHMEM.noise3d,0,sizeof(GSHMEM.noise3d));
  }

  myLamp.blur2d(Scale/5+1);    // < -- размытие хвоста
  myLamp.dimAll(255-Scale/66);            // < -- затухание эффекта для последующего кадра
  byte xx = 2 + sin8( millis() / 10) / 22;
  byte yy = 2 + cos8( millis() / 9) / 22;
  myLamp.setLeds(myLamp.getPixelNumber( xx, yy), 0x0000FF);

  xx = 4 + sin8( millis() / 10) / 32;
  yy = 4 + cos8( millis() / 7) / 32;
  myLamp.setLeds(myLamp.getPixelNumber( xx, yy), 0xFF0000);
  myLamp.setLeds(myLamp.getPixelNumber( e_centerX, e_centerY), 0x00FF00);

  uint16_t sc = (uint16_t)myLamp.effects.getSpeed() * 30 + 500; //64 + 1000;
  uint16_t sc2 = (float)myLamp.effects.getSpeed()/127.0+1.5; //1.5...3.5;
  for(uint8_t i=0; i<NUM_LAYERS; i++){
    GSHMEM.e_x[i] += 1500*sc2;
    GSHMEM.e_y[i] += 1500*sc2;
    GSHMEM.e_z[i] += 1500*sc2;
    GSHMEM.e_scaleX[i] = sc; // 8000;
    GSHMEM.e_scaleY[i] = sc; // 8000;
    FillNoise(i);
  }
  MoveFractionalNoiseX(2);
  MoveFractionalNoiseY(2, 0.33);
}