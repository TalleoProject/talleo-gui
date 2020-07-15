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

#include "ExportKeyDialog.h"
#include "IWalletAdapter.h"
#include "Common/StringTools.h"
#include "CryptoNoteCore/Account.h"
#include "Mnemonics/electrum-words.h"
#include "Settings/Settings.h"
#include "Style/Style.h"
#include "ui_ExportKeyDialog.h"

namespace WalletGui {

namespace {

const char EXPORT_KEY_DIALOG_STYLE_SHEET_TEMPLATE[] =
  "* {"
    "font-family: %fontFamily%;"
  "}"

  "WalletGui--ExportKeyDialog #m_keyEdit {"
    "font-size: %fontSizeNormal%;"
    "border-radius: 2px;"
    "border: 1px solid %borderColorDark%;"
  "}";

}

ExportKeyDialog::ExportKeyDialog(const QByteArray& _key, bool _isTracking, QWidget *_parent)
  : QDialog(_parent, static_cast<Qt::WindowFlags>(Qt::WindowCloseButtonHint))
  , m_ui(new Ui::ExportKeyDialog)
  , m_isTracking(_isTracking)
  , m_isPrivateKeyExport(false)
  , m_key(_key) {
  m_ui->setupUi(this);
  setWindowTitle(m_isTracking ? tr("Export tracking key") : tr("Export key"));
  m_ui->m_fileButton->setText(tr("Save to file"));
  m_ui->m_okButton->setText(tr("Close"));
  m_ui->m_keyEdit->setReadOnly(true);
  m_ui->m_keyEdit->setPlainText(m_key.toHex().toUpper());

  m_ui->m_cancelButton->hide();
  setFixedHeight(195);
  setStyleSheet(Settings::instance().getCurrentStyle().makeStyleSheet(EXPORT_KEY_DIALOG_STYLE_SHEET_TEMPLATE));
}

ExportKeyDialog::ExportKeyDialog(const QByteArray& _key, bool _isTracking, bool _isPrivateKeyExport, QWidget *_parent)
	: QDialog(_parent, static_cast<Qt::WindowFlags>(Qt::WindowCloseButtonHint))
	, m_ui(new Ui::ExportKeyDialog)
	, m_isTracking(_isTracking)
	, m_isPrivateKeyExport(true)
	, m_key(_key) {
	m_ui->setupUi(this);
	m_ui->m_fileButton->hide();
	m_ui->m_okButton->setText(tr("Close"));
	m_ui->m_keyEdit->setReadOnly(true);
	QString secretSpendKey = m_key.toHex().toUpper().mid(0,64);
	QString secretViewKey = m_key.toHex().toUpper().mid(64);
	QString keys = "Secret spend key:\n" + secretSpendKey + "\n\nSecret view key:\n" + secretViewKey;
	//
	Crypto::SecretKey privateSpendKey, privateViewKey;
	if (Common::podFromHex(secretSpendKey.toStdString(), privateSpendKey) && Common::podFromHex(secretViewKey.toStdString(), privateViewKey)) {
		Crypto::SecretKey derivedPrivateViewKey;

		CryptoNote::AccountBase::generateViewFromSpend(privateSpendKey, derivedPrivateViewKey);

		if (derivedPrivateViewKey == privateViewKey) {
			std::string mnemonicSeed;

			crypto::ElectrumWords::bytes_to_words(privateSpendKey, mnemonicSeed, "English");

			keys += "\n\nMnemonic seed:\n" + QString::fromStdString(mnemonicSeed);
		}
	}
	//
	m_ui->m_keyEdit->setPlainText(keys);

	m_ui->m_cancelButton->hide();
	setFixedHeight(195);
	setStyleSheet(Settings::instance().getCurrentStyle().makeStyleSheet(EXPORT_KEY_DIALOG_STYLE_SHEET_TEMPLATE));
	setWindowTitle(tr("Export secret keys"));
}

ExportKeyDialog::~ExportKeyDialog() {
}

bool ExportKeyDialog::isTrackingKeys(const QByteArray& _array) {
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

QByteArray ExportKeyDialog::getKey() const {
  return QByteArray::fromHex(m_ui->m_keyEdit->toPlainText().toLatin1());
}

void ExportKeyDialog::saveKey() {
  QString filePath = QFileDialog::getSaveFileName(this, m_isTracking ? tr("Save tracking key to...") : tr("Save key to..."),
#ifdef Q_OS_WIN
    QApplication::applicationDirPath(),
#else
    QDir::homePath(),
#endif
    m_isTracking ? tr("Tracking key (*.trackingkey)") : tr("Wallet key (*.walletkey)"));
  if (filePath.isEmpty()) {
    return;
  }

  if (m_isTracking && !filePath.endsWith(".trackingkey")) {
    filePath.append(".trackingkey");
  } else if (!m_isTracking && !filePath.endsWith(".walletkey")) {
    filePath.append(".walletkey");
  }

  QFile keyFile(filePath);
  if (!keyFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    return;
  }

  keyFile.write(m_key);
  keyFile.close();
}

void ExportKeyDialog::fileClicked() {
  saveKey();
  accept();
}

void ExportKeyDialog::keyChanged() {
  m_isTracking = isTrackingKeys(getKey());
  if (m_isTracking) {
    setWindowTitle(tr("Export tracking key"));
    m_ui->m_descriptionLabel->setText(tr("Tracking key allows other people to see all incoming transactions of this wallet.\n"
      "It doesn't allow spending your funds."));
  } else {
    setWindowTitle(tr("Export key"));
    if (m_isPrivateKeyExport) {
      m_ui->m_descriptionLabel->setText(tr("These keys allow restoration of your wallet in simplewallet."));
    } else {
      m_ui->m_descriptionLabel->clear();
    }
  }
}

}
