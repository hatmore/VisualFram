#pragma execution_character_set("utf-8")
#pragma once
#include <QColorDialog>
#include <QColor>
// 在文件顶部 #include 之后任意合适位置（比如匿名命名空间）加入：
QColor ShowStyledColorDialog(const QColor& initial, QWidget* parent)
{
    QColorDialog dlg(initial, parent);
    dlg.setOption(QColorDialog::ShowAlphaChannel, true);
    dlg.setOption(QColorDialog::DontUseNativeDialog, true); // 必须：否则样式不会作用(原生对话框不支持)
    dlg.setWindowTitle("选择绘制颜色");
    dlg.setWindowIcon(QIcon(":/qdialog/image/ColorDialog.png"));
    // 深色 QSS
    const QString qss = R"(
        QColorDialog {
            background: rgba(20,20,20,200);
            border: 1px solid #2F2F2F;
        }
        QColorDialog QWidget {
            background: #1E1E1E;
            color: #E0E0E0;
            font: 10pt 'Microsoft YaHei';
            qproperty-alignment: 'AlignCenter';
            text-align: center;                /* 居中 */
        }
        QLabel {
            background: transparent;
            border: 0px solid #3A3A3A;
            color: #CCCCCC;
        }
        QLineEdit {
            background: #252525;
            border: 1px solid #3A3A3A;
            border-radius: 3px;
            padding: 2px;
            selection-background-color: #555A7A;
        }
        QPushButton, QToolButton {
            background: #2B2B2B;
            border: 1px solid #3C3C3C;
            border-radius: 4px;
            padding: 2px 4px;
        }
        QPushButton:hover, QToolButton:hover {
            background: #3A3A3A;
        }
        QPushButton:pressed, QToolButton:pressed {
            background: #444444;
        }
        QAbstractItemView {
            background: #202020;
            border: 1px solid #333333;
            selection-background-color: #4A5568;
            selection-color: #FFFFFF;
        }
        /* 预设颜色面板 */
        #qt_colorpicker {
            background: #1E1E1E;
        }
        #qt_colorpicker QTableView {
            gridline-color: #444444;
            outline: none;
        }
        /* 色彩滑块 / 面板（部分私有类可能名称变化，无法全部精确控制时用通配）*/
        QSlider::groove:horizontal {
            height: 6px;
            background: #2C2C2C;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            width: 12px;
            background: #AAAAAA;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSpinBox, QDoubleSpinBox {
            background: #252525;
            border: 1px solid #3A3A3A;
            border-radius: 3px;
            padding: 1px 2px;
            width: 60px;
            height: 25px;
        }
    )";
    dlg.setStyleSheet(qss);
    if (dlg.exec() == QDialog::Accepted) {
        return dlg.selectedColor();
    }
    return initial; // 未选择返回原色
}
