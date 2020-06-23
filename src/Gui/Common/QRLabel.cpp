// Copyright (c) 2011-2015 The Cryptonote developers
// Copyright (c) 2015 XDN developers
// Copyright (c) 2020 The Talleo developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <QImage>
#include <QStringList>

#include "qrencode.h"
#include "CryptoNoteWalletConfig.h"

#include "QRLabel.h"

namespace WalletGui {

QRLabel::QRLabel(QWidget* _parent) : QLabel(_parent) {
}

QRLabel::~QRLabel() {
}

void QRLabel::showQRCode(const QString& _dataString) {
  QRcode *qrcode = QRcode_encodeString(_dataString.toUtf8().constData(), 1, QR_ECLEVEL_L, QR_MODE_8, true);
  if (qrcode == nullptr) {
    return;
  }

  QImage qrCodeImage = QImage(qrcode->width + 8, qrcode->width + 8, QImage::Format_RGB32);
  qrCodeImage.fill(Qt::white);
  unsigned char *p = qrcode->data;
  for (int y = 0; y < qrcode->width; y++) {
    for (int x = 0; x < qrcode->width; x++) {
      if (*p & 1) {
        qrCodeImage.setPixelColor(x + 4, y + 4, Qt::black);
      }
      p++;
    }
  }

  QRcode_free(qrcode);
  setPixmap(QPixmap::fromImage(qrCodeImage).scaled(100, 100));
  setEnabled(true);
}

void QRLabel::setAddress(const QString& _address) {
  m_address = _address;
  if (_address.isEmpty()) {
    clear();
  } else {
    QStringList uri;
    uri.append(QString(BYTECOIN_URI_SCHEME_NAME));
    uri.append(m_address);
    showQRCode(uri.join(":"));
  }
}

const QString& QRLabel::address() {
  return m_address;
}

}
