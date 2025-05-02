#include "concertoenergyjudger.h"

ConcertoEnergyJudger::ConcertoEnergyJudger(QObject *parent) : QObject(parent)
{
    m_isBusy.store(0);
}


void ConcertoEnergyJudger::stop(){
    m_isBusy.store(0);
}


bool ConcertoEnergyJudger::isBusy() const {
    if(m_isBusy.load() == 0 ){
        return false;
    }
    else{
        return true;
    }
}

void ConcertoEnergyJudger::on_start_concertoEnergyJudge(){
    m_isBusy.store(1);

    TowerBattleDataManager& dataManager = TowerBattleDataManager::Instance();
    // 获取当前配队
    QVector<TowerBattleDataManager::Charactor> team = dataManager.getCurrentTeamVec();
    double tickRate = dataManager.getTickRate();
    double sleepMs = 1000.0 / tickRate;

    // 获取此配队所有成员的美术资源
    QMap<TowerBattleDataManager::Charactor, QMap<QString, cv::Mat>> currentTeamRes;
    for(auto charactor : team){
        QMap<QString, cv::Mat> thisCharactorRes;
        thisCharactorRes = dataManager.getArtResByName(charactor);
        currentTeamRes[charactor] = thisCharactorRes;
    }

    while(isBusy()){

        int charactorIdx = dataManager.getCurrentIndex();
        TowerBattleDataManager::Charactor selectCharactor = team[charactorIdx];

        const cv::Rect resonanceSkillRoi = {479, 649, 39, 39};
        // 每个角色的判断依据都不一样
        if(selectCharactor == TowerBattleDataManager::Charactor::UNDEFINED){
            // 无需判断 0号角色占位而已
            dataManager.setConcertoEnergy(0);
            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Sanhua){
            // 散华的颜色是RGB 69 159 223
            const cv::Vec3b stdBrg = {223,159, 69};
            // 获取待判断的圆环
            std::vector<cv::Point> getConcertoRing = Utils::getCircleContour(19, 19, 16);
            if(getConcertoRing.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            cv::Mat toCalcRingMat = lastCapImg(resonanceSkillRoi).clone();
            const double simiThres = 0.8;
            int validPnts = 0;
            int totalPnts = 0;

            // 创建可调整窗口
            //cv::namedWindow("resonanceSkillRoi", cv::WINDOW_NORMAL);
            // 显示图像
            //cv::imshow("resonanceSkillRoi", toCalcRingMat);
            // 按任意键关闭
            //cv::waitKey(0);

            for(auto pnt : getConcertoRing){
                if(pnt.y < 0 || pnt.y >= toCalcRingMat.rows || pnt.x < 0 || pnt.x >= toCalcRingMat.cols){
                    continue;
                }
                else{
                    totalPnts++;
                }
                cv::Vec3b curPntBGR = toCalcRingMat.at<cv::Vec3b>(pnt.y, pnt.x);
                double thisPntSimi = Utils::colorSimilarity(stdBrg, curPntBGR);
                if(thisPntSimi >= simiThres){
                    validPnts++;
                }
            }

            double concertoEnergy = (double)validPnts / (double)totalPnts;   // getConcertoRing.size();
            dataManager.setConcertoEnergy(concertoEnergy);

            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Camellya){
            // 椿的颜色是RGB 239 076 173
            const cv::Vec3b stdBrg = {153, 76, 200};
            // 获取待判断的圆环
            std::vector<cv::Point> getConcertoRing = Utils::getCircleContour(19, 19, 16);
            if(getConcertoRing.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            cv::Mat toCalcRingMat = lastCapImg(resonanceSkillRoi).clone();
            const double simiThres = 0.8;
            int validPnts = 0;
            int totalPnts = 0;

            cv::Mat toCalcRingMatWithMask = toCalcRingMat.clone();
            for(auto pnt : getConcertoRing){
                toCalcRingMatWithMask.at<cv::Vec3b>(pnt.y, pnt.x) = {255,255,255};
            }

            // 创建可调整窗口
            //cv::namedWindow("resonanceSkillRoi", cv::WINDOW_NORMAL);
            // 显示图像
            //cv::imshow("resonanceSkillRoi", toCalcRingMat);
            // 按任意键关闭
            //cv::waitKey(0);

            for(int iPnt = 0; iPnt < getConcertoRing.size(); iPnt++){
                auto pnt = getConcertoRing[iPnt];
                if(pnt.y < 0 || pnt.y >= toCalcRingMat.rows || pnt.x < 0 || pnt.x >= toCalcRingMat.cols){
                    continue;
                }
                else{
                    totalPnts++;
                }
                cv::Vec3b curPntBGR = toCalcRingMat.at<cv::Vec3b>(pnt.y, pnt.x);
                double thisPntSimi = Utils::colorSimilarity(stdBrg, curPntBGR);
                if(thisPntSimi >= simiThres){
                    validPnts++;
                }
            }

            double concertoEnergy = (double)validPnts / (double)totalPnts;   // getConcertoRing.size();
            dataManager.setConcertoEnergy(concertoEnergy);

            QThread::msleep(sleepMs);
            continue;
        }
        else if(selectCharactor == TowerBattleDataManager::Charactor::Shorekeeper){
            // 守的颜色是RGB 216 203 106
            const cv::Vec3b stdBrg = {106, 203, 216};
            // 获取待判断的圆环
            std::vector<cv::Point> getConcertoRing = Utils::getCircleContour(19, 19, 16);
            if(getConcertoRing.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            cv::Mat lastCapImg = dataManager.getLastCapImg();
            if(lastCapImg.empty()){
                QThread::msleep(sleepMs);
                continue;
            }

            cv::Mat toCalcRingMat = lastCapImg(resonanceSkillRoi).clone();
            const double simiThres = 0.8;
            int validPnts = 0;
            int totalPnts = 0;

            cv::Mat toCalcRingMatWithMask = toCalcRingMat.clone();
            for(auto pnt : getConcertoRing){
                toCalcRingMatWithMask.at<cv::Vec3b>(pnt.y, pnt.x) = {255,255,255};
            }

            // 创建可调整窗口
            //cv::namedWindow("resonanceSkillRoi", cv::WINDOW_NORMAL);
            // 显示图像
            //cv::imshow("resonanceSkillRoi", toCalcRingMat);
            // 按任意键关闭
            //cv::waitKey(0);

            for(int iPnt = 0; iPnt < getConcertoRing.size(); iPnt++){
                auto pnt = getConcertoRing[iPnt];
                if(pnt.y < 0 || pnt.y >= toCalcRingMat.rows || pnt.x < 0 || pnt.x >= toCalcRingMat.cols){
                    continue;
                }
                else{
                    totalPnts++;
                }
                cv::Vec3b curPntBGR = toCalcRingMat.at<cv::Vec3b>(pnt.y, pnt.x);
                double thisPntSimi = Utils::colorSimilarity(stdBrg, curPntBGR);
                if(thisPntSimi >= simiThres){
                    validPnts++;
                }
            }

            double concertoEnergy = (double)validPnts / (double)totalPnts;   // getConcertoRing.size();
            dataManager.setConcertoEnergy(concertoEnergy);

            QThread::msleep(sleepMs);
            continue;
        }
        else{
            QThread::msleep(sleepMs);
            continue;
        }

    }


    m_isBusy.store(0);
    return;



}
