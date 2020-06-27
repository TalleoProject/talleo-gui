// Copyright (c) 2015-2018, The Bytecoin developers
// Copyright (c) 2018, The PinkstarcoinV2 developers
// Copyright (c) 2018, The Bittorium developers
// Copyright (c) 2020, The Talleo developers
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

#pragma once

#include <QDialog>

namespace Ui {
  class KeyDialog;
}

namespace WalletGui {

class KeyDialog
  : public QDialog {
  Q_OBJECT
  Q_DISABLE_COPY(KeyDialog)

public:
  KeyDialog(const QByteArray& _key, bool _isTracking, QWidget *_parent);
  KeyDialog(const QByteArray& _key, bool _isTracking, bool _isPrivateKeyExport, QWidget *_parent);
  KeyDialog(QWidget *_parent, bool _isSeedImport);
  ~KeyDialog();

  QByteArray getKey() const;
  static bool isTrackingKeys(const QByteArray& _array);

private:
  QScopedPointer<Ui::KeyDialog> m_ui;
  bool m_isTracking;
  bool m_isExport;
  bool m_isPrivateKeyExport;
  bool m_isSeedImport;
  QByteArray m_key;

  void saveKey();
  void loadKey();

  Q_SLOT void fileClicked();
  Q_SLOT void keyChanged();
};

}
