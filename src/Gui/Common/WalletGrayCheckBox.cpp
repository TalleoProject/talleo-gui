// Copyright (c) 2015-2018, The Bytecoin developers
// Copyright (c) 2018, The PinkstarcoinV2 developers
// Copyright (c) 2018, The Bittorium developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "WalletGrayCheckBox.h"
#include "Settings/Settings.h"
#include "Style/Style.h"

namespace WalletGui {

const char GRAY_CHECK_STYLE_SHEET_TEMPLATE[] =
  "* {"
    "font-family: %fontFamily%;"
  "}"

  "WalletGui--WalletGrayCheckBox {"
    "color: %fontColorGray%;"
    "font-size: %fontSizeNormal%;"
    "border: none;"
  "}";

WalletGrayCheckBox::WalletGrayCheckBox(QWidget *parent) : QCheckBox(parent) {
  setStyleSheet(Settings::instance().getCurrentStyle().makeStyleSheet(GRAY_CHECK_STYLE_SHEET_TEMPLATE));
}


WalletGrayCheckBox::~WalletGrayCheckBox() {
}

}