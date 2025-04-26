#include "towerofadversitywidget.h"
#include "ui_towerofadversitywidget.h"

TowerOfAdversityWidget::TowerOfAdversityWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TowerOfAdversityWidget)
{
    TowerBattleDataManager& towerBattleDataManager = TowerBattleDataManager::Instance();
    //构造队伍信息
    initTeamInfoMap();
    initTeamCharactorMap();

    // 初始化UI
    ui->setupUi(this);

    // 填充到UI
    for(auto pair : m_teamInfoMap.keys()){
        ui->supportTeam->addItem(towerBattleDataManager.teamEnum2QString(pair));
        ui->teamInfo->setText(m_teamInfoMap[pair]);
    }

    ui->readme->setText(QString("教学工具并不能帮你完成所有操作。你需要手动完成开始游戏和闪避等。\n可以动脑多多思考"));

    // ============= 线程管理 =============
    // 采图线程相关
    m_imageCapturer.moveToThread(&m_imageCapturerThread);
    m_imageCapturerThread.start();
    m_imageCapturerMonitor.moveToThread(&m_imageCapturerMonitorThread);
    m_imageCapturerMonitorThread.start();

    // 启动采图线程
    connect(this, &TowerOfAdversityWidget::start_capturer, &this->m_imageCapturer, &ImageCapturer::on_start_capturer);
    connect(this, &TowerOfAdversityWidget::start_capturerMonitor, &this->m_imageCapturerMonitor, &ImageCapturerMonitor::on_start_monitor);

    // 接收监视器发来的新图像
    connect(&this->m_imageCapturerMonitor, &ImageCapturerMonitor::updateGameMonitorStatus, this, &TowerOfAdversityWidget::on_updateGameMonitorStatus);

    // 配队当前角色1 2 3
    m_teamIdxRecognitor.moveToThread(&m_teamIdxRecognitorThread);
    m_teamIdxRecognitorThread.start();
    m_teamIdxRecognitionMonitor.moveToThread(&m_teamIdxRecognitonMonitorThread);
    m_teamIdxRecognitonMonitorThread.start();

    connect(this, &TowerOfAdversityWidget::start_teamIdxRecognitor, &this->m_teamIdxRecognitor, &TeamIdxRecognitor::on_start_teamIdxRecognition);
    connect(this, &TowerOfAdversityWidget::start_teamIdxRecognitonMonitor, &this->m_teamIdxRecognitionMonitor, &TeamIdxRecognitionMonitor::on_start_teamIdxRecognitorMonitor);

    // 接收更新的当前角色idx
    connect(&this->m_teamIdxRecognitionMonitor, &TeamIdxRecognitionMonitor::updateCurrentTeamIdx, this, &TowerOfAdversityWidget::on_updateCurrentTeamIdx);

}

TowerOfAdversityWidget::~TowerOfAdversityWidget()
{
    if(m_imageCapturerThread.isRunning()){
        m_imageCapturer.stop();
        m_imageCapturerThread.quit();
        m_imageCapturerThread.wait();
    }

    if(m_imageCapturerMonitorThread.isRunning()){
        m_imageCapturerMonitor.stop();
        m_imageCapturerMonitorThread.quit();
        m_imageCapturerMonitorThread.wait();
    }

    if(m_teamIdxRecognitorThread.isRunning()){
        m_teamIdxRecognitor.stop();
        m_teamIdxRecognitorThread.quit();
        m_teamIdxRecognitorThread.wait();
    }

    if(m_teamIdxRecognitonMonitorThread.isRunning()){
        m_teamIdxRecognitionMonitor.stop();
        m_teamIdxRecognitonMonitorThread.quit();
        m_teamIdxRecognitonMonitorThread.wait();
    }

    delete ui;
}


void TowerOfAdversityWidget::initTeamInfoMap(){
    QString Camellya_Sanhua_Shorekeeper_str;
    Camellya_Sanhua_Shorekeeper_str = QString("1号必须是椿 2号必须是散华 3号必须是守岸人\n");
    Camellya_Sanhua_Shorekeeper_str += QString("椿 要求人物武器90级 00（千古）或01-声骸43311-暴击率65-爆伤250\n");
    Camellya_Sanhua_Shorekeeper_str += QString("散华 要求人物武器70级 至少5链-共鸣效率120%-生命值10000\n");
    Camellya_Sanhua_Shorekeeper_str += QString("守 要求人物武器70级 0链奇幻变奏或01-共鸣效率230%-4C必须无归谬误-生命值20000\n");
    Camellya_Sanhua_Shorekeeper_str += QString("配置越高越好，越高越容易过关。\n");
    Camellya_Sanhua_Shorekeeper_str += QString("手法讲解:\n\n");
    Camellya_Sanhua_Shorekeeper_str += QString("启动轴 \n");
    Camellya_Sanhua_Shorekeeper_str += QString("1椿-EQ\n");
    Camellya_Sanhua_Shorekeeper_str += QString("2散-ERZ\n");
    Camellya_Sanhua_Shorekeeper_str += QString("3守-AAAAZ-AAAAZ-EQR↓\n");
    Camellya_Sanhua_Shorekeeper_str += QString("1椿-EAAAAAAA\n");
    Camellya_Sanhua_Shorekeeper_str += QString("2散-EAQ↓\n");
    Camellya_Sanhua_Shorekeeper_str += QString("1椿-AAAE(一日花)R-AAAAAAAAAA↓\n\n");

    Camellya_Sanhua_Shorekeeper_str += QString("循环轴 \n");
    Camellya_Sanhua_Shorekeeper_str += QString("3守-被动\n");
    Camellya_Sanhua_Shorekeeper_str += QString("2散-ERZ\n");
    Camellya_Sanhua_Shorekeeper_str += QString("3守-AAAAZ-EQR↓\n");
    Camellya_Sanhua_Shorekeeper_str += QString("1椿-EAAAAAAA\n");
    Camellya_Sanhua_Shorekeeper_str += QString("2散-EQ↓\n");
    Camellya_Sanhua_Shorekeeper_str += QString("1椿-AAAE(一日花)R-AAAAAAAAAA↓\n\n");
    m_teamInfoMap[TowerBattleDataManager::Team::Camellya_Sanhua_Shorekeeper] = Camellya_Sanhua_Shorekeeper_str;

}



void TowerOfAdversityWidget::initTeamCharactorMap(){
    m_teamCharactorMap[TowerBattleDataManager::Team::Camellya_Sanhua_Shorekeeper] =\
    {TowerBattleDataManager::Charactor::UNDEFINED, \
     TowerBattleDataManager::Charactor::Camellya, \
     TowerBattleDataManager::Charactor::Sanhua, \
     TowerBattleDataManager::Charactor::Shorekeeper};
}

void TowerOfAdversityWidget::on_supportTeam_currentTextChanged(const QString &text){
    TowerBattleDataManager& towerBattleDataManager = TowerBattleDataManager::Instance();
    bool isFound = false;
    for(auto key : m_teamInfoMap.keys()){
        if(text == towerBattleDataManager.teamEnum2QString(key)){
            QVector<TowerBattleDataManager::Charactor> selectTeam = m_teamCharactorMap[key];
            towerBattleDataManager.setCurrentTeamVec(selectTeam);
            isFound = true;
            break;
        }
    }

    if(!isFound){
        qCritical() << QString("选择配队 %1 非法，尚未支持").arg(text);
    }

    return;
}


void TowerOfAdversityWidget::on_startButton_clicked(){
    qInfo() << QString("深塔教学工具 启动");
    emit start_capturer();
    emit start_capturerMonitor(&this->m_imageCapturer);

    emit start_teamIdxRecognitor();
    emit start_teamIdxRecognitonMonitor(&this->m_teamIdxRecognitor);

}

void TowerOfAdversityWidget::onStop(){
    m_imageCapturer.stop();
    //m_imageCapturerMonitor.stop();

    m_teamIdxRecognitor.stop();

}


void TowerOfAdversityWidget::on_updateGameMonitorStatus(const bool& isBusy, const cv::Mat& mat){
    if(!isBusy){
        QColor dynamicColor(128, 128, 128); // 灰色
        ui->gameMonitorStatus->setStyleSheet(
            QString("background-color: %1;").arg(dynamicColor.name())
        );
    }
    else{
        //TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
        //cv::Mat lastCapImg = dataManager.getLastCapImg();
        Utils::displayMatOnLabel(ui->gameMonitor, Utils::cvMat2QImage(mat));
        QColor dynamicColor(0, 255, 0); // 绿色
        ui->gameMonitorStatus->setStyleSheet(
            QString("background-color: %1;").arg(dynamicColor.name())
        );
    }
}

void TowerOfAdversityWidget::on_updateCurrentTeamIdx(const int& idx){
    ui->currentIdx->setValue(idx);
}










