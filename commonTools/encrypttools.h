#ifndef ENCRYPTTOOLS_H
#define ENCRYPTTOOLS_H


#include <QApplication>
#include <QMainWindow>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QInputDialog>
#include <QCryptographicHash>
#include <QDebug>



// ========== 配置部分 ==========
static const unsigned char OBFUSCATED_HASH[] = {
0x54, 0x07, 0x07, 0x00, 0x51, 0x04, 0x57, 0x00, 0x53, 0x00, 0x51, 0x03, 0x03, 0x56, 0x00, 0x53, 0x53, 0x54, 0x57, 0x53, 0x03, 0x0D, 0x02, 0x0D, 0x0D, 0x04, 0x54, 0x06, 0x06, 0x07, 0x51, 0x54, 0x0C, 0x0D, 0x06, 0x56, 0x0D, 0x51, 0x02, 0x56, 0x03, 0x00, 0x01, 0x04, 0x57, 0x01, 0x56, 0x02, 0x51, 0x50, 0x00, 0x57, 0x03, 0x00, 0x57, 0x05, 0x02, 0x03, 0x02, 0x56, 0x02, 0x05, 0x06, 0x04,
};
static const size_t OBFUSCATED_HASH_SIZE = sizeof(OBFUSCATED_HASH)/sizeof(OBFUSCATED_HASH[0]);

// 同样的异或KEY
static const quint8 XOR_KEY = 0x35;

// 函数：把 OBFUSCATED_HASH[] 还原成“原先的 64字节HEX串”
static QString getAdminHashHex()
{
    QByteArray recovered;
    recovered.reserve(OBFUSCATED_HASH_SIZE);
    for (size_t i=0; i<OBFUSCATED_HASH_SIZE; i++) {
        unsigned char c = OBFUSCATED_HASH[i] ^ XOR_KEY;
        recovered.append(static_cast<char>(c));
    }
    // 这里的 recovered 就是当初的 64字节 ascii
    return QString::fromLatin1(recovered);
}


// 用于哈希时加一点 Salt
static const QByteArray SALT = "MY_SALT_1234";

// 存储起点时间的文件名（与 exe 同路径）
static const QString LICENSE_FILE_NAME = QStringLiteral("license.dat");

// 超时时长：1 个月(30天)的毫秒数
static const qint64 MAX_MSECS = 30ll * 24ll * 60ll * 60ll * 1000ll;
//static const qint64 MAX_MSECS = 30 * 1000ll;

static const QString DONT_TRY_DECREPT = "don't be so rude, my friend";



// ========== 工具函数 ==========

// 把 QDateTime -> ISO8601 字符串，例如 "2025-01-14T12:34:56"
static QString dateTimeToString(const QDateTime &dt)
{
    return dt.toString(Qt::ISODate);
}

// 从字符串恢复成 QDateTime
static QDateTime stringToDateTime(const QString &str)
{
    return QDateTime::fromString(str, Qt::ISODate);
}

// 简单异或加密/解密
static QByteArray xorEncrypt(const QByteArray &plain)
{
    QByteArray result;
    result.reserve(plain.size());
    for (char c : plain) {
        result.append(c ^ XOR_KEY);
    }
    return result;
}

// 生成哈希：SHA256( SALT + xorData )
static QByteArray generateHash(const QByteArray &xorData)
{
    QByteArray raw = SALT + xorData;
    QByteArray hash = QCryptographicHash::hash(raw, QCryptographicHash::Sha256);
    return hash.toHex(); // 转为 hex 便于存储
}

// 写入文件：写入两行
//   第1行：异或后的起点时间的 Base64
//   第2行：哈希( hex )
static bool writeLicenseFile(const QDateTime &startTime)
{
    // 1) 把起点时间转字符串
    QString timeStr = dateTimeToString(startTime);
    if (!startTime.isValid() || timeStr.isEmpty()) {
        qWarning() << "[writeLicenseFile] invalid startTime";
        return false;
    }
    // 2) 异或加密
    QByteArray xorData = xorEncrypt(timeStr.toUtf8());
    QByteArray xorBase64 = xorData.toBase64();
    // 3) 生成哈希
    QByteArray hashHex = generateHash(xorData);

    // 4) 写文件
    QString filePath = QCoreApplication::applicationDirPath() + QDir::separator() + LICENSE_FILE_NAME;
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate )) {
        qWarning() << "[writeLicenseFile] cannot open for write:" << filePath;
        return false;
    }
    file.write(xorBase64);
    file.write("\n");
    file.write(hashHex);
    file.close();

    return true;
}

// 从文件读取起点时间：
//   第1行 base64 -> 异或后字节 -> 再异或还原出真正 timeStr
//   第2行 hashHex -> 和我们现算的哈希做对比
static QDateTime readLicenseFile(bool &ok)
{
    ok = false;

    QString filePath = QCoreApplication::applicationDirPath() + QDir::separator() + LICENSE_FILE_NAME;
    QFile file(filePath);
    if(!file.exists()) {
        qWarning() << "[readLicenseFile] file not exist:" << filePath;
        return QDateTime(); // 不存在
    }
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[readLicenseFile] cannot open file:" << filePath;
        return QDateTime();
    }

    QByteArray line1 = file.readLine().trimmed(); // xorBase64
    QByteArray line2 = file.readLine().trimmed(); // hashHex
    file.close();

    if(line1.isEmpty() || line2.isEmpty()) {
        qWarning() << "[readLicenseFile] empty lines, file corrupted";
        return QDateTime();
    }

    // 解出异或后的字节
    QByteArray xorData = QByteArray::fromBase64(line1);
    // 计算哈希
    QByteArray expectedHash = generateHash(xorData);
    if(expectedHash != line2) {
        qWarning() << "[readLicenseFile] hash mismatch, file tampered";
        // 哈希不匹配，说明被篡改
        return QDateTime();
    }

    // 异或还原明文
    QByteArray plain = xorEncrypt(xorData);
    QString timeStr = QString::fromUtf8(plain);
    QDateTime dt = stringToDateTime(timeStr);

    if(!dt.isValid()) {
        qWarning() << "[readLicenseFile] dateTime parse fail, file corrupted?";
        return QDateTime();
    }

    ok = true;
    return dt;
}


// 弹窗输入密码，返回是否通过
static bool requestAdminPassword(const QString &msg, QWidget *parent = nullptr)
{
    bool ok = false;
    QString input = QInputDialog::getText(parent,
                                          QStringLiteral("需要最高权限"),
                                          msg,
                                          QLineEdit::Password,
                                          QString(),
                                          &ok);
    if(!ok || input.isEmpty()) {
        return false;
    }

    // 计算用户输入的 SHA256，然后转hex
    QByteArray inputHash = QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Sha256).toHex();

    // 还原出当初生成的哈希(hex串)
    QString storedHashHex = getAdminHashHex();

    // 对比
    if (QString::fromLatin1(inputHash) == storedHashHex) {
        return true;
    }
    return false;
}
#endif // ENCRYPTTOOLS_H
