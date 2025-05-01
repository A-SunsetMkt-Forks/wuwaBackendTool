#ifndef RESONANCESKILLJUDGER_H
#define RESONANCESKILLJUDGER_H

#include <QObject>
#include <QAtomicInt>
#include <QThread>

#include "utils.h"
#include "towerBattle/towerbattledatamanager.h"

class ResonanceSkillJudger : public QObject
{
    Q_OBJECT
public:
    explicit ResonanceSkillJudger(QObject *parent = nullptr);
    void stop();
    bool isBusy() const ;

signals:

public slots:
    void on_start_resonance_skill_recognition();

private:
    QAtomicInt m_isBusy;

    // 判断散华的共鸣技能
    void SanHua(const cv::Mat& resonanceSkillRoiMat, const cv::Mat& target);
};

#endif // RESONANCESKILLJUDGER_H
