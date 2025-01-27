#ifndef CUSTOMDELEGATE_H
#define CUSTOMDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>

// 为日志等级comboBox 各个等级文本设置颜色

class CustomDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    CustomDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        if (index.row() == 0) {
            opt.palette.setColor(QPalette::Text, Qt::black);
            opt.palette.setColor(QPalette::Base, Qt::white);
        } else if (index.row() == 1) {
            opt.palette.setColor(QPalette::Text, Qt::blue);
            opt.palette.setColor(QPalette::Base, Qt::white);
        } else if (index.row() == 2) {
            opt.palette.setColor(QPalette::Text, Qt::yellow);
            opt.palette.setColor(QPalette::Base, Qt::black);
        } else if (index.row() == 3) {
            opt.palette.setColor(QPalette::Text, Qt::red);
            opt.palette.setColor(QPalette::Base, Qt::white);
        }

        QStyledItemDelegate::paint(painter, opt, index);
    }
};

#endif // CUSTOMDELEGATE_H
