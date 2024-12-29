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
        Rover   // 无妄者 二姐
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
    float startDelay = 0.125f;      // 启动时机（秒）

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
            .arg(static_cast<double>(startDelay))
            .arg(ultimateCheckMode == UltimateCheckMode::Mode1 ? "Mode1" : "Mode2")
            .arg(customDungeonLevel);
    }
};
Q_DECLARE_METATYPE(SpecialBossSetting)


#endif // SETTINGPARAMS_H
