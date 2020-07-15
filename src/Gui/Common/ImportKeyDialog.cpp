// Copyright (c) 2015-2018, The Bytecoin developers
// Copyright (c) 2018, The PinkstarcoinV2 developers
// Copyright (c) 2018, The Bittorium developers
// Copyright (c) 2019-2020, The Talleo developers
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

#include <QFileDialog>
#include <QMessageBox>

#include "ImportKeyDialog.h"
#include "IWalletAdapter.h"
#include "Common/StringTools.h"
#include "CryptoNoteCore/Account.h"
#include "Mnemonics/electrum-words.h"
#include "Settings/Settings.h"
#include "Style/Style.h"
#include "ui_ImportKeyDialog.h"

namespace WalletGui {

namespace {

const char IMPORT_KEY_DIALOG_STYLE_SHEET_TEMPLATE[] =
  "* {"
    "font-family: %fontFamily%;"
  "}"

  "WalletGui--KeyDialog #m_spendKey {"
    "font-size: %fontSizeNormal%;"
    "border-radius: 2px;"
    "border: 1px solid %borderColorDark%;"
  "}"

  "WalletGui--KeyDialog #m_viewKey {"
    "font-size: %fontSizeNormal%;"
    "border-radius: 2px;"
    "border: 1px solid %borderColorDark%;"
  "}"

  "WalletGui--KeyDialog #m_keyEdit {"
    "font-size: %fontSizeNormal%;"
    "border-radius: 2px;"
    "border: 1px solid %borderColorDark%;"
  "}";

}

ImportKeyDialog::ImportKeyDialog(QWidget* _parent, bool _isTracking, bool _isSeed, bool _isPrivateKeys)
  : QDialog(_parent, static_cast<Qt::WindowFlags>(Qt::WindowCloseButtonHint))
  , m_ui(new Ui::ImportKeyDialog)
  , m_isTracking(_isTracking)
  , m_isSeedImport(_isSeed)
  , m_isPrivateKeysImport(_isPrivateKeys) {
  m_ui->setupUi(this);
  setStyleSheet(Settings::instance().getCurrentStyle().makeStyleSheet(IMPORT_KEY_DIALOG_STYLE_SHEET_TEMPLATE));
  if (m_isSeedImport) {
    setWindowTitle(tr("Import mnemonic seed"));
    m_ui->m_fileButton->hide();
    m_ui->m_keysFrame->hide();
  } else if (m_isPrivateKeysImport) {
    setWindowTitle(tr("Import private keys"));
    m_ui->m_fileButton->hide();
    m_ui->m_keyEdit->hide();
  } else {
    m_ui->m_fileButton->setText(tr("Load from file"));
    m_ui->m_keysFrame->hide();

    setWindowTitle(_isTracking ? tr("Import tracking key") : tr("Import key"));
  }

  setFixedHeight(195);
}

ImportKeyDialog::~ImportKeyDialog() {
}

bool ImportKeyDialog::isTrackingKeys(const QByteArray& _array) {
  if (_array.size() < sizeof(AccountKeys)) {
    return false;
  }

  AccountKeys accountKeys;
  QDataStream trackingKeysDataStream(_array);
  trackingKeysDataStream.readRawData(reinterpret_cast<char*>(&accountKeys.spendKeys.publicKey), sizeof(Crypto::PublicKey));
  trackingKeysDataStream.readRawData(reinterpret_cast<char*>(&accountKeys.viewKeys.publicKey), sizeof(Crypto::PublicKey));
  trackingKeysDataStream.readRawData(reinterpret_cast<char*>(&accountKeys.spendKeys.secretKey), sizeof(Crypto::SecretKey));
  trackingKeysDataStream.readRawData(reinterpret_cast<char*>(&accountKeys.viewKeys.secretKey), sizeof(Crypto::SecretKey));
  return (std::memcmp(&accountKeys.spendKeys.secretKey, &CryptoNote::NULL_SECRET_KEY, sizeof(Crypto::SecretKey)) == 0);
}

QByteArray ImportKeyDialog::getKey() const {
  if (m_isPrivateKeysImport) {
    QByteArray spendKey = m_ui->m_spendKey->toPlainText().toUtf8();
    QByteArray viewKey = m_ui->m_viewKey->toPlainText().toUtf8();
    AccountKeys accountKeys;
    if (Common::podFromHex(spendKey.constData(), accountKeys.spendKeys.secretKey) &&
        Common::podFromHex(viewKey.constData(), accountKeys.viewKeys.secretKey)) {
      Crypto::secret_key_to_public_key(accountKeys.spendKeys.secretKey, accountKeys.spendKeys.publicKey);
      Crypto::secret_key_to_public_key(accountKeys.viewKeys.secretKey, accountKeys.viewKeys.publicKey);
      QByteArray _keys;
      QDataStream keysDataStream(&_keys, QIODevice::WriteOnly);
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.spendKeys.publicKey), sizeof(Crypto::PublicKey));
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.viewKeys.publicKey), sizeof(Crypto::PublicKey));
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.spendKeys.secretKey), sizeof(Crypto::SecretKey));
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.viewKeys.secretKey), sizeof(Crypto::SecretKey));
      return _keys;
    } else {
      return QByteArray();
    }
  }
  if (m_isSeedImport) {
    QByteArray seed = m_ui->m_keyEdit->toPlainText().toUtf8();
    AccountKeys accountKeys;
    if (crypto::ElectrumWords::is_valid_mnemonic(seed.constData(), accountKeys.spendKeys.secretKey)) {
      CryptoNote::AccountBase::generateViewFromSpend(accountKeys.spendKeys.secretKey, accountKeys.viewKeys.secretKey);
      Crypto::secret_key_to_public_key(accountKeys.spendKeys.secretKey, accountKeys.spendKeys.publicKey);
      Crypto::secret_key_to_public_key(accountKeys.viewKeys.secretKey, accountKeys.viewKeys.publicKey);
      QByteArray _keys;
      QDataStream keysDataStream(&_keys, QIODevice::WriteOnly);
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.spendKeys.publicKey), sizeof(Crypto::PublicKey));
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.viewKeys.publicKey), sizeof(Crypto::PublicKey));
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.spendKeys.secretKey), sizeof(Crypto::SecretKey));
      keysDataStream.writeRawData(reinterpret_cast<char*>(&accountKeys.viewKeys.secretKey), sizeof(Crypto::SecretKey));
      return _keys;
    } else {
      return QByteArray();
    }
  }
  return QByteArray::fromHex(m_ui->m_keyEdit->toPlainText().toLatin1());
}

void ImportKeyDialog::loadKey() {
  QString filePath = QFileDialog::getOpenFileName(this, m_isTracking ? tr("Load tracking key from...") : tr("Load key from..."),
#ifdef Q_OS_WIN
    QApplication::applicationDirPath(),
#else
    QDir::homePath(),
#endif
    m_isTracking ? tr("Tracking key (*.trackingkey)") : tr("Wallet key (*.walletkey)"));
  if (filePath.isEmpty()) {
    return;
  }

  QFile keyFile(filePath);
  if (!keyFile.open(QIODevice::ReadOnly)) {
    return;
  }

  m_key = keyFile.readAll();
  keyFile.close();
  m_ui->m_keyEdit->setPlainText(m_key.toHex().toUpper());
}

void ImportKeyDialog::fileClicked() {
  loadKey();
}

void ImportKeyDialog::keyChanged() {
  m_isTracking = isTrackingKeys(getKey());
  if (m_isSeedImport) {
     setWindowTitle(tr("Import mnemonic seed"));
  } else if (m_isPrivateKeysImport) {
     setWindowTitle(tr("Import private keys"));
  } else {
    if (m_isTracking) {
      setWindowTitle(tr("Import tracking key"));
      m_ui->m_descriptionLabel->setText(tr("Import a tracking key of a wallet to see all its incoming transactions.\n"
        "It doesn't allow spending funds."));
    } else {
      setWindowTitle(tr("Import key"));
      m_ui->m_descriptionLabel->clear();
    }
  }
}

}
