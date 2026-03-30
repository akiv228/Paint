#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QSize>

namespace constants {
const int kWindowWidth = 500;          
const int kWindowHeight = 700;         
const int kMaxWindowSize = 16777215;   
const int kButtonSize = 20;            
const int kPenWidthButtonWidth = 30;   
const int kLabelWidth = 70;            
const int kLabelHeight = 20;           
const int kFontSizeSmall = 7;          
const int kFontSizeMedium = 8;         
const int kFontSizeNormal = 10;        
const int kFontSizeLarge = 12;         
const int kFontSizeExtraLarge = 14;    


const int kTimerIntervalMs = 50;        


const int kMinPointsCount = 5;          
const int kMaxStarPointsCount = 1000;   
const int kMaxPolygonPointsCount = 10000; 
const int kPointsCountStep = 1;         


const qreal kMinPenWidth = 0.1;         
const qreal kMaxPenWidth = 999.0;       
const int kPenWidthDecimals = 1;        
}


#endif 
