#include "VideoWidget.h"
#include <QPainter>

VideoWidget::VideoWidget(QWidget* parent) 
	: QWidget(parent) {

}

VideoWidget::VideoWidget() {
}

VideoWidget::~VideoWidget()
{
}

void VideoWidget::resizeEvent(QResizeEvent* event) {
    Q_UNUSED(event)
    des_w_h_ratio_ = static_cast<double>(this->width()) / static_cast<double>(this->height());
    update();
}

void VideoWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    if (pixmap_.isNull())  {
        painter.fillRect(this->rect(), QColor(40, 40, 40));
    } else {
        int dest_h = this->height();
        int dest_w = this->width();

        if (des_w_h_ratio_ >= src_w_h_ratio_) {
            scale_h_ = dest_h;
            scale_w_ = static_cast<int>(dest_w / src_w_h_ratio_);
        }

        QPixmap temp_pixmap = pixmap_.scaled(scale_w_, scale_h_);
        int adjust_pos_h = dest_h - temp_pixmap.height();
        int adjust_pos_w = dest_w - temp_pixmap.width();

        QRect draw_rect = this->rect();
        draw_rect.adjust(adjust_pos_w / 2, adjust_pos_h / 2, -adjust_pos_w / 2, -adjust_pos_h / 2);

        //draw
        painter.fillRect(this->rect(), Qt::black);
        painter.drawPixmap(draw_rect, temp_pixmap);
    }
}

void VideoWidget::SetVideoPixmap(QPixmap& pixmap) {
    pixmap_ = pixmap;
    src_w_h_ratio_ = static_cast<double>(pixmap_.width()) / static_cast<double>(pixmap_.height());
    update();
}

void VideoWidget::slot_GetOneFrame(QImage image)
{
    SetVideoPixmap(QPixmap::fromImage(image));
}
