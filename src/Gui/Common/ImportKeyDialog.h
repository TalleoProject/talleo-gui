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
  class ImportKeyDialog;
}

namespace WalletGui {

class ImportKeyDialog
  : public QDialog {
  Q_OBJECT
  Q_DISABLE_COPY(ImportKeyDialog)

public:
  ImportKeyDialog(QWidget *_parent, bool _isTracking, bool _isSeedImport, bool _isPrivateKeysImport);
  ~ImportKeyDialog();

  QByteArray getKey() const;
  static bool isTrackingKeys(const QByteArray& _array);

private:
  QScopedPointer<Ui::ImportKeyDialog> m_ui;
  bool m_isTracking;
  bool m_isSeedImport;
  bool m_isPrivateKeysImport;
  QByteArray m_key;

  void loadKey();

  Q_SLOT void fileClicked();
  Q_SLOT void keyChanged();
};

}
