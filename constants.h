#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QSize>

namespace constants {
const int kWindowWidth = 500;          // Ширина окна
const int kWindowHeight = 700;         // Высота окна
const int kMaxWindowSize = 16777215;   // Максимальный размер окна
const int kButtonSize = 20;            // Размер кнопок
const int kPenWidthButtonWidth = 30;   // Ширина кнопки толщины пера
const int kLabelWidth = 70;            // Ширина меток статуса
const int kLabelHeight = 20;           // Высота меток статуса
const int kFontSizeSmall = 7;          // Малый размер шрифта
const int kFontSizeMedium = 8;         // Средний размер шрифта
const int kFontSizeNormal = 10;        // Нормальный размер шрифта
const int kFontSizeLarge = 12;         // Большой размер шрифта
const int kFontSizeExtraLarge = 14;    // Очень большой размер шрифта

// Таймеры
const int kTimerIntervalMs = 50;        // Интервал таймеров в миллисекундах

// Диапазоны для ввода количества точек
const int kMinPointsCount = 5;          // Минимальное количество точек для звезды/многоугольника
const int kMaxStarPointsCount = 1000;   // Максимальное количество точек для звезды
const int kMaxPolygonPointsCount = 10000; // Максимальное количество точек для многоугольника
const int kPointsCountStep = 1;         // Шаг изменения количества точек

// Диапазоны для толщины пера
const qreal kMinPenWidth = 0.1;         // Минимальная толщина пера
const qreal kMaxPenWidth = 999.0;       // Максимальная толщина пера
const int kPenWidthDecimals = 1;        // Количество знаков после запятой для толщины пера
}


#endif // CONSTANTS_H
