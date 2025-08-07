#include "interception.h"
#include "macro_execution.h"
#include "macro_management.h"
#include "message_handler.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <map>

#pragma region keymap

std::map<std::string, InterceptionKeyStroke> keyMap;

void setupKeyMap() {
    keyMap["A"] = { 0x1E, INTERCEPTION_KEY_DOWN };
    keyMap["B"] = { 0x30, INTERCEPTION_KEY_DOWN };
    keyMap["C"] = { 0x2E, INTERCEPTION_KEY_DOWN };
    keyMap["D"] = { 0x20, INTERCEPTION_KEY_DOWN };
    keyMap["E"] = { 0x12, INTERCEPTION_KEY_DOWN };
    keyMap["F"] = { 0x21, INTERCEPTION_KEY_DOWN };
    keyMap["G"] = { 0x22, INTERCEPTION_KEY_DOWN };
    keyMap["H"] = { 0x23, INTERCEPTION_KEY_DOWN };
    keyMap["I"] = { 0x17, INTERCEPTION_KEY_DOWN };
    keyMap["J"] = { 0x24, INTERCEPTION_KEY_DOWN };
    keyMap["K"] = { 0x25, INTERCEPTION_KEY_DOWN };
    keyMap["L"] = { 0x26, INTERCEPTION_KEY_DOWN };
    keyMap["M"] = { 0x32, INTERCEPTION_KEY_DOWN };
    keyMap["N"] = { 0x31, INTERCEPTION_KEY_DOWN };
    keyMap["O"] = { 0x18, INTERCEPTION_KEY_DOWN };
    keyMap["P"] = { 0x19, INTERCEPTION_KEY_DOWN };
    keyMap["Q"] = { 0x10, INTERCEPTION_KEY_DOWN };
    keyMap["R"] = { 0x13, INTERCEPTION_KEY_DOWN };
    keyMap["S"] = { 0x1F, INTERCEPTION_KEY_DOWN };
    keyMap["T"] = { 0x14, INTERCEPTION_KEY_DOWN };
    keyMap["U"] = { 0x16, INTERCEPTION_KEY_DOWN };
    keyMap["V"] = { 0x2F, INTERCEPTION_KEY_DOWN };
    keyMap["W"] = { 0x11, INTERCEPTION_KEY_DOWN };
    keyMap["X"] = { 0x2D, INTERCEPTION_KEY_DOWN };
    keyMap["Y"] = { 0x15, INTERCEPTION_KEY_DOWN };
    keyMap["Z"] = { 0x2C, INTERCEPTION_KEY_DOWN };

    keyMap["1"] = { 0x02, INTERCEPTION_KEY_DOWN };
    keyMap["2"] = { 0x03, INTERCEPTION_KEY_DOWN };
    keyMap["3"] = { 0x04, INTERCEPTION_KEY_DOWN };
    keyMap["4"] = { 0x05, INTERCEPTION_KEY_DOWN };
    keyMap["5"] = { 0x06, INTERCEPTION_KEY_DOWN };
    keyMap["6"] = { 0x07, INTERCEPTION_KEY_DOWN };
    keyMap["7"] = { 0x08, INTERCEPTION_KEY_DOWN };
    keyMap["8"] = { 0x09, INTERCEPTION_KEY_DOWN };
    keyMap["9"] = { 0x0A, INTERCEPTION_KEY_DOWN };
    keyMap["0"] = { 0x0B, INTERCEPTION_KEY_DOWN };

    // Özel Tuşlar
    keyMap["ESC"] = { 0x01, INTERCEPTION_KEY_DOWN };
    keyMap["TAB"] = { 0x0F, INTERCEPTION_KEY_DOWN };
    keyMap["ENTER"] = { 0x1C, INTERCEPTION_KEY_DOWN };
    keyMap["SPACE"] = { 0x39, INTERCEPTION_KEY_DOWN };
    keyMap["BACKSPACE"] = { 0x0E, INTERCEPTION_KEY_DOWN };
    keyMap["CAPS_LOCK"] = { 0x3A, INTERCEPTION_KEY_DOWN };
    keyMap["SHIFT_SOL"] = { 0x2A, INTERCEPTION_KEY_DOWN };
    keyMap["CTRL_SOL"] = { 0x1D, INTERCEPTION_KEY_DOWN };
    keyMap["ALT_SOL"] = { 0x38, INTERCEPTION_KEY_DOWN };

    // Fonksiyon Tuşları
    keyMap["F1"] = { 0x3B, INTERCEPTION_KEY_DOWN };
    keyMap["F2"] = { 0x3C, INTERCEPTION_KEY_DOWN };
    keyMap["F3"] = { 0x3D, INTERCEPTION_KEY_DOWN };
    keyMap["F4"] = { 0x3E, INTERCEPTION_KEY_DOWN };
    keyMap["F5"] = { 0x3F, INTERCEPTION_KEY_DOWN };
    keyMap["F6"] = { 0x40, INTERCEPTION_KEY_DOWN };
    keyMap["F7"] = { 0x41, INTERCEPTION_KEY_DOWN };
    keyMap["F8"] = { 0x42, INTERCEPTION_KEY_DOWN };
    keyMap["F9"] = { 0x43, INTERCEPTION_KEY_DOWN };
    keyMap["F10"] = { 0x44, INTERCEPTION_KEY_DOWN };
    keyMap["F11"] = { 0x57, INTERCEPTION_KEY_DOWN };
    keyMap["F12"] = { 0x58, INTERCEPTION_KEY_DOWN };

    // Yön Tuşları ve Diğerleri
    keyMap["YUKARI"] = { 0x48, INTERCEPTION_KEY_DOWN };
    keyMap["SOL"] = { 0x4B, INTERCEPTION_KEY_DOWN };
    keyMap["SAG"] = { 0x4D, INTERCEPTION_KEY_DOWN };
    keyMap["ASAGI"] = { 0x50, INTERCEPTION_KEY_DOWN };
    keyMap["DELETE"] = { 0x53, INTERCEPTION_KEY_DOWN };

    // Fare Tuşları
    // Not: Fare tuşları için 'state' alanını kullanmak yerine 'flags' alanını kullanırız.
    // Ancak harita yapısı için birleşik bir şekilde tutulması gerekir.
    // 'state' yerine fare tuşlarının olayı için özel kodları kullanacağız.
    keyMap["SOL_TIK"] = { INTERCEPTION_MOUSE_BUTTON_1_DOWN, 0 };
    keyMap["SAG_TIK"] = { INTERCEPTION_MOUSE_BUTTON_2_DOWN, 0 };
    keyMap["ORTA_TIK"] = { INTERCEPTION_MOUSE_BUTTON_3_DOWN, 0 };
    keyMap["MOUSE4"] = { INTERCEPTION_MOUSE_BUTTON_4_DOWN, 0 };
    keyMap["MOUSE5"] = { INTERCEPTION_MOUSE_BUTTON_5_DOWN, 0 };

    // Tuş Bırakma Olayları
    // Her tuşun bırakma olayını da haritaya eklemek, kullanımı kolaylaştırır.
    // Sadece "A_UP" veya "SOL_TIK_UP" gibi anahtarlar kullanabiliriz.
    keyMap["A_UP"] = { 0x1E, INTERCEPTION_KEY_UP };
    keyMap["ENTER_UP"] = { 0x1C, INTERCEPTION_KEY_UP };
    keyMap["SOL_TIK_UP"] = { INTERCEPTION_MOUSE_BUTTON_1_UP, 0 };
    keyMap["MOUSE4_UP"] = { INTERCEPTION_MOUSE_BUTTON_4_UP, 0 };
    keyMap["MOUSE5_UP"] = { INTERCEPTION_MOUSE_BUTTON_5_UP, 0 };
}
#pragma endregion
