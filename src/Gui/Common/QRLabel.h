// Copyright (c) 2011-2015 The Cryptonote developers
// Copyright (c) 2015 XDN developers
// Copyright (c) 2020 The Talleo developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <QLabel>

namespace WalletGui {

class QRLabel : public QLabel {
  Q_OBJECT
  Q_PROPERTY(QString address READ address WRITE setAddress)
public:
  QRLabel(QWidget* _parent);
  ~QRLabel();

  void showQRCode(const QString& _dataString);
  const QString& address();
public slots:
  void setAddress(const QString& _text);
private:
  QString m_address;
};

}
