// Visual Micro is in vMicro>General>Tutorial Mode
// 
/*
    Name:       Number_Entry_GUI.ino
    Created:	5/3/2022 8:31:33 PM
    Author:     OFFICEPC\Marco Flag
*/


#pragma region Included Libs
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "XPT2046_Touchscreen.h"
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeMonoBoldOblique9pt7b.h>
#include <Fonts/FreeSerifBold9pt7b.h>
#include "Math.h"
#include <RotaryEncoder.h>
#pragma endregion

#pragma region Display Colors
#define BLACK			ILI9341_BLACK      	//0x0000 /*   0,   0,   0 */
#define NAVY			ILI9341_NAVY       	//0x000F /*   0,   0, 128 */
#define DARKGREEN		ILI9341_DARKGREEN  	//0x03E0 /*   0, 128,   0 */
#define DARKCYAN		ILI9341_DARKCYAN   	//0x03EF /*   0, 128, 128 */
#define MAROON			ILI9341_MAROON     	//0x7800 /*   128, 0,   0 */
#define PURPLE			ILI9341_PURPLE     	//0x780F /*   128,  0, 128 */
#define OLIVE			ILI9341_OLIVE      	//0x7BE0 /*   128, 128,  0 */
#define LIGHTGREY		ILI9341_LIGHTGREY  	//0xC618 /*   192, 192, 192 */
#define DARKGREY		ILI9341_DARKGREY   	//0x7BEF /*   128, 128, 128 */
#define BLUE			ILI9341_BLUE       	//0x001F /*   0,     0, 255 */
#define GREEN			ILI9341_GREEN      	//0x07E0 /*   0,   255,   0 */
#define CYAN			ILI9341_CYAN       	//0x07FF /*   0,   255, 255 */
#define RED				ILI9341_RED        	//0xF800 /*   255,   0,   0 */
#define MAGENTA			ILI9341_MAGENTA    	//0xF81F /*   255,   0, 255 */
#define YELLOW			ILI9341_YELLOW     	//0xFFE0 /*   255, 255,   0 */
#define WHITE			ILI9341_WHITE      	//0xFFFF /*   255, 255, 255 */
#define ORANGE			ILI9341_ORANGE     	//0xFD20 /*   255, 165,   0 */
#define GREENYELLOW		ILI9341_GREENYELLOW //0xAFE5 /*   173, 255,  47 */
#define PINK			ILI9341_PINK       	//0xF81F
#pragma endregion

#pragma region variable definitions
uint32_t Number = 0;
int Steep = 1;
bool Rotary_enabled = false;
bool Blank_Digit = false;
int digt_ID = 0;
boolean Calc_clickButton = false;
int16_t previous_Number = 0;
int16_t Digit_ = 0;

#pragma endregion

#pragma region Rotary Encoder definitions
#define PIN_IN1 2 // interrupt enabled 
#define PIN_IN2 3 // interrupt enabled
static int pos = 0;
static int newPos = 0;
static int Direction = 0;
#pragma endregion

#pragma region TFT SPI display - SPI Display/TouchScreen definitions
#define TFT_CS 10
#define TFT_DC 9
#define TFT_MOSI 11 //Also connected Touchscreen "T_DIN" pin
#define TFT_CLK 13  //Also connected Touchscreen "T_CLK" pin
#define TFT_RST 8
#define TFT_MISO 12 //Also connected Touchscreen "T_DO" pin
//---------------------------//
#define T_CS 7   //Touchscreen T_CS pin (Chip select)
#define T_IRQ 2  //Touchscreen T_IRQ pin (Chip interrupt request)
#pragma endregion

#pragma region Disply/Touch definitions and initialization
#define MINPRESSURE 150
#define MAXPRESSURE 1000
#define ROTATION 1
#define displyBackground BLACK
    //Calibration values // These values were captured after Touchscreen calibration.
float xCalM = -0.09, yCalM = 0.07; // gradients
float xCalC = 335.01, yCalC = -15.18; // y axis crossing points
TS_Point p; // This Class is defined in the XPT2046 library for Touch Controller
    //Initiate the TFT disply
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
// Initiate the TouchScreen.
XPT2046_Touchscreen ts(T_CS);
#pragma endregion

#pragma region GUI Objects definitions
//7 Sements rendering position//
#define SEG_FRAME_X 20
#define SEG_FRAME_Y 30
//calculate_Get Number - Button parameters//
#define Calc_BUTTON_X 285
#define Calc_BUTTON_Y 220
#define Calc_BUTTON_W 70
#define Calc_BUTTON_H 40
#pragma endregion

#pragma region Classes definition
class ScreenPoint {
public:
    int16_t x;
    int16_t y;

    ScreenPoint() {
    } //Default contructor

    ScreenPoint(int16_t xIn, int16_t yIn) {
        x = xIn;
        y = yIn;
    }
};
class SevenSegDisplay {
private:
    const int digit_array[10][7] = {
        // a, b, c, d, e, f, g //
          {1, 1, 1, 1, 1, 1, 0},  // 0
          {0, 1, 1, 0, 0, 0, 0},  // 1
          {1, 1, 0, 1, 1, 0, 1},  // 2
          {1, 1, 1, 1, 0, 0, 1},  // 3
          {0, 1, 1, 0, 0, 1, 1},  // 4
          {1, 0, 1, 1, 0, 1, 1},  // 5
          {1, 0, 1, 1, 1, 1, 1},  // 6
          {1, 1, 1, 0, 0, 0, 0},  // 7
          {1, 1, 1, 1, 1, 1, 1},  // 8
          {1, 1, 1, 1, 0, 1, 1}   // 9
    };
    int previous_digit[6] = { 0,0,0,0,0,0 }; // Used to confirm if new digit is difernt compared with the old one.
    int size = 2;
    uint8_t spacing_x = 30;

protected:
    int pos_x;
    int pos_y;
    int Width = 35;
    int Height = 58;

public:
    uint16_t Seg_color;
    uint16_t Back_color;
    uint16_t Frame_color;
    uint8_t digit;      //Holds the number value
    boolean Flag = false;
    //Contructor
    SevenSegDisplay(int pos_x_, int pos_y_, uint16_t Seg_color_, uint16_t Back_color_, uint16_t Frame_color_) {
        this->pos_x = pos_x_;
        this->pos_y = pos_y_;
        this->Seg_color = Seg_color_;
        this->Back_color = Back_color_;
        this->Frame_color = Frame_color_;
    };
    void render_digit_segments_big(uint8_t digit_, boolean DP) {
        this->digit = digit_;
        // loop through 7 segments
        for (uint8_t i = 0; i < 7; i++) {
            bool seg_on = digit_array[digit][i];
            // if seg_on is true draw segment
            if (seg_on) {
                switch (i) {
                case 0:
                    tft.fillRoundRect(4 + pos_x, pos_y - 8, 18, 6, 4, Seg_color); // SEG a; //x, y, w, h, r, color 
                    break;
                case 1:
                    tft.fillRoundRect(20 + pos_x, pos_y - 4, 6, 20, 4, Seg_color); // SEG b
                    break;
                case 2:
                    tft.fillRoundRect(20 + pos_x, 18 + pos_y, 6, 20, 4, Seg_color); // SEG c
                    break;
                case 3:
                    tft.fillRoundRect(4 + pos_x, 36 + pos_y, 18, 6, 4, Seg_color); // SEG d
                    break;
                case 4:
                    tft.fillRoundRect(0 + pos_x, 18 + pos_y, 6, 20, 4, Seg_color); // SEG e
                    break;
                case 5:
                    tft.fillRoundRect(0 + pos_x, pos_y - 4, 6, 20, 4, Seg_color); // SEG f
                    break;
                case 6:
                    tft.fillRoundRect(4 + pos_x, 14 + pos_y, 18, 6, 4, Seg_color); // SEG g
                    break;
                }
                seg_on = false;
            }
        }
        if (DP)
        {
            tft.fillRoundRect(22 + pos_x, 38 + pos_y, 7, 7, 4, Seg_color); //x, y, w, h, r, color 
        }
        else if (!DP)
        {
            tft.fillRoundRect(22 + pos_x, 38 + pos_y, 7, 7, 4, Back_color); //x, y, w, h, r, color 
        }
    }

    //void render_digit_segments(uint32_t digit) {
    //    uint8_t digit_array[] = { digit / 100000 % 10, digit / 10000 % 10, digit / 1000 % 10, digit / 100 % 10, digit / 10 % 10, digit % 10 };
    //    if (digit > 99999) {
    //        if (previous_digit[6] != digit_array[0])
    //        {
    //            tft.fillRect(pos_x, pos_y - 5, 30, 60, WHITE);
    //            render_digit_segments_big(pos_x, pos_y, digit_array[0], color);
    //            previous_digit[6] = digit_array[0];
    //        }
    //    }
    //    else {
    //        tft.fillRect(pos_x, pos_y - 5, 30, 60, WHITE);
    //    }
    //    if (digit > 9999) {
    //        if (previous_digit[5] != digit_array[1]) {
    //            tft.fillRect(pos_x + spacing_x, pos_y - 5, 45, 60, WHITE);
    //            render_digit_segments_big(pos_x + spacing_x, pos_y, digit_array[1], color);
    //            previous_digit[5] = digit_array[1];
    //        }
    //    }
    //    else {
    //        tft.fillRect(pos_x + spacing_x, pos_y - 5, 30, 60, WHITE);
    //    }
    //    if (digit > 999) {
    //        if (previous_digit[4] != digit_array[2]) {
    //            tft.fillRect(pos_x + (spacing_x * 2), pos_y - 5, 45, 60, WHITE);
    //            render_digit_segments_big(pos_x + (spacing_x * 2), pos_y, digit_array[2], color);
    //            previous_digit[4] = digit_array[2];
    //            tft.fillRoundRect(32 + (pos_x + (spacing_x * 2)), 42 + pos_y, 8, 8, 4, RED); // Decimal point
    //        }
    //    }
    //    else {
    //        tft.fillRect(pos_x + (spacing_x * 2), pos_y - 5, 30, 60, WHITE);
    //        tft.fillRect(30 + pos_x + (spacing_x * 2), pos_y - 5, 15, 60, WHITE); // Used to fill the Decimal point space.
    //    }
    //    if (digit > 99) {
    //        if (previous_digit[3] != digit_array[3]) {
    //            tft.fillRect(pos_x + (spacing_x * 3 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
    //            render_digit_segments_big(pos_x + (spacing_x * 3 + (spacing_x / 2)), pos_y, digit_array[3], color);
    //            previous_digit[3] = digit_array[3];
    //        }
    //    }
    //    else {
    //        tft.fillRect(pos_x + (spacing_x * 3 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
    //    }
    //    if (digit > 9) {
    //        if (previous_digit[2] != digit_array[4]) {
    //            tft.fillRect(pos_x + (spacing_x * 4 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
    //            render_digit_segments_big(pos_x + (spacing_x * 4 + (spacing_x / 2)), pos_y, digit_array[4], color);
    //            previous_digit[2] = digit_array[4];
    //        }
    //    }
    //    else {
    //        tft.fillRect(pos_x + (spacing_x * 4 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
    //    }
    //    if (previous_digit[1] != digit_array[5]) {
    //        tft.fillRect(pos_x + (spacing_x * 5 + (spacing_x / 2)), pos_y - 5, 30, 60, WHITE);
    //        render_digit_segments_big(pos_x + (spacing_x * 5 + (spacing_x / 2)), pos_y, digit_array[5], color);
    //        previous_digit[1] = digit_array[5];  // previous_Number must be an array with digits 
    //    }
    //}
    void drawSeg(boolean Visible)   //Drawing the 7 segment box and frame
    {
        if (Visible) {
            tft.fillRect(pos_x - 4, pos_y - 12, Width, Height, Back_color);
            tft.drawRoundRect(pos_x - 4, pos_y - 13, Width, Height + 2, 2, Frame_color); // (x, y, w, h, angle)
            tft.drawRoundRect(pos_x - 3, pos_y - 12, Width, Height, 2, Frame_color);
        }
        else if (!Visible) {
            tft.fillRect(pos_x - 4, pos_y - 12, Width, Height, Seg_color);
            tft.drawRoundRect(pos_x - 4, pos_y - 13, Width, Height + 2, 2, Seg_color); // (x, y, w, h, angle)
            tft.drawRoundRect(pos_x - 3, pos_y - 12, Width, Height, 2, Seg_color);
        }
    }
    void drawFrame() {
        tft.drawRoundRect(pos_x - 4, pos_y - 13, Width, Height + 2, 2, Frame_color); // (x, y, w, h, angle)
        tft.drawRoundRect(pos_x - 3, pos_y - 12, Width, Height, 2, Frame_color);
        Flag = false;
    };
    void IsChecked(ScreenPoint sp_) {    //Select and hilight the checked digit frame
        if ((sp_.x > pos_x - 20) && (sp_.x < (pos_x + 2))) {
            if ((sp_.y > pos_y - 4) && (sp_.y < ((pos_y - 6) + Height))) {
                delay(200);
                if (Flag == true) {
                    delay(200);
                    Flag = false;
                    tft.drawRoundRect(pos_x - 4, pos_y - 13, Width, Height + 2, 2, Frame_color); // (x, y, w, h)
                    tft.drawRoundRect(pos_x - 3, pos_y - 12, Width, Height, 2, Frame_color);
                    //return 0;
                }
                else {
                    delay(200);
                    Flag = true;
                    tft.drawRoundRect(pos_x - 4, pos_y - 13, Width, Height + 2, 2, GREEN); // (x, y, w, h)
                    tft.drawRoundRect(pos_x - 3, pos_y - 12, Width, Height, 2, GREEN);
                    //return 1;
                }
            }
        }
    }
};

class up_downButton {
private:

    int X_arrwsPodition;
    int Y_arrwsPodition;
    int arrwWidth;
    int arrwHeight;
    uint16_t frameColor;
    uint16_t errowColor;

    typedef struct {
        int X;
        int Y;
        int width;
        int height;
    }rect;
    typedef struct {
        int Min;
        int Max;
        int Init;
        int Step;
    }arrowRec;

    rect arrwFrameR;
    rect upArrwR;
    rect dwnArrwR;
    TS_Point upArrwTip, upArrwRBase, upArrwLBase;
    TS_Point dwnArrwTip, dwnArrwRBase, dwnArrwLBase;

public:
    //Constructor
    up_downButton(int X_arrwsPodition, int Y_arrwsPodition, int arrwWidth, int arrwHeight, uint16_t frameColor, uint16_t errowColor) {
        this->X_arrwsPodition = X_arrwsPodition;
        this->Y_arrwsPodition = Y_arrwsPodition;
        this->arrwWidth = arrwWidth;
        this->arrwHeight = arrwHeight;
        this->frameColor = frameColor;
        this->errowColor = errowColor;

        arrwFrameR = { X_arrwsPodition, Y_arrwsPodition, arrwWidth, arrwHeight * 2 };
        upArrwR = { arrwFrameR.X, arrwFrameR.Y, arrwWidth, arrwHeight };
        dwnArrwR = { arrwFrameR.X, arrwFrameR.Y + arrwFrameR.height / 2, arrwWidth, arrwHeight };
    };
    void littleArrows()
    {
        tft.drawRect(arrwFrameR.X + 1, arrwFrameR.Y + 1, arrwFrameR.width, arrwFrameR.height, frameColor);
        //tft.fillRect(arrwFrameR.X + 2, arrwFrameR.Y + 2, arrwFrameR.width - 2, arrwFrameR.height - 2, YELLOW);
        upArrwTip.x = upArrwR.X + upArrwR.width / 2;
        upArrwTip.y = upArrwR.Y + 5;
        upArrwLBase.x = upArrwR.X + 5;
        upArrwLBase.y = upArrwR.Y + upArrwR.height - 5;
        upArrwRBase.x = upArrwR.X + upArrwR.width - 5;
        upArrwRBase.y = upArrwR.Y + upArrwR.height - 5;
        tft.fillTriangle(upArrwTip.x, upArrwTip.y, upArrwLBase.x, upArrwLBase.y, upArrwRBase.x, upArrwRBase.y, errowColor);
        dwnArrwTip.x = dwnArrwR.X + dwnArrwR.width / 2;
        dwnArrwTip.y = dwnArrwR.Y + dwnArrwR.height - 5;
        dwnArrwLBase.x = dwnArrwR.X + 5;
        dwnArrwLBase.y = dwnArrwR.Y + 5;
        dwnArrwRBase.x = dwnArrwR.X + dwnArrwR.width - 5;
        dwnArrwRBase.y = dwnArrwR.Y + 5;
        tft.fillTriangle(dwnArrwTip.x, dwnArrwTip.y, dwnArrwLBase.x, dwnArrwLBase.y, dwnArrwRBase.x, dwnArrwRBase.y, errowColor);
    }

    int isErrowPressed(ScreenPoint sp_) {
        if ((sp_.x > upArrwR.X - upArrwR.width / 2) && (sp_.x < (upArrwR.X + (upArrwR.width / 2.5)))) {
            if ((sp_.y > upArrwR.Y) && (sp_.y <= (upArrwR.Y + upArrwR.height))) {
                //Serial.println("up Errow btn hit");
                //upArrowHandler();
                return 1;
            }
        }
        if ((sp_.x > dwnArrwR.X - upArrwR.width / 2) && (sp_.x < (dwnArrwR.X + dwnArrwR.width / 2.5))) {
            if ((sp_.y > dwnArrwR.Y) && (sp_.y <= (dwnArrwR.Y + dwnArrwR.height))) {
                //Serial.println("down Errow btn hit");
                //downArrowHandler();
                return 2;
            }
        }
    }
};
class CheckBox {
private:
    int xPos_;
    int yPos_;
    int boxWidth_;
    int boxHeight_;
    char* rectText_;
    uint16_t checked_BOX_color_;   // = ILI9341_GREEN;
    uint16_t unchecked_BOX_color_; // = displyBackground;
    uint16_t rect_color_;   // Color of the full Box
public:
    boolean chkBoxFlag = false;
    //Contructor
    CheckBox(int xPos_, int yPos_, int boxWidth_, int boxHeight, char* rectText_, uint16_t rect_color_) {
        this->xPos_ = xPos_;
        this->yPos_ = yPos_;
        this->boxWidth_ = boxWidth_;
        this->boxHeight_ = boxWidth_;
        this->rectText_ = rectText_;
        this->checked_BOX_color_ = checked_BOX_color_;
        this->unchecked_BOX_color_ = unchecked_BOX_color_;
        this->rect_color_ = rect_color_;
    };
    void drawFrame() {
        tft.fillRect(xPos_ - 5, yPos_ - 5, boxWidth_ + 60, boxHeight_ + 10, rect_color_);
        uint16_t check_BOX_color_Frame = checked_BOX_color_ >> 6;
        tft.drawRect(xPos_ - 2, yPos_ - 2, boxWidth_ + 4, boxHeight_ + 4, check_BOX_color_Frame); // (x, y, w, h)
        tft.fillRect(xPos_, yPos_, boxWidth_, boxHeight_, unchecked_BOX_color_);
        uint16_t rect_BOX_color_Frame = checked_BOX_color_ << 3;
        tft.drawRect(xPos_ - 5, yPos_ - 5, boxWidth_ + 60, boxHeight_ + 11, rect_BOX_color_Frame);

        tft.setCursor(xPos_ + 24, yPos_ + 15);
        tft.setTextSize(1);
        tft.setTextColor(RED);
        tft.print(rectText_);
    }
    int IsChecked(ScreenPoint sp_) {
        if ((sp_.x > xPos_ - boxWidth_) && (sp_.x < (xPos_ + (boxWidth_ * 2.5)))) {
            if ((sp_.y > yPos_ + 10) && (sp_.y < (yPos_ + boxHeight_ + 25))) {
                delay(200);
                if (chkBoxFlag == false) {
                    chkBoxFlag = true;
                    //return 1;
                }
                else {
                    chkBoxFlag = false;
                    checked(displyBackground);
                    //return 0;
                }
            }
        }
        return chkBoxFlag;
    }
    void checked(uint16_t chk_color) {
        tft.fillRect(xPos_, yPos_, boxWidth_, boxHeight_, chk_color);
    }
};
#pragma endregion

#pragma region Classe instances
SevenSegDisplay SevenSeg1 = SevenSegDisplay(SEG_FRAME_X, SEG_FRAME_Y, BLACK, WHITE, RED); // int pos_x_, int pos_y_, uint16_t Seg_color_, uint16_t Back_color_, uint16_t Frame_color_
SevenSegDisplay SevenSeg2 = SevenSegDisplay(SEG_FRAME_X + 36, SEG_FRAME_Y, BLACK, WHITE, RED);
SevenSegDisplay SevenSeg3 = SevenSegDisplay(SEG_FRAME_X + 72, SEG_FRAME_Y, BLACK, WHITE, RED);
SevenSegDisplay SevenSeg4 = SevenSegDisplay(SEG_FRAME_X + 108, SEG_FRAME_Y, BLACK, WHITE, RED);
//up_downButton up_downButton_1 = up_downButton(223, 22, 32, 32, WHITE, BLUE);
SevenSegDisplay* Digit_Obejects[4] = {
       &SevenSeg1,
       &SevenSeg2,
       &SevenSeg3,
       &SevenSeg4
};
Adafruit_GFX_Button Calculate_BTN;
RotaryEncoder* encoder = nullptr; //A pointer to the dynamic created rotary encoder instance. // This will be done in setup()
#pragma endregion

#pragma region Functions definition
ScreenPoint getScreenCoords(int16_t x, int16_t y) {
    int16_t xCoord = round((x * xCalM) + xCalC);
    int16_t yCoord = round((y * yCalM) + yCalC);
    if (xCoord < 0) xCoord = 0;
    if (xCoord >= tft.width()) xCoord = tft.width() - 1;
    if (yCoord < 0) yCoord = 0;
    if (yCoord >= tft.height()) yCoord = tft.height() - 1;
    return(ScreenPoint(xCoord, yCoord));
}
void Deselec_SegDigits() // Used to clear the previos selected digit
{
    SevenSeg1.drawFrame();
    SevenSeg2.drawFrame();
    SevenSeg3.drawFrame();
    SevenSeg4.drawFrame();
}
void Rotary_checkPosition() //Rotary encoder
{
    encoder->tick(); // just call tick() to check the state.
    if (Rotary_enabled == true) {
        newPos = encoder->getPosition();
        if (pos != newPos) {
            //Serial.print("pos:");
            //Serial.print(newPos);
            //Serial.print(" dir:");
            //Serial.println((int)(encoder->getDirection()));
            //Steep = 1;        
            Direction = (int)encoder->getDirection();
            if (Direction == -1) {
                Number = Number + Steep;
            }
            else if (Direction == 1) {
                Number = Number - Steep;
            };
        };
    };
}
void check_Number() /*Keep number limit between 0 and 999999*/ {
    if (Number > 99999990) { //Avoid counting below 0 - When decrementing below 0 counting Number changes to 4294967295 - This helps to workaroud any counting bug.
        //Serial.print(" Number:");
        //Serial.println(Number);
        //Digit_Obejects[digt_ID]->drawSeg(true);
        Number = 0;
        Steep = 1;
        pos = 0;
        newPos = 0;
        Rotary_enabled = true;
        Blank_Digit = true;
    }
    else if (Number > 9) {
        Number = 9;
        Steep = 1;
        pos = 9;
        newPos = 9;
        Rotary_enabled = true;
        //Blank_Digit = false;
    }
}
void RenderSend_Nunber() {
    /*Serial.print(" Number:");
    Serial.println(Number);*/
    Digit_Obejects[digt_ID]->drawSeg(true); // Clear the old digit
    if (digt_ID == 0) {
        Digit_Obejects[digt_ID]->render_digit_segments_big(Number, true); // includ the decimal point afer 3rd digit
    }
    else {
        Digit_Obejects[digt_ID]->render_digit_segments_big(Number, false);
    }
    //if (Number <= 0) {
    //    Number = 1; // Need a sart Number.        
    //}
}
int buttonPressed_Handler(int xPos_, int yPos_, int buttonWidth_, int buttonHeight_, ScreenPoint sp_, Adafruit_GFX_Button* btn_Name) {
    if ((sp_.x > (xPos_ - (buttonWidth_ / 1.2))) && (sp_.x < (xPos_ + (buttonWidth_ / 3.5)))) {
        if ((sp_.y > yPos_ - (buttonHeight_ / 4)) && (sp_.y <= (yPos_ + (buttonHeight_ / 1.5)))) {
            //Serial.println(btn_Name);
            btn_Name->drawButton(true);
            Steep = 1;
            return 1;
        }
    }
    else return 0;
}
void Concatenate_Number()
{
    //Not Working!!!!!//
    for (int i = 0; i < 4; i++) //Check which Digit Object was selected
    {
        Digit_ = Digit_Obejects[i]->digit;
        //Number = Digit_ * (i * 10) + previous_Number;
        //previous_Number = Number;
        int Muitiply = Muitiply * 10;
        Number = Digit_ * (10 ^ i);
        Serial.print("Number ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(Number);
    }
}

#pragma endregion


// The setup() function runs once each time the micro-controller starts
void setup()
{
    Serial.begin(9600);
    tft.begin();
    tft.setRotation(ROTATION);
    tft.fillScreen(displyBackground);
    ts.begin();
    ts.setRotation(ROTATION);
    //tft.setFont(&FreeSerifBold9pt7b); // Bigger Fonts for the CheckBox test
    //tft.setFont();// Smaller fonts for the buttons text
    Digit_Obejects[0]->drawSeg(true);
    Digit_Obejects[1]->drawSeg(true);
    Digit_Obejects[2]->drawSeg(true);
    Digit_Obejects[3]->drawSeg(true);
    //up_downButton_1.littleArrows();
    SevenSeg1.render_digit_segments_big(8, true);
    //delay(1000);
    SevenSeg2.render_digit_segments_big(5, false);
    //delay(1000);
    SevenSeg3.render_digit_segments_big(3, false);
    //delay(1000);
    SevenSeg4.render_digit_segments_big(6, false);
    Calculate_BTN.initButton(&tft, Calc_BUTTON_X, Calc_BUTTON_Y, Calc_BUTTON_W, Calc_BUTTON_H, GREEN, RED, YELLOW, "Calc", 2, 2);
    Calculate_BTN.drawButton();
    encoder = new RotaryEncoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03); // Rotary encode configurations
    // use TWO03 mode when PIN_IN1, PIN_IN2 signals are both LOW or HIGH in latch position.
    attachInterrupt(digitalPinToInterrupt(PIN_IN1), Rotary_checkPosition /*function call*/, CHANGE); // register interrupt routine
    attachInterrupt(digitalPinToInterrupt(PIN_IN2), Rotary_checkPosition /*function call*/, CHANGE); // register interrupt routine
}

// Add the main program code into the continuous loop() function
void loop()
{
    if (pos != newPos) /*Check Rotary encoder position*/ {
        //Serial.print(" Rotary Position = ");
        //Serial.println(pos);
        //pos = Digit_Obejects[digt_ID]->digit;
        Blank_Digit = false;
        check_Number();
        RenderSend_Nunber();
        if (Blank_Digit && digt_ID == 0) {
            Digit_Obejects[digt_ID]->drawSeg(1);
        }
        else if (Blank_Digit && (Digit_Obejects[digt_ID - 1]->digit == 0)) {
            Digit_Obejects[digt_ID]->drawSeg(1);
            Digit_Obejects[digt_ID - 1]->drawSeg(1);
        }
        pos = newPos;
    }
    ScreenPoint sp;
    if (ts.touched()) { //Check wich screen object was touched 
        p = ts.getPoint();
        sp = getScreenCoords(p.x, p.y);
        if (buttonPressed_Handler(Calc_BUTTON_X, Calc_BUTTON_Y, Calc_BUTTON_W, Calc_BUTTON_H, sp, &Calculate_BTN) == 1) { //Check if Calc button was touched 
            Calc_clickButton = true;
            Concatenate_Number();
            //RenderSend_Nunber();
        }
        Deselec_SegDigits(); //Clear all Selected and hilighted digit frame
        Rotary_enabled = false;
        for (int i = 0; i < 4; i++) //Check which Digit Object was selected
        {
            Digit_Obejects[i]->IsChecked(sp);
            if (Digit_Obejects[i]->Flag == true) {
                Rotary_enabled = true;
                digt_ID = i;  // Identify wich digit was selected
                Number = Digit_Obejects[digt_ID]->digit;
                /*Serial.print(" Digit ");
                Serial.print(digt_ID + 1);
                Serial.print(", Number = ");
                Serial.println(Digit_Obejects[digt_ID]->digit);*/
            }
        }
    }
    else if (Calc_clickButton) {
        Calculate_BTN.drawButton();
        Calc_clickButton = false;
    }
}
