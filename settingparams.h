#ifndef SETTINGPARAMS_H
#define SETTINGPARAMS_H

// 这个页面存储大量自定义结构体

#include <QString>
#include <QDebug>
#include <QPoint>

struct RebootGameSetting{
    bool isReboot = true;
    QPoint startGameBtnPos = {0, 0};
    unsigned int rebootCycles = 99999;   // 刷怪这么多次后 进行重启
    unsigned int rebootWaitS = 300;

    QString toQString() const {
        return QString(
            "Reboot: %1\n"
            "Reboot cycles: %2\n"
            "Start Game Button Position: (%3, %4)\n"
            "Reboot Wait Time: %5 seconds")
            .arg(isReboot ? "Yes" : "No")
            .arg(rebootCycles)
            .arg(startGameBtnPos.x())
            .arg(startGameBtnPos.y())
            .arg(rebootWaitS);
    }
};
Q_DECLARE_METATYPE(RebootGameSetting)

struct SpecialBossSetting{
    enum SpecialBoss{
        Jue,   // 角 今州岁主
        Rover,   // 无妄者 二姐
        Hecate,    // 赫卡忒
        Frederice   // ​芙露德莉丝
    };

    enum PickupShape {
        Square,  // 方形
        Normal,   // 普通形
        Line,    // 直线
        Spiral    // 蚊香形
    };

    SpecialBoss boss = SpecialBoss::Jue;
    PickupShape pickupShape = PickupShape::Square; // 默认拾取范围方式为方形
    int echoPickupRange = 9;       // 声骸拾取范围
    int spiralPickupRange = 12;       // 蚊香形拾取范围
    bool pressFDuringBattle = false; // 战斗时按F
    int delayAction = 300;          // 动作额外延迟（毫秒）
    bool jumpRecovery = false;      // 跳回血
    bool bossHealthAssist = false;  // BOSS血条辅助判定
    bool startWithoutSwitch = false;// 开启不切人战斗
    float startLeftHP = 0.125f;      // 启动时机（ 剩余血量
    float absorbThres = 0.9f;        // 吸收文字 模板匹配阈值

    enum UltimateCheckMode {
        Mode1,  // 方式一
        Mode2   // 方式二
    };

    UltimateCheckMode ultimateCheckMode = UltimateCheckMode::Mode1; // 默认大招检测方式为方式一
    int customDungeonLevel = 0; // 自选副本等级，默认0表示未选择

    QString toQString() const {
        return QString(
            "Boss: %1\n"
            "Pickup Shape: %2\n"
            "Echo Pickup Range: %3\n"
            "Spiral Pickup Range: %4\n"
            "Press F During Battle: %5\n"
            "Delay Action: %6 ms\n"
            "Jump Recovery: %7\n"
            "Boss Health Assist: %8\n"
            "Start Without Switch: %9\n"
            "Start Delay: %10 s\n"
            "Ultimate Check Mode: %11\n"
            "Custom Dungeon Level: %12")
            .arg(boss == SpecialBoss::Jue ? "Jue" : "Rover")
            .arg(pickupShape == PickupShape::Square ? "Square" :
                 pickupShape == PickupShape::Normal ? "Normal" :
                 pickupShape == PickupShape::Line ? "Line" : "Spiral")
            .arg(echoPickupRange)
            .arg(spiralPickupRange)
            .arg(pressFDuringBattle ? "Yes" : "No")
            .arg(delayAction)
            .arg(jumpRecovery ? "Yes" : "No")
            .arg(bossHealthAssist ? "Yes" : "No")
            .arg(startWithoutSwitch ? "Yes" : "No")
            .arg(static_cast<double>(startLeftHP))
            .arg(ultimateCheckMode == UltimateCheckMode::Mode1 ? "Mode1" : "Mode2")
            .arg(customDungeonLevel);
    }
};
Q_DECLARE_METATYPE(SpecialBossSetting)


// 某个声骸套装的设置
struct SingleEchoSetting{
    bool isDiscardedJudge = true;  // true表示丢弃的 参与判断
    bool isLockJudge = true;      // true表示锁定的 参与判断
    bool isNormalJudge = true;    // true表示普通的 参与判断

    bool islevel2 = false;     // 2星默认不判断
    bool islevel3 = false;     // 3星默认不判断
    bool islevel4 = false;     // 4星默认不判断
    bool islevel5 = false;     // 5星默认不判断

    QMap<int, QVector<QString>> cost2EntryMap;  // 通过cost 1 3 4可以索引得到想要的词条

    QString toQString() const {
        QString result;
        result += "isDiscardedJudge\t" + QString::number(isDiscardedJudge) + "\n";
        result += "isLockJudge\t" + QString::number(isLockJudge) + "\n";
        result += "isNormalJudge\t" + QString::number(isNormalJudge) + "\n";

        result += "islevel2\t" + QString::number(islevel2) + "\n";
        result += "islevel3\t" + QString::number(islevel3) + "\n";
        result += "islevel4\t" + QString::number(islevel4) + "\n";
        result += "islevel5\t" + QString::number(islevel5) + "\n";

        result += "cost2EntryMap\t\n";
        for (auto it = cost2EntryMap.constBegin(); it != cost2EntryMap.constEnd(); ++it) {
            result += QString("\tcost%1\t\n").arg(it.key());
            for (const QString& entry : it.value()) {
                result += QString("\t\t%1\n").arg(entry);
            }
        }

        return result;
    }
};
Q_DECLARE_METATYPE(SingleEchoSetting)

// 总声骸锁定设置
struct LockEchoSetting{
    QMap<QString, SingleEchoSetting> echoSetName2singleSetting; // 通过声骸套装名可以索引得到设置

    QString toQString() const {
        QString result;

        for (auto it = echoSetName2singleSetting.constBegin(); it != echoSetName2singleSetting.constEnd(); ++it) {
            result += QString("echoSetName\t%1\n").arg(it.key());
            result += it.value().toQString();
        }

        return result;
    }

};
Q_DECLARE_METATYPE(LockEchoSetting)

// Boss枚举  少了乌龟
enum class NormalBossEnum {
    DragonOfDirge,            // 叹息古龙
    SentryConstruct,          // 异构武装
    Crownless,                // 无冠者
    Lorelei,                  // 罗蕾莱
    FallacyOfNoReturn,        // 无归的谬误
    FeilianBeringal,          // 飞廉之猩
    ImpermanenceHeron,        // 无常凶鹭
    InfernoRider,             // 燎照之骑
    LampylumenMyriad,         // 辉萤军势
    MechAbomination,          // 机械聚偶
    MourningAix,              // 哀声鸷
    TempestMephis,            // 云闪之鳞
    ThunderingMephis,         // 朔雷之鳞
    BellBorneGeochelone,      // 钟鸣之龟
    NightmareCrownless,       // 梦魇无冠者
    NightmareFeilianBeringal, // 梦魇飞廉之猩
    NightmareImpermanenceHeron, // 梦魇无常凶鹭
    NightmareInfernoRider,    // 梦魇燎照之骑士
    NightmareMourningAix,     // 梦魇哀声鸷
    NightmareTempestMephis,   // 梦魇云闪之鳞
    NightmareThunderingMephis // 梦魇朔雷之鳞
};

// Boss设置结构
struct NormalBossSetting {
    QMap<NormalBossEnum, bool> bossSettings;

    // 默认构造函数，初始化所有Boss为启用
    NormalBossSetting() {
        for (int i = static_cast<int>(NormalBossEnum::DragonOfDirge);
             i <= static_cast<int>(NormalBossEnum::NightmareThunderingMephis); ++i) {
            bossSettings[static_cast<NormalBossEnum>(i)] = false;
        }
    }

    // 将枚举值转换为英文名称
    static QString Enum2QString(NormalBossEnum boss) {
        switch (boss) {
            case NormalBossEnum::DragonOfDirge: return "dragonOfDirge";
            case NormalBossEnum::SentryConstruct: return "sentryConstruct";
            case NormalBossEnum::Crownless: return "crownless";
            case NormalBossEnum::Lorelei: return "lorelei";
            case NormalBossEnum::FallacyOfNoReturn: return "fallacyOfNoReturn";
            case NormalBossEnum::FeilianBeringal: return "feilianBeringal";
            case NormalBossEnum::ImpermanenceHeron: return "impermanenceHeron";
            case NormalBossEnum::InfernoRider: return "infernoRider";
            case NormalBossEnum::LampylumenMyriad: return "lampylumenMyriad";
            case NormalBossEnum::MechAbomination: return "mechAbomination";
            case NormalBossEnum::MourningAix: return "mourningAix";
            case NormalBossEnum::TempestMephis: return "tempestMephis";
            case NormalBossEnum::ThunderingMephis: return "thunderingMephis";
            case NormalBossEnum::BellBorneGeochelone: return "bellBorneGeochelone";
            case NormalBossEnum::NightmareCrownless: return "nightmareCrownless";
            case NormalBossEnum::NightmareFeilianBeringal: return "nightmareFeilianBeringal";
            case NormalBossEnum::NightmareImpermanenceHeron: return "nightmareImpermanenceHeron";
            case NormalBossEnum::NightmareInfernoRider: return "nightmareInfernoRider";
            case NormalBossEnum::NightmareMourningAix: return "nightmareMourningAix";
            case NormalBossEnum::NightmareTempestMephis: return "nightmareTempestMephis";
            case NormalBossEnum::NightmareThunderingMephis: return "nightmareThunderingMephis";
            default: return "unknown";
        }
    }

    // 转换为 QString 的方法
    QString toQString() const {
        QString result;
        for (auto it = bossSettings.begin(); it != bossSettings.end(); ++it) {
            result += Enum2QString(it.key()) + ": " + (it.value() ? "是" : "否") + "\n";
        }
        return result;
    }

    // 查询某个Boss是否启用
    bool isBossEnabled(NormalBossEnum boss) const {
        return bossSettings.value(boss, false);
    }

    // 设置某个Boss的启用状态
    void setBossEnabled(NormalBossEnum boss, bool enabled) {
        bossSettings[boss] = enabled;
    }
};

Q_DECLARE_METATYPE(NormalBossSetting)


#endif // SETTINGPARAMS_H
